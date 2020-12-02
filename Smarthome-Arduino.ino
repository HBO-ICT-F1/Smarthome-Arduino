#include "ESP8266WiFi.h"
 
const char* ssid = "";
const char* password = "";
WiFiServer wifiServer(80);

void setup() {
  Serial.begin(115200);
  delay(1000);
 
  WiFi.begin(ssid, password);
  WiFi.hostname("WeMos");
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting..");
  }
 
  Serial.print("Connected to WiFi. IP:");
  Serial.println(WiFi.localIP());
  wifiServer.begin();
}
 
void loop() {
  WiFiClient client = wifiServer.available();
 
  if (client) {
    Serial.println("Client connected");
    
    while (client.connected()) {
      String data = "";
      
      while (client.available() > 0) {
        char c = client.read();        
        data.concat(c);
      }
      
      if(data != ""){
        Serial.println(data);
      }
    }
    
    client.stop(); 
    Serial.println("Client disconnected");
  }
}
