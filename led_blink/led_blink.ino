#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

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

bool matrixReady = false;

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
