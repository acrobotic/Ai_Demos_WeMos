#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DHT.h>
#define DHTTYPE DHT22
#define DHTPIN  2

const char* ssid     = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

ESP8266WebServer server(80);
DHT dht(DHTPIN, DHTTYPE, 11);
 
float humidity, temp_f;
String webString="";
unsigned long previousMillis = 0;
const long interval = 2000;
 
void setup(void)
{
  Serial.begin(115200);
  dht.begin();

  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rWorking to connect");

  while (WiFi.status() != WL_CONNECTED)
	{
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("DHT Data Plotter");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
   
  server.on("/", handle_root);
  server.on("/temp", handle_temp);
  server.on("/humidity", handle_hum);
  server.on("/", []()
  {
    server.send(200, "text/html", webpage);
  });
  
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void)
{
  server.handleClient();
} 

void handle_root()
{
  server.send(200, "text/plain", "Use /temp or /humidity paths to visualize the data!");
}

void handle_temp()
{
  getdata();
  webString="Temperature: "+String((int)temp_f)+" F";
  server.send(200, "text/plain", webString);
}
 
void handle_hum()
{
  getdata();
  webString="Humidity: "+String((int)humidity)+"%";
  server.send(200, "text/plain", webString);
}

void getdata()
{
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis >= interval)
	{
    previousMillis = currentMillis;   

    humidity = dht.readHumidity();          // Read RH (%)
    temp_f = dht.readTemperature(true);     // Read deg F
    if (isnan(humidity) || isnan(temp_f))
		{
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
  }
}

const char webpage[] PROGMEM = R"=====(
<html>
  <head>
    <title>Interactive Line Graph</title>
    <script src="http://ajax.aspnetcdn.com/ajax/jQuery/jquery-1.6.1.min.js"></script>
    <script>
      var graph;
      var xPadding = 30;
      var yPadding = 30;
      
      var data = { values:[
        { X: "Jan", Y: 12 },
        { X: "Feb", Y: 28 },
        { X: "Mar", Y: 18 },
        { X: "Apr", Y: 34 },
        { X: "May", Y: 40 },
      ]};

      // Returns the max Y value in our data list
      function getMaxY() {
        var max = 0;
        
        for(var i = 0; i < data.values.length; i ++) {
          if(data.values[i].Y > max) {
            max = data.values[i].Y;
          }
        }
        
        max += 10 - max % 10;
        return max;
      }
      
      // Return the x pixel for a graph point
      function getXPixel(val) {
        return ((graph.width() - xPadding) / data.values.length) * val + (xPadding * 1.5);
      }
      
      // Return the y pixel for a graph point
      function getYPixel(val) {
        return graph.height() - (((graph.height() - yPadding) / getMaxY()) * val) - yPadding;
      }

      $(document).ready(function() {
        graph = $('#graph');
        var c = graph[0].getContext('2d');      
        
        c.lineWidth = 2;
        c.strokeStyle = '#333';
        c.font = 'italic 8pt sans-serif';
        c.textAlign = "center";
        
        // Draw the axises
        c.beginPath();
        c.moveTo(xPadding, 0);
        c.lineTo(xPadding, graph.height() - yPadding);
        c.lineTo(graph.width(), graph.height() - yPadding);
        c.stroke();
        
        // Draw the X value texts
        for(var i = 0; i < data.values.length; i ++) {
          c.fillText(data.values[i].X, getXPixel(i), graph.height() - yPadding + 20);
        }
        
        // Draw the Y value texts
        c.textAlign = "right"
        c.textBaseline = "middle";
        
        for(var i = 0; i < getMaxY(); i += 10) {
          c.fillText(i, xPadding - 10, getYPixel(i));
        }
        
        c.strokeStyle = '#f00';
        
        // Draw the line graph
        c.beginPath();
        c.moveTo(getXPixel(0), getYPixel(data.values[0].Y));
        for(var i = 1; i < data.values.length; i ++) {
          c.lineTo(getXPixel(i), getYPixel(data.values[i].Y));
        }
        c.stroke();
        
        // Draw the dots
        c.fillStyle = '#333';
        
        for(var i = 0; i < data.values.length; i ++) {  
          c.beginPath();
          c.arc(getXPixel(i), getYPixel(data.values[i].Y), 4, 0, Math.PI * 2, true);
          c.fill();
        }
      });
    </script>
  </head>
  <body>
    <canvas id="graph" width="200" height="150">   
    </canvas> 
  </body>
</html>
)=====";
