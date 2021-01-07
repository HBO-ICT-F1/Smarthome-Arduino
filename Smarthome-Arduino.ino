#include "DHT.h"

#ifdef ARDUINO_ARCH_ESP8266
  #include "ESP8266WiFi.h"
  
  #define Server WiFiServer
  #define Client WiFiClient


  // Wifi SSID
//  #define SSID "Secret"

  // Wifi password
//  #define PASSWORD "Secret"

  //Define pins
  #define BUZZERPIN D9
  #define DHTPIN D8
#else
  #include "Ethernet2.h"

  #define Server EthernetServer
  #define Client EthernetClient

  byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
  };

  //Define pins
  #define BUZZERPIN 9
  #define DHTPIN 8
#endif

//Define DHT TYPE
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Device hostname
#define HOSTNAME "Smarthome"

// Server port
#define PORT 80

//Define global variable
bool buzzerActive = false;
unsigned long buzzerDelay = millis();
bool systemActive = false;
float temperature = 0.0;
float humidity = 0.0;
bool dhtReady = false;
uint8_t pairCode = 0;

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
  Ethernet.begin(mac);
  Serial.print("Connected to Ethernet with ip ");
  Serial.println(Ethernet.localIP());
#endif

  server.begin();
  dht.begin();

  //pinModes
  pinMode(BUZZERPIN, OUTPUT);
}

void ethernetDisconnect(Client client){
  #ifndef ARDUINO_ARCH_ESP8266
      client.stop();
  #endif
}

void buzzer(){
  if(!buzzerActive) return;
  if(buzzerDelay < millis()){
    tone(BUZZERPIN, 2000);
    if(buzzerDelay + 100 < millis()){
      noTone(BUZZERPIN);
      buzzerDelay = millis() + 100;
    }
  }
}

void dhtSensor(){
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  dhtReady = true;
   if (isnan(humidity)){
    dhtReady = false;
    humidity = 0.0;
   }
   if(isnan(temperature)) {
    dhtReady = false;
    temperature = 0.0;
   }
}

void loop(){
  buzzer();
  dhtSensor();
  Client client = server.available();
  if (!client) return;

  String request = client.readStringUntil('\r');
  client.flush();

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println(""); //  do not forget this one

  if (request.indexOf("/pair") != -1){
    if(pairCode != 0){
      client.println("{\"error\": \"device is already paired\"}");
      ethernetDisconnect(client);
      return;
    }
    randomSeed(millis());
    pairCode = random(1000, 10000);
    Serial.print("Pair code: ");
    Serial.println(pairCode);
    client.println("{\"code\": " + String(pairCode) + "}");
    ethernetDisconnect(client);
    return;
  }
  
  if(pairCode == 0){
    client.println("{\"error\": \"device not paired\"}");
    ethernetDisconnect(client);
    return;
  }

  if (request.indexOf("code=" + String(pairCode)) == -1){
    client.println("{\"error\": \"Invalid pair code\"}");
    ethernetDisconnect(client);
    return;
  }
  
  if (request.indexOf("buzzer=") != -1){
    buzzerActive = request.indexOf("true") != -1;
    noTone(BUZZERPIN);
  }
  
  client.print("{");
  client.print("\"dth\": {");
    client.print("\"temperature\": " + String(temperature) + ", ");
    client.print("\"humidity\": " + String(humidity) + ", ");
    client.print("\"ready\": " + String(dhtReady) + "");
  client.print("}, ");
  client.print("\"buzzer\": " + String(buzzerActive) + "");
  client.println("}");
  ethernetDisconnect(client);
}
