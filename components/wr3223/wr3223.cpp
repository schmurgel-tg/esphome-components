#include "wr3223.h"
#include "esphome/core/log.h"

namespace esphome {
namespace wr3223 {

static const char *const TAG = "wr3223";

void WR3223::setup() {
    ESP_LOGI(TAG, "WR3223 Setup gestartet!");
}

void WR3223::update() {
    ESP_LOGI(TAG, "WR3223 Update ausgeführt!");
}

void WR3223::dump_config() {
    ESP_LOGCONFIG(TAG, "WR3223 Konfiguration:");
    ESP_LOGCONFIG(TAG, "  - Update Intervall: %d ms", this->get_update_interval());
}

}  // namespace wr3223
}  // namespace esphome
