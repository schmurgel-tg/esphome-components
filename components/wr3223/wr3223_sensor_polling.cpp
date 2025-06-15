#include "wr3223_sensor_polling.h"
#include "esphome/core/log.h"

namespace esphome
{
    namespace wr3223
    {

        static const char *const TAG = "wr3223_sensor_polling";

        void WR3223SensorPollingComponent::update()
        {
            ESP_LOGD(TAG, "Sende Anfrage für Kommando: %s", command_);

            this->parent_->connector_->send_request(command_, [this](char *response, bool success)
                                                    {
        if (success)
            this->process_response(response);
        else
            ESP_LOGW(TAG, "Command %s: Timeout", command_); });
        }

        void WR3223SensorPollingComponent::process_response(char *response)
        {
            if (!response || strlen(response) == 0)
            {
                ESP_LOGW(TAG, "Command %s: Keine Antwort vom WR3223 erhalten.", command_);
                return;
            }

            ESP_LOGI(TAG, "Command %s: Antwort = %s", command_, response);

            if (sensor_)
            {
                float value = atof(response);
                ESP_LOGD(TAG, "Sensorwert für %s: %f", command_, value);
                sensor_->publish_state(value);
            }
            else if (binary_sensor_)
            {
                bool state = (strcmp(response, "1") == 0);
                ESP_LOGD(TAG, "BinarySensor für %s: %d", command_, state);
                binary_sensor_->publish_state(state);
            }
            else if (text_sensor_)
            {
                ESP_LOGD(TAG, "TextSensor für %s: %s", command_, response);
                text_sensor_->publish_state(response);
            }
        }

    } // namespace wr3223
} // namespace esphome
