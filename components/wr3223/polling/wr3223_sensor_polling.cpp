#include "wr3223_sensor_polling.h"
#include "wr3223.h"
#include "esphome/core/log.h"

namespace esphome {
namespace wr3223 {

static const char *const TAG = "wr3223_temp_sensor";

void WR3223SensorPollingComponent::setup() {
  ESP_LOGI(TAG, "WR3223 Temperatur-Sensoren Setup gestartet!");
}

void WR3223SensorPollingComponent::update() {
  ESP_LOGI(TAG, "Temperaturabfrage gestartet...");

  for (auto &cmd_pair : this->command_pairs_) {
    char response[20];
    int data_len = this->parent_->connector_.readLine(response, sizeof(response), cmd_pair.cmd);

    if (data_len > 0) {
      std::string result(response);
      ESP_LOGI(TAG, "Sensor %s: Temperatur = %s", cmd_pair.cmd, result.c_str());
      cmd_pair.publish_data(result);
    } else {
      ESP_LOGW(TAG, "Sensor %s: Keine Antwort vom WR3223 erhalten.", cmd_pair.cmd);
    }
  }
}

void WR3223SensorPollingComponent::register_sensor(sensor::Sensor *sensor, const char *command) {
  this->command_pairs_.emplace_back(command, sensor);
}

}  // namespace wr3223
}  // namespace esphome
