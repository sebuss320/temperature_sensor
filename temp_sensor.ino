
 /*
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 * 
 */

#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>

//zapis do pamieci
#include <EEPROM.h>

// initialize the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// the 8 arrays that form each segment of the custom numbers
byte LT[8] = {B00111, B01111, B11111, B11111, B11111, B11111, B11111, B11111};
byte UB[8] = {B11111, B11111, B11111, B00000, B00000, B00000, B00000, B00000};
byte RT[8] = {B11100, B11110, B11111, B11111, B11111, B11111, B11111, B11111};
byte LL[8] = {B11111, B11111, B11111, B11111, B11111, B11111, B01111, B00111};
byte LB[8] = {B00000, B00000, B00000, B00000, B00000, B11111, B11111, B11111};
byte LR[8] = {B11111, B11111, B11111, B11111, B11111, B11111, B11110, B11100};
byte UMB[8] = {B11111, B11111, B11111, B00000, B00000, B00000, B11111, B11111};
byte LMB[8] = {B11111, B00000, B00000, B00000, B00000, B11111, B11111, B11111};
byte CC[8] = {B00000, B00000, B00000, B00000, B00000, B01110, B01110, B01110};
int x = 0;

void custom0()
{
  lcd.setCursor(x, 0); lcd.write((byte)0); lcd.write(1); lcd.write(2); lcd.setCursor(x, 1); lcd.write(3); lcd.write(4); lcd.write(5);
}

void custom1()
{
  lcd.setCursor(x,0); lcd.write(1); lcd.write(2); lcd.setCursor(x+1,1); lcd.write(5);
}

void custom2()
{
  lcd.setCursor(x,0); lcd.write(6); lcd.write(6); lcd.write(2); lcd.setCursor(x, 1); lcd.write(3); lcd.write(7); lcd.write(7);
}

void custom3()
{
  lcd.setCursor(x,0); lcd.write(6); lcd.write(6); lcd.write(2); lcd.setCursor(x, 1); lcd.write(7); lcd.write(7); lcd.write(5); 
}

void custom4()
{
  lcd.setCursor(x,0); lcd.write(3); lcd.write(4); lcd.write(2); lcd.setCursor(x+2, 1); lcd.write(5);
}

void custom5()
{
  lcd.setCursor(x,0); lcd.write((byte)0); lcd.write(6); lcd.write(6); lcd.setCursor(x, 1); lcd.write(7); lcd.write(7); lcd.write(5);
}

void custom6()
{
  lcd.setCursor(x,0); lcd.write((byte)0); lcd.write(6); lcd.write(6); lcd.setCursor(x, 1); lcd.write(3); lcd.write(7); lcd.write(5);
}

void custom7()
{
  lcd.setCursor(x,0); lcd.write(1); lcd.write(1); lcd.write(2); lcd.setCursor(x+1, 1); lcd.write((byte)0);
}

void custom8()
{
  lcd.setCursor(x,0); lcd.write((byte)0); lcd.write(6); lcd.write(2); lcd.setCursor(x, 1); lcd.write(3); lcd.write(7); lcd.write(5);
}

void custom9()
{
  lcd.setCursor(x,0); lcd.write((byte)0); lcd.write(6); lcd.write(2); lcd.setCursor(x+1, 1); lcd.write(4); lcd.write(5);
}
void customChar()
{
  lcd.setCursor(7,0); lcd.write(4);  lcd.write(4); lcd.setCursor(7,1); lcd.write(1); lcd.write(1);
}



void wirteLCD(int var)
{
  switch (var) {
    case 0:
        custom0();
      break;
    case 1:
        custom1();
      break;
    case 2:
        custom2();
      break;
    case 3:
        custom3();
      break;
    case 4:
        custom4();
      break;
    case 5:
        custom5();
      break;
    case 6:
        custom6();
      break;
    case 7:
        custom7();
      break;
    case 8:
        custom8();
      break;
    case 9:
        custom9();
      break;
  }
}

float temperature = 0;
float prevTemp = 0;
float tmax = 30;
unsigned long diodaTime = 0;
unsigned long lcdTime = 0;
bool trigger = false;
bool changeHour = true;
bool firstTime = true;

