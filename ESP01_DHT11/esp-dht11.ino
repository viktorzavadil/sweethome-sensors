#include <DHT.h>
#include <ESP8266WiFi.h>
#include "config.h"

/*
 * A   RXD     |   GPIO3  o o VCC
 * N   SPI_CS2 |   GPIO0  o o RESET (pulled up)
 * T               GPIO2  o o CH_PD (pulled up)
 *                   GND  o o GPIO1 | TXD
 */
#define DHTPIN 2
#define DHTTYPE DHT11

#define CONNECTING_STATUS_DELAY = 500;
#define READING_PERIOD = 60000;
#define ERROR_DELAY = 5000;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);

  dht.begin();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(CONNECTING_STATUS_DELAY);
  }
  Serial.println("\nConnection established");
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Reading from the DHT sensor failed!");
    delay(ERROR_DELAY);
    return;
  }

  WiFiClient client;
  if (client.connect(TMEP_HOST, TMEP_PORT)) {
    Serial.println("Connected to " + String(TMEP_HOST));

    String query = "?" + String(TMEP_DHT_SENSOR_GUID) + "=" + String(temperature) + "&humV=" + String((int) humidity);

    client.print(String("GET /") + query + " HTTP/1.1\r\n" +
      "Host: " + TMEP_HOST + "\r\n" +
      "Connection: close\r\n" +
      "\r\n"
    );
  
    Serial.println("Request GET " + String(TMEP_HOST) + "/" + query + " sent");

    Serial.println("Response:");
    while (client.connected() || client.available()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line);
      }
    }
  
    client.stop();

    delay(READING_PERIOD);
  } else {
    Serial.println("Connection to " + String(TMEP_HOST) + " failed");
    client.stop();
    delay(ERROR_DELAY);
    return;
  }
}