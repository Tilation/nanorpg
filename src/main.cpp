#include <Arduino.h>
#include <ss_oled.h>
#include <screen.h>
#include <entity.h>
#include <enums.h>
#include <button.h>
#include <xenemies.h>
#include <player.h>
#include <xstrings.h>

#define SDA_PIN 18
#define SCL_PIN 19
#define RESET_PIN -1

void StepStateMachine(int state);
void EnableInput();
void DisableInput();
void ShowNumber();
void GenerateRandomNumber();
void ToggleFont();
void ShowFreeMem();
void buttonBackEvent();
void buttonSwitchEvent();
void buttonOkEvent();
void DisableInputFor(word);


Player player("Caspr");
static uint8_t ucBackBuffer[512];
char strbuffer[6];
int rc;
SSOLED oled;

Screen * ActiveScreen;
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

word millisecondCounter;
bool isInputDisabled = false;
bool isMillisecondCounterEnabled = false;



// Timer 0 Interrupt is called once a millisecond
SIGNAL(TIMER0_COMPA_vect)
{
  if (isMillisecondCounterEnabled)
  {
    millisecondCounter--;
    if (millisecondCounter == 0)
    {
      isMillisecondCounterEnabled = false;
      isInputDisabled = false;
    }
  }

  if (isInputDisabled == true) return;
  
  for (int i = 0; i < buttonCount; i++)
  {
    buttons[i].buttonCallBack();
  }
}


void setup()
{
  Serial.begin(9600);
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

    oledSetBackBuffer(&oled, ucBackBuffer);
    oledFill(&oled, 0, 1);
    oledSetContrast(&oled, 127);
    oledWriteString(&oled, 0, 0, 0, (char *)"**************** ", FONT_6x8, 0, 1);
    oledWriteString(&oled, 0, 4, 1, (char *)"BitBank SS_OLED", FONT_6x8, 0, 1);
    oledWriteString(&oled, 0, 8, 2, (char *)"SSD1306 128x32", FONT_6x8, 0, 1);
    oledWriteString(&oled, 0, 0, 3, (char *)"**************** ", FONT_6x8, 0, 1);
    oledFill(&oled, 0, 1);

    DisableInputFor(4000);
    StepStateMachine(STATE::GENERATE_NEW_ENEMY);
  }
}


void loop() {}

void StepStateMachine(int state)
{
  switch (state)
  {
    case STATE::RESUMEBATTLE:
      ActiveScreen = &battlescreen;
      break;

    case STATE::SHOWENEMYINFO:
      enemyinfoscreen.Setup(&battlescreen.Enemy);
      ActiveScreen = &enemyinfoscreen;
      break;

    case STATE::GENERATE_NEW_ENEMY:
      ActiveScreen = &enemygenscreen;
      break;
      
    case STATE::ENEMYGENERATED:
      ActiveScreen = &battlescreen;
      break;
      
    case STATE::KILL_SCREEN_END:
      ActiveScreen = &enemygenscreen;
      break;
      
    case STATE::ENEMYKILLED:
      ActiveScreen = &enemydeadscreen;
      break;
  }
  
  (*ActiveScreen).Show();
}

void buttonBackEvent()
{
    (*ActiveScreen).ButtonBackPressed();
}

void buttonSwitchEvent()
{
    (*ActiveScreen).ButtonSwitchPressed();
}

void buttonOkEvent()
{
    (*ActiveScreen).ButtonOkPressed();
}
void EnableInput()
{
  isInputDisabled = false;
  isMillisecondCounterEnabled = false;
}

void DisableInput()
{
  isInputDisabled = true;
  isMillisecondCounterEnabled = false;
}

void DisableInputFor(word ms)
{
  millisecondCounter = min(1,ms);
  isInputDisabled = true;
  isMillisecondCounterEnabled = true;
}