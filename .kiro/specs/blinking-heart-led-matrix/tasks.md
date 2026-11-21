# Implementation Plan: Blinking Heart LED Matrix

## Overview

Write a single Arduino sketch (`led_blink/led_blink.ino`) that blinks a heart on the UNO R4 WiFi LED matrix, add a small `build.sh` that runs compile then upload with `arduino-cli`, then compile and upload. No automated tests are created, per user instruction.

## Tasks

- [x] 1. Write the Blink_Sketch
  - [x] 1.1 Create `led_blink/led_blink.ino` with the heart frame and blink loop
    - Include `Arduino_LED_Matrix.h` and declare `ArduinoLEDMatrix matrix;`
    - Define `const uint8_t heartFrame[8][12]` using the heart bitmap from the design
    - Add a `bool matrixReady` flag set in `setup()` from the `matrix.begin()` result (nonzero means success)
    - In `loop()`, return immediately if not ready; otherwise `matrix.renderBitmap(heartFrame, 8, 12)`, `delay(1000)`, `matrix.clear()`, `delay(1000)`
    - Pass the array by name to `renderBitmap` (it is a macro)
    - _Requirements: 1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 2.1, 2.2_

- [x] 2. Add the build script
  - [x] 2.1 Create `led_blink/build.sh` that runs compile then upload
    - Step 1: `arduino-cli compile --fqbn arduino:renesas_uno:unor4wifi` on the sketch folder; on failure print "compile step failed", skip upload, exit non-zero
    - Step 2: find the port with `arduino-cli board list` for the UNO R4 WiFi; if none, print "no board found", skip upload, exit non-zero
    - Run `arduino-cli upload -p <port> --fqbn arduino:renesas_uno:unor4wifi` under a 60 s timeout with no retry; on failure print "upload step failed" with the reason and exit non-zero
    - Run exactly these two steps and no test steps; make the script executable
    - _Requirements: 2.3, 2.4, 2.5, 2.6, 2.7, 3.2, 3.3, 3.4_

- [x] 3. Compile and upload
  - [x] 3.1 Compile the sketch and upload it to the board
    - Run `build.sh` (or the two `arduino-cli` commands) from the project root
    - Confirm compile exits 0 with no errors and the upload reports success
    - Fix any compile errors in the sketch and rerun
    - _Requirements: 2.3, 2.4_

- [x] 4. Final checkpoint
  - Ensure the build succeeded, ask the user if questions arise.

## Notes

- No test tasks are included because automated testing is out of scope (Requirement 3.1), and the design has no Correctness Properties section.
- Visual check on the board (heart about 1 s on, about 1 s off) is left to the user.

## Task Dependency Graph

```json
{
  "waves": [
    { "id": 0, "tasks": ["1.1"] },
    { "id": 1, "tasks": ["2.1"] },
    { "id": 2, "tasks": ["3.1"] }
  ]
}
```
