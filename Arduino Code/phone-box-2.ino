#include <Time.h> 
#include <TimeLib.h> 
 
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>

 
#include <EEPROM.h> 
#include <Servo.h> 
 
//LiquidCrystal lcd(12,  11, 5, 4, 3, 2); 
LiquidCrystal_I2C lcd(0x27, 16, 2);

 
Servo myServo; 
 
const int BackButtonPin = 2; 
const int StartButtonPin = 5; 
const int MinusButtonPin = 9; 
const int PlusButtonPin  = 12; 
 
int settingHours = 0;    
int settingMinutes = 0;   
int settingSeconds  = 0;   
time_t settingTime = 0; 
 
int activeHours = 0; 
int activeMinutes  = 0; 
int activeSeconds = 0; 
time_t activeTime = 0;  
 
 
time_t  startTime = 0; 
time_t elapsedTime = 0; 
 
int BackButtonState = LOW; 
//long  BackButtonLongPressTime = 0; 
int StartButtonState = LOW; 
int PlusButtonState  = LOW; 
int MinusButtonState = LOW; 
int BackButtonPrevState = LOW; 
int  StartButtonPrevState = LOW; 
int PlusButtonPrevState = LOW; 
int MinusButtonPrevState  = LOW; 
bool BackButtonPress = false; 
//bool BackButtonLongPress = false; 
bool StartButtonPress = false; 
bool PlusButtonPress = false; 
bool MinusButtonPress  = false; 
 
bool locked = false; 
 
const int READY = 0; 
const int  SETTINGS = 1; 
const int LOCKED = 2; 
const int UNLOCKED = 3; 
 
int  activeState = READY;     
  
 
int timeData = 0;   
 
void setup()  { 
  lcd.init(); // initialize the lcd
  lcd.backlight(); 
  lcd.begin(16, 2);
  pinMode(BackButtonPin, INPUT); 
  pinMode(StartButtonPin,  INPUT); 
  pinMode(PlusButtonPin, INPUT); 
  pinMode(MinusButtonPin, INPUT);  
  myServo.attach(A0); 
  Serial.begin(9600); 
  myServo.write(0); 
  Reset();
  
} 
 
