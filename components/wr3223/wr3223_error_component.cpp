#include "esphome/core/log.h"
#include "wr3223_error_component.h"


namespace esphome {
namespace wr3223 {

static const char *const TAG = "wr3223_error_component";

void WR3223ErrorComponent::setup() {
    ESP_LOGD(TAG, "WR3223ErrorComponent setup initialized");
  }
  
  #ifdef USE_BINARY_SENSOR
  void WR3223ErrorComponent::register_status_sensor(binary_sensor::BinarySensor *sensor) {
    this->errorStatus = sensor;
    ESP_LOGD(TAG, "Registered error status binary sensor");
  }
  #endif
  
  #ifdef USE_TEXT_SENSOR
  void WR3223ErrorComponent::register_text_sensor(text_sensor::TextSensor *sensor) {
    this->errorText = sensor;
    ESP_LOGD(TAG, "Registered error text sensor");
  }
  #endif

void WR3223ErrorComponent::update() {}

} // namespace wr3223
} // namespace esphome
