#include "wr3223_sensor_polling.h"
#include "esphome/core/log.h"

namespace esphome {
namespace wr3223 {

static const char *const TAG = "wr3223_temp_sensor";

void WR3223SensorPollingComponent::setup() {}

void WR3223SensorPollingComponent::update() {

  ESP_LOGD(TAG, "Update CommandPair: %s", cmd_pair.cmd);
  char response[20];
  int data_len =
      this->parent_->connector_.readLine(response, sizeof(response), cmd_pair);
  yield();
  if (data_len > 0) {
    std::string result(response);
    ESP_LOGI(TAG, "Sensor %s: Temperatur = %s", cmd_pair.cmd, result.c_str());
    cmd_pair.publish_data(result);
  } else {
    ESP_LOGW(TAG, "Sensor %s: Keine Antwort vom WR3223 erhalten.",
             cmd_pair.cmd);
  }
}

} // namespace wr3223
} // namespace esphome
