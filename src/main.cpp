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
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include <ArduinoJson.h>
#include "max6675.h"
#include <Adafruit_AHTX0.h>
#include "HX711.h"
JsonDocument doc;

WiFiClient espClient;
PubSubClient client(espClient);

// LED Pin
const int ledPin = 4;
/*
Essentially, if we want to read from several different spi devices (MAX6675) we need to bridge miso mosi, vin & gnd, the only thing we do that is new and different, is that we need a dedicated pin for CS (Chip select) we can set this to any pin, but need to set it high whenever we want to read from that device
*/
// first things first, we need to add thr 4 thermocouples, and get them working
MAX6675 Thermocouple1(GPIO_NUM_14,GPIO_NUM_27,GPIO_NUM_12);
MAX6675 Thermocouple2(GPIO_NUM_14,GPIO_NUM_26,GPIO_NUM_12);
MAX6675 Thermocouple3(GPIO_NUM_14,GPIO_NUM_25,GPIO_NUM_12);
MAX6675 Thermocouple4(GPIO_NUM_14,GPIO_NUM_32,GPIO_NUM_12);
// ambient temperature and humidity
Adafruit_AHTX0 aht;
HX711 scale;

sensor_data_t sens_data;
void setup()
{
  Serial.begin(115200);
  delay(2000);
  // once serial is available
  aht.begin();
  // // set up mqtt
  // setup_wifi();
  // client.setServer(mqtt_server, 1883);
  // client.setCallback(callback);

  // pinMode(ledPin, OUTPUT);
}
// TODO build out the arduinoJSON messages to send
// TODO experiment with connecting to wifi, and see whats up
// TODO experiment with the 12v relay rails
// TODO check the water/oil pressure sensors
// TODO check the water/oil temperature sensors
// TODO experiment with MQTT, and maybe using one hosted on the internet
// TODO build out proper enumeration of data, and pushing it to the JSON string

void loop()
{

  // in this function, we should prepare all the data necessary, then prepare to send it to the mqtt server
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  Serial.printf("t:%5f,h:%5f\n",temp.temperature,humidity.relative_humidity);
  delay(500);

  // Completely disable I2C and reset pins
  Wire.end();
  Serial.println("End Wire");
  
  // Force pin reset - set to INPUT with pullups disabled
  pinMode(SDA, INPUT);
  digitalWrite(SDA, LOW);  // Disable internal pullup
  pinMode(SCL, INPUT);
  digitalWrite(SCL, LOW);  // Disable internal pullup
  delay(50);  // Give pins time to stabilize
  
  // Configure for HX711 with explicit pin mode changes
  pinMode(SDA, INPUT);     // Data pin for HX711
  pinMode(SCL, OUTPUT);    // Clock pin for HX711
  delay(50);
  
  // Initialize the HX711 with the repurposed pins
  scale.begin(SDA, SCL);
  Serial.println("Scale initialized");
  
  // Add both a timeout and a retry mechanism
  int retries = 3;
  bool scale_ready = false;
  
  while (retries > 0 && !scale_ready) {
    unsigned long startTime = millis();
    
    while (!scale.is_ready() && (millis() - startTime < 1000)) {
      delay(300);
      Serial.printf("Waiting on scale... Retry: %d\n", 4-retries);
    }
    
    if (scale.is_ready()) {
      scale_ready = true;
      break;
    }
    
    retries--;
    // Reset the HX711 connection between retries
    pinMode(SCL, OUTPUT);
    digitalWrite(SCL, HIGH);
    delay(100);
    digitalWrite(SCL, LOW);
    delay(100);
  }
  
  if (scale_ready) {
    // Read from the scale
    long reading = scale.read();
    Serial.printf("Scale: %ld\n", reading);
  } else {
    Serial.println("Failed to get scale ready after multiple attempts");
  }
  
  // Reset pins before re-enabling I2C
  pinMode(SDA, INPUT);
  digitalWrite(SDA, HIGH);  // Restore pullup if needed for I2C
  pinMode(SCL, INPUT);
  digitalWrite(SCL, HIGH);  // Restore pullup if needed for I2C
  delay(50);
  
  // Re-enable I2C
  Wire.begin();

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
  if (String(topic) == "esp32/output")
  {
    Serial.print("Changing output to ");
    if (messageTemp == "on")
    {
      Serial.println("on");
      digitalWrite(ledPin, HIGH);
    }
    else if (messageTemp == "off")
    {
      Serial.println("off");
      digitalWrite(ledPin, LOW);
    }
  }
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