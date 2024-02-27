#include <Arduino.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

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