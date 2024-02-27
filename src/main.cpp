#include <Arduino.h>
#include <BluetoothSerial.h>
#include <DataTypes.hpp>
#include <ArduinoJson.h>

BluetoothSerial SerialBT;
sensor_data_t sens_data;
void setup()
{
  SerialBT.begin("LaunchKey", true);
  Serial.begin(115200);
}

void loop()
{
  String inputText;

  if (SerialBT.available())
  {
    inputText = SerialBT.readStringUntil('~');
    Serial.println(inputText);
  }
}
