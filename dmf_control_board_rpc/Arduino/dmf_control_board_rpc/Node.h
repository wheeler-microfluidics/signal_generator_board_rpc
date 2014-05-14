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

  /**************************************************************
   * METHODS
   **************************************************************/

  uint8_t update_channel(const uint16_t channel, const uint8_t state) {
    return board_.update_channel(channel, state);
  }
#if 0
  void update_all_channels() {
    board_.update_all_channels();
  }
  /* `get_state_of_all_channels`
   *
   * Return state of channels.
   *
   * ## TODO ##
   *
   * I/O multiplier switching board:
   *
   *  - Use 5 I2C requests to each switching board to load channel states.
   *
   * Arduino-based switching board:
   *
   *  - Use 5 I2C protocol buffer requests to each switching board to load
   *    channel states. */
  void get_state_of_all_channels();
#endif
  void send_spi(uint8_t pin, uint8_t address, uint8_t data) {
    board_.send_spi(pin, address, data);
  }
  uint8_t set_pot(uint8_t index, uint8_t value) {
    return board_.set_pot(index, value);
  }
  uint8_t set_series_resistor_index(const uint8_t channel, const uint8_t index) {
    return board_.set_series_resistor(channel, index);
  }
  int8_t series_resistor_index(const uint8_t channel) {
    switch(channel) {
      case 0:
        return board_.A0_series_resistor_index_;
        break;
      case 1:
        return board_.A1_series_resistor_index_;
        break;
      default:
        return -1;
        break;
    }
  }
#if 0
  void load_config(bool use_defaults=false);
  void save_config();
#endif
  int8_t set_waveform_voltage(float output_vrms, bool wait_for_reply=true) {
#if ___HARDWARE_MAJOR_VERSION___==1
    /* Return waveform-voltage from signal-generator board. */
    float step = output_vrms / board_.amplifier_gain_ * 2 * sqrt(2) / 4 * 255;
    if (output_vrms < 0 || step > 255) {
      return -1;
    } else {
      board_.waveform_voltage_ = output_vrms;
      board_.set_pot(board_.POT_INDEX_WAVEOUT_GAIN_2_, step);
      return 0;
    }
#else
    /* ## TODO ##
     *
     * Set waveform-voltage on signal-generator board. */
#endif
    return 0;
  }

  int8_t set_waveform_frequency(float frequency) {
#if ___HARDWARE_MAJOR_VERSION___ == 1
        // The frequency of the LTC6904 oscillator needs to be set to 50x the
        // fundamental frequency
        board_.waveform_frequency_ = frequency;
        float freq = board_.waveform_frequency_ * 50;
        // valid frequencies are 1kHz to 68MHz
        if (freq < 1e3 || freq > 68e6) {
          return -1;
        } else {
          uint8_t oct = 3.322 * log(freq / 1039) / log(10);
          uint16_t dac = round(2048 - (2078 * pow(2, 10 + oct)) / freq);
          uint8_t cnf = 2; // CLK on, /CLK off
          // msb = OCT3 OCT2 OCT1 OCT0 DAC9 DAC8 DAC7 DAC6
          uint8_t msb = (oct << 4) | (dac >> 6);
          // lsb =  DAC5 DAC4 DAC3 DAC2 DAC1 DAC0 CNF1 CNF0
          uint8_t lsb = (dac << 2) | cnf;
          Wire.beginTransmission(board_.LTC6904_);
          Wire.write(msb);
          Wire.write(lsb);
          Wire.endTransmission();     // stop transmitting
          return 0;
        }
#else  // #if ___HARDWARE_MAJOR_VERSION___ == 1
    /* ## TODO ##
     *
     * Set waveform-frequency on signal-generator board. */
#endif
    return 0;
  }

  uint8_t set_adc_prescaler(const uint8_t index) {
    return board_.set_adc_prescaler(index);
  }

  uint8_t number_of_channels() const {
    return board_.number_of_channels_;
  }

  /* `get_state_of_all_channels`
   *
   * Return state of a single channel.
   *
   * ## TODO ##
   *
   * I/O multiplier switching board:
   *
   *  - Use I2C request to corresponding switching board to load channel
   *    state.
   *
   * Arduino-based switching board:
   *
   *  - Use I2C protocol buffer request to corresponding switching board to
   *    load channel state. */
  bool state_of_channel(uint16_t channel) const {
    return false;
  }

#if ___HARDWARE_MAJOR_VERSION___ == 1
  uint8_t waveform() const {
    return digitalRead(board_.WAVEFORM_SELECT_);
  }

  void set_waveform(uint16_t waveform_type) {
    if (waveform == board_.SINE || waveform == board_.SQUARE) {
      digitalWrite(board_.WAVEFORM_SELECT_, waveform_type);
    }
  }
