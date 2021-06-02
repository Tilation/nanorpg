#pragma once
//#include <ss_obd.h>
#include <entity.h>
#include <enums.h>
#include <data.h>
#include <player.h>
#include <menus.h>
#include <OneBitDisplay.h>

#define true 1
#define false 0

class Screen
{
protected:
  static void (*StateMachineStep)(int);
  static OBDISP *oled;
  static Player *player;
  static Screen * ActiveScreen;
  static Screen * previous_screen;

public:



static void ShowActiveScreen()
{
  if (ActiveScreen != 0)
  {
    ActiveScreen->Show();
  }
}

static void ShowPreviousScreen()
{
  if (previous_screen != 0)
  {
    ActiveScreen = previous_screen;
    previous_screen = 0;
  }
  ShowActiveScreen();
}

static void ActiveScreenButtonOk()
{
if (ActiveScreen != 0)
  {
    ActiveScreen->ButtonOkPressed();
  }
}

static void ActiveScreenButtonSwitch()
{
  if (ActiveScreen != 0)
  {
    ActiveScreen->ButtonSwitchPressed();
  }
}

static void ActiveScreenButtonBack()
{
  if (ActiveScreen != 0)
  {
    ActiveScreen->ButtonBackPressed();
  }
}
static void SetActiveScreen(Screen * previous, Screen  * active)
{
  previous_screen = previous;
  ActiveScreen = active;
}
static void SetActiveScreen(Screen * newscreen)
{
  previous_screen = ActiveScreen;
  ActiveScreen = newscreen;
}
  Screen() {}

  static void Setup(void (*sms)(int ns), OBDISP *o, Player *p)
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
OBDISP *Screen::oled;
Player *Screen::player;
Screen *Screen::ActiveScreen = 0;
Screen *Screen::previous_screen = 0;

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

