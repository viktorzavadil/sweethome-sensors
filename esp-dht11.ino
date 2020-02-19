#include <DHT.h>
#include <ESP8266WiFi.h>
#define DHTPIN 2
#define DHTTYPE  DHT11

const String ssid = "***";
const char* password = "***";
const char* host = "sweethome.tmep.cz";
const int port = 80;
const String guid = "xyz";
const int period = 60000;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);

  dht.begin();

  WiFi.begin(ssid, password);

  Serial.print("Connecting ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnection established");
}

void loop() {
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Reading from the DHT sensor failed!");
    return;
  }

  float hic = dht.computeHeatIndex(temperature, humidity, false);

  WiFiClient client;
  if (client.connect(host, port)) {
    Serial.println("Connected to " + String(host));

    String query = "?" + guid + "=" + String(temperature) + "&humV=" + String((int) humidity);

    client.print(String("GET /") + query + " HTTP/1.1\r\n" +
      "Host: " + host + "\r\n" +
      "Connection: close\r\n" +
      "\r\n"
    );
  
    Serial.println("Request GET " + String(host) + "/" + query + " sent");

    Serial.println("Response:");
    while (client.connected() || client.available()) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line);
      }
    }
  
    client.stop();

    delay(period);
  } else {
    Serial.println("Connection to " + String(host) + " failed");
    client.stop();
    delay(5000);
    return;
  }
}
