#ifndef ___NODE__H___
#define ___NODE__H___

#include <stdint.h>
#include "Array.h"
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
  static const uint8_t REGISTER_COUNT = 5;
  static const uint8_t CHANNEL_COUNT = REGISTER_COUNT * 8;
  uint8_t state_of_channels_[REGISTER_COUNT];
  uint8_t pwm_state_[REGISTER_COUNT][16];
  boolean pwm_enabled_;
	
  uint32_t total_ram_size() { return ram_size(); }
  uint32_t ram_free() { return free_memory(); }

  bool get_state_of_channel(uint8_t channel) {
	uint8_t register_id = channel / 8;
	uint8_t bit = channel % 8;
	return (state_of_channels_[register_id] >> bit) & 0x1;
  }

  void set_state_of_all_channels(uint32_t four_bytes, uint8_t byte_five) {
	/* Load first four register states from `four_bytes` into
	 * `state_of_channels_` array. */
	for (uint8_t i = 0; i < 4; i++) {
        state_of_channels_[i] = (four_bytes >> (8 * i)) & 0x0FF;
    }
    /* Set state of most significant register with contents of `byte_five`. */
	state_of_channels_[4] = byte_five;
	if (!pwm_enabled_) {
		for (int i = REGISTER_COUNT-1; i >= 0; i--) {
			spi_write(state_of_channels_[i]);
		}
	}
  }

  bool get_pwm_enabled() {
	return pwm_enabled_;
  }

  void set_pwm_enabled(bool value) {
	pwm_enabled_ = value;
  }

void set_pwm_state(uint32_t four_bytes, uint8_t byte_five, uint16_t time_slice) {
	for (uint16_t j = 0; j < 16; j++) {
	  for (uint8_t i = 0; i < 4; i++) {
        pwm_state_[i][j] = (four_bytes >> (8 * i)) & 0x0FF;
      }
      /* Set state of most significant register with contents of `byte_five`. */
      pwm_state_[4][j] = byte_five;
    }
  }

  void spi_write(uint8_t byte) {
    digitalWrite(SS, LOW);
    SPI.transfer(byte);
    digitalWrite(SS, HIGH);
  }

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
    SPI.transfer(byte_five);
    for (int i = 24; i >= 0; i -= 8) {
      SPI.transfer((four_bytes >> i) & 0x0FF);
    }
    digitalWrite(SS, HIGH);
  }
};


#endif  // #ifndef ___NODE__H___
