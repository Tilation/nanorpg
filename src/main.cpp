#include <Arduino.h>
#include <ss_oled.h>
#include <screen.h>
#include <entity.h>
#include <enums.h>
#include <button.h>
#include <xenemies.h>
#include <player.h>
#include <xstrings.h>
#include <MemoryFree.h>

#define SDA_PIN 18
#define SCL_PIN 19
#define RESET_PIN -1

void StepStateMachine(int state);
void buttonBackEvent();
void buttonSwitchEvent();
void buttonOkEvent();

Player player("Caspr",CLASS::WARRIOR);
static uint8_t ucBackBuffer[512];

int rc;
SSOLED oled;

Screen * ActiveScreen = 0;
HomeScreen homescreen;
EnemyInfoScreen enemyinfoscreen;
BattleScreen battlescreen;
EnemyGeneratorScreen enemygenscreen(&battlescreen);
EnemyDeadScreen enemydeadscreen(&battlescreen);

Button buttons[] = {
  {5, buttonBackEvent},
  {6, buttonSwitchEvent},
  {7, buttonOkEvent}
};
int buttonCount = sizeof(buttons) / sizeof(buttons[0]);

// Timer 0 Interrupt is called once a millisecond
SIGNAL(TIMER0_COMPA_vect)
{  
  for (int i = 0; i < buttonCount; i++)
  {
    buttons[i].buttonCallBack();
  }
}
auto ptr = &internalbuffer;

void setup()
{
  
  Serial.begin(9600);
  while (!Serial);

  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);

  for (int i = 0; i < buttonCount; i++)
  {
    pinMode(buttons[i].PIN(), INPUT);
  }

  rc = oledInit(&oled, OLED_128x32, 0x3c, 0, 0, 0, SDA_PIN, SCL_PIN, RESET_PIN, 1000000L);
  if (rc != OLED_NOT_FOUND)
  {
    Screen::Setup(StepStateMachine,&oled,&player);

    // internal char buffer shenanigans
    // minimum 2 prints for the program to load (wtf?)
    Serial.println(internalbuffer);
    Serial.println(internalbuffer);   
    //Serial.println(internalbuffer);
    //Serial.println(internalbuffer);
    
    oledSetBackBuffer(&oled, ucBackBuffer);
    oledFill(&oled, 0, 1);
    oledSetContrast(&oled, 127);

    StepStateMachine(STATE::HOME_SCREEN);
  }
}


void loop() {}

void StepStateMachine(int state)
{
  switch (state)
  {
    case STATE::RESUMEBATTLE: ActiveScreen = &battlescreen; break;
    case STATE::SHOWENEMYINFO: enemyinfoscreen.enemy = &battlescreen.Enemy; ActiveScreen = &enemyinfoscreen; break;
    case STATE::GENERATE_NEW_ENEMY: ActiveScreen = &enemygenscreen; break;
    case STATE::ENEMYGENERATED: ActiveScreen = &battlescreen; break;
    case STATE::KILL_SCREEN_END: ActiveScreen = &enemygenscreen; break;
    case STATE::ENEMYKILLED: ActiveScreen = &enemydeadscreen; break;
    case STATE::HOME_SCREEN: ActiveScreen = &homescreen; break;
  }
  
  if (ActiveScreen != 0) (*ActiveScreen).Show();
}

void buttonBackEvent()
{
  if (ActiveScreen != 0) (*ActiveScreen).ButtonBackPressed();
}

void buttonSwitchEvent()
{
  if (ActiveScreen != 0) (*ActiveScreen).ButtonSwitchPressed();
}

void buttonOkEvent()
{
  if (ActiveScreen != 0) (*ActiveScreen).ButtonOkPressed();
}