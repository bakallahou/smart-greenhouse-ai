#include <WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "esp_camera.h"
#include "DHT.h"

// --- CONFIGURATION DES BROCHES (CÂBLAGE RECOMMANDÉ ET VALIDÉ) ---
#define SOIL_PIN 42
#define LDR_PIN 40
#define DHTPIN 41
#define DHTTYPE DHT22

#define RELAY_PUMP 35
#define RELAY_FAN 36
#define RELAY_LIGHT 37
#define RELAY_RESERVE 38

#define I2C_SDA 1
#define I2C_SCL 2

// --- CONFIGURATION PINS CAMÉRA (ESP32S3_EYE) ---
#define XCLK_GPIO_NUM     15
#define SIOD_GPIO_NUM     4
#define SIOC_GPIO_NUM     5
#define Y2_GPIO_NUM       11
#define Y3_GPIO_NUM       9
#define Y4_GPIO_NUM       8
#define Y5_GPIO_NUM       10
#define Y6_GPIO_NUM       12
#define Y7_GPIO_NUM       18
#define Y8_GPIO_NUM       17
#define Y9_GPIO_NUM       16
#define VSYNC_GPIO_NUM    6
#define HREF_GPIO_NUM     7
#define PCLK_GPIO_NUM     13

// --- PARAMÈTRES RÉSEAU ET BACKEND ---
const char* ssid = "Redmi Note 7";
const char* password = "simolhaou4";

const char* serverIP = "192.168.43.82";
const int serverPort = 8000;

// --- TEMPORISATIONS (Non-bloquantes via millis) ---
unsigned long lastDataTime = 0;
const unsigned long dataInterval = 5000; // JSON toutes les 5 secondes

unsigned long lastCameraTime = 0;
const unsigned long cameraInterval = 5 * 60 * 1000; // Image toutes les 5 minutes

unsigned long lastLcdSwitchTime = 0;
bool lcdPageToggle = true;

LiquidCrystal_I2C lcd(0x27, 16, 2);
DHT dht(DHTPIN, DHTTYPE);

// --- 1. INITIALISATION DE LA CAMÉRA (AVEC TES MODIFICATIONS) ---
bool initCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  
  // 1️⃣ MODIFICATION ICI : Ajout des pins manquantes caméra
  config.pin_pwdn = -1;
  config.pin_reset = -1;
  
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // 2️⃣ MODIFICATION ICI : Passage de la résolution VGA à SVGA pour l'IA
  if(psramFound()){
    config.frame_size = FRAMESIZE_SVGA; 
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed: 0x%x\n", err);
    return false;
  }
  Serial.println("Camera Ready!");
  return true;
}

// --- 2. ENVOI MULTIPART STANDARD VIA WIFICLIENT ---
void uploadCameraImage() {
  camera_fb_t * fb = esp_camera_fb_get();
  if(!fb) {
    Serial.println("Camera capture failed");
    return;
  }

  WiFiClient client;
  if (client.connect(serverIP, serverPort)) {
    Serial.println("Connecté au serveur pour l'upload de l'image...");

    String boundary = "----SmartGreenhouseBoundary";
    
    String head = "--" + boundary + "\r\n";
    head += "Content-Disposition: form-data; name=\"file\"; filename=\"esp32_cam.jpg\"\r\n";
    head += "Content-Type: image/jpeg\r\n\r\n";
    
    String tail = "\r\n--" + boundary + "--\r\n";

    uint32_t totalLength = head.length() + fb->len + tail.length();

    client.println("POST /upload HTTP/1.1");
    client.println("Host: " + String(serverIP) + ":" + String(serverPort));
    client.println("Content-Type: multipart/form-data; boundary=" + boundary);
    client.print("Content-Length: ");
    client.println(totalLength);
    client.println("Connection: keep-alive");
    client.println(); 

    client.print(head);
    client.write(fb->buf, fb->len);
    client.print(tail);

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        esp_camera_fb_return(fb);
        return;
      }
    }
    
    if(client.available()) {
      String responseLine = client.readStringUntil('\n');
      Serial.println("Serveur: " + responseLine);
    }
    
  } else {
    Serial.println("Échec de connexion au serveur pour l'image.");
  }
  
  client.stop();
  esp_camera_fb_return(fb); // Libération du framebuffer
}

