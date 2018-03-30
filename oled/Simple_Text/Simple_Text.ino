/*------------------------------------------------------------------------------
  03/01/2018
  Author: Makerbro
  Platforms: ESP826 (WeMos)
  Language: C++/Arduino
  File: simple_text.ino
  ------------------------------------------------------------------------------
  Description: 
  Code for YouTube video demonstrating how to use the WeMos D1 Mini OLED Shield
  to display the WeMos D1 Mini's IP address as a string:
  https://youtube.com/acrobotic
  ------------------------------------------------------------------------------
  Please consider buying products from ACROBOTIC to help fund future
  Open-Source projects like this! We'll always put our best effort in every
  project, and release all our design files and code for you to use. 
  https://acrobotic.com/
  ------------------------------------------------------------------------------
  License:
  Please see attached LICENSE.txt file for details.
------------------------------------------------------------------------------*/
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <SFE_MicroOLED.h>  // Include the SFE_MicroOLED library

const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

#define PIN_RESET 255  //
#define DC_JUMPER 0  // I2C Addres: 0 - 0x3C, 1 - 0x3D

MicroOLED oled(PIN_RESET, DC_JUMPER);  // I2C

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);

  // Before you can start using the OLED, call begin() to init
  // all of the pins and configure the OLED.
  oled.begin();
  // clear(ALL) will clear out the OLED's graphic memory.
  // clear(PAGE) will clear the Arduino's display buffer.
  oled.clear(ALL);  // Clear the display's memory (gets rid of artifacts)
  // To actually draw anything on the display, you must call the
  // display() function. 
  oled.display();   
  
  //oled.clear(ALL);
  //oled.display(); // display the memory buffer drawn
  
  delay(10);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  displayIP();
  delay(10000);
}

// Test string to display
void displayIP()
{
  String ip_address = WiFi.localIP().toString();
  printText(ip_address, 0);
}

void printText(String title, int font)
{ 
  oled.clear(PAGE);
  oled.setFontType(font);
  oled.setCursor(0,0);
  oled.print(title);
  oled.display();
  delay(1500);
  oled.clear(PAGE);
}
