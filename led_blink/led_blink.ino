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
  Serial.print("Command: '");
  Serial.print(cmd);
  Serial.println("'");
  if (!matrixReady) {
    Serial.println("Matrix not ready, ignored");
    return;
  }
  if (cmd == "HEART") {
    matrix.renderBitmap(heartFrame, 8, 12);
    Serial.println("Rendered HEART");
  } else if (cmd == "SMILE") {
    matrix.renderBitmap(smileFrame, 8, 12);
    Serial.println("Rendered SMILE");
  } else {
    Serial.println("Unknown command, ignored");
  }
}

void handleMessage(int size) {
  Serial.print("Message received on '");
  Serial.print(mqtt.messageTopic());
  Serial.print("', size ");
  Serial.println(size);
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
    Serial.print("Connecting to Wi-Fi SSID '");
    Serial.print(SECRET_SSID);
    Serial.println("'...");
    WiFi.begin(SECRET_SSID, SECRET_PASS);
    delay(2000);
  }
  Serial.print("Wi-Fi connected, IP: ");
  Serial.println(WiFi.localIP());
}

bool connectMqtt() {
  Serial.println("Connecting to MQTT test.mosquitto.org:1883...");
  if (!mqtt.connect("test.mosquitto.org", 1883)) {
    Serial.print("MQTT connect failed, error ");
    Serial.println(mqtt.connectError());
    return false;
  }
  mqtt.onMessage(handleMessage);
  if (!mqtt.subscribe("bandungsoda/kiro/led/command")) {
    Serial.println("MQTT subscribe failed");
  }
  Serial.println("MQTT connected, subscribed to bandungsoda/kiro/led/command");
  return true;
}

void setup() {
  Serial.begin(115200);
  unsigned long start = millis();
  while (!Serial && millis() - start < 3000) {
  }
  Serial.println("Starting");
  // begin() returns nonzero on success in the renesas_uno core.
  matrixReady = (matrix.begin() != 0);
  Serial.print("Matrix ready: ");
  Serial.println(matrixReady ? "yes" : "no");
  connectWifi();
  mqttReady = connectMqtt();
}

void loop() {
  // No rendering here: the last frame persists.
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi lost");
    mqttReady = false;
    connectWifi();
  }
  if (!mqttReady || !mqtt.connected()) {
    mqttReady = connectMqtt();
    if (!mqttReady) {
      delay(3000);
      return;
    }
  }
  mqtt.poll();
}
