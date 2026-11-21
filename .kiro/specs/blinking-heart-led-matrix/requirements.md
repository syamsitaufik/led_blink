# Requirements Document

## Introduction

This feature is an Arduino sketch for the Arduino UNO R4 WiFi. The sketch shows a heart shape on the built-in 12x8 LED matrix. The heart is visible for 1 second, hidden for 1 second, and the cycle repeats continuously while the board is powered. The sketch is compiled and uploaded to the board. Automated testing is out of scope, per the user's instruction.

## Glossary

- **Blink_Sketch**: The Arduino sketch that runs on the Arduino UNO R4 WiFi and controls the heart animation.
- **LED_Matrix**: The built-in 12-column by 8-row LED matrix of the Arduino UNO R4 WiFi.
- **Heart_Frame**: A 12x8 bitmap in which the lit pixels form a recognizable heart shape.
- **Blank_Frame**: A 12x8 bitmap in which all pixels are off.
- **Blink_Cycle**: One period consisting of 1 second of Heart_Frame display followed by 1 second of Blank_Frame display.
- **Build_Toolchain**: The Arduino command-line tooling used to compile the Blink_Sketch and upload it to the board.

## Requirements

### Requirement 1: Blink a Heart on the LED Matrix

**User Story:** As a maker, I want a heart to blink on the built-in LED matrix, so that the board shows a simple, recognizable animation.

#### Acceptance Criteria

1. THE Blink_Sketch SHALL define a Heart_Frame of exactly 12 columns by 8 rows (96 pixels), in which at least 1 pixel is on and at least 1 pixel is off.
2. WHEN the board starts, THE Blink_Sketch SHALL initialize the LED_Matrix before displaying any frame.
3. WHEN the LED_Matrix is initialized, THE Blink_Sketch SHALL display the Heart_Frame for 1000 milliseconds (±50 ms).
4. WHEN the Heart_Frame has been displayed for 1000 milliseconds, THE Blink_Sketch SHALL display the Blank_Frame, in which all 96 pixels are off, for 1000 milliseconds (±50 ms).
5. WHEN the Blank_Frame has been displayed for 1000 milliseconds, THE Blink_Sketch SHALL display the Heart_Frame again.
6. WHILE the board is powered, THE Blink_Sketch SHALL repeat the Blink_Cycle, where one Blink_Cycle is the Heart_Frame for 1000 milliseconds followed by the Blank_Frame for 1000 milliseconds, without stopping or changing the frame content.
7. IF the LED_Matrix fails to initialize, THEN THE Blink_Sketch SHALL NOT display any frame and SHALL leave all 96 pixels off.

### Requirement 2: Compile and Upload to the Board

**User Story:** As a maker, I want the sketch compiled and uploaded to my board, so that I can see the animation without extra steps.

#### Acceptance Criteria

1. THE Blink_Sketch SHALL target the Arduino UNO R4 WiFi board (fully qualified board name `arduino:renesas_uno:unor4wifi`).
2. THE Blink_Sketch SHALL use the `Arduino_LED_Matrix` library bundled with the Arduino UNO R4 board package.
3. WHEN the Blink_Sketch is built, THE Build_Toolchain SHALL compile the Blink_Sketch with zero compilation errors and exit with a success status within 120 seconds.
4. WHEN compilation succeeds and exactly one Arduino UNO R4 WiFi is connected, THE Build_Toolchain SHALL upload the compiled Blink_Sketch to the board and report successful completion to the user within 60 seconds of starting the upload.
5. IF no Arduino UNO R4 WiFi is detected, THEN THE Build_Toolchain SHALL NOT start the upload, SHALL report an error message indicating that no board was found, and SHALL exit with a failure status.
6. IF the upload fails or does not complete within 60 seconds, THEN THE Build_Toolchain SHALL stop the upload, report an error message indicating the failure reason, and exit with a failure status.
7. IF compilation fails, THEN THE Build_Toolchain SHALL NOT start the upload, SHALL report the compiler error messages including the file and line of each error, and SHALL exit with a failure status.

### Requirement 3: Exclude Automated Testing

**User Story:** As a maker, I want the project to skip automated tests, so that delivery stays quick and simple.

#### Acceptance Criteria

1. THE Blink_Sketch project SHALL contain zero automated test files, where a test file is any source file whose purpose is to execute assertions against sketch behavior (for example, unit, integration, or simulation test sources) or any test-framework configuration file.
2. WHEN the maker starts a build, THE Build_Toolchain SHALL execute exactly two steps in order, compile and then upload, and SHALL execute zero test steps.
3. IF the compile step fails, THEN THE Build_Toolchain SHALL skip the upload step, report a failure indication that identifies the compile step as the failing step, and leave any previously uploaded program on the board unchanged.
4. IF the upload step fails, THEN THE Build_Toolchain SHALL report a failure indication that identifies the upload step as the failing step and SHALL NOT retry the upload automatically.
