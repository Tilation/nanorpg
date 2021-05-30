#pragma once
#include <ss_oled.h>
#include <entity.h>
#include <enums.h>
#include <xenemies.h>
#include <xstrings.h>
#include <player.h>
#include <menus.h>

#define true 1
#define false 0

class Screen
{
protected:
  static void (*StateMachineStep)(int);
  static SSOLED *oled;
  static Player *player;

public:
  Screen() {}

  static void Setup(void (*sms)(int ns), SSOLED *o, Player *p)
  {
    StateMachineStep = sms;
    oled = o;
    player = p;
  }

  virtual void Show() {}
  virtual void ButtonBackPressed() {}
  virtual void ButtonSwitchPressed() {}
  virtual void ButtonOkPressed() {}
};

void (*Screen::StateMachineStep)(int);
SSOLED *Screen::oled;
Player *Screen::player;

class BattleScreen : public Screen
{
public:
  STRING menuOpts[4] = {STRING::menu_attck, STRING::menu_objct, STRING::menu_info, STRING::menu_flee};
  Menu menu;
  Entity Enemy;
  bool isPlayerTurn;

  BattleScreen()
  {
    menu.SetupStrings(menuOpts, 4);
  }

  void Attack()
  {
    word dmg = Screen::player->GenerateDamage();
    bool dead = Enemy.Damage(dmg);
    if (dead == true)
    {
      Screen::StateMachineStep(STATE::ENEMYKILLED);
      return;
    }
    Show();
  }

  void ButtonOkPressed() override
  {
    switch (menu.selected)
    {
    case 0: Attack(); EnemyTurn(); break;
    case 1: break;
    case 2: Screen::StateMachineStep(STATE::SHOWENEMYINFO); break;
    case 3: TryEscape(); EnemyTurn(); break;
    }
  }

  void EnemyTurn()
  {
    word dmg = Enemy.GenerateDamage();
    bool dead = player->Damage(dmg);
    if (dead == true)
    {
      Screen::StateMachineStep(STATE::HOME_SCREEN);
      return;
    }
    Show();
  }

  void TryEscape()
  {
      if (random(0,3) == 0)
      {
          Screen::StateMachineStep(STATE::HOME_SCREEN);
      }
  }

  void ButtonSwitchPressed() override
  {
    menu.ButtonSwitch();
    Show();
  }

  void Show() override
  {
    oledFill(Screen::oled, 0, 0);
    //Draw Player Name
    oledWriteString(Screen::oled, 0, 0, 0, Screen::player->Name, FONT_6x8, 0, 0);

    //Draw Player HP
    xGetString(STRING::battlescreen_hp);
    sprintf(strbuffer, internalbuffer, Screen::player->Hp);
    oledWriteString(Screen::oled, 0, 0, 1, strbuffer, FONT_6x8, 0, 0);

    //Draw Player DMG
    xGetString(STRING::battlescreen_dm);
    sprintf(strbuffer, internalbuffer, Screen::player->Dmg);
    oledWriteString(Screen::oled, 0, 0, 2, strbuffer, FONT_6x8, 0, 0);

    //Draw Player DEF
    xGetString(STRING::battlescreen_df);
    sprintf(strbuffer, internalbuffer, Screen::player->Def);
    oledWriteString(Screen::oled, 0, 0, 3, strbuffer, FONT_6x8, 0, 0);

    // DRAW SEPARATOR
    oledDrawLine(Screen::oled, 31, 0, 31, 31, 0);

    menu.Draw(Screen::oled, 32, 0, 4);

    // DRAW ENEMY PORTRAIT
    // BORDER
    oledRectangle(Screen::oled, 65, 0, 65 + 31, 31, 1, 0);

    oledRectangle(Screen::oled, 67, 2, 67 + 27, 2 + 27, 1, 0);

    //Draw Enemy Name
    oledWriteString(Screen::oled, 0, 98, 0, Enemy.Name, FONT_6x8, 0, 0);

    //Draw Enemy HP
    xGetString(STRING::battlescreen_hp);
    sprintf(strbuffer, internalbuffer, Enemy.Hp);
    oledWriteString(Screen::oled, 0, 98, 1, strbuffer, FONT_6x8, 0, 0);

    //Draw Enemy DMG

    xGetString(STRING::battlescreen_dm);
    sprintf(strbuffer, internalbuffer, Enemy.Dmg);
    oledWriteString(Screen::oled, 0, 98, 2, strbuffer, FONT_6x8, 0, 0);

    //Draw Enemy DEF

    xGetString(STRING::battlescreen_dm);
    sprintf(strbuffer, internalbuffer, Enemy.Def);
    oledWriteString(Screen::oled, 0, 98, 3, strbuffer, FONT_6x8, 0, 0);

    oledDumpBuffer(Screen::oled, NULL);
  }

  void SetPlayerTurn(bool ATurn)
  {
    isPlayerTurn = ATurn;
  }
};

class EnemyGeneratorScreen : public Screen
{
protected:
  BattleScreen *battlescreen;

public:
  EnemyGeneratorScreen(BattleScreen *bs)
  {
    battlescreen = bs;
  }

