#include "ESP8266WiFi.h"
 
const char* ssid = "";
const char* password = "";
WiFiServer wifiServer(6001);

void setup(){
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
 
void loop(){
  WiFiClient client = wifiServer.available();
  if (!client) return;
  Serial.println("Client connected");
  for(uint8_t i = 0; i < 40 && client.connected(); i++){ // 2 Seconds
    String data = "";
    
    while (client.available() > 0) {
      char c = client.read();        
      data.concat(c);
    }
       client.println("Hallo");
    if(data != ""){
      data.trim(); 
      if(data == "example"){
        client.println("example");
        break;
      }
    }
    delay(50);
  }
  
  client.stop(); 
  Serial.println("Client disconnected");
}
