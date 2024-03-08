#ifndef C5601963_DA06_46B2_8C3A_40EDBCE16198
#define C5601963_DA06_46B2_8C3A_40EDBCE16198
#include <Arduino.h>
using temperature_sensor_t = struct
{
  float temperature;
  String name;
};
using pressure_sensor_t = struct
{
  float pressure;
  String name;
};

using sensor_data_t = struct
{
  temperature_sensor_t t1;
  temperature_sensor_t t2;
  temperature_sensor_t t3;
  temperature_sensor_t t4;
};

#endif /* C5601963_DA06_46B2_8C3A_40EDBCE16198 */
