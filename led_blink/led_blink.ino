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

// Display_Controller: exact, case-sensitive command match.
void applyCommand(const String& cmd) {
  if (!matrixReady) {
    return;
  }
  if (cmd == "HEART") {
    matrix.renderBitmap(heartFrame, 8, 12);
  } else if (cmd == "SMILE") {
    matrix.renderBitmap(smileFrame, 8, 12);
  }
  // Any other value is ignored; the matrix content is unchanged.
}

void handleMessage(int size) {
  String payload;
  payload.reserve(size);
  for (int i = 0; i < size; i++) {
    int c = mqtt.read();
    if (c < 0) {
      break;
    }
    payload += (char)c;
  }
  applyCommand(payload);
}

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
  connectWifi();
  mqttReady = connectMqtt();
}

void loop() {
  // No rendering here: the last frame persists.
  if (mqttReady) {
    mqtt.poll();
  }
}
