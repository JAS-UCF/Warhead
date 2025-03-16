/**
 * @file main.cpp
 * @author Peter Cross (peter.cross222@gmail.com)
 * @brief Main file to compute the launch key code, this will allow us to read in MQTT information, and handle sending information off to the nuclear football
 * @version 0.3
 * @date 2024-03-04
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "main.hpp"
#include <DataTypes.hpp>
#include <vector>
#include "RelayDrive.h"
#include "PressureTransducer.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include <Thermistor.h>
#include <NTC_Thermistor.h>
#include <ArduinoJson.h>
#include "max6675.h"
#include <Adafruit_AHTX0.h>
#include "HX711.h"
#include <ESP32Servo.h>

JsonDocument doc;

WiFiClient espClient;
PubSubClient client(espClient);

/*
Essentially, if we want to read from several different spi devices (MAX6675) we need to bridge miso mosi, vin & gnd, the only thing we do that is new and different, is that we need a dedicated pin for CS (Chip select) we can set this to any pin, but need to set it high whenever we want to read from that device
*/
// first things first, we need to add thr 4 thermocouples, and get them working
MAX6675 Thermocouple1(GPIO_NUM_14, GPIO_NUM_27, GPIO_NUM_12);
MAX6675 Thermocouple2(GPIO_NUM_14, GPIO_NUM_26, GPIO_NUM_12);
MAX6675 Thermocouple3(GPIO_NUM_14, GPIO_NUM_25, GPIO_NUM_12);
MAX6675 Thermocouple4(GPIO_NUM_14, GPIO_NUM_32, GPIO_NUM_12);

// add the 3 relay drives0p
RelayDrive r1(GPIO_NUM_5);
RelayDrive r2(GPIO_NUM_19);
RelayDrive r3(GPIO_NUM_18);

// add the 2 pressure transducers
PressureTransducer *waterPressure;
PressureTransducer *oilPressure;

// sensors
Adafruit_AHTX0 aht;
HX711 scale;
// servo
Servo fuelServo;
constexpr uint SERVO_MINu = 1000;
constexpr uint SERVO_MAXu = 2000;
constexpr gpio_num_t SERVO_PIN = GPIO_NUM_33;

sensor_data_t sens_data;
long scaleCalibration = 0;

// Pin definitions
constexpr uint WATER_TEMPERATURE_PIN = 15; // D4 on ESP32
constexpr uint OIL_TEMPERATURE_PIN = 15;   // D4 on ESP32

constexpr uint REFERENCE_RESISTANCE = 4800; // 10k + (10k || 10k thermistor)
constexpr uint NOMINAL_RESISTANCE = 10000;
constexpr uint NOMINAL_TEMPERATURE = 25;
constexpr uint B_VALUE = 3950;
constexpr uint ESP32_ANALOG_RESOLUTION = 4095;
constexpr uint ESP32_ADC_VREF_MV = 3300;

Thermistor *waterTemperature;
Thermistor *oilTemperature;
void setup()
{
  Serial.begin(115200);
  delay(2000);
  waterTemperature = new NTC_Thermistor_ESP32(
      WATER_TEMPERATURE_PIN,
      REFERENCE_RESISTANCE,
      NOMINAL_RESISTANCE,
      NOMINAL_TEMPERATURE,
      B_VALUE,
      ESP32_ADC_VREF_MV,
      ESP32_ANALOG_RESOLUTION);
  oilTemperature = new NTC_Thermistor_ESP32(
      OIL_TEMPERATURE_PIN,
      REFERENCE_RESISTANCE,
      NOMINAL_RESISTANCE,
      NOMINAL_TEMPERATURE,
      B_VALUE,
      ESP32_ADC_VREF_MV,
      ESP32_ANALOG_RESOLUTION);
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  // once serial is available
  aht.begin();
  // // set up mqtt
  // setup_wifi();
  // client.setServer(mqtt_server, 1883);
  // client.setCallback(callback);
  // long calibrationSum = 0;
  // for (int i = 0; i < 10; i++)
  // {
  //   delay(250);
  //   Serial.printf("Taking Reading %d/10\n",i);
  //   calibrationSum += scale.read_average(2);
  // }
  // scaleCalibration = calibrationSum / 10;

  // pinMode(ledPin, OUTPUT);
  // EXAMPLE SERVO DRIVE
  // fuelServo.attach(SERVO_PIN);
  // while (true)
  // {
  //   delay(3000);
  //   fuelServo.write(100);
  //   delay(3000);
  //   fuelServo.write(0);
  // }
  // r1.enable();

  // setup wtr and oil Pressure with their calibrations
  waterPressure = new PressureTransducer(GPIO_NUM_35, WTR_CAL);
  oilPressure = new PressureTransducer(GPIO_NUM_34, OIL_CAL);
}
// TODO build out the arduinoJSON messages to send
// TODO experiment with connecting to wifi, and see whats up
// TODO experiment with the 12v relay rails
// TODO check the water/oil pressure sensors
// TODO check the water/oil temperature sensors
// TODO experiment with MQTT, and maybe using one hosted on the internet
// TODO build out proper enumeration of data, and pushing it to the JSON string
// TODO work in some code to work with servos

