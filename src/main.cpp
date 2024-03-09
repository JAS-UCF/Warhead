/**
 * @file main.cpp
 * @author Peter Cross (peter.cross222@gmail.com)
 * @brief Main file to compute the launch key code, this will allow us to read in MQTT information, and handle sending information off to the nuclear football
 * @version 0.2
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

JsonDocument doc;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

// LED Pin
const int ledPin = 4;

sensor_data_t sens_data;
void setup()
{
  Serial.begin(115200);
  // set up mqtt
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  pinMode(ledPin, OUTPUT);
}

void loop()
{
  doc.clear();

  // esp32 will read in the sensor data, then publish the data to mqtt
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  // template of how to publish a message
  client.publish("esp32/status", "ALIVE");

  // read sensor data here
  /*
  Example :
  doc["sensor1"] = sensor1_value;
  doc["sensor2"] = sensor2_value;
  doc["sensor3"] = sensor3_value;
  doc["sensor4"] = sensor4_value;
  */

  String mqtt_message_temp;
  serializeJson(doc, mqtt_message_temp);
  client.publish("esp32/sensor_data", mqtt_message_temp.c_str());
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

void callback(char *topic, byte *message, unsigned int length)
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