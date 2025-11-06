#include <DHT.h>

#define DHTPIN 5      // D5 pin = GPIO5
#define LEDPIN 2      // D4 pin = GPIO2

#define DHTTYPE DHT11 // or DHT22 if you use that

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  pinMode(LEDPIN, OUTPUT);

  dht.begin();
  Serial.println("DHT11 + LED Test Start...");
}

void loop() {
  // LED blink test
  digitalWrite(LEDPIN, HIGH);
  delay(500);
  digitalWrite(LEDPIN, LOW);
  delay(500);

  // Read sensor values
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Temp: ");
  Serial.print(t);
  Serial.print(" °C  |  Humidity: ");
  Serial.print(h);
  Serial.println(" %");
}