void loop() { 
   
  StartButtonPress = false; 
  PlusButtonPress  = false; 
  MinusButtonPress = false; 
 
  BackButtonPress = false; 
  //BackButtonLongPress = false; 
  BackButtonState = digitalRead(BackButtonPin);
  
  //
  // if(BackButtonState == HIGH){
  //   Serial.print("BackButton Pressed");
  // }
  //
  if(BackButtonState != BackButtonPrevState) 
  { 
    BackButtonPress  = BackButtonState == HIGH; 
    BackButtonPrevState = BackButtonState;
    delay(250);  
  } 
 
  StartButtonPress = false; 
  StartButtonState = digitalRead(StartButtonPin);
  //
  // if(StartButtonState == HIGH){
  //   Serial.println("StartButtonState Pressed");
  // }
  //
  if(StartButtonState != StartButtonPrevState) 
  { 
    StartButtonPress  = StartButtonState == HIGH; 
    StartButtonPrevState = StartButtonState;
    delay(100);  
  }  
 
  MinusButtonPress = false; 
  MinusButtonState = digitalRead(MinusButtonPin);
  //
  if(MinusButtonState == HIGH){
    Serial.println("MinusButtonState Pressed");
  }
  //
  if(MinusButtonState != MinusButtonPrevState) 
  { 
    MinusButtonPress  = MinusButtonState == HIGH; 
    MinusButtonPrevState = MinusButtonState;
    delay(100);  
  } 
 
  PlusButtonPress = false; 
  PlusButtonState = digitalRead(PlusButtonPin);
  
  //
  if(PlusButtonState == HIGH){
    Serial.println("PlusButtonState Pressed");
  }
  //

  if(PlusButtonState != PlusButtonPrevState) 
  { 
    PlusButtonPress  = PlusButtonState == HIGH; 
    PlusButtonPrevState = PlusButtonState;
    delay(100);   
  }  
 
  switch(activeState) 
  { 
    case READY: 
    myServo.write(0);  
      if(BackButtonPress) 
      { 
        Reset();
        activeState = SETTINGS;  
      } 
      if(StartButtonPress) 
      { 
        activeState =  activeState == READY ? LOCKED : READY; 
        if(activeState == LOCKED) 
        { 
          startTime = now(); 
        } 
      } 
      break;  
 
    case SETTINGS: 
    myServo.write(0); 
      if(BackButtonPress)  
      { 
        settingTime = settingSeconds + (60 * settingMinutes) + (3600  * settingHours); 
        activeHours = settingHours; 
        activeMinutes  = settingMinutes; 
        activeSeconds = settingSeconds; 
        timeData  = 0; 
        activeState = READY; 
      } 
      if(StartButtonPress)  
      { 
        timeData++; 
        if(timeData == 3) 
        {  
          timeData = 0; 
 
        } 
      } 
      if(MinusButtonPress)  
      { 
        switch(timeData) 
        { 
          case 0:  
            settingHours--; 
            if(settingHours == -1) 
            {  
              settingHours = 99; 
            } 
            break; 
          case 1:  
            settingMinutes--; 
            if(settingMinutes  == -1) 
            { 
              settingMinutes = 59; 
            }  
            break; 
          case 2:  
            settingSeconds--;  
            if(settingSeconds == -1) 
            { 
              settingSeconds  = 59; 
            } 
            break; 
        }  
 
      } 
      if(PlusButtonPress) 
      { 
        switch(timeData) 
        {  
          case 0:  
            settingHours++; 
            if(settingHours  == 100) 
            { 
              settingHours = 0; 
            }  
            break; 
          case 1:  
            settingMinutes++;  
            if(settingMinutes == 60) 
            { 
              settingMinutes  = 0; 
            } 
            break; 
          case 2:  
            settingSeconds++;  
            if(settingSeconds == 60) 
            { 
              settingSeconds  = 0; 
            } 
            break; 
        } 
      } 
 
      break; 
     
    case LOCKED: 
    myServo.write(90); 
      if(StartButtonPress)  
      { 
        activeState = READY; 
      } 
      if(BackButtonPress)  
      { 
        activeState = READY; 
      } 
      break; 
 
    case UNLOCKED: 
    myServo.write(0); 
      if(BackButtonPress  || StartButtonPress || MinusButtonPress || PlusButtonPress) 
      { 
        activeState  = READY; 
      } 
      break; 
  } 
 
  switch(activeState) 
  { 
    case READY: 
    case SETTINGS:  
 
      break; 
    case  LOCKED: 
      activeTime = settingTime - (now() - startTime); 
      if(activeTime  <= 0) 
      { 
        activeState = UNLOCKED; 
      } 
      break;  
  } 
 
  lcd.setCursor(0, 0); 
  switch(activeState) 
  { 
    case  READY: 
      lcd.print("Ready: Start    "); 
      lcd.setCursor(0, 1);  
      lcd.print(activeHours); 
      lcd.print("  "); 
      lcd.print(activeMinutes);  
      lcd.print("  "); 
      lcd.print(activeSeconds); 
      lcd.print("     "); 
      break; 
    case SETTINGS: 
      lcd.print("Set Timer:  "); 
      switch(timeData) 
      { 
        case 0: 
 
          lcd.print("HRS  "); 
          break; 
        case 1: 
          lcd.print("MINS");  
          break; 
        case 2: 
          lcd.print("SECS"); 
          break; 
      } 
      lcd.setCursor(0, 1); 
      lcd.print(settingHours);  
      lcd.print("  "); 
      lcd.print(settingMinutes); 
      lcd.print("  "); 
      lcd.print(settingSeconds); 
      lcd.print("     "); 
      break; 
    case LOCKED: 
      lcd.print("Time Left...       "); 
      lcd.setCursor(0, 1); 
      if(hour(activeTime) < 10) lcd.print("0");  
      lcd.print(hour(activeTime)); 
      lcd.print(": "); 
      if(minute(activeTime)  < 10) lcd.print("0"); 
      lcd.print(minute(activeTime)); 
      lcd.print(":  "); 
      if(second(activeTime) < 10) lcd.print("0"); 
      lcd.print(second(activeTime));   
      break; 
    case UNLOCKED: 
      lcd.print("   UNLOCKED!   "); 
      lcd.setCursor(0, 1); 
      lcd.print("              ");  
      break; 
  } 
  delay(10); 
} 
 
void Reset() 
{ 
  activeState = READY; 
  activeHours = settingHours; 
  activeMinutes = settingMinutes;  
  activeSeconds = settingSeconds; 
}
    