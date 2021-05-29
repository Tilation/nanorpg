#pragma once
#include <ss_oled.h>
#include <entity.h>
#include <enums.h>
#include <xdata.h>
#include <player.h>

class Screen
{
protected:
  static void (*StateMachineStep)(int);
  static SSOLED *oled;
  static Player *player;
  static char strbuffer[6];

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
char Screen::strbuffer[6];

class BattleScreen : public Screen
{
private:
public:
  BattleScreen() {}
  char *menus[4][6] =
      {
          "Attck",
          "Objct",
          "Info",
          "Flee"};
  byte menusCount = 4;
  byte selectedMenu = 0;

  Entity Enemy;
  bool isPlayerTurn;

  void ButtonOkPressed() override
  {
    switch (selectedMenu)
    {
    case 0:
    {
      word dmg = Screen::player->GenerateDamage();
      bool dead = Enemy.Damage(dmg);
      if (dead == true)
      {
        Screen::StateMachineStep(STATE::ENEMYKILLED);
        return;
      }
      Show();
      break;
    }
    case 1:
    {
      break;
    }
    case 2:
    {
      Screen::StateMachineStep(STATE::SHOWENEMYINFO);
      return;
      break;
    }
    case 3:
    {
      break;
    }
    }
  }

  void ButtonSwitchPressed() override
  {
    selectedMenu++;
    selectedMenu = selectedMenu >= menusCount ? 0 : selectedMenu;
    Show();
  }

  void Show() override
  {

    oledFill(Screen::oled, 0, 0);
    //Draw Player Name
    oledWriteString(Screen::oled, 0, 0, 0, Screen::player->Name, FONT_6x8, 0, 0);

    //Draw Player HP
    sprintf(strbuffer, "HP%3d", Screen::player->Hp);
    oledWriteString(Screen::oled, 0, 0, 1, strbuffer, FONT_6x8, 0, 0);

    //Draw Player DMG
    sprintf(strbuffer, "DM%3d", Screen::player->Dmg);
    oledWriteString(Screen::oled, 0, 0, 2, strbuffer, FONT_6x8, 0, 0);

    //Draw Player DEF
    sprintf(strbuffer, "DF%3d", Screen::player->Def);
    oledWriteString(Screen::oled, 0, 0, 3, strbuffer, FONT_6x8, 0, 0);

    // DRAW SEPARATOR
    oledDrawLine(Screen::oled, 31, 0, 31, 31, 0);

    // DRAW ACTIONS
    for (byte i = 0; i < menusCount; i++)
    {
      oledWriteString(Screen::oled, 0, 32, i, (*menus)[i], FONT_6x8, selectedMenu == i ? 1 : 0, 0);
    }

    // DRAW SEPARATOR
    //      oledDrawLine(Screen::oled,  63,  0,  63,  31, 0);
    //
    //DRAW ENEMY SPRITE

    oledRectangle(Screen::oled, 65, 0, 65 + 32, 31, 1, 0);

    //Draw Enemy Name
    oledWriteString(Screen::oled, 0, 98, 0, Enemy.Name, FONT_6x8, 0, 0);

    //Draw Enemy HP
    sprintf(strbuffer, "HP%3d", Enemy.Hp);
    oledWriteString(Screen::oled, 0, 98, 1, strbuffer, FONT_6x8, 0, 0);

    //Draw Enemy DMG
    sprintf(strbuffer, "DM%3d", Enemy.Dmg);
    oledWriteString(Screen::oled, 0, 98, 2, strbuffer, FONT_6x8, 0, 0);

    //Draw Enemy DEF
    sprintf(strbuffer, "DF%3d", Enemy.Def);
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

    oledWriteString(Screen::oled, 0, 4, 1, (char *)"Next up is ", FONT_6x8, 0, 0);
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

    oledWriteString(Screen::oled, 0, 64 - 3 * 6, 2, (char *)"Killed", FONT_6x8, 0, 0);
    oledWriteString(Screen::oled, 0, 64 - 3 * 6, 1, battlescreen->Enemy.Name, FONT_6x8, 0, 0);
    oledDumpBuffer(Screen::oled, NULL);
  }

  void ButtonOkPressed() override
  {
    word exp = (battlescreen->Enemy.MaxHp + battlescreen->Enemy.Dmg  * battlescreen->Enemy.Def);
    bool lvlup = battlescreen->player->AddExp(exp);
    Screen::StateMachineStep(STATE::KILL_SCREEN_END);
  }
};

class EnemyInfoScreen : public Screen
{
private:
  Entity *enemy;

public:
  EnemyInfoScreen() {}

  void Setup(Entity *e)
  {
    enemy = e;
  }

  void Show() override
  {
    oledFill(Screen::oled, 0, 0);
    oledRectangle(Screen::oled, 0, 0, 127, 31, 1, 0);
    oledSetPixel(Screen::oled, 0, 0, 0, 0);
    oledSetPixel(Screen::oled, 127, 0, 0, 0);
    oledSetPixel(Screen::oled, 127, 31, 0, 0);
    oledSetPixel(Screen::oled, 127, 31, 0, 0);

    char bff[22];

    sprintf(bff, "INFO: DM/DF:%3d/%-3d", enemy->Dmg, enemy->Def);
    oledWriteString(Screen::oled, 0, 4, 1, bff, FONT_6x8, 0, 0);

    sprintf(bff, "%-5s    HP:%3d/%-3d", enemy->Name, enemy->Hp, enemy->MaxHp);
    oledWriteString(Screen::oled, 0, 4, 2, bff, FONT_6x8, 0, 0);

    oledDumpBuffer(Screen::oled, NULL);
  }

  void ButtonOkPressed() override
  {
    Screen::StateMachineStep(STATE::RESUMEBATTLE);
  }
};