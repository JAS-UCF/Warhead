# LaunchKey

esp32 code to handle communicating with the nuclear football

## Game Plan

ESP32 will connect over WiFi to the RPi
ESP32 will make connection with mosquito MQTT broker running on the PI
ESP32 will publish to relevant topics over MQTT
ESP32 will subscribe to topics relevant to activations / shutdowns
ESP32 will interface with 4 MAX6675 SPI K-Type thermocouple amplifiers
ESP32 will allow for turning off of core subsystems over GPIO pins provided
