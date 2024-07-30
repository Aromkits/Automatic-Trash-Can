#define BLYNK_PRINT Serial 
#define BLYNK_TEMPLATE_ID "TMPLJ5BYJGxm" 
#define i2c_Address 0x3C 
#define SCREEN_WIDTH 128  
#define SCREEN_HEIGHT 64 
#define OLED_RESET -1  
#define LINE_TOKEN  "M6XAde8CxXFZ4W9AasLosvEehtClAAG1lFs9KQMnlzx" 

#include <ESP8266WiFi.h> 
#include <BlynkSimpleEsp8266.h> 

#include <Adafruit_SH110X.h> 
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); 

#include <TridentTD_LineNotify.h> 
#include "Adafruit_VL53L0X.h"  
Adafruit_VL53L0X laser = Adafruit_VL53L0X (); 

#include<Servo.h> 
Servo servo1;

char auth[] = "YM7t5xXhlsUi0FlG2gU-WghSL0OmD0CZ"; 
char ssid[] = "CHU_2.4G"; 
char pass[] = "Chu123456"; 

float height_Bin = 234; 
float trash = 0; 
int trigPin = D5;
int echoPin = D6; 
long distance; 
long duration;

bool alerted = false; 

void ultrasonic() 
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  Serial.println(String(distance) + " CM");
  
}

void setup()
{
  Serial.begin(115200);
  display.begin(i2c_Address, true);

  LINE.setToken(LINE_TOKEN);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  servo1.attach(D7); 

  if(!laser.begin ()) 
  {
    Serial.println(F("Failed to boot VL53L0X")); 
    while(1);
  }

  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); //เจอโมดูล
  laser.startRangeContinuous (); 

  Blynk.begin(auth, ssid, pass); 
}

void loop()
{
  Blynk.run(); 

  ultrasonic(); 
  display.clearDisplay(); 
  display.setTextColor(SH110X_WHITE); 
  display.setCursor(0, 0); 
  display.print("Bin = " + String(height_Bin)+" mm"); 
  display.setCursor(0, 16);
  display.print("Trash = "+String (trash) +" mm");

  if (distance <= 10) 
  {
    servo1.write(180);
    display.setCursor(0, 32);
    display.print("Status: Open");
  } 
  else 
  {
    servo1.write(15);
    display.setCursor(0, 32);
    display.print("Status: Close");
  }

  laser.startRangeContinuous();

  if(laser.isRangeComplete()) 
  {
    trash = laser.readRange();
    if(trash > 234)
    {
      trash = 0; 
    }
    else if(trash == 0) 
    {
      trash = 0;
    }
    else 
    {
      trash = height_Bin - trash;
      if(trash >= 160 && !alerted) 
      {
        LINE.notify("ถังขยะเต็ม");
        alerted = true; 
      }
      else if(trash < 160 && alerted) 
      {
        alerted = false; 
      }
    }
    Serial.println("trash: " + String(trash));
  }

  Blynk.virtualWrite(V0, trash); 
  display.display(); 
  delay(100);
}
