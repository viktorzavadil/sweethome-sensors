/*
 * Wiring
 * * * * * * * * * * * * * * *
 * ESP-01 VCC to 3.3V
 * ESP-01 GND to GND
 * ESP-01 TX to DHT11 DATA
 * ESP-01 GPIO0 to BMP280 SDI
 * ESP-01 GPIO2 to BMP280 SCK
 * BMP280 VCC to 3.3V
 * BMP280 GND to GND
 */

#include <Adafruit_BMP280.h>
#include <ESP8266WiFi.h>

#include "config.h"
#include "tmep.h"

#define SENSOR_READING_DELAY 60000

void setupI2C();
void setupBMP();
void setupWiFiConnection();

Adafruit_BMP280 bmpSensor;

void setup() {
  Serial.begin(9600);
  setupI2C();
  setupBMPSensor();  
  setupWiFiConnection();
}

void setupI2C() {
  Wire.pins(0, 2);
  Wire.begin(0, 2);
}

void setupBMPSensor() {
  if (!bmpSensor.begin()) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }

  bmpSensor.setSampling(Adafruit_BMP280::MODE_FORCED,     /* Operating Mode. */
                        Adafruit_BMP280::SAMPLING_X1,     /* Temp. oversampling */
                        Adafruit_BMP280::SAMPLING_X1,    /* Pressure oversampling */
                        Adafruit_BMP280::FILTER_OFF,      /* Filtering. */
                        Adafruit_BMP280::STANDBY_MS_1000); /* Standby time. */
}

void setupWiFiConnection() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnection established");
}

void loop() {
  float temperature = bmpSensor.readTemperature();
  float pressure = bmpSensor.readPressure() / 100;
  float altitude = bmpSensor.readAltitude(SEA_LEVEL_PRESSURE);
  
  Serial.println("temperature: " + String(temperature));
  Serial.println("pressure: " + String(pressure));
  Serial.println("altitude: " + String(altitude));

  tmep::TmepOption temperatureSensorOption;
  temperatureSensorOption.host = TMEP_OUTSIDE_TEMPERATURE_HOST;
  temperatureSensorOption.sensorGuid = TMEP_BMP280_TEMPERATURE_GUID;
  tmep::sendValue(&temperatureSensorOption, String(temperature));
  
  tmep::TmepOption pressureSensorOption;
  pressureSensorOption.host = TMEP_OUTSIDE_PRESSURE_HOST;
  pressureSensorOption.sensorGuid = TMEP_BMP280_PRESSURE_GUID;
  tmep::sendValue(&pressureSensorOption, String(pressure));

  delay(SENSOR_READING_DELAY);
}
