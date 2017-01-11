/////////////////////////////////////////////
//                                         //
//          Gandalf, the fridge            //
//                                         //
/////////////////////////////////////////////

// Arduino AG, University of Hamburg, 2016
// Version 0.1


#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F,16, 2);   // If not working, try 0x27

#include <OneWire.h>
#include <DallasTemperature.h>

const unsigned char One_Wire_Bus = 8;
 
OneWire oneWire(One_Wire_Bus);
DallasTemperature sensors(&oneWire);

DeviceAddress sensor1 = { 0x28, 0xFF, 0x9B, 0x78, 0x84, 0x16, 0x04, 0xE0 }; 
DeviceAddress sensor2 = { 0x28, 0xFF, 0x96, 0xCE, 0x81, 0x16, 0x03, 0x22 };

const int temp1 = 0;                
const int temp2 = 1;                
const int compressor = 13;          // select the pin for the LED
const int tempreg = A0;             // select the input pin for the potentiometer

byte degree[8] = 
{
  0b00110,
  0b01001,
  0b01001,
  0b00110,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

float tempValue1 = 0;                 // 
float tempValue2 = 0;                 // 
float tempSmooth = 0;                 // variable to store the value coming from the sensor
float tempTarget = 22;
int coolState = 0;

int sensorNumber = 0;

long millisLast = 0;
long millisDebounce = 3000;

void setup() {
  pinMode(compressor, OUTPUT);
  pinMode(temp1, INPUT_PULLUP);
  pinMode(temp2, INPUT_PULLUP);
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, degree);
  sensors.begin();
  sensorNumber = sensors.getDeviceCount();
}


void loop() {
  tempTarget = analogRead(tempreg) / 56;
  
  sensors.requestTemperatures();
  tempValue1 = sensors.getTempC(sensor1);
  tempValue2 = sensors.getTempC(sensor2);
  tempSmooth = tempSmooth * 0.7 + tempValue1 * 0.15 + tempValue2 * 0.15;

  if(tempSmooth > tempTarget + 1)
  {
    if(millis() - millisLast > millisDebounce)
    {
      coolState = 1;
    }
  }
  else
  {
    millisLast = millis();
  }

  if(coolState == 1)
  {
    if(tempSmooth > tempTarget + 0.1)
    {
      digitalWrite(compressor, HIGH);
    }
    else
    {
      digitalWrite(compressor, LOW);
      coolState = 0;
    }
  }

  lcd.setCursor(0,0);
  lcd.print("Ziel: ");
  lcd.print(tempTarget);
  lcd.write(0);
  lcd.print("C  ");
  lcd.setCursor(0,1);
  lcd.print("Temp: ");
  lcd.print(tempSmooth);
  lcd.write(0);
  lcd.print("C  ");
 
  Serial.println(tempSmooth);
  delay(100);
}





