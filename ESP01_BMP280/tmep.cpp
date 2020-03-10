#include <ESP8266WiFi.h>

#include "tmep.h"

namespace tmep {
  
  void sendValue(TmepOption *option, String value) {
    WiFiClient client;
    if (client.connect((*option).host, (*option).port)) {
      Serial.println("Connected to " + String((*option).host));
  
      String query = "?" + String((*option).sensorGuid) + "=" + String(value);
  
      client.print(String("GET /") + query + " HTTP/1.1\r\n" +
        "Host: " + (*option).host + "\r\n" +
        "Connection: close\r\n" +
        "\r\n"
      );
    
      Serial.println("Request GET " + String((*option).host) + "/" + query + " sent");
  
      Serial.println("Response:");
      while (client.connected() || client.available()) {
        if (client.available()) {
          String line = client.readStringUntil('\n');
          Serial.println(line);
        }
      }
    } else {
      Serial.println("Connection to " + String((*option).host) + " failed");
    }
    
    client.stop();
  }
}
