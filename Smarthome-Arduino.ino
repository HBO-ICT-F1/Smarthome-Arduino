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
  #define DHTPIN  D8
#else
  #include "Ethernet2.h"

  #define Server EthernetServer
  #define Client EthernetClient

  byte mac[] = {
    0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
  };
  IPAddress ip(192, 168, 2, 69);

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

// Timeout in seconds
#define TIMEOUT 2

//Define global variable
bool buzzerActive = false;
float temperature = 0.0;
float humidity = 0.0;
bool dhtReady = false;

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
  Serial.print("Connected to Ethernet with ip ");
  Serial.println(Ethernet.localIP());
  w5500.setRetransmissionCount(1);
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
  tone(BUZZERPIN, 2000);
  delay(100);
  noTone(BUZZERPIN);
  delay(100);
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

  if (request.indexOf("/buzzer") != -1){
    buzzerActive = !buzzerActive;
    client.println("{\"buzzer\": " + String(buzzerActive) + "}");
    ethernetDisconnect(client);
    return;
  }

  if (request.indexOf("/temp") != -1){
    Serial.print("Temp: ");
    Serial.println(temperature);
    Serial.print("Humi: ");
    Serial.println(humidity);
    client.println("{\"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + ", \"ready\": " + String(dhtReady) + "}");
    ethernetDisconnect(client);
    return;
  }
  ethernetDisconnect(client);
}