// dołączam przycisk
bool buttonStatusUP;
bool buttonStatusDOWN;
int buttonPinUP = 1;
int buttonPinDOWN = 0;
unsigned long timer = 0;

//zapis do pamieci
int addr = 0;

//cyfry na wyswietlaczu
int first = 0;
int second = 0;
int third = 0;
int fourth = 0;

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 8

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

void setup() 
{
  // assignes each segment a write number
  lcd.createChar(0,LT);
  lcd.createChar(1,UB);
  lcd.createChar(2,RT);
  lcd.createChar(3,LL);
  lcd.createChar(4,LB);
  lcd.createChar(5,LR);
  lcd.createChar(6,UMB);
  lcd.createChar(7,LMB);
  
  // sets the LCD's rows and colums:
  lcd.begin(16, 2);     

  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2); 
  pinMode(9, OUTPUT);

  //Dodaje przycisk
  pinMode(buttonPinUP, INPUT);
  pinMode(buttonPinDOWN, INPUT);

  //oczyt z pamieci
  tmax = EEPROM.read(addr);
}

bool isEqual(float var, float prev)
{
  return (var == prev);
}

void loop() 
{
  sensors.requestTemperatures(); // Send the command to get temperatures  
  temperature = sensors.getTempCByIndex(0);

  if(changeHour)
  {
    first = ( (String) (temperature * 100) ).substring(0, 1).toInt();
    second = ( (String) (temperature * 100) ).substring(1, 2).toInt();
    third = ( (String) (temperature * 100) ).substring(2, 3).toInt();
    fourth = ( (String) (temperature * 100) ).substring(3, 4).toInt();
  }

  if(((millis() - lcdTime > 1000 || firstTime) && !isEqual(temperature,prevTemp)) || !changeHour)
  {
   lcd.clear();  // clears the display 
   customChar();
   x=0;
   wirteLCD(first);  
   x = x + 4;   
   wirteLCD(second); 
   x = x + 5;
   wirteLCD(third); 
   x = x + 4;
   wirteLCD(fourth); 
   x=0;
   lcdTime = millis();  
   firstTime = false;
   prevTemp = temperature;
   if(!changeHour)
   {
      changeHour = true;
      prevTemp = prevTemp + 1;
   }

  }

// delay(4000);  // delay 4 seconds

  
    if (temperature > tmax)
    {
      if(millis() - diodaTime > 500 && !trigger)
      {
        // wlaczenie diody
        digitalWrite(9, HIGH);
        diodaTime = millis();
        trigger = true;
      }
      
      if(millis() - diodaTime > 500 && trigger)
      {
        trigger = false;
        // wylaczenie diody
        digitalWrite(9, LOW);
        diodaTime = millis();
      }
    }
    else
    {
      digitalWrite(9, LOW);
      diodaTime = millis();
    }
    
  buttonStatusUP = digitalRead(buttonPinUP);
  buttonStatusDOWN = digitalRead(buttonPinDOWN);
  
  //dodawanie w góre
  if(buttonStatusUP == false)
  {
    if(millis() - timer > 200){
      timer = millis();
      tmax = tmax + 1;
    }
    
    first = ( (String) (tmax * 100) ).substring(0, 1).toInt();
    second = ( (String) (tmax * 100) ).substring(1, 2).toInt();
    third = ( (String) (tmax * 100) ).substring(2, 3).toInt();
    fourth = ( (String) (tmax * 100) ).substring(3, 4).toInt();
    changeHour = false;
    
  }

  //dodawanie w dół
    if(buttonStatusDOWN == false)
  {
    if(millis() - timer > 200){
      timer = millis();
      tmax = tmax - 1;
    }

    first = ( (String) (tmax * 100) ).substring(0, 1).toInt();
    second = ( (String) (tmax * 100) ).substring(1, 2).toInt();
    third = ( (String) (tmax * 100) ).substring(2, 3).toInt();
    fourth = ( (String) (tmax * 100) ).substring(3, 4).toInt();
    changeHour = false;
    
  }


  //zapis do pamieci
  EEPROM.write(addr,tmax);
}
