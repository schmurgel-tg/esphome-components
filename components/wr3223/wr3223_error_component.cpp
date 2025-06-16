#include "wr3223_error_component.h"
#include "esphome/core/log.h"

namespace esphome
{
  namespace wr3223
  {

    static const char *const TAG = "wr3223_error_component";

    // Fehlercode-Mapping für bessere Lesbarkeit
    static const std::map<int, std::string> ERROR_CODES = {
        {1, "Temp-Sensor Kurzschluss"},
        {2, "Offset-Fehler"},
        {3, "Temp-Sensor unterbrochen"},
        {4, "Hochdruckfehler"},
        {61, "Error Sys RAM"},
        {62, "Error Sys ROM"},
        {65, "Error Sys EE"},
        {66, "Error Sys IO"},
        {67, "Error Sys AD"},
        {128, "Zuluft fehlt"},
        {130, "Kondensationsfehler"},
        {132, "Abluft fehlt"},
        {133, "Vorheizfehler"}};

    void WR3223ErrorComponent::setup()
    {
      ESP_LOGD(TAG, "WR3223ErrorComponent wurde initialisiert.");
    }

    void WR3223ErrorComponent::update()
    {
      ESP_LOGD(TAG, "Sende Fehlerabfrage (ER-Kommando).");

      this->parent_->connector_->send_request(
          WR3223Commands::ER,
          [this](char *response, bool success)
          {
            if (success)
              this->process_response(response);
            else
              ESP_LOGW(TAG, "Fehlerabfrage Timeout");
          });
    }

    void WR3223ErrorComponent::process_response(char *response)
    {
      if (!response || strlen(response) == 0)
      {
        ESP_LOGW(TAG, "Keine Antwort vom WR3223 erhalten.");
        return;
      }

      ESP_LOGI(TAG, "Fehlercode empfangen: %s", response);

      // Fehlercode in eine Zahl umwandeln
      int error_code = atoi(response);
      bool has_error = (error_code != 0);
      std::string error_message;

      // Alle Fehler dekodieren
      for (const auto &entry : ERROR_CODES)
      {
        if (error_code & entry.first)
        {
          if (!error_message.empty())
          {
            error_message += ", "; // Fehler mit Komma trennen
          }
          error_message += entry.second;
        }
      }

      // Falls keine Fehler gefunden wurden
      if (error_message.empty())
      {
        error_message = "kein Fehler";
      }

      // Falls Sensoren registriert sind, setze Werte
      if (this->errorStatus)
      {
        this->errorStatus->publish_state(has_error);
      }
      if (this->errorText)
      {
        this->errorText->publish_state(error_message);
      }

      ESP_LOGI(TAG, "Erkannte Fehler: %s", error_message.c_str());
    }

    void WR3223ErrorComponent::register_status_sensor(
        binary_sensor::BinarySensor *sensor)
    {
      this->errorStatus = sensor;
      ESP_LOGD(TAG, "Registered error status binary sensor");
    }

    void WR3223ErrorComponent::register_text_sensor(
        text_sensor::TextSensor *sensor)
    {
      this->errorText = sensor;
      ESP_LOGD(TAG, "Registered error text sensor");
    }

  } // namespace wr3223
} // namespace esphome
