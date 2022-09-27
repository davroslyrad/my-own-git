

// include the library code:
#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"

// Connect via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_LiquidCrystal lcd(0);

#define STATE_ENTER   2
#define STATE_CLICK   3
#define STATE_DONE    4

#define ENTER_STATE_MODE    3
#define ENTER_STATE_DIG1    4
#define ENTER_STATE_DIG2    5
#define ENTER_STATE_DIG3    6
#define ENTER_STATE_DIG4    7
#define ENTER_STATE_NONE    8

#define MODE_FAST    0
#define MODE_NORMAL  1
#define MODE_SLOW    2

const byte buttonRight = 4;
const byte buttonUp = 5;
const byte buttonLeft = 6;
const byte buttonDown = 8;
const byte buttonOk = 9;
const byte ledPin = 13;
const byte clickPin = 11;

byte bRight = HIGH;
byte bUp = HIGH;
byte bLeft = HIGH;
byte bDown = HIGH;
byte bOk = HIGH;

byte state;
byte substate_enter;
byte digit1;
byte digit2;
byte digit3;
byte digit4;
byte current_mode;

int clickCount = 0;
byte needCount = 0;

void setup() {
  // set up the LCD's number of rows and columns: 
  lcd.begin(16, 2);
  randomSeed(analogRead(0));
  pinMode(ledPin, OUTPUT);
  
  pinMode(buttonRight, INPUT_PULLUP);
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonLeft, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonOk, INPUT_PULLUP);
  
  pinMode(clickPin, OUTPUT);
  
  state = STATE_ENTER;
  substate_enter = ENTER_STATE_MODE;
  
  digit1 = 0;
  digit2 = 0;
  digit3 = 0;
  digit4 = 0;
  
  current_mode = MODE_FAST;
  
  // Print a message to the LCD.
  //lcd.print("hello, world!");
}

void loop() {
  byte  bRead;
  byte  retVal = 0;
  int   upDelay;
  int   downDelay;
  
  bRead = localButtonRead();
  printDbg(bRead);
  if (state == STATE_ENTER)
  {
      /* Look at the sub state */
      switch(substate_enter)
      {
        case ENTER_STATE_MODE:
          lcd.setCursor(0,0);
          lcd.print("Set delay mode:");
          printDelayMode();
          if (bRead != 100)
          {
            retVal = selectMode(bRead);
            if (retVal == 1)
            {
              substate_enter = ENTER_STATE_DIG1;
              changeLcdState();
            }
            resetPins();
          }
          break;
        case ENTER_STATE_DIG1:
          lcd.setCursor(0,0);
          lcd.print("Set Digit 1:");
          printDigit(1, digit1);
          if (bRead != 100)
          {
            retVal = wrapDigit(1, bRead);
            if (retVal == 1)
            {
              substate_enter = ENTER_STATE_DIG2;
            }
            if (retVal == 2)
            {
              substate_enter = ENTER_STATE_MODE;
            }
            resetPins();
          }
          break;
        case ENTER_STATE_DIG2:
          lcd.setCursor(0,0);
          lcd.print("Set Digit 2:");
          printDigit(2, digit2);
          if (bRead != 100)
          {
            retVal = wrapDigit(2, bRead);
            if (retVal == 1)
            {
              substate_enter = ENTER_STATE_DIG3;
            }
            if(retVal == 2)
            {
              substate_enter = ENTER_STATE_DIG1;
            }
            resetPins();
          }
          break;
        case ENTER_STATE_DIG3:
          lcd.setCursor(0,0);
          lcd.print("Set Digit 3:");
          printDigit(3, digit3);
          if (bRead != 100)
          {
            retVal = wrapDigit(3, bRead);
            if (retVal == 1)
            {
              substate_enter = ENTER_STATE_DIG4;
            }
            if(retVal == 2)
            {
              substate_enter = ENTER_STATE_DIG2;
            }
            resetPins();
          }
          break;
        case ENTER_STATE_DIG4:
          lcd.setCursor(0,0);
          lcd.print("Set Digit 4:");
          printDigit(4, digit4);
          if (bRead != 100)
          {
            retVal = wrapDigit(4, bRead);
            if (retVal == 1)
            {
              substate_enter = ENTER_STATE_NONE;
              state = STATE_CLICK;
              changeLcdState();
            }
            if(retVal == 2)
            {
              substate_enter = ENTER_STATE_DIG3;
            }
            resetPins();
          }
          break;
        default:
          state = STATE_CLICK;
          changeLcdState();
          break;
      }
    delay(200);
  }
  else if (state == STATE_CLICK)
  {
    lcd.setCursor(0,0);
    lcd.print("Click Mode:");
    if (needCount == 0)
    {
      needCount = 1;
      clickCount += digit4;
      clickCount += digit3 * 10;
      clickCount += digit2 * 100;
      clickCount += digit1 * 1000;
    }
    lcd.setCursor(2, 1);
    lcd.print("    ");
    lcd.setCursor(2, 1);
    lcd.print(clickCount);
    if (clickCount > 0)
    {
      digitalWrite(ledPin, HIGH);
      digitalWrite(clickPin, HIGH);
      downDelay = random(200, 350);
      delay(downDelay);
      digitalWrite(ledPin, LOW);
      digitalWrite(clickPin, LOW);
      switch(current_mode)
      {
        case MODE_SLOW:
          upDelay = random(3155, 3345);
          break;
        case MODE_NORMAL:
          upDelay = random(2600, 2895);
          break;
        case MODE_FAST:
          upDelay = random(1375, 1560);
          break;
      }
      delay(upDelay);
      clickCount--;
    }
    else
    {
      state = STATE_DONE;
      changeLcdState();
    }
  }
  else if (state == STATE_DONE)
  {
    lcd.setCursor(0,0);
    lcd.print("Click Finish");
    delay(1000);
  }
}

