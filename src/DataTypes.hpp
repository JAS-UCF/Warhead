#ifndef C5601963_DA06_46B2_8C3A_40EDBCE16198
#define C5601963_DA06_46B2_8C3A_40EDBCE16198
#include <Arduino.h>
typedef struct
{
  float temperature;
  String name;
} temperature_sensor_t;
typedef struct
{
  float pressure;
  String name;
} pressure_sensor_t;


typedef struct
{
} ;


typedef struct
{
  temperature_sensor_t t1;
  temperature_sensor_t t2;
  temperature_sensor_t t3;
  temperature_sensor_t t4;
} sensor_data_t;

#endif /* C5601963_DA06_46B2_8C3A_40EDBCE16198 */
