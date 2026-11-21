#include <WiFiS3.h>
#include <ArduinoMqttClient.h>
#include "Arduino_LED_Matrix.h"
#include "arduino_secrets.h"

ArduinoLEDMatrix matrix;
WiFiClient wifiClient;
MqttClient mqtt(wifiClient);
bool mqttReady = false;

// Heart_Frame: 8 rows (top to bottom) by 12 columns, 1 = pixel on.
// Not const: loadPixels() takes a non-const uint8_t*.
uint8_t heartFrame[8][12] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0},
  {0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0},
  {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
  {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
  {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
  {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0}
};

// Smile_Frame: 8 rows by 12 columns, 1 = pixel on.
uint8_t smileFrame[8][12] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
  {0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
  {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

bool matrixReady = false;

// Implemented in a later task.
void handleMessage(int size);

void connectWifi() {
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
}

bool connectMqtt() {
  if (!mqtt.connect("test.mosquitto.org", 1883)) {
    return false;
  }
  mqtt.onMessage(handleMessage);
  mqtt.subscribe("bandungsoda/kiro/led/command");
  return true;
}

void setup() {
  // begin() returns nonzero on success in the renesas_uno core.
  matrixReady = (matrix.begin() != 0);
}

void loop() {
  if (!matrixReady) {
    return;  // init failed: render nothing, matrix stays off
  }
  // renderBitmap is a macro: pass the array by name.
  matrix.renderBitmap(heartFrame, 8, 12);
  delay(1000);
  matrix.clear();
  delay(1000);
}
