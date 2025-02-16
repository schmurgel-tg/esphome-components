#include "wr3223.h"
#include "esphome/core/log.h"

namespace esphome {
namespace wr3223 {

static const char *const TAG = "wr3223";

void WR3223::setup() {
    ESP_LOGI(TAG, "WR3223 Hauptkomponente Setup gestartet!");
}

void WR3223::update() {
    ESP_LOGI(TAG, "WR3223 Hauptkomponente Update ausgeführt!");
}

void WR3223::dump_config() {
    ESP_LOGCONFIG(TAG, "WR3223 Konfiguration:");
    ESP_LOGCONFIG(TAG, "  - Update Intervall: %d ms", this->get_update_interval());
}

void WR3223::set_connector(WR3223Connector *connector) {
    this->connector_ = connector;
    ESP_LOGD("WR3223", "WR3223Connector wurde erfolgreich gesetzt.");
}

}  // namespace wr3223
}  // namespace esphome
