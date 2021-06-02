#pragma once
enum CLASS
{
  WARRIOR = 0,
  WIZARD,
  PALADIN,
};

enum CLASS_LEVELUP
{
  MAXHP = 5,
  MAXMANA,
  INT,
  DEF,
  DMG
};

enum LOCATION
{
  PORT = 0b0000001,
  CAPITAL = 0b0000010,
};

enum STATE
{
  ENEMYKILLED = 0,
  BATTLE,
  KILL_SCREEN_END,
  ENEMYGENERATED,
  GENERATE_NEW_ENEMY,
  SHOWENEMYINFO,
  RESUMEBATTLE,
  HOME_SCREEN,
  SHOW_SAVE_LOAD_SCREEN,
  STORY_SHOW_GAMELOGO
};

enum STRING
{
  battlescreen_hp = 0,
  battlescreen_dm,
  battlescreen_df,

  info_1,
  info_2,

  menu_attck,
  menu_objct,
  menu_info,
  menu_flee,

  enemygeneration_nextup,
  enemydead,

  empty_text,

  get_enemy_format,

  home_adventure,
  home_items,
  home_stats,
  home_craft,
  home_welcome,
  home_welcome_nos,
  home_welcome_s,
  home_trade,
  home_data,

  data_save,
  data_load,

  blank_space,

  history_0,
  history_1,
  history_2,
  history_3,

  get_location_format
};