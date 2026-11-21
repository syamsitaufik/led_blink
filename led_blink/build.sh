#!/usr/bin/env bash
# Build: secrets check, install library, compile, then upload. No tests, no upload retry.
FQBN="arduino:renesas_uno:unor4wifi"
SKETCH_DIR="$(cd "$(dirname "$0")" && pwd)"
UPLOAD_TIMEOUT=60

# Secrets check: the sketch includes arduino_secrets.h, which is git-ignored
if [ ! -f "$SKETCH_DIR/arduino_secrets.h" ]; then
  echo "arduino_secrets.h is missing: copy arduino_secrets.h.example to arduino_secrets.h and fill in your values" >&2
  exit 1
fi

# Step 1: install the MQTT library
echo "Step 1/3: install ArduinoMqttClient"
if ! arduino-cli lib install ArduinoMqttClient; then
  echo "library install step failed" >&2
  exit 1
fi

# Step 2: compile
echo "Step 2/3: compile"
if ! arduino-cli compile --fqbn "$FQBN" "$SKETCH_DIR"; then
  echo "compile step failed" >&2
  exit 1
fi

# Find the UNO R4 WiFi port
PORT="$(arduino-cli board list 2>/dev/null | awk -v f="$FQBN" 'index($0, f) { print $1; exit }')"
if [ -z "$PORT" ]; then
  echo "no board found" >&2
  exit 1
fi

# Step 2: upload with a 60 s timeout (portable: timeout, gtimeout, or shell watchdog)
echo "Step 3/3: upload to $PORT"
UPLOAD_CMD=(arduino-cli upload -p "$PORT" --fqbn "$FQBN" "$SKETCH_DIR")
if command -v timeout >/dev/null 2>&1; then
  timeout "$UPLOAD_TIMEOUT" "${UPLOAD_CMD[@]}"
  RC=$?
elif command -v gtimeout >/dev/null 2>&1; then
  gtimeout "$UPLOAD_TIMEOUT" "${UPLOAD_CMD[@]}"
  RC=$?
else
  "${UPLOAD_CMD[@]}" &
  UPLOAD_PID=$!
  FLAG="$(mktemp)"
  rm -f "$FLAG"
  ( sleep "$UPLOAD_TIMEOUT"; if kill -0 "$UPLOAD_PID" 2>/dev/null; then : > "$FLAG"; kill "$UPLOAD_PID" 2>/dev/null; fi ) &
  WATCHDOG_PID=$!
  wait "$UPLOAD_PID"
  RC=$?
  kill "$WATCHDOG_PID" 2>/dev/null
  wait "$WATCHDOG_PID" 2>/dev/null
  if [ -e "$FLAG" ]; then
    rm -f "$FLAG"
    RC=124
  fi
fi

if [ "$RC" -ne 0 ]; then
  if [ "$RC" -eq 124 ]; then
    REASON="timed out after ${UPLOAD_TIMEOUT}s"
  else
    REASON="arduino-cli upload exited with status $RC"
  fi
  echo "upload step failed: $REASON" >&2
  exit 1
fi

echo "Upload completed successfully"