#endif  //#if ___HARDWARE_MAJOR_VERSION___ == 1

  float waveform_voltage() const {
#if ___HARDWARE_MAJOR_VERSION___ == 1
  /* No signal generator board, so just return local waveform-voltage. */
    return board_.waveform_voltage_;
#else  // #if ___HARDWARE_MAJOR_VERSION___ == 1
  /* ## TODO ##
   *
   * Return waveform-voltage from signal-generator board. */
    return 0;
#endif  // #if ___HARDWARE_MAJOR_VERSION___ == 1
  }

  float waveform_frequency() const {
#if ___HARDWARE_MAJOR_VERSION___ == 1
  /* No signal generator board, so just return local waveform-voltage. */
    return board_.waveform_frequency;
#else  // #if ___HARDWARE_MAJOR_VERSION___ == 1
  /* ## TODO ##
   *
   * Return waveform-frequency from signal-generator board. */
    return 0;
#endif  // #if ___HARDWARE_MAJOR_VERSION___ == 1
  }

#ifdef AVR // only on Arduino Mega 2560
  float sampling_rate() const {
    return board_.SAMPLING_RATES_[board_.sampling_rate_index_];
  }

  void set_sampling_rate(uint8_t rate) { set_adc_prescaler(rate); }
#endif

  float series_resistance(uint8_t channel) {
    switch(channel) {
      case 0:
        return board_.config_settings_.A0_series_resistance
          [board_.A0_series_resistor_index_];
        break;
      case 1:
        return board_.config_settings_.A1_series_resistance
          [board_.A1_series_resistor_index_];
        break;
      default:
        break;
    }
    return -1;
  }

  int8_t set_series_resistance(uint8_t channel, float value) {
    switch(channel) {
      case 0:
        board_.config_settings_.A0_series_resistance
          [board_.A0_series_resistor_index_] = value;
        board_.save_config();
        return 0;
        break;
      case 1:
        board_.config_settings_.A1_series_resistance
          [board_.A1_series_resistor_index_] = value;
        board_.save_config();
        return 0;
        break;
      default:
        break;
    }
    return -1;
  }

  float series_capacitance(uint8_t channel) {
    switch(channel) {
      case 0:
        return board_.config_settings_.A0_series_capacitance
          [board_.A0_series_resistor_index_];
        break;
      case 1:
        return board_.config_settings_.A1_series_capacitance
          [board_.A1_series_resistor_index_];
        break;
      default:
        break;
    }
    return -1;
  }

  int8_t set_series_capacitance(uint8_t channel, float value) {
    switch(channel) {
      case 0:
        board_.config_settings_.A0_series_capacitance
          [board_.A0_series_resistor_index_] = value;
        board_.save_config();
        return 0;
        break;
      case 1:
        board_.config_settings_.A1_series_capacitance
          [board_.A1_series_resistor_index_] = value;
        board_.save_config();
        return 0;
        break;
      default:
        break;
    }
    return -1;
  }

  float amplifier_gain() const { return board_.amplifier_gain_; }

  void set_amplifier_gain(float value) {
    if (value > 0) {
      if (board_.auto_adjust_amplifier_gain_) {
        board_.amplifier_gain_ = value;
      } else {
        board_.config_settings_.amplifier_gain = value;
        board_.auto_adjust_amplifier_gain_ = false;
        board_.save_config();
      }
    }
  }

  bool auto_adjust_amplifier_gain() const {
    return board_.config_settings_.amplifier_gain <= 0;
  }

  void set_auto_adjust_amplifier_gain(bool value) {
    if (value) {
      board_.config_settings_.amplifier_gain = 0;
    } else {
      board_.config_settings_.amplifier_gain = board_.amplifier_gain_;
    }
    /* Trigger enabling of auto-adjust amplifier gain based on rules in
     * `save_config`. */
    board_.save_config();
  }

  float measure_impedance(uint16_t sampling_time_ms, uint16_t n_samples,
                          uint16_t delay_between_samples_ms) {
    board_.measure_impedance(sampling_time_ms, n_samples,
                             delay_between_samples_ms);
  }

  void reset_config_to_defaults() { board_.load_config(true); }

#if (___HARDWARE_MAJOR_VERSION___ == 1 && ___HARDWARE_MINOR_VERSION___ > 1) ||\
        ___HARDWARE_MAJOR_VERSION___ == 2
  uint8_t power_supply_pin() const {
    return board_.POWER_SUPPLY_ON_PIN_;
  }
#endif

  bool watchdog_enabled() const { return board_.watchdog_enabled(); }
  void set_watchdog_enabled(bool value) const { board_.watchdog_enabled(value); }
  bool watchdog_state() const { return board_.watchdog_state(); }
  void set_watchdog_state(bool value) const { board_.watchdog_state(value); }
#ifdef ATX_POWER_SUPPLY
  bool atx_power_state() const { return board_.atx_power_state(); }
  void set_atx_power_state(bool value) const {
    if (value) {
      board_.atx_power_on();
    } else {
      board_.atx_power_off();
    }
  }
#endif  // ATX_POWER_SUPPLY
};


#endif  // #ifndef ___NODE__H___
