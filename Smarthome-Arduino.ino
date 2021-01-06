#ifdef ARDUINO_ARCH_ESP8266
  #include "ESP8266WiFi.h"
  
  #define Server WiFiServer
  #define Client WiFiClient


  // Wifi SSID
//  #define SSID "Secret"
  
  // Wifi password
//  #define PASSWORD "Secret"

  //Define pins
  #define BUZZER D9
#else
  #include "SPI.h"
  #include "Ethernet.h"
  
  #define Server EthernetServer
  #define Client EthernetClient

  byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
  };
  IPAddress ip(192, 168, 1, 177);

  //Define pins
  #define BUZZER 9
#endif

// Device hostname
#define HOSTNAME "Smarthome"

// Server port
#define PORT 80

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
  }else if(Ethernet.linkStatus() == LinkOFF) {
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
  Client client = server.available();
  buzzer();
  if (!client) return;

  String request = client.readStringUntil('\r');
  client.flush();

  if (request.indexOf("/BUZZER") != -1){
    buzzerActive = !buzzerActive;
  }

    // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println(""); //  do not forget this one
  client.println("{\"name\": \"sonoo\", \"salary\": 56000, \"married\": true}");
}
