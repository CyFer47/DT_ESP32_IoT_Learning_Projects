#define BLYNK_TEMPLATE_ID "TMPL4U7OMKPca"
#define BLYNK_TEMPLATE_NAME "ESP 32 DHT 11"
#define BLYNK_AUTH_TOKEN "dbrzrUIP226JFfzrP7x4b54SA_6Dwwty"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// Wi-Fi credentials
char ssid[] = "Your_WiFi_Name";
char pass[] = "Your_WiFi_Password";

// DHT setup
#define DHTPIN 5       // DHT11 connected to GPIO5 (D5)
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// LED setup
#define LEDPIN 2       // LED connected to GPIO2 (D4)

// Blynk virtual pins
#define VPIN_TEMP V0
#define VPIN_HUMI V1
#define VPIN_LED  V2

BlynkTimer timer;

void sendSensorData() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("❌ Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temp: ");
  Serial.print(t);
  Serial.print(" °C  |  Humidity: ");
  Serial.println(h);

  // Send values to Blynk app
  Blynk.virtualWrite(VPIN_TEMP, t);
  Blynk.virtualWrite(VPIN_HUMI, h);
}

// Control LED from Blynk
BLYNK_WRITE(VPIN_LED) {
  int ledState = param.asInt();
  digitalWrite(LEDPIN, ledState);
}

void setup() {
  Serial.begin(115200);
  pinMode(LEDPIN, OUTPUT);
  dht.begin();

  Serial.println("Connecting to Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Run sensor reading every 2 seconds
  timer.setInterval(2000L, sendSensorData);
}

void loop() {
  Blynk.run();
  timer.run();
}
