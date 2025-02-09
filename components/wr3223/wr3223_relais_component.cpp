#include "wr3223_relais_component.h"
#include "esphome/core/log.h"

namespace esphome {
namespace wr3223 {

static const char *TAG = "wr3223_relais";

void WR3223RelaisComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up WR3223 Relais Component...");
}

void WR3223RelaisComponent::update() {
  if (!this->parent_) {
    ESP_LOGE(TAG, "No WR3223 parent component found!");
    return;
  }

  char answer[20];
  int dataLength = this->parent_->connector_.readLine(answer, sizeof(answer),
                                                      WR3223Commands::RL);

  // Anzahl der registrierten Sensoren loggen
  ESP_LOGD(TAG, "Number of registered relais sensors: %d", relais_sensors_.size());


  if (dataLength < 1) {
    ESP_LOGW(TAG, "Failed to read relais status!");
    return;
  }
#ifdef USE_BINARY_SENSOR
  int relais_status = std::stoi(answer);

  ESP_LOGD(TAG, "Received relais status: %d", relais_status);

  // Sensoren anhand der Bit-Flags setzen
  for (auto it = relais_sensors_.begin(); it != relais_sensors_.end(); ++it) {
    int flag = it->first;
    auto sensor = it->second;
    bool state = (relais_status & flag) != 0;
    sensor->publish_state(state);
}
#endif
}

#ifdef USE_BINARY_SENSOR
void WR3223RelaisComponent::register_relais_sensor(
    int flag, binary_sensor::BinarySensor *sensor) {
  this->relais_sensors_[flag] = sensor;
}
#endif

} // namespace wr3223
} // namespace esphome
