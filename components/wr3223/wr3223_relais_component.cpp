#include "wr3223_relais_component.h"
#include "esphome/core/log.h"

namespace esphome
{
  namespace wr3223
  {

    static const char *TAG = "wr3223_relais";

    void WR3223RelaisComponent::setup()
    {
      ESP_LOGCONFIG(TAG, "Setting up WR3223 Relais Component...");
    }

    void WR3223RelaisComponent::update()
    {
      if (!this->connector_)
      {
        ESP_LOGE(TAG, "Kein gültiger WR3223-Connector gefunden!");
        return;
      }

      this->connector_->send_request(WR3223Commands::RL, [this](char *response, bool success)
                                              {
      if (success)
          this->process_response(response);
      else
          ESP_LOGW(TAG, "Relais Kommando Timeout"); });

      ESP_LOGD(TAG, "Relais-Status-Anfrage gesendet.");
    }

    /// @brief Verarbeitet die Antwort der Steuerung.
    /// @param response Antwort-String mit den reinen Datenbits.
    void WR3223RelaisComponent::process_response(char *response)
    {
      if (!response || strlen(response) == 0)
      {
        ESP_LOGE(TAG, "Antwort ist leer oder NULL!");
        return;
      }

      int relais_status = std::stoi(response); // Direkt die Datenbits als Integer interpretieren
      ESP_LOGD(TAG, "Erhaltener Relais-Status: %d", relais_status);

      // update flag for 'bedienteil aktiv'
      bedienteil_aktiv_ = (relais_status & 64) != 0;

      // Sensoren anhand der Bit-Flags setzen
      for (auto it = relais_sensors_.begin(); it != relais_sensors_.end(); ++it)
      {
        int flag = it->first;
        auto sensor = it->second;
        bool state = (relais_status & flag) != 0;
        sensor->publish_state(state);
      }
    }

    void WR3223RelaisComponent::register_relais_sensor(
        int flag, binary_sensor::BinarySensor *sensor)
    {
      this->relais_sensors_[flag] = sensor;
    }

  } // namespace wr3223
} // namespace esphome
