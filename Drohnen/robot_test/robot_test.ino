#include <avr/interrupt.h>
#include <avr/io.h>

int speed = 190;
int leftSignal = 0;
int rightSignal = 1;
int rightBack = 3;
int rightForeward = 5;
int leftForeward = 9;
int leftBack = 10;

//for speed measurement
int leftSpeed = 0;
int rightSpeed = 0;
int leftCounter = 0;
int rightCounter = 0;
boolean leftZero = true;
boolean rightZero = true;

long lastUpdate;

void setup() {
  // put your setup code here, to run once:
  //Set baudrate to 9600
  Serial.begin(9600);
  //set pin I/O
  pinMode(0, INPUT);
  pinMode(1, INPUT);
  //pinMode(2, INPUT);	//encoder right
  //pinMode(3, INPUT);	//encoder left
  pinMode(5,OUTPUT);	//right motor foreward
  pinMode(rightBack,OUTPUT);	//right motor backward
  pinMode(7,OUTPUT);	//red line sensor
  pinMode(9, OUTPUT);	//left motor foreward
  pinMode(leftBack, OUTPUT);	//left motor backward
  pinMode(8, OUTPUT);   //statusled
  pinMode(A2, INPUT);	//battery sensor
  pinMode(A6, INPUT);	//linesensor left
  pinMode(A7, INPUT);	//linesensor right
  
  analogWrite(rightForeward,speed);
  analogWrite(leftForeward,speed);
  
  lastUpdate = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.print(analogRead(0));
  //Serial.print(",");
  
  if (millis() - lastUpdate > 100) {
    if (leftCounter > rightCounter) {
      //links langsamer
    } else {
      //rechts langsamer
    }
    Serial.println("left: " + String(leftCounter));
    Serial.println("right: " + String(rightCounter));
    leftCounter = 0;
    rightCounter = 0;
    lastUpdate = millis();
  }
  
  if (analogRead(1) < 400) {
    rightZero = true;
  } else if (rightZero) {
    rightCounter++;
    rightZero = false;
  }
  if (analogRead(0) < 400) {
    leftZero = true;
  } else if (leftZero) {
    leftCounter++;
    leftZero = false;
  }
}