  bool Attack()
  {
    word dmg = Screen::player->GenerateDamage();
    bool dead = Enemy.Damage(dmg);
    Show();
    return dead;
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

  bool TryEscape()
  {
    return random(0, 2) == 0;
  }

  void ButtonOkPressed() override
  {
    switch (menu.selected)
    {
    case 0:
      if (!Attack())
        EnemyTurn();
      else
        Screen::StateMachineStep(STATE::ENEMYKILLED);
      break;

    case 1:
      break;

    case 2:
      Screen::StateMachineStep(STATE::SHOWENEMYINFO);
      break;

    case 3:
      if (!TryEscape())
        EnemyTurn();
      else
        Screen::StateMachineStep(STATE::HOME_SCREEN);
      break;
    }
  }
  void ButtonSwitchPressed() override
  {
    menu.ButtonSwitch();
    Show();
  }

  void Show() override
  {
    obdFill(Screen::oled, 0, 0);

    //Draw Player Name
    obdWriteString(Screen::oled, 0, 0, 0, Screen::player->Name, FONT_6x8, 0, 0);

    //Draw Player HP
    Data::xGetString(STRING::battlescreen_hp);
    sprintf(Data::strbuffer,Data::internalbuffer, Screen::player->Hp);
    obdWriteString(Screen::oled, 0, 0, 1, Data::strbuffer, FONT_6x8, 0, 0);

    //Draw Player DMG
    
    sprintf(Data::strbuffer, Data::xGetString(STRING::battlescreen_dm), Screen::player->Dmg);
    obdWriteString(Screen::oled, 0, 0, 2, Data::strbuffer, FONT_6x8, 0, 0);

    //Draw Player DEF
    
    sprintf(Data::strbuffer, Data::xGetString(STRING::battlescreen_df), Screen::player->Def);
    obdWriteString(Screen::oled, 0, 0, 3, Data::strbuffer, FONT_6x8, 0, 0);

    // DRAW SEPARATOR
    obdDrawLine(Screen::oled, 31, 0, 31, 31, 1,0);

    menu.Draw(Screen::oled, 32, 0, 4);

    // DRAW ENEMY PORTRAIT
    // BORDER
    obdRectangle(Screen::oled, 65, 0, 65 + 31, 31, 1, 0);

    obdRectangle(Screen::oled, 67, 2, 67 + 27, 2 + 27, 1, 0);

    //Draw Enemy Name
    obdWriteString(Screen::oled, 0, 98, 0, Enemy.Name, FONT_6x8, 0, 0);

    //Draw Enemy HP
    
    sprintf(Data::strbuffer, Data::xGetString(STRING::battlescreen_hp), Enemy.Hp);
    obdWriteString(Screen::oled, 0, 98, 1, Data::strbuffer, FONT_6x8, 0, 0);

    //Draw Enemy DMG

    
    sprintf(Data::strbuffer, Data::xGetString(STRING::battlescreen_dm), Enemy.Dmg);
    obdWriteString(Screen::oled, 0, 98, 2, Data::strbuffer, FONT_6x8, 0, 0);

    //Draw Enemy DEF

    
    sprintf(Data::strbuffer, Data::xGetString(STRING::battlescreen_dm), Enemy.Def);
    obdWriteString(Screen::oled, 0, 98, 3, Data::strbuffer, FONT_6x8, 0, 0);

    obdDumpBuffer(Screen::oled, NULL);
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

    Data::GetEnemy(&(battlescreen->Enemy), idx);
    battlescreen->SetPlayerTurn(true);

    obdFill(Screen::oled, 0, 0);
    obdRectangle(Screen::oled, 0, 0, 127, 31, 1, 0);

    obdWriteString(Screen::oled, 0, 4, 1, Data::xGetString(STRING::enemygeneration_nextup), FONT_6x8, 0, 0);
    obdWriteString(Screen::oled, 0, 4 + 11 * 6, 1, battlescreen->Enemy.Name, FONT_6x8, 0, 0);

    obdDumpBuffer(Screen::oled, NULL);
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
    obdFill(Screen::oled, 0, 0);

    obdRectangle(Screen::oled, 0, 0, 127, 31, 1, 0);

    obdWriteString(Screen::oled, 0, 64 - 3 * 6, 2, Data::xGetString(STRING::enemydead), FONT_6x8, 0, 0);
    obdWriteString(Screen::oled, 0, 64 - 3 * 6, 1, battlescreen->Enemy.Name, FONT_6x8, 0, 0);
    obdDumpBuffer(Screen::oled, NULL);
  }

  void ButtonOkPressed() override
  {
    word exp = (battlescreen->Enemy.MaxHp + battlescreen->Enemy.Dmg * battlescreen->Enemy.Def);
    byte lvlup = battlescreen->player->AddExp(exp);
    for (int i = 0; i<lvlup; i++)
    {
      Data::PlayerAddLevelUpStats(battlescreen->player);
    }
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
    obdFill(Screen::oled, 0, 0);
    obdRectangle(Screen::oled, 0, 0, 127, 31, 1, 0);

    obdRectangle(Screen::oled, 0,    0,  1,    1,   0,  1);        //tl  
    obdRectangle(Screen::oled, 126,  0,  127,  0,   0,  1);    //tr
    obdRectangle(Screen::oled, 0,    30, 1,    31,  0,  1);        //bl
    obdRectangle(Screen::oled, 126,  30, 127,  31,  0,  1);    //br

    obdRectangle(Screen::oled, 1,    1,  2,    2,   1,  1);        //tl  
    obdRectangle(Screen::oled, 125,  1,  125,  2,   1,  1);    //tr
    obdRectangle(Screen::oled, 1,    29, 2,    30,  1,  1);        //bl
    obdRectangle(Screen::oled, 125,  29, 126,  30,  1,  1);    //br


    obdSetPixel(Screen::oled, 2,   2,  0, 0);
    obdSetPixel(Screen::oled, 125, 2,  0, 0);
    obdSetPixel(Screen::oled, 2,   29, 0, 0);
    obdSetPixel(Screen::oled, 125, 29, 0, 0);

    /*
__XXXXXX__
_X______X_
X________X
X________X
X________X
_X______X_
__XXXXXX__
*/

    
    sprintf(Data::strbuffer, Data::xGetString(STRING::info_1), enemy->Dmg, enemy->Def);

    obdWriteString(Screen::oled, 0, 4, 1, Data::strbuffer, FONT_6x8, 0, 0);

    sprintf(Data::strbuffer, Data::xGetString(STRING::info_2), enemy->Name, enemy->Hp, enemy->MaxHp);
    obdWriteString(Screen::oled, 0, 4, 2, Data::strbuffer, FONT_6x8, 0, 0);

    obdDumpBuffer(Screen::oled, NULL);
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
  STRING menuOpts[6] = {
      STRING::home_adventure,
      STRING::home_items,
      STRING::home_craft,
      STRING::home_trade,
      STRING::home_stats,
      STRING::home_data};

  HomeScreen()
  {
    menu.draw_bar = true;
    menu.SetupStrings(menuOpts, 6);
  }

  void Show() override
  {
    obdFill(Screen::oled, 0, 0);

    obdWriteString(Screen::oled, 0, 0, 0, player->Name, FONT_6x8, 0, 0);

    bool has_s = (player->Name[strlen(player->Name) - 1]) == 's' || (player->Name[strlen(player->Name) - 1]) == 'S';
    if (has_s)
    {
      obdWriteString(Screen::oled, 0, (strlen(player->Name)) * 6, 0, Data::xGetString(STRING::home_welcome_s), FONT_6x8, 0, 0);
    }
    else
    { 
      obdWriteString(Screen::oled, 0, (strlen(player->Name)) * 6, 0, Data::xGetString(STRING::home_welcome_nos), FONT_6x8, 0, 0);
    }

    obdWriteString(Screen::oled, 0, (strlen(player->Name) + 2 + (!has_s)) * 6, 0, Data::xGetString(STRING::home_welcome), FONT_6x8, 0, 0);

    obdDrawLine(Screen::oled, 0, 8, 0, 31, 1,0);

    menu.Draw(Screen::oled, 1, 1, 3);

    obdDrawLine(Screen::oled, 5 * 6 + 2, 8, 5 * 6 + 2, 31,1, 0);

    obdDumpBuffer(Screen::oled, NULL);
  }

  void ButtonOkPressed() override
  {
    switch (menu.selected)
    {
    case 0: Screen::StateMachineStep(STATE::GENERATE_NEW_ENEMY); break;
    case 5: Screen::StateMachineStep(STATE::SHOW_SAVE_LOAD_SCREEN); break;
    }
  }

  void ButtonSwitchPressed() override
  {
    menu.ButtonSwitch();
    Show();
  }
};

class DataScreen : public Screen
{
  public:
    Menu menu;
      STRING menuOpts[2] = {
      STRING::data_save,
      STRING::data_load};

  DataScreen()
  {
    menu.draw_bar = true;
    menu.SetupStrings(menuOpts, 2);
  }

  void ButtonBackPressed() override
  {
    Screen::StateMachineStep(STATE::HOME_SCREEN);
  }

  void Show() override
  {
      obdFill(Screen::oled,0,0);
      menu.Draw(Screen::oled, 126 - 5*6,2,2);
      obdDumpBuffer(Screen::oled, NULL);
  }

  void SaveData()
  {
    EEPROM.put(0, (*Screen::player));
    Screen::StateMachineStep(STATE::HOME_SCREEN);
  }

  void LoadData()
  {
    EEPROM.get(0, player);
    Screen::StateMachineStep(STATE::HOME_SCREEN);
  }

  void ButtonOkPressed() override 
  {
    switch(menu.selected)
    {
      case 0: Data::PlayerSaveToEEPROM(player); break; //save
      case 1: Data::PlayerLoadFromEEPROM(player); break; //load
    }

    Screen::StateMachineStep(STATE::HOME_SCREEN);
  }

  void ButtonSwitchPressed() override
  {
    menu.ButtonSwitch();
    Show();
  }
};

class StoryScreen : public Screen
{
  public:
    static STRING text_rows[4];

  StoryScreen()
  {
    Clear();
  }
  void Clear()
  {
    text_rows[0] = STRING::blank_space;
    text_rows[1] = STRING::blank_space;
    text_rows[2] = STRING::blank_space;
    text_rows[3] = STRING::blank_space;
  }
  void Show() override
  {
      obdFill(Screen::oled, 0,0);

      if (text_rows[0] != STRING::blank_space) obdWriteString(Screen::oled, 0, 3, 0, Data::xGetString(text_rows[0]),0,0,0);
      if (text_rows[0] != STRING::blank_space) obdWriteString(Screen::oled, 0, 3, 1, Data::xGetString(text_rows[1]),0,0,0);
      if (text_rows[0] != STRING::blank_space) obdWriteString(Screen::oled, 0, 3, 2, Data::xGetString(text_rows[2]),0,0,0);
      if (text_rows[0] != STRING::blank_space) obdWriteString(Screen::oled, 0, 3, 3, Data::xGetString(text_rows[3]),0,0,0);

      obdDrawLine(Screen::oled, 2,0,0,15,1,0);
      obdDrawLine(Screen::oled, 0,15,2,31,1,0);
      obdDrawLine(Screen::oled, 124,0,127,15,1,0);
      obdDrawLine(Screen::oled, 127,15,124,31,1,0);
      obdDumpBuffer(Screen::oled, NULL);
  }

  void ButtonBackPressed() override
  {
    ShowPreviousScreen();
  }
  void ButtonOkPressed() override
  {
    ShowPreviousScreen();
  }
};
STRING StoryScreen::text_rows[4];