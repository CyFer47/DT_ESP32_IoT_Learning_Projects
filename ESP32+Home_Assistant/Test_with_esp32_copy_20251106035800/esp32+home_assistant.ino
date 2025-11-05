#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// -------------------- WiFi Credentials --------------------
const char* ssid = "Chamara fernando";     // WiFi Network Name
const char* password = "BBB12345";         // WiFi Password

// -------------------- MQTT Broker Settings --------------------
const char* mqtt_server = "172.20.10.3";   // Home Assistant IP Address
const int mqtt_port = 1883;                // MQTT Port
const char* mqtt_user = "mqtt_user";       // MQTT Username
const char* mqtt_pass = "root";            // MQTT Password

// -------------------- DHT Sensor Setup --------------------
#define DHTPIN 5                            // GPIO pin connected to DHT11
#define DHTTYPE DHT11                       // Sensor type DHT11
DHT dht(DHTPIN, DHTTYPE);

// -------------------- MQTT Client --------------------
WiFiClient espClient;
PubSubClient client(espClient);

// -------------------- LED Pin --------------------
#define LED_PIN 2                           // On-board LED pin

// -------------------- Timer for Publishing --------------------
unsigned long lastPublish = 0;
const unsigned long PUBLISH_INTERVAL = 10000;  // 10 seconds

// ------------------------------------------------------------
// Setup runs once when ESP32 powers ON
// ------------------------------------------------------------
void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  dht.begin();

  // Connect to WiFi
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi Connection Failed");
  }

  // Configure MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(onMqttMessage);
}

// ------------------------------------------------------------
// Main Loop
// ------------------------------------------------------------
void loop() {
  // Reconnect WiFi if disconnected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost, reconnecting...");
    WiFi.begin(ssid, password);
    delay(2000);
    return;
  }

  // Reconnect MQTT if disconnected
  if (!client.connected()) {
    reconnectMQTT();
  }

  client.loop();

  // Publish sensor data every 10 seconds
  if (millis() - lastPublish > PUBLISH_INTERVAL) {
    publishSensorData();
    lastPublish = millis();
  }
}

// ------------------------------------------------------------
// Publish Sensor Data to MQTT
// ------------------------------------------------------------
void publishSensorData() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Sensor Read Error");
    return;
  }

  client.publish("home/sensor/temperature", String(temperature).c_str());
  client.publish("home/sensor/humidity", String(humidity).c_str());

  String json = "{\"temperature\":" + String(temperature, 2) + ",\"humidity\":" + String(humidity, 2) + "}";
  client.publish("home/sensor/data", json.c_str());

  Serial.println("Data Sent:");
  Serial.print("Temperature: "); Serial.println(temperature);
  Serial.print("Humidity: "); Serial.println(humidity);
}

// ------------------------------------------------------------
// Reconnect to MQTT Broker
// ------------------------------------------------------------
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting to MQTT... ");

    if (client.connect("ESP32Client", mqtt_user, mqtt_pass)) {
      Serial.println("Connected");
      client.subscribe("home/control/led");   // Subscribe to LED control topic
    } else {
      Serial.print("Failed, State=");
      Serial.print(client.state());
      Serial.println(" - Retrying in 5 seconds");
      delay(5000);
    }
  }
}

// ------------------------------------------------------------
// Callback: Run when MQTT message received
// ------------------------------------------------------------
void onMqttMessage(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message on topic: ");
  Serial.println(topic);

  String msg;
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.print("Message: ");
  Serial.println(msg);

  if (String(topic) == "home/control/led") {
    if (msg == "ON") {
      digitalWrite(LED_PIN, HIGH);
      Serial.println("LED ON");
    } else if (msg == "OFF") {
      digitalWrite(LED_PIN, LOW);
      Serial.println("LED OFF");
    }
  }
}
