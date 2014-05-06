#ifndef ___NODE__H___
#define ___NODE__H___

#include <stdint.h>
#include "Memory.h"

/* # Arduino digital pins connected to shift registers #
 *
 * Note that both the _output-enable_ and _clear_ pins are _active low_,
 * meaning that setting the respective pin to logic `LOW` will trigger the
 * corresponding behavioural response.  For instance, driving the `SRCLR` pin
 * `LOW` will cause the shift register contents to be cleared, _i.e., set to
 * 0_. */
/* Output enable. */
#define OE            8
/* Clear. */
#define SRCLR         9


#ifndef DISABLE_I2C
/* Callback functions to respond to I2C communication. */
extern void i2c_receive_event(int byte_count);
extern void i2c_request_event();
#endif  // #ifndef DISABLE_I2C


class Node {
public:
  uint32_t total_ram_size() { return ram_size(); }
  uint32_t ram_data_size() { return data_size(); }
  uint32_t ram_bss_size() { return bss_size(); }
  uint32_t ram_heap_size() { return heap_size(); }
  uint32_t ram_stack_size() { return stack_size(); }
  uint32_t ram_free() { return free_memory(); }

  uint8_t high() const { return HIGH; }
  uint8_t low() const { return LOW; }
  uint8_t output() const { return OUTPUT; }
  uint8_t input() const { return INPUT; }

  void pin_mode(uint8_t pin, uint8_t mode) { return pinMode(pin, mode); }
  void delay_ms(uint32_t milliseconds) const { delay(milliseconds); }
  bool digital_read(uint8_t pin) const { return digitalRead(pin); }
  void digital_write(uint8_t pin, uint8_t value) { digitalWrite(pin, value); }
  uint16_t analog_read(uint8_t pin) const { return analogRead(pin); }
  void analog_write(uint8_t pin, uint8_t value) { return analogWrite(pin, value); }
  uint32_t get_millis() const { return millis(); }
  uint32_t get_micros() const { return micros(); }
#ifndef DISABLE_I2C
  int8_t forward_i2c_request(uint8_t address, uint8_t *request) const { return 0; }
#endif  // #ifndef DISABLE_I2C

  void shift_register_reset() {
    /* Clear the state of the shift-register. */
    digital_write(SRCLR, LOW);
    delay(1);
    digital_write(SRCLR, HIGH);
  }

  void spi_write(uint8_t byte) {
    digital_write(SS, LOW);
    SPI.transfer(byte);
    digital_write(SS, HIGH);
  }

#if 0
  void spi_write2(uint16_t two_bytes) {
    spi_write((two_bytes >> 8) & 0x0FF);
    spi_write(two_bytes & 0x0FF);
  }

  void spi_write4(uint32_t four_bytes) {
    spi_write2((four_bytes >> 16) & 0x0FFFF);
    spi_write2(four_bytes & 0x0FFFF);
  }

  void spi_write5(uint32_t four_bytes, uint8_t byte_five) {
    digitalWrite(SS, LOW);
    for (int i = 24; i >= 0; i -= 8) {
      SPI.transfer((four_bytes >> i) & 0x0FF);
    }
    SPI.transfer(byte_five);
    digitalWrite(SS, HIGH);
  }
#endif
};


#endif  // #ifndef ___NODE__H___
