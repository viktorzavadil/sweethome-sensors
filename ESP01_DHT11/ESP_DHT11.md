Simple WiFi client which send data to tmep.cz server. The program collect data from DHT11 sensor (temperature & humidity) and these send to the server.

Code:
```c
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
```

Output:
```
Connecting .......
01:28:42.495 -> Connection established
01:28:42.561 -> Connected to sweethome.tmep.cz
01:28:42.594 -> Request GET sweethome.tmep.cz/?xyz=22.70&humV=37 sent
01:28:42.660 -> Response:
01:28:42.694 -> HTTP/1.1 200 OK
01:28:42.694 -> Date: Wed, 19 Feb 2020 00:28:40 GMT
01:28:42.760 -> Server: Apache/2.4.18 (Ubuntu)
01:28:42.793 -> Vary: Accept-Encoding
01:28:42.793 -> Connection: close
01:28:42.826 -> Transfer-Encoding: chunked
01:28:42.859 -> Content-Type: text/html; charset=UTF-8
01:28:42.892 -> Content-Language: cs
01:28:42.926 -> 
01:28:42.926 -> 0
01:28:42.926 -> 
01:29:42.823 -> Connected to sweethome.tmep.cz
01:29:42.857 -> Request GET sweethome.tmep.cz/?xyz=24.10&humV=37 sent
01:29:42.956 -> Response:
01:29:42.956 -> HTTP/1.1 200 OK
01:29:42.989 -> Date: Wed, 19 Feb 2020 00:29:41 GMT
01:29:43.022 -> Server: Apache/2.4.18 (Ubuntu)
01:29:43.055 -> Vary: Accept-Encoding
01:29:43.088 -> Connection: close
01:29:43.122 -> Transfer-Encoding: chunked
01:29:43.155 -> Content-Type: text/html; charset=UTF-8
01:29:43.188 -> Content-Language: cs
01:29:43.235 -> 
01:29:43.235 -> 0
01:29:43.235 -> 
```
