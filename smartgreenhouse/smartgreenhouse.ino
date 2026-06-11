#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

// =====================
// OLED SSD1306
// =====================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// =====================
// DHT22
// =====================

#define DHTPIN 41
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);

// =====================
// CAPTEURS
// =====================

#define SOIL_PIN 42
#define LDR_PIN 40

// =====================
// RELAIS
// =====================

#define RELAY_PUMP 35
#define RELAY_FAN 36
#define RELAY_LIGHT 37
#define RELAY_RESERVE 38

// =====================
// I2C OLED
// =====================

#define SDA_PIN 1
#define SCL_PIN 2

// =====================
// WIFI
// =====================

const char* ssid = ".";
const char* password = "houssam200";

// IP PC
String serverName =
"http://192.168.43.7:8000/esp32/data";

// =====================

unsigned long lastSend = 0;
unsigned long lastScreen = 0;

bool page = false;

// =====================

void setup()
{
  Serial.begin(115200);

  pinMode(RELAY_PUMP, OUTPUT);
  pinMode(RELAY_FAN, OUTPUT);
  pinMode(RELAY_LIGHT, OUTPUT);
  pinMode(RELAY_RESERVE, OUTPUT);

  digitalWrite(RELAY_PUMP, HIGH);
  digitalWrite(RELAY_FAN, HIGH);
  digitalWrite(RELAY_LIGHT, HIGH);
  digitalWrite(RELAY_RESERVE, HIGH);

  Wire.begin(SDA_PIN, SCL_PIN);

  if(!display.begin(
      SSD1306_SWITCHCAPVCC,
      0x3C))
  {
      Serial.println("OLED ERROR");
      while(true);
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);

  display.setCursor(15,15);
  display.println("SMART GREEN");

  display.setCursor(20,35);
  display.println("HOUSE AI");

  display.display();

  dht.begin();

  delay(2000);

  WiFi.begin(ssid,password);

  display.clearDisplay();
  display.setCursor(0,20);
  display.println("Connecting WiFi");
  display.display();

  while(WiFi.status()!=WL_CONNECTED)
  {
      delay(500);
      Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Connected");

  Serial.print("IP : ");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0,20);
  display.println("WiFi Connected");
  display.display();

  delay(1500);
}

// =====================

void loop()
{
  float temperature =
      dht.readTemperature();

  float humidity =
      dht.readHumidity();

  if(isnan(temperature) ||
     isnan(humidity))
  {
      temperature = 28.5;
      humidity = 65.0;
  }

  int soilRaw =
      analogRead(SOIL_PIN);

  int ldrRaw =
      analogRead(LDR_PIN);

  int soilPercent =
      map(
          soilRaw,
          4095,
          0,
          0,
          100
      );

  soilPercent =
      constrain(
          soilPercent,
          0,
          100
      );

  int lightPercent =
      map(
          ldrRaw,
          4095,
          0,
          0,
          100
      );

  lightPercent =
      constrain(
          lightPercent,
          0,
          100
      );

  // =====================
  // RELAIS
  // =====================

  if(soilPercent < 30)
      digitalWrite(RELAY_PUMP,LOW);
  else
      digitalWrite(RELAY_PUMP,HIGH);

  if(temperature > 30)
      digitalWrite(RELAY_FAN,LOW);
  else
      digitalWrite(RELAY_FAN,HIGH);

  if(lightPercent < 40)
      digitalWrite(RELAY_LIGHT,LOW);
  else
      digitalWrite(RELAY_LIGHT,HIGH);

  // =====================
  // OLED
  // =====================

  if(millis()-lastScreen > 3000)
  {
      lastScreen = millis();

      page = !page;

      display.clearDisplay();

      if(page)
      {
          display.setTextSize(2);

          display.setCursor(0,0);
          display.print("T:");
          display.print(
              temperature,
              1
          );

          display.setCursor(0,30);
          display.print("H:");
          display.print(
              humidity,
              0
          );
          display.print("%");
      }
      else
      {
          display.setTextSize(2);

          display.setCursor(0,0);
          display.print("S:");
          display.print(
              soilPercent
          );
          display.print("%");

          display.setCursor(0,30);
          display.print("L:");
          display.print(
              lightPercent
          );
          display.print("%");
      }

      display.display();
  }

  // =====================
  // FASTAPI
  // =====================

  if(millis()-lastSend > 5000)
  {
      lastSend = millis();

      if(WiFi.status()==WL_CONNECTED)
      {
          HTTPClient http;

          http.begin(serverName);

          http.addHeader(
              "Content-Type",
              "application/json"
          );

          String jsonData = "{";

          jsonData +=
          "\"temperature\":";
          jsonData +=
          String(temperature);

          jsonData +=
          ",\"humidity\":";
          jsonData +=
          String(humidity);

          jsonData +=
          ",\"soil\":";
          jsonData +=
          String(soilPercent);

          jsonData +=
          ",\"light\":";
          jsonData +=
          String(lightPercent);

          jsonData += "}";

          int response =
              http.POST(
                  jsonData
              );

          Serial.print(
              "HTTP : "
          );
          Serial.println(
              response
          );

          http.end();
      }
  }
}