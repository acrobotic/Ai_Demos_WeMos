#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server;
uint8_t pin_relay = D1;
char* ssid = "YOUR_SSID";
char* password = "YOUR_PASSWORD";

void setup()
{
  pinMode(pin_relay, OUTPUT);
  WiFi.begin(ssid,password);
  Serial.begin(115200);
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());

  server.on("/",[](){server.send(200,"text/plain","Hello Relay! Navigate to /toggle-relay to turn the Relay On/Off.");});
  server.on("/toggle-relay",toggleRelay);
  server.begin();
}

void loop()
{
  server.handleClient();
}

void toggleRelay()
{
  uint8_t new_state = !digitalRead(pin_relay);
  String new_state_str = (new_state?"ON":"OFF");
  digitalWrite(pin_relay,new_state);
  server.send(200,"text/plain", "Relay is now "+new_state_str+"!");
}
