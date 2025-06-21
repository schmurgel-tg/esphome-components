#include "wr3223.h"
#include "wr3223_relais_component.h"
#include "esphome/core/log.h"

namespace esphome
{
    namespace wr3223
    {
        static const char *const TAG = "wr3223";

        void WR3223::setup()
        {
            ESP_LOGI(TAG, "WR3223 Hauptkomponente Setup gestartet!");
        }

        void WR3223::update()
        {                        
            if (fresh_start_ && relais_component_ != nullptr)
            {
                ESP_LOGI(TAG, "WR3223 Hauptkomponente FreshStart im Update wird ausgeführt!");
                relais_component_->update();                
            }
        }

        void WR3223::dump_config()
        {
            ESP_LOGCONFIG(TAG, "WR3223 Konfiguration:");
            ESP_LOGCONFIG(TAG, "  - Update Intervall: %d ms", this->get_update_interval());
        }

        void WR3223::on_relais_update()
        {
            if (!fresh_start_)
                return;

            startup_counter_++;
            bool bd_active = is_bedienteil_aktiv();
            ESP_LOGD(TAG, "Relais update %u/%u bedienteil=%d", startup_counter_, max_restore_attempts_, bd_active);

            if (!bd_active)
            {
                ESP_LOGD(TAG, "Startup conditions met - notifying listeners");
                for (auto *listener : startup_listeners_)
                {
                    if (listener != nullptr)
                        listener->on_startup();
                }
                fresh_start_ = false;
            }
            else if (startup_counter_ >= max_restore_attempts_)
            {
                ESP_LOGW(TAG, "Startup failed after %u attempts", startup_counter_);
                fresh_start_ = false;
            }
        }

        bool WR3223::is_bedienteil_aktiv()
        {
            if (relais_component_ != nullptr)
                return relais_component_->is_bedienteil_aktiv();

            return true; // haben wir keinen Zugriff auf die RelaisComponent, so gilt der Schreibschutz
        }

    } // namespace wr3223
} // namespace esphome
