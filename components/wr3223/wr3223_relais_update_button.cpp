#include "wr3223_relais_update_button.h"
#include "esphome/core/log.h"

namespace esphome
{
    namespace wr3223
    {

        static const char *TAG = "wr3223_relais_button";

        void WR3223RelaisUpdateButton::dump_config()
        {
            LOG_BUTTON("", "WR3223 Relais Update Button", this);
        }

        void WR3223RelaisUpdateButton::press_action()
        {
            if (relais_ != nullptr)
            {
                ESP_LOGD(TAG, "Manuelle Relaisaktualisierung ausgelöst");
                relais_->update();
            }
        }

    } // namespace wr3223
} // namespace esphome