#pragma once

#include "esphome/core/preferences.h"
#include "wr3223_helper.h"
#include "esphome/core/log.h"

namespace esphome {
namespace wr3223 {

enum WR3223EnumStatusSW : char {
  OFF = 0b00000000,
  HEAT_PUMP_OFF = 0b00000001,
  VENT_LEVEL_1 = 0b00000110,
  VENT_LEVEL_2 = 0b00000010,
  VENT_LEVEL_3 = 0b00000100,
  ADDITIONAL_HEATING_OFF = 0b00001000,
  VENT_LEVEL_0 = 0b00010000,
  COOLING_OFF = 0b00100000,

  STATUS_MASK_SW = 0b00111111
};

enum WR3223EnumStatusTa : char {
  MALFUNCTION = 0b00010000,
  HEAT_PUMP_STATUS = 0b00100000,
  ADDITIONAL_HEATER_STATUS = 0b01000000,

  STATUS_MASK_TA = 0b01110000
};

class WR3223StatusValueHolder {
 public:
  WR3223StatusValueHolder() : pref_(global_preferences->make_preference<int>(0x01)) {}

  void setHeatPumpOn(bool on) {
    if (on)
      stateValueSW &= ~WR3223EnumStatusSW::HEAT_PUMP_OFF;
    else
      stateValueSW |= WR3223EnumStatusSW::HEAT_PUMP_OFF;
  }

  void setVentilationLevel(int level) {
    stateValueSW &= ~(WR3223EnumStatusSW::VENT_LEVEL_0 |
                       WR3223EnumStatusSW::VENT_LEVEL_1 |
                       WR3223EnumStatusSW::VENT_LEVEL_2 |
                       WR3223EnumStatusSW::VENT_LEVEL_3);
    switch (level) {
      case 1:
        stateValueSW |= WR3223EnumStatusSW::VENT_LEVEL_1;
        break;
      case 2:
        stateValueSW |= WR3223EnumStatusSW::VENT_LEVEL_2;
        break;
      case 3:
        stateValueSW |= WR3223EnumStatusSW::VENT_LEVEL_3;
        break;
      default:
        stateValueSW |= WR3223EnumStatusSW::VENT_LEVEL_0;
        break;
    }
  }

  void setAdditionalHeatingOn(bool on) {
    if (on)
      stateValueSW &= ~WR3223EnumStatusSW::ADDITIONAL_HEATING_OFF;
    else
      stateValueSW |= WR3223EnumStatusSW::ADDITIONAL_HEATING_OFF;
  }

  void setCoolingOn(bool on) {
    if (on) {
      stateValueSW &= ~WR3223EnumStatusSW::COOLING_OFF;
      stateValueSW &= ~WR3223EnumStatusSW::HEAT_PUMP_OFF;
    } else {
      stateValueSW |= WR3223EnumStatusSW::COOLING_OFF;
      stateValueSW |= WR3223EnumStatusSW::HEAT_PUMP_OFF;
    }
  }

  bool getCoolingOnStatus() const { return !(stateValueSW & WR3223EnumStatusSW::COOLING_OFF); }
  bool isMalfunction() const { return stateValueTa & WR3223EnumStatusTa::MALFUNCTION; }
  bool getAdditionalHeatingOnStatus() const { return stateValueTa & WR3223EnumStatusTa::ADDITIONAL_HEATER_STATUS; }
  bool getHeatPumpOnStatus() const { return stateValueTa & WR3223EnumStatusTa::HEAT_PUMP_STATUS; }

  void restore_state_sw() { pref_.load(&stateValueSW); }
  void save_state_sw() { pref_.save(&stateValueSW); }

  int getSwStatus() const { return stateValueSW; }

  bool setSWStatus(const char *read) {
    try {
      int value = WR3223Helper::to_int(read, true);
      ESP_LOGD("VALUEHOLDER", "setSWStaus: %d", value);
      stateValueSW = value & WR3223EnumStatusSW::STATUS_MASK_SW;
      ESP_LOGD("VALUEHOLDER", "setSWStaus (masked): %d", stateValueSW);
      return true;
    } catch (const std::exception &e) {
      ESP_LOGE("STATUS HOLDER", "Exception in setSWStaus: %s (SetValue: %s)", e.what(), read);
      return false;
    }
  }

  bool setTaStatus(const char *read) {
    try {
      int value = WR3223Helper::to_int(read, true);
      ESP_LOGD("VALUEHOLDER", "setTaStatus: %d", value);
      stateValueTa = value & WR3223EnumStatusTa::STATUS_MASK_TA;
      ESP_LOGD("VALUEHOLDER", "setTaStatus (masked): %d", stateValueTa);
      return true;
    } catch (const std::exception &e) {
      ESP_LOGE("STATUS HOLDER", "Exception in setTaStatus: %s (SetValue: %s)", e.what(), read);
      return false;
    }
  }

 private:
  ESPPreferenceObject pref_;
  int stateValueSW{0};
  int stateValueTa{0};
};

}  // namespace wr3223
}  // namespace esphome