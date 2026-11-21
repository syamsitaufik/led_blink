# Requirements Document

## Introduction

This feature changes the existing led_blink sketch (a blinking heart on the built-in 12x8 LED matrix of the Arduino UNO R4 WiFi) so that the board is controlled over MQTT. At startup the board connects to a Wi-Fi network, connects once to the public MQTT broker test.mosquitto.org, and subscribes to the topic bandungsoda/kiro/led/command. The payload HEART displays a heart and the payload SMILE displays a smiley face on the LED matrix. Wi-Fi credentials are kept out of version-controlled source files. The whole development process (requirements, design, implementation, compilation, firmware upload) uses Kiro and Arduino CLI, with no Arduino IDE.

The Sketch also prints progress and received commands to the serial port, and it reconnects to Wi-Fi and the Broker when a connection is lost.

Out of scope: automated tests and manual hardware verification.

## Glossary

- **Sketch**: The Arduino program in led_blink/led_blink.ino that runs on the Board.
- **Board**: The Arduino UNO R4 WiFi, fully qualified board name (FQBN) arduino:renesas_uno:unor4wifi.
- **Matrix**: The built-in 12-column by 8-row LED matrix of the Board.
- **Wi-Fi_Client**: The Sketch component that connects the Board to the Wi-Fi network.
- **MQTT_Client**: The Sketch component that connects to the Broker, subscribes to the Command_Topic, and receives messages.
- **Broker**: The public MQTT broker at test.mosquitto.org.
- **Command_Topic**: The MQTT topic bandungsoda/kiro/led/command.
- **Command**: The text payload of a message received on the Command_Topic. The valid values are HEART and SMILE.
- **Display_Controller**: The Sketch component that renders a frame on the Matrix.
- **Serial_Log**: Text the Sketch prints to the Board's USB serial port at 115200 baud, viewed with a serial monitor.
- **Heart_Frame**: The 12x8 bitmap of a heart.
- **Smile_Frame**: The 12x8 bitmap of a smiley face.
- **Secrets_File**: The git-ignored header file led_blink/led_blink/arduino_secrets.h that defines the Wi-Fi SSID and password.
- **Secrets_Example_File**: The committed header file led_blink/led_blink/arduino_secrets.h.example that contains placeholder values for the Secrets_File.
- **Build_Script**: The script led_blink/build.sh that compiles and uploads the Sketch with Arduino CLI.
- **MQTT_Library**: The ArduinoMqttClient library installed through Arduino CLI.

## Requirements

### Requirement 1: Wi-Fi connection

**User Story:** As a developer, I want the Board to connect to my Wi-Fi network at startup, so that the Board can reach the Broker.

#### Acceptance Criteria

1. WHEN the Board starts, THE Wi_Fi_Client SHALL connect to the Wi-Fi network using the SSID and password defined in the Secrets_File.
2. WHILE the Board is not connected to the Wi-Fi network, THE Wi_Fi_Client SHALL retry the connection until it succeeds.
3. IF the Wi-Fi connection is lost after startup, THEN THE Wi_Fi_Client SHALL reconnect to the Wi-Fi network.

### Requirement 2: MQTT connection and subscription

**User Story:** As a developer, I want the Board to connect to the public MQTT broker and subscribe to the command topic, so that the Board receives LED commands.

#### Acceptance Criteria

1. WHEN the Wi-Fi connection is established, THE MQTT_Client SHALL connect to the Broker on port 1883.
2. WHEN the MQTT_Client has connected to the Broker, THE MQTT_Client SHALL subscribe to the Command_Topic.
3. WHEN a message arrives on the Command_Topic, THE MQTT_Client SHALL pass the message payload to the Display_Controller as a Command.
4. IF the connection to the Broker fails or is lost, THEN THE MQTT_Client SHALL retry the connection and subscribe to the Command_Topic again once it succeeds, waiting 3 seconds between failed attempts.

### Requirement 3: LED matrix commands

**User Story:** As a user, I want to send HEART or SMILE over MQTT, so that I can choose what the LED matrix shows.

#### Acceptance Criteria

1. WHEN the Display_Controller receives the Command HEART, THE Display_Controller SHALL render the Heart_Frame on the Matrix.
2. WHEN the Display_Controller receives the Command SMILE, THE Display_Controller SHALL render the Smile_Frame on the Matrix.
3. IF the Display_Controller receives a Command that is neither HEART nor SMILE, THEN THE Display_Controller SHALL keep the Matrix content unchanged.
4. WHEN a Command is rendered, THE Display_Controller SHALL keep the rendered frame on the Matrix until the next valid Command arrives.
5. WHEN the Board starts, THE Display_Controller SHALL leave the Matrix off until the first valid Command arrives.

### Requirement 4: Serial log

**User Story:** As a developer, I want the Board to print what it is doing to the serial port, so that I can find out why a command has no effect.

#### Acceptance Criteria

1. WHEN the Board starts, THE Sketch SHALL open the serial port at 115200 baud, waiting up to 3 seconds for a serial monitor to attach, and SHALL print whether the Matrix initialised.
2. WHILE the Wi-Fi_Client is connecting, THE Sketch SHALL print each attempt with the SSID, and SHALL print the IP address once connected. The Sketch SHALL NOT print the Wi-Fi password.
3. WHEN the MQTT_Client connects, THE Sketch SHALL print the result. IF the connection fails, THEN THE Sketch SHALL print the error code. IF the subscription fails, THEN THE Sketch SHALL print that it failed.
4. WHEN a message arrives on the Command_Topic, THE Sketch SHALL print the topic, the payload size, the payload text, and the action taken (HEART rendered, SMILE rendered, unknown command ignored, or Matrix not ready).
5. WHEN a Wi-Fi connection is lost, THE Sketch SHALL print that it was lost before reconnecting.

### Requirement 5: Wi-Fi credentials outside version control

**User Story:** As a developer, I want Wi-Fi credentials kept out of version control, so that secrets are not published in the repository.

#### Acceptance Criteria

1. THE Sketch SHALL read the Wi-Fi SSID and password from the Secrets_File.
2. THE .gitignore file SHALL list the Secrets_File path.
3. THE repository SHALL contain the Secrets_Example_File with placeholder values for the Wi-Fi SSID and password.

### Requirement 6: Arduino CLI build and upload

**User Story:** As a developer, I want to build, install dependencies, and upload with Arduino CLI only, so that the whole workflow runs without the Arduino IDE.

#### Acceptance Criteria

1. WHEN the Build_Script runs, THE Build_Script SHALL install the MQTT_Library with Arduino CLI before compiling the Sketch.
2. WHEN the Build_Script runs, THE Build_Script SHALL compile the Sketch for the FQBN arduino:renesas_uno:unor4wifi with Arduino CLI.
3. WHEN compilation succeeds, THE Build_Script SHALL upload the compiled Sketch to the Board with Arduino CLI.
4. IF the Secrets_File does not exist, THEN THE Build_Script SHALL stop with a non-zero exit status and print a message that names the Secrets_Example_File.
5. IF the compile step or the upload step fails, THEN THE Build_Script SHALL exit with a non-zero status.
