#ifndef E7581552_74A3_421B_88BA_9866B150A112
#define E7581552_74A3_421B_88BA_9866B150A112
#include <Arduino.h>

#define OIL_CAL 0.0923076923077
#define WTR_CAL 0.105633802817

class PressureTransducer

{
private:
  uint8_t pin;
  double calibration;

public:
  PressureTransducer(uint8_t pin, double calibrationConst) : calibration(calibrationConst), pin(pin)
  {
    analogReadResolution(12);
    analogSetPinAttenuation(pin, ADC_11db); // For full range
    pinMode(pin, INPUT);
  }
  ~PressureTransducer()
  {
  }
  /**
   * @brief Get the latest reading
   *
   * @return float
   */
  float getReading()
  {
    return static_cast<float>((float)analogRead(pin) * this->calibration);
  }
};

#endif /* E7581552_74A3_421B_88BA_9866B150A112 */
