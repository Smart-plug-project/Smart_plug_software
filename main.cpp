
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
#include "SinricProSwitch.h"
#include "SinricProPowerSensor.h"
#include "ACS712.h"

#define WIFI_SSID         "OnePlus 9 5G"
#define WIFI_PASS         "e2cxwz37"
#define APP_KEY           "1357066d-0b38-47c6-98d6-3b9cd933b937"
#define APP_SECRET        "c61037e6-448e-4743-92a0-1c14f87ff657-c958fbf1-3d8f-4849-b0be-6b8b797285b0"

#define SWITCH_ID_1       "644908c7312d40edc3fb8090"
#define POWERSENSOR_ID    "644918be312d40edc3fb8aa4"
#define RELAYPIN_1        5
#define SAMPLE_EVERY_SEC  60                  
#define BAUD_RATE         9600                // Change baudrate to your need

bool powerState = false;
struct {
  float voltage;
  float current;
  float power;
  float apparentPower;
  float reactivePower;
  float factor;
} powerMeasure;
ACS712 sensor(ACS712_30A, A0);


bool onPowerState1(const String &deviceId, bool &state) {
 Serial.printf("Device 1 turned %s", state?"on":"off");
 digitalWrite(RELAYPIN_1, state ? LOW:HIGH);
 return true; // request handled properly
}
void doPowerMeasure() {
  powerMeasure.voltage = 230;
  powerMeasure.current = sensor.getCurrentAC();
  powerMeasure.power = powerMeasure.voltage * powerMeasure.current;
  powerMeasure.apparentPower = powerMeasure.power;
 
}
bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device %s power turned %s \r\n", deviceId.c_str(), state?"on":"off");
  powerState = state;
  if (powerState) doPowerMeasure(); // start a measurement when device is turned on
  return true; // request handled properly
}
bool sendPowerSensorData() {
  // dont send data if device is turned off
  if (!powerState) return false;

  // limit data rate to SAMPLE_EVERY_SEC
  static unsigned long lastEvent = 0;
  unsigned long actualMillis = millis();
  if (actualMillis - lastEvent < (SAMPLE_EVERY_SEC * 1000)) return false;
  lastEvent = actualMillis;

  // send measured data
  SinricProPowerSensor &myPowerSensor = SinricPro[POWERSENSOR_ID];
  bool success = myPowerSensor.sendPowerSensorEvent(powerMeasure.voltage, powerMeasure.current, powerMeasure.power, powerMeasure.apparentPower);
  // if measured data was sent do a new measure
  if (success) doPowerMeasure();
  return success;
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

// setup function for SinricPro
void setupSinricPro() {
  // add devices and callbacks to SinricPro
  pinMode(RELAYPIN_1, OUTPUT);
   SinricProSwitch& mySwitch1 = SinricPro[SWITCH_ID_1];
  mySwitch1.onPowerState(onPowerState1);
    SinricProPowerSensor &myPowerSensor = SinricPro[POWERSENSOR_ID];

  // set callback function to device
  myPowerSensor.onPowerState(onPowerState);

 
  
  
  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  SinricPro.restoreDeviceStates(true); // Uncomment to restore the last known state from the server.
   
  SinricPro.begin(APP_KEY, APP_SECRET);
}

// main setup function
void setup() {
  sensor.calibrate();
  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");
  setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
  sendPowerSensorData();
}
