#ifndef D6D16073_49D6_477E_937E_F573241C4654
#define D6D16073_49D6_477E_937E_F573241C4654
#include <Arduino.h>
class RelayDrive
{
private:
  bool state;
  gpio_num_t pin;

public:
  RelayDrive(gpio_num_t pin) : pin(pin)
  {
    state = false;
    pinMode(pin, OUTPUT);
    digitalWrite(pin, 0);
  }
  /**
   * @brief Enable the relay drive
   *
   */
  void enable()
  {
    digitalWrite(pin, HIGH);
    state = true;
  }
  /**
   * @brief Disable the relay drive
   *
   */
  void disable()
  {
    digitalWrite(pin, LOW);
    state = false;
  }
  /**
   * @brief Get the State object
   *
   * @return true
   * @return false
   */
  bool getState() const
  {
    return state;
  }
  /**
   * @brief Flip the state of the relay drive
   *
   */
  void flipState()
  {
    state = !state;
    digitalWrite(pin, state);
  }
  /**
   * @brief Write a defined state for the relay drive, true on, false off
   *
   * @param state on or off
   * @return true if new state is true
   * @return false if new state is false
   */
  bool write(bool newState)
  {
    this->state = newState;
    digitalWrite(pin, newState);
    return this->state;
  }
  ~RelayDrive()
  {
    delete &state;
    delete &pin;
  }
};

#endif /* D6D16073_49D6_477E_937E_F573241C4654 */
