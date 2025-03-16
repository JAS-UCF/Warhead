#ifndef BBF42581_A579_4323_9568_CBE7EF3CAA91
#define BBF42581_A579_4323_9568_CBE7EF3CAA91
#include <Arduino.h>
void setup_wifi();
void callback(const char *topic, const byte *message, const unsigned int length);
void reconnect();

// TODO: fix this math
float convertToPSI(int rawValue)
{
  // Convert ADC reading to voltage (ESP32 has 12-bit ADC = 4095 steps)
  float voltage = rawValue * (3.3 / 4095.0); // Voltage at the ESP32 pin

  // Account for the voltage divider (5K and 2K resistors)
  // Vout = Vin * (R2 / (R1 + R2)) => Vin = Vout * (R1 + R2) / R2
  float sensorVoltage = voltage * (5000.0 + 2000.0) / 2000.0;

  // Convert voltage to PSI using the sensor's spec
  // 0.5V = 0 PSI, 4.5V = 100 PSI
  // float psi = (sensorVoltage - 0.5) * (100.0 / 4.0);
  float psi = static_cast<float>(map(sensorVoltage, 0.5, 4.5, 0, 100));

  // Constrain output to valid range
  // if (psi < 0)
  //   psi = 0;
  // if (psi > 100)
  //   psi = 100;

  return psi;
}
#endif /* BBF42581_A579_4323_9568_CBE7EF3CAA91 */
