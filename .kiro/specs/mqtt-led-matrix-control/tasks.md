# Implementation Plan: MQTT LED Matrix Control

## Overview

Add a git-ignored secrets header with a committed example, rewrite the sketch to connect to Wi-Fi and the MQTT broker and switch between a heart and a smile frame, extend `build.sh` with a library install step and a secrets check, then compile and upload with the Arduino CLI.

## Tasks

- [x] 1. Add Wi-Fi secrets files and ignore rule
  - [x] 1.1 Create `led_blink/led_blink/arduino_secrets.h.example`
    - Define `SECRET_SSID` and `SECRET_PASS` with placeholder values
    - _Requirements: 1.1_
  - [x] 1.2 Add `led_blink/led_blink/arduino_secrets.h` to the root `.gitignore`
    - Create `.gitignore` if it does not exist
    - Ensure the `.example` file is not matched by the entry
    - _Requirements: 1.1_
  - [x] 1.3 Create a local `led_blink/led_blink/arduino_secrets.h` from the example
    - Copy the example so the sketch can compile; the user fills in real credentials
    - _Requirements: 1.1_

- [x] 2. Rewrite the sketch in `led_blink/led_blink/led_blink.ino`
  - [x] 2.1 Add includes, globals, and frame data
    - Include `WiFiS3.h`, `ArduinoMqttClient.h`, `Arduino_LED_Matrix.h`, `arduino_secrets.h`
    - Declare `matrix`, `wifiClient`, `mqtt`, `mqttReady`, `heartFrame` (reuse the existing bitmap) and `smileFrame`
    - _Requirements: 3.1, 3.2_
  - [x] 2.2 Implement `connectWifi()` and `connectMqtt()`
    - `connectWifi()` retries `WiFi.begin` until `WL_CONNECTED`, with a short delay between attempts
    - `connectMqtt()` connects once to `test.mosquitto.org:1883`, registers `handleMessage`, subscribes to `bandungsoda/kiro/led/command`, and returns a bool
    - _Requirements: 1.1, 1.2, 2.1, 2.2, 4.1_
  - [x] 2.3 Implement `handleMessage()` and `applyCommand()`
    - `handleMessage()` reads the payload into a `String` and calls `applyCommand()`
    - `applyCommand()` renders the heart for `HEART`, the smile for `SMILE`, and ignores any other payload
    - _Requirements: 2.3, 3.1, 3.2, 3.3_
  - [x] 2.4 Implement `setup()` and `loop()`
    - `setup()` calls `matrix.begin()` without rendering, then `connectWifi()` and `connectMqtt()`
    - `loop()` calls `mqtt.poll()` when `mqttReady` is true and renders nothing else
    - _Requirements: 3.4, 3.5_

- [x] 3. Update `led_blink/build.sh`
  - [x] 3.1 Add the secrets check and library install step
    - Exit 1 with a stderr message naming `arduino_secrets.h.example` when `arduino_secrets.h` is missing
    - Run `arduino-cli lib install ArduinoMqttClient` before compiling
    - Update the step counters in the echo messages
    - Keep the existing compile, port detection, and 60 s upload timeout logic
    - _Requirements: 5.1, 5.2, 5.3, 5.4, 5.5_

- [x] 4. Compile and upload
  - [x] 4.1 Run `led_blink/build.sh` to install the library, compile, and upload
    - Compile for `arduino:renesas_uno:unor4wifi` and upload to the detected board
    - Fix any compile errors in the sketch or script
    - _Requirements: 5.1, 5.2, 5.3_

## Notes

- No test tasks, logging, or MQTT reconnection are included, per the agreed scope.
- Each task references specific requirements for traceability.

## Task Dependency Graph

```json
{
  "waves": [
    { "id": 0, "tasks": ["1.1", "1.2", "2.1", "3.1"] },
    { "id": 1, "tasks": ["1.3", "2.2"] },
    { "id": 2, "tasks": ["2.3"] },
    { "id": 3, "tasks": ["2.4"] },
    { "id": 4, "tasks": ["4.1"] }
  ]
}
```
