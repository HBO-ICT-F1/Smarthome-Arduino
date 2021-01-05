#ifdef ARDUINO_ARCH_ESP8266
  #include "ESP8266WiFi.h"
  
  #define Server WiFiServer
  #define Client WiFiClient


  // Wifi SSID
  #define SSID "<ssid here>"
  
  // Wifi password
  #define PASSWORD "<password here>"
#else
  #include "SPI.h"
  #include "Ethernet.h"
  
  #define Server EthernetServer
  #define Client EthernetClient

  byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
  };
  IPAddress ip(192, 168, 1, 177);
#endif

// Device hostname
#define HOSTNAME "Smarthome"

// Server port
#define PORT 6001

// Timeout in seconds
#define TIMEOUT 2


Server server(PORT);

void setup(){
  Serial.begin(115200);
  delay(1000);

#ifdef ARDUINO_ARCH_ESP8266
  WiFi.begin(SSID, PASSWORD);
  WiFi.hostname(HOSTNAME);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting..");
  }
  
  Serial.print("Connected to WiFi with ip ");
  Serial.println(WiFi.localIP());
#else
  Ethernet.init(10);
  Ethernet.begin(mac, ip);

  if(Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield not found");
    return;
  }else if(Ethernet.linkStatus() == LinkOFF) {
    Serial.println("No Ethernet cable connected");
    return;
  }
  
  Serial.print("Connected to Ethernet with ip ");
  Serial.println(Ethernet.localIP());
#endif
 
  server.begin();
}
 
void loop(){
  Client client = server.available();
  if (!client) return;
  Serial.println("Client connected");
  
  for(uint8_t i = 0; i < TIMEOUT * 20 && client.connected(); i++){
    String data = "";
    
    while (client.available() > 0) {
      char c = client.read();        
      data.concat(c);
    }
    
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
