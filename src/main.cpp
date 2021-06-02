#include <Arduino.h>

#include <screen.h>
#include <entity.h>
#include <enums.h>
#include <button.h>
#include <data.h>
#include <player.h>
#include <OneBitDisplay.h>

#define true 1
#define false 0
#define SDA_PIN 18
#define SCL_PIN 19
#define RESET_PIN -1
#define buffersize 32

bool timer;
long internaltimer;

void Wait(long milis)
{
  timer = true;
  internaltimer = milis;
  do
  { 
    timer = internaltimer > 0;
  }while(timer == true);
}

void StepStateMachine(int state);
void buttonBackEvent();
void buttonSwitchEvent();
void buttonOkEvent();

char internalbuffer[buffersize];
char strbuffer[buffersize];
OBDISP oled;
char * Data::strbuffer = strbuffer;
char * Data::internalbuffer = internalbuffer;

Player player("unsa", CLASS::WARRIOR);
static uint8_t ucBackBuffer[512];

int rc;
//SSOLED oled;

HomeScreen homescreen;
EnemyInfoScreen enemyinfoscreen;
BattleScreen battlescreen;
EnemyGeneratorScreen enemygenscreen(&battlescreen);
EnemyDeadScreen enemydeadscreen(&battlescreen);
DataScreen datascreen;
StoryScreen storyscreen;

Button buttons[] = {
    {5, buttonBackEvent},
    {6, buttonSwitchEvent},
    {7, buttonOkEvent}};
int buttonCount = sizeof(buttons) / sizeof(buttons[0]);

// Timer 0 Interrupt is called once a millisecond
SIGNAL(TIMER0_COMPA_vect)
{
  internaltimer -= (internaltimer > 0) * timer;
  for (int i = 0; i < buttonCount; i++)
  {
    buttons[i].buttonCallBack();
  }
}

void InitializeButtons()
{
  for (int i = 0; i < buttonCount; i++)
  {
    pinMode(buttons[i].PIN(), INPUT);
  }
}

void setup()
{
  Data::Setup(strbuffer, internalbuffer);
  Data::PlayerLoadBaseStats(&player);
  
  Serial.begin(9600);

  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);

  InitializeButtons();

  rc = obdI2CInit(&oled, OLED_128x32, 0x3c,0,0,0,SDA_PIN, SCL_PIN, RESET_PIN, 1000000L);
  if (rc != OLED_NOT_FOUND)
  {
    Screen::Setup(StepStateMachine, &oled, &player);
    
    //char buffer shenanigans
    // minimum 2 prints for the program to load (wtf?)
    //Serial.println(internalbuffer);
    //Wait(1000);
    Serial.println(internalbuffer);
    Serial.println(internalbuffer);

    obdSetBackBuffer(&oled, ucBackBuffer);
    obdSetContrast(&oled, 127);
    obdFill(&oled, 1, 1);
    StepStateMachine(STATE::STORY_SHOW_GAMELOGO);
  }
}

void loop() {}

void StepStateMachine(int state)
{
  switch (state)
  {
  case STATE::RESUMEBATTLE:
    Screen::SetActiveScreen(&battlescreen);
    break;
  case STATE::SHOWENEMYINFO:
    enemyinfoscreen.enemy = &battlescreen.Enemy;
    Screen::SetActiveScreen(&enemyinfoscreen);
    break;
  case STATE::GENERATE_NEW_ENEMY:
    Screen::SetActiveScreen(&enemygenscreen);
    break;
  case STATE::ENEMYGENERATED:
    Screen::SetActiveScreen(&battlescreen);
    break;
  case STATE::KILL_SCREEN_END:
    Screen::SetActiveScreen(&enemygenscreen);
    break;
  case STATE::ENEMYKILLED:
    Screen::SetActiveScreen(&enemydeadscreen);
    break;
  case STATE::HOME_SCREEN:
    Screen::SetActiveScreen(&homescreen);
    break;
  case STATE::SHOW_SAVE_LOAD_SCREEN:
    Screen::SetActiveScreen(&datascreen);
    break;
  case STATE::STORY_SHOW_GAMELOGO:
    Screen::SetActiveScreen(&homescreen, &storyscreen);
    storyscreen.text_rows[0] = STRING::history_0;
    storyscreen.text_rows[1] = STRING::history_1;
    storyscreen.text_rows[2] = STRING::history_2;
    storyscreen.text_rows[3] = STRING::history_3;
    break;
  }

  Screen::ShowActiveScreen();
}

void buttonBackEvent()
{
  Screen::ActiveScreenButtonBack();
}

void buttonSwitchEvent()
{
  Screen::ActiveScreenButtonSwitch();
}

void buttonOkEvent()
{
  Screen::ActiveScreenButtonOk();
}