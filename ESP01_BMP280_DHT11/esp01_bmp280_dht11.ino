/*
 * Wiring
 * * * * * * * * * * * * * * *
 * ESP-01 VCC to 3.3V
 * ESP-01 GND to GND
 * ESP-01 TX to DHT11 DATA
 * ESP-01 GPIO0 to BMP280 SDI
 * ESP-01 GPIO2 to BMP280 SCK
 * DHT11 VCC to 3.3V
 * DHT11 GND to GND
 * BMP280 VCC to 3.3V
 * BMP280 GND to GND
 */

#include <Adafruit_BMP280.h>
#include <ESP8266WiFi.h>
#include <DHT.h>

#include "config.h"
#include "tmep.h"

#define SENSOR_READING_DELAY 60000

#define DHTPIN 1
#define DHTTYPE DHT11

void setupI2C();
void setupBMPSensor();
void setupDHTSensor();
void setupWiFiConnection();

Adafruit_BMP280 bmpSensor;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // Serial.begin(9600);
  setupI2C();
  setupBMPSensor();  
  setupDHTSensor();
  setupWiFiConnection();
}

void setupI2C() {
  Wire.pins(0, 2);
  Wire.begin(0, 2);
}

void setupBMPSensor() {
  if (!bmpSensor.begin()) {
    // Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }

  bmpSensor.setSampling(Adafruit_BMP280::MODE_FORCED,     /* Operating Mode. */
                        Adafruit_BMP280::SAMPLING_X1,     /* Temp. oversampling */
                        Adafruit_BMP280::SAMPLING_X1,    /* Pressure oversampling */
                        Adafruit_BMP280::FILTER_OFF,      /* Filtering. */
                        Adafruit_BMP280::STANDBY_MS_1000); /* Standby time. */
}

void setupDHTSensor() {
  dht.begin();  
}

void setupWiFiConnection() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // Serial.print("Connecting ");
  while (WiFi.status() != WL_CONNECTED) {
    // Serial.print(".");
    delay(1000);
  }
  // Serial.println("\nConnection established");
}

void loop() {
  float bmpTemperature = bmpSensor.readTemperature();
  float bmpPressure = bmpSensor.readPressure() / 100;
  
  // Serial.println("temperature: " + String(bmpTemperature));
  // Serial.println("pressure: " + String(bmpPressure));
  // Serial.println("altitude: " + String(altitude));

  tmep::TmepOption temperatureSensorOption;
  temperatureSensorOption.host = TMEP_OUTSIDE_TEMPERATURE_HOST;
  temperatureSensorOption.sensorGuid = TMEP_BMP280_TEMPERATURE_GUID;
  tmep::sendValue(&temperatureSensorOption, String(bmpTemperature));
  
  tmep::TmepOption pressureSensorOption;
  pressureSensorOption.host = TMEP_OUTSIDE_PRESSURE_HOST;
  pressureSensorOption.sensorGuid = TMEP_BMP280_PRESSURE_GUID;
  tmep::sendValue(&pressureSensorOption, String(bmpPressure));

  float dhtHumidity = dht.readHumidity();
  float dhtTemperature = dht.readTemperature();
  if (!isnan(dhtHumidity) && !isnan(dhtTemperature)) {
    tmep::TmepOption dhtOption;
    dhtOption.host = TMEP_GREENGARDEN_HOST;
    dhtOption.sensorGuid = TMEP_DHT_SENSOR_GUID;
    tmep::sendValue(&dhtOption, String(dhtTemperature) + "&humV=" + String((int) dhtHumidity));
  }

  delay(SENSOR_READING_DELAY);
}