void changeLcdState()
{
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
}

void resetPins()
{
  bRight = HIGH;
  bUp = HIGH;
  bLeft = HIGH;
  bDown = HIGH;
  bOk = HIGH;
}

byte localButtonRead()
{
  byte retValue = 100;
  byte pinVal;
  
  /* Go through the buttons to see which one has been read */
  pinVal = digitalRead(buttonRight);
  if (pinVal != bRight)
  {
    bRight = pinVal;
    retValue = buttonRight;
    return retValue;
  }
  pinVal = digitalRead(buttonLeft);
  if (pinVal != bLeft)
  {
    bLeft = pinVal;
    retValue = buttonLeft;
    return retValue;
  }
  pinVal = digitalRead(buttonUp);
  if (pinVal != bUp)
  {
    bUp = pinVal;
    retValue = buttonUp;
    return retValue;
  }
  pinVal = digitalRead(buttonDown);
  if (pinVal != bDown)
  {
    bDown = pinVal;
    retValue = buttonDown;
    return retValue;
  }
  pinVal = digitalRead(buttonOk);
  if (pinVal != bOk)
  {
    bOk = pinVal;
    retValue = buttonOk;
    return retValue;
  }
  return retValue;
}

void printDbg(int dbgIn)
{
  lcd.setCursor(15,1);
  if (dbgIn == 100)
  {
    lcd.print(" ");
  }
  else
  {
    lcd.print("P");
  }
  
  delay(50);
}

void rotateMode(int in)
{
  if (in == 0)
  {
    switch(current_mode)
    {
      case MODE_FAST:
        current_mode = MODE_SLOW;
        break;
      case MODE_NORMAL:
        current_mode = MODE_FAST;
        break;
      case MODE_SLOW:
        current_mode = MODE_NORMAL;
        break;
    }
  }
  else if (in == 1)
  {
    switch(current_mode)
    {
      case MODE_FAST:
        current_mode = MODE_NORMAL;
        break;
      case MODE_NORMAL:
        current_mode = MODE_SLOW;
        break;
      case MODE_SLOW:
        current_mode = MODE_FAST;
        break;
    } 
  }
}

void wrapUp(int pos)
{
  switch(pos)
  {
    case 1:
      digit1++;
      if (digit1 > 9)
      {
        digit1 = 0;
      }
      break;
    case 2:
      digit2++;
      if (digit2 > 9)
      {
        digit2 = 0;
      }
      break;
    case 3:
      digit3++;
      if (digit3 > 9)
      {
        digit3 = 0;
      }
      break;
    case 4:
      digit4++;
      if (digit4 > 9)
      {
        digit4 = 0;
      }
      break;
  }
}

void wrapDown(int pos)
{
  switch(pos)
  {
    case 1:
      digit1--;
      if (digit1 == 255)
      {
        digit1 = 9;
      }
      break;
    case 2:
      digit2--;
      if (digit2 == 255)
      {
        digit2 = 9;
      }
      break;
    case 3:
      digit3--;
      if (digit3 == 255)
      {
        digit3 = 9;
      }
      break;
    case 4:
      digit4--;
      if (digit4 == 255)
      {
        digit4 = 9;
      }
      break;
  }
}

byte wrapDigit(int pos, byte bIn)
{
  byte retVal = 0;
  
  switch(bIn)
  {
    case buttonUp:
      wrapUp(pos);
      break;
    case buttonDown:
      wrapDown(pos);
      break;
    case buttonOk:
    case buttonRight:
      retVal = 1;
      break;
    case buttonLeft:
      retVal = 2;
      break;
    default:
      retVal = 0;
  }
  
  return retVal;
}

byte selectMode(byte bIn)
{
  byte retVal = 0;
  
  switch(bIn)
  {
    case buttonUp:
      rotateMode(0);
      retVal = 0;
      lcd.setCursor(2,1);
      lcd.print("      ");
      printDelayMode();
      break;
    case buttonDown:
      rotateMode(1);
      retVal = 0;
      lcd.setCursor(2,1);
      lcd.print("      ");
      printDelayMode();
      break;
    case buttonOk:
      retVal = 1;
      break;
    default:
      retVal = 0;
  }
  
  return retVal;
}

void printDigit(int pos, byte d)
{
  lcd.setCursor((pos+1), 1);
  lcd.print(d);
}

void printDelayMode()
{
  lcd.setCursor(2,1);
  if (current_mode == MODE_FAST)
  {
    lcd.print("FAST");
  }
  else if (current_mode == MODE_NORMAL)
  {
    lcd.print("NORMAL");
  }
  else if (current_mode == MODE_SLOW)
  {
    lcd.print("SLOW");
  }
  else
  {
    lcd.print("????");
  }
}
