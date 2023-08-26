#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include "DHT.h"

#define DHTTYPE DHT11

const char* ssid = "Panha";
const char* password = "09070004";
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

ESP8266WebServer server(80);

#define DHTPin 2
DHT dht(DHTPin, DHTTYPE);

float t;
float h;

void setup() {
  Serial.begin(115200);
  delay(100);

  pinMode(DHTPin, INPUT);
  dht.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  // Read temperature and humidity from the sensor
  t = dht.readTemperature();
  h = dht.readHumidity();

  // Display temperature and humidity on OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Temperature: ");
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.print(t);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(2);
  display.print("C");

  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print("Humidity: ");
  display.setTextSize(2);
  display.setCursor(0, 45);
  display.print(h);
  display.print(" %");

  display.display();
}

void handle_OnConnect() {
  server.send(200, "text/html", SendHTML());
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not Found");
}

String SendHTML()
{
  
  // Create a HTML string for the web page
  String ptr = "<!DOCTYPE html><html>\n";
ptr += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
ptr += "<style>body {font-family: Arial;text-align: center;margin: 0;}\n";
ptr += "h1 {color: white;background-color: #2196F3;padding: 20px;}\n";
ptr += "p {font-size: 20px;}\n";
ptr += ".card {box-shadow: 0 4px 8px 0 rgba(0,0,0,0.2);transition: 0.3s;width: 40%;margin: auto;margin-top: 20px;}\n";
ptr += ".card:hover {box-shadow: 0 8px 16px 0 rgba(0,0,0,0.2);}\n";
ptr += ".container {padding: 16px;}\n";
ptr += ".icon {height: 100px;width: 100px;margin: auto;}\n";
ptr += ".temp {color: #FF5722;}\n";
ptr += ".hum {color: #009688;}\n";
ptr += "</style>\n";
ptr += "</head>\n";
ptr += "<body>\n";
ptr += "<h1>ESP8266 Web Server</h1>\n";

ptr += "<!-- Temperature card -->\n";
ptr += "<div class=\"card\">\n";
ptr += "<img src=\"https://cdn-icons-png.flaticon.com/512/1146/1146869.png\" alt=\"Temperature\" class=\"icon\">\n";
ptr += "<div class=\"container\">\n";
ptr += "<p class=\"temp\">Temperature: " + String(t) + " °C</p>\n";
ptr += "</div>\n";
ptr += "</div>\n";

ptr += "<!-- Humidity card -->\n";
ptr += "<div class=\"card\">\n";
ptr += "<img src=\"https://cdn-icons-png.flaticon.com/512/8923/8923690.png\" alt=\"Humidity\" class=\"icon\">\n";
ptr += "<div class=\"container\">\n";
ptr += "<p class=\"hum\">Humidity: " + String(h) + " %</p>\n";
ptr += "</div>\n";
ptr += "</div>\n";

ptr += "</body>\n";
ptr += "</html>";
return ptr;
}