// --- 3. ENVOI DES DONNÉES JSON ---
void sendJsonData(float temp, float hum, int soil, int light) {
  WiFiClient client;
  if (client.connect(serverIP, serverPort)) {
    String jsonData = "{\"temperature\":" + String(temp, 1) + 
                      ",\"humidity\":" + String(hum, 1) + 
                      ",\"soil\":" + String(soil) + 
                      ",\"light\":" + String(light) + "}";

    client.println("POST /esp32/data HTTP/1.1");
    client.println("Host: " + String(serverIP) + ":" + String(serverPort));
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(jsonData.length());
    client.println("Connection: close");
    client.println();
    client.print(jsonData);
    
    client.stop();
    Serial.println("Données JSON envoyées.");
  }
}

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

  Wire.begin(I2C_SDA, I2C_SCL);
  dht.begin();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("SMART GREEN");
  lcd.setCursor(0,1);
  lcd.print("HOUSE AI");

  delay(2000);

  if(!initCamera()) {
    lcd.clear();
    lcd.print("Camera ERR");
    delay(2000);
  }

  WiFi.begin(ssid, password);
  lcd.clear();
  lcd.print("Connecting");

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  lcd.clear();
  lcd.print("WiFi OK");
  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());
  delay(1500);
}

void loop()
{
  // Lecture capteurs
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    temperature = 28.5; 
    humidity = 65.0;
  }

  int soilRaw = analogRead(SOIL_PIN);
  int ldrRaw = analogRead(LDR_PIN);

  int soilPercent = map(soilRaw, 4095, 0, 0, 100);
  soilPercent = constrain(soilPercent, 0, 100);

  int lightPercent = map(ldrRaw, 4095, 0, 0, 100);
  lightPercent = constrain(lightPercent, 0, 100);

  // Automatisation Relais
  digitalWrite(RELAY_PUMP, (soilPercent < 30) ? LOW : HIGH);
  digitalWrite(RELAY_FAN, (temperature > 30) ? LOW : HIGH);
  digitalWrite(RELAY_LIGHT, (lightPercent < 40) ? LOW : HIGH);

  // Gestion Écran LCD (Changement de page toutes les 3 secondes)
  if (millis() - lastLcdSwitchTime >= 3000) {
    lastLcdSwitchTime = millis();
    lcdPageToggle = !lcdPageToggle;
    lcd.clear();
    if (lcdPageToggle) {
      lcd.setCursor(0, 0); lcd.print("T: "); lcd.print(temperature, 1); lcd.print(" C");
      lcd.setCursor(0, 1); lcd.print("H: "); lcd.print(humidity, 0); lcd.print(" %");
    } else {
      lcd.setCursor(0, 0); lcd.print("Soil: "); lcd.print(soilPercent); lcd.print(" %");
      lcd.setCursor(0, 1); lcd.print("Light: "); lcd.print(lightPercent); lcd.print(" %");
    }
  }

  // Envoi périodique des données numériques JSON
  if (millis() - lastDataTime >= dataInterval) {
    lastDataTime = millis();
    if(WiFi.status() == WL_CONNECTED) {
      sendJsonData(temperature, humidity, soilPercent, lightPercent);
    }
  }

  // Capture et Upload automatique de l'image (Multipart brute)
  if (millis() - lastCameraTime >= cameraInterval) {
    lastCameraTime = millis();
    if(WiFi.status() == WL_CONNECTED) {
      Serial.println("Lancement de la capture automatique de l'image...");
      uploadCameraImage();
    }
  }
}