  void Show() override
  {
    byte idx = random(0, enemyCount);

    GetEnemy(&(battlescreen->Enemy), idx);
    battlescreen->SetPlayerTurn(true);

    oledFill(Screen::oled, 0, 0);
    oledRectangle(Screen::oled, 0, 0, 127, 31, 1, 0);

    xGetString(STRING::enemygeneration_nextup);
    oledWriteString(Screen::oled, 0, 4, 1, internalbuffer, FONT_6x8, 0, 0);
    oledWriteString(Screen::oled, 0, 4 + 11 * 6, 1, battlescreen->Enemy.Name, FONT_6x8, 0, 0);

    oledDumpBuffer(Screen::oled, NULL);
  }

  void ButtonOkPressed() override
  {
    Screen::StateMachineStep(STATE::ENEMYGENERATED);
  }
};

class EnemyDeadScreen : public Screen
{
protected:
  BattleScreen *battlescreen;

public:
  EnemyDeadScreen(BattleScreen *bs)
  {
    battlescreen = bs;
  }

  void Show() override
  {
    oledFill(Screen::oled, 0, 0);

    oledRectangle(Screen::oled, 0, 0, 127, 31, 1, 0);

    xGetString(STRING::enemydead);
    oledWriteString(Screen::oled, 0, 64 - 3 * 6, 2, internalbuffer, FONT_6x8, 0, 0);
    oledWriteString(Screen::oled, 0, 64 - 3 * 6, 1, battlescreen->Enemy.Name, FONT_6x8, 0, 0);
    oledDumpBuffer(Screen::oled, NULL);
  }

  void ButtonOkPressed() override
  {
    word exp = (battlescreen->Enemy.MaxHp + battlescreen->Enemy.Dmg * battlescreen->Enemy.Def);
    bool lvlup = battlescreen->player->AddExp(exp);
    Screen::StateMachineStep(STATE::KILL_SCREEN_END);
  }
};

class EnemyInfoScreen : public Screen
{
private:
public:
  EnemyInfoScreen() {}
  Entity *enemy;

  void Show() override
  {
    oledFill(Screen::oled, 0, 0);
    oledRectangle(Screen::oled, 0, 0, 127, 31, 1, 0);
    oledSetPixel(Screen::oled, 0, 0, 0, 0);
    oledSetPixel(Screen::oled, 127, 0, 0, 0);
    oledSetPixel(Screen::oled, 127, 31, 0, 0);
    oledSetPixel(Screen::oled, 127, 31, 0, 0);

    xGetString(STRING::info_1);
    sprintf(strbuffer, internalbuffer, enemy->Dmg, enemy->Def);

    oledWriteString(Screen::oled, 0, 4, 1, strbuffer, FONT_6x8, 0, 0);

    xGetString(STRING::info_2);
    sprintf(strbuffer, internalbuffer, enemy->Name, enemy->Hp, enemy->MaxHp);
    oledWriteString(Screen::oled, 0, 4, 2, strbuffer, FONT_6x8, 0, 0);

    oledDumpBuffer(Screen::oled, NULL);
  }

  void ButtonOkPressed() override
  {
    Screen::StateMachineStep(STATE::RESUMEBATTLE);
  }
};

class HomeScreen : public Screen
{
public:
  Menu menu;
  STRING menuOpts[5] = {
    STRING::home_adventure,
    STRING::home_items,
    STRING::home_craft,
    STRING::home_trade,
    STRING::home_stats
    };

  HomeScreen()
  {
    menu.draw_bar = true;
    menu.SetupStrings(menuOpts, 5);
  }

  void Show() override
  {
    oledFill(Screen::oled, 0, 0);

    oledWriteString(Screen::oled, 0, 0, 0, player->Name, FONT_6x8, 0, 0);

    bool has_s = (player->Name[strlen(player->Name) - 1]) == 's' || (player->Name[strlen(player->Name) - 1]) == 'S';
    if (has_s)
    {
      xGetString(STRING::home_welcome_s);
      oledWriteString(Screen::oled, 0, (strlen(player->Name)) * 6, 0, internalbuffer, FONT_6x8, 0, 0);
    }
    else
    {
      xGetString(STRING::home_welcome_nos);
      oledWriteString(Screen::oled, 0, (strlen(player->Name)) * 6, 0, internalbuffer, FONT_6x8, 0, 0);
    }

    xGetString(STRING::home_welcome);
    oledWriteString(Screen::oled, 0, (strlen(player->Name)+ 2 + (!has_s)) * 6, 0, internalbuffer, FONT_6x8, 0, 0);

    oledDrawLine(Screen::oled, 0, 8, 0, 31, 0);

    menu.Draw(Screen::oled, 1, 1, 3);

    oledDrawLine(Screen::oled, 5 * 6 + 2, 8, 5 * 6 + 2, 31, 0);

    oledDumpBuffer(Screen::oled, NULL);
  }

  void ButtonOkPressed() override
  {
    switch (menu.selected)
    {
      case 0: Screen::StateMachineStep(STATE::GENERATE_NEW_ENEMY); break;
      case 1: break;
      case 2: break;
      case 3: break;
    }
  }

  void ButtonSwitchPressed() override
  {
    menu.ButtonSwitch();
    Show();
  }
};
