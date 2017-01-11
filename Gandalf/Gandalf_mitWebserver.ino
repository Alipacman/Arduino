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

/*************
Network stuff
**************/
#include <SPI.h>
#include <Ethernet.h>

//MAC Adress of the device
//TODO: Change to the one on the sticker
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};

IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet server library
// Port is 80 for default HTTP
EthernetServer server(80);

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
  
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
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
  
  //Server Code below:
  //*****vvvvvvv******
  // listen for incoming clients
  
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          //body
          client.println("<h1>Gandalf, the fridge.</h1>");
          // output the value of each analog input pin
          client.print("<h3>Aktuelle Temperatur: ");
          client.print(tempSmooth);
          client.println("</h3>");
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
  }
}





