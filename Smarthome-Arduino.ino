// Check if the application should run as a wifi server or ethernet server
#ifdef ARDUINO_ARCH_ESP8266
  // Include ESP8266WiFi header for WiFi server
  #include "ESP8266WiFi.h"

  // Use ESP8266WiFi classes for running server
  #define Server WiFiServer
  #define Client WiFiClient

  // Wifi SSID
  #define SSID "<wifi ssid>"
  
  // Wifi password
  #define PASSWORD "<wifi password>"

  //Define pins
  #define BUZZER D9
#else
  // Include Ethernet headers for running a normal server
  #include "SPI.h"
  #include "Ethernet.h"

  // Use default ethernet classes for running server
  #define Server EthernetServer
  #define Client EthernetClient

  byte mac[] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
  IPAddress ip(192, 168, 0, 1);

  //Define pins
  #define BUZZER 9
#endif

// Device hostname
#define HOSTNAME "Smarthome"

// Server port
#define PORT 6001

// Timeout in seconds
#define TIMEOUT 2

//Define global variable
bool buzzerActive = false;

Server server(PORT);

void setup(){
  Serial.begin(115200);
  while (!Serial) {
    delay(10);
  }

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
  }
  
  if(Ethernet.linkStatus() == LinkOFF) {
    Serial.println("No Ethernet cable connected");
    return;
  }
  
  Serial.print("Connected to Ethernet with ip ");
  Serial.println(Ethernet.localIP());
#endif
 
  server.begin();

  //pinModes
  pinMode(BUZZER, OUTPUT);
}

void buzzer(){
  if(!buzzerActive) return;
  tone(BUZZER, 2000);
  delay(100); 
  noTone(BUZZER);
  delay(100);
}
 
void loop(){
  buzzer();
  
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
      
      if(data == "buzzer"){
        buzzerActive = !buzzerActive;
        break;
      }
    }
    
    delay(50);
  }
  
  client.stop(); 
  Serial.println("Client disconnected");
}
