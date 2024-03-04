/**
 * @file main.cpp
 * @author Peter Cross (peter.cross222@gmail.com)
 * @brief Main file to compute the launch key code, this will allow us to read in BT information, and handle sending information off to the nuclear football
 * @version 0.1
 * @date 2024-03-04
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <Arduino.h>
#include <BluetoothSerial.h>
#include <DataTypes.hpp>
#include <ArduinoJson.h>
#include "fifo.h"

BluetoothSerial SerialBT;
std::vector<queue_item> fifo_queue;

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
    if (inputText[0] == '!')
    {
      // if the message contains the proper starting keyLeter, then we will begin computing shit
      // begin the if else chain from this point forward
      if (strcmp(inputText, "!START~"))
      {
        // this command means we need to start the motor, so any applications or function required to start the motor can be placed here
      }
      else if (strcmp(inputText, "!STOP~"))
      {
        // This command signals that we need to turn off everything asap, any functionality for controling such things should go here
      }
      // the message might consist of the callsigns for changing the thrust, write that here
    }
  }
}

// The game plan from here is to read in sensor infomation as soon as we can, then, parse it into a message to send into the BT queue, then send the message over to the client on the other end
// Messages recieved in the loop() function can be read in, and will have to be computed
