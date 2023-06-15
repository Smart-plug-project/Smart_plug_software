
#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
   #define DEBUG_ESP_PORT Serial
   #define NODEBUG_WEBSOCKETS
   #define NDEBUG
#endif 

#include <Arduino.h>
#ifdef ESP8266 
   #include <ESP8266WiFi.h>
#endif 
#ifdef ESP32   
   #include <WiFi.h>
#endif

#include "SinricPro.h"
#define WIFI_SSID         "OnePlus 9 5G"
#define WIFI_PASS         "e2cxwz37"
#define APP_KEY           "1357066d-0b38-47c6-98d6-3b9cd933b937"
#define APP_SECRET        "c61037e6-448e-4743-92a0-1c14f87ff657-c958fbf1-3d8f-4849-b0be-6b8b797285b0"

#define SWITCH_ID_1       "644908c7312d40edc3fb8090"
#define POWERSENSOR_ID    "644918be312d40edc3fb8aa4"
#define RELAYPIN_1        5
#define SAMPLE_EVERY_SEC  60                  
#define BAUD_RATE         9600                // Change baudrate to your need



// setup function for SinricPro
void setupSinricPro() {
  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.restoreDeviceStates(true); // Uncomment to restore the last known state from the server.
   
  SinricPro.begin(APP_KEY, APP_SECRET);
}
// setup function for WiFi connection
void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }

  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

// main setup function
void setup() {
  
  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");
   setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
  
}