// TODO work in subscriber for MQTT for enabling and disabling relay lines
// TODO work in some options, potentially for tuning scenarios or something idk
bool state;
void loop()
{
  r1.enable();
  Serial.println("ENABLE");
  delay(1000);
  r1.disable();
  Serial.println("DISABLE");
  delay(1000);

  // Serial.printf("Water Pressure: %f | Oil Pressure: %f \n", waterPressure.getReading(), oilPressure.getReading());
  // delay(500);
  // for (int posDegrees = 0; posDegrees <= 270; posDegrees++)
  // {
  //   fuelServo.write(posDegrees);
  //   Serial.println(posDegrees);
  //   delay(100);
  // }
  // test servo power drive

  // in this function, we should prepare all the data necessary, then prepare to send it to the mqtt server
  // sensors_event_t humidity, temp;
  // aht.getEvent(&humidity, &temp);
  // Serial.printf("temperature C:%5f, humidity:%5f\n",temp.temperature,humidity.relative_humidity);
  // delay(500);

  // // Completely disable I2C and reset pins
  // Wire.end();

  // // Force pin reset - set to INPUT with pullups disabled
  // pinMode(SDA, INPUT);
  // digitalWrite(SDA, LOW);  // Disable internal pullup
  // pinMode(SCL, INPUT);
  // digitalWrite(SCL, LOW);  // Disable internal pullup
  // delay(50);  // Give pins time to stabilize

  // // Configure for HX711 with explicit pin mode changes
  // pinMode(SDA, INPUT);     // Data pin for HX711
  // pinMode(SCL, OUTPUT);    // Clock pin for HX711
  // delay(50);

  // // Initialize the HX711 with the repurposed pins
  // scale.begin(SDA, SCL);

  // // Add both a timeout and a retry mechanism
  // int retries = 3;
  // bool scale_ready = false;

  // while (retries > 0 && !scale_ready) {
  //   unsigned long startTime = millis();

  //   while (!scale.is_ready() && (millis() - startTime < 1000)) {
  //     delay(300);
  //     // Serial.printf("Waiting on scale... Retry: %d\n", 4-retries);
  //   }

  //   if (scale.is_ready()) {
  //     scale_ready = true;
  //     break;
  //   }

  //   retries--;
  //   // Reset the HX711 connection between retries
  //   pinMode(SCL, OUTPUT);
  //   digitalWrite(SCL, HIGH);
  //   delay(100);
  //   digitalWrite(SCL, LOW);
  //   delay(100);
  // }

  // if (scale_ready) {
  //   // Read from the scale
  //   long reading = scale.read();
  //   Serial.printf("Scale: %ld\n", reading - scaleCalibration);
  // } else {
  //   Serial.println("Failed to get scale ready after multiple attempts");
  // }

  // // Reset pins before re-enabling I2C
  // pinMode(SDA, INPUT);
  // digitalWrite(SDA, HIGH);  // Restore pullup if needed for I2C
  // pinMode(SCL, INPUT);
  // digitalWrite(SCL, HIGH);  // Restore pullup if needed for I2C
  // delay(50);

  // // Re-enable I2C
  // Wire.begin();

  // // esp32 will read in the sensor data, then publish the data to mqtt
  // if (!client.connected())
  // {
  //   reconnect();
  // }
  // client.loop();

  // // read sensor data here
  // /*
  // Example :
  // doc["sensor1"] = sensor1_value;
  // doc["sensor2"] = sensor2_value;
  // doc["sensor3"] = sensor3_value;
  // doc["sensor4"] = sensor4_value;
  // */
  // get thermocouple data
  // JsonObject thermocouples = doc.createNestedObject("Thermocouples");
  // thermocouples["tc1"] = Thermocouple1.readCelsius();
  // thermocouples["tc2"] = Thermocouple2.readCelsius();
  // thermocouples["tc3"] = Thermocouple3.readCelsius();
  // thermocouples["tc4"] = Thermocouple4.readCelsius();

  // get ambient air stuff
  // sensors_event_t humidity, temp;
  // aht.getEvent(&humidity, &temp);

  // template for aht20
  // JsonObject ambient = doc.createNestedObject("Ambient");
  // ambient["temp"] = temp.temperature;
  // ambient["humd"] = humidity.relative_humidity;

  // // template for water stats
  //   JsonObject water = doc.createNestedObject("Water");
  //   // Convert analog readings to appropriate units (adjust conversion as needed)
  //   water["pressure"] = analogReadToWaterPressure(analogRead(waterPressurePin));
  //   water["temperatureC"] = analogReadToWaterTemp(analogRead(waterTempPin));
  // // emplate for oil stats
  //   JsonObject oil = doc.createNestedObject("Oil");
  //   oil["pressure"] = analogReadToOilPressure(analogRead(oilPressurePin));
  //   oil["temperature"] = analogReadToOilTemp(analogRead(oilTempPin));

  // template for force stats
  // doc["force"] = analogReadToForce(analogRead(forceRegulationPin));

  // String mqtt_message_temp;
  // serializeJson(doc, mqtt_message_temp);
  // client.publish("esp32/sensor_data", mqtt_message_temp.c_str());
}

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(const char *topic, const byte *message, const unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
}
void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("Launch Key"))
    {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}