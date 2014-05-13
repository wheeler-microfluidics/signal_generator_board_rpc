#ifndef ___NODE__H___
#define ___NODE__H___

#include <stdint.h>
#include "Array.h"
#include "Memory.h"
#include "DMFControlBoard.h"

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
  DMFControlBoard &board_;

  Node(DMFControlBoard &board) : board_(board) {}

  uint32_t total_ram_size() { return ram_size(); }
  uint32_t ram_free() { return free_memory(); }

  static const uint8_t NUMBER_OF_ADC_CHANNELS = 2;

  /**************************************************************
   * METHODS
   **************************************************************/

  uint8_t update_channel(const uint16_t channel, const uint8_t state) {
    return board_.update_channel(channel, state);
  }
#if 0
  void update_all_channels();
  void send_spi(uint8_t pin, uint8_t address, uint8_t data);
  uint8_t set_pot(uint8_t index, uint8_t value);
  uint8_t set_series_resistor(const uint8_t channel,
                              const uint8_t index);
  void load_config(bool use_defaults=false);
  void save_config();
  uint8_t set_waveform_voltage(const float output_vrms,
                               const bool wait_for_reply=true);
  uint8_t set_adc_prescaler(const uint8_t index);
#endif
};


#endif  // #ifndef ___NODE__H___
