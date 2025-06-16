#include "wr3223_vent_speed_number.h"
#include "wr3223_helper.h"
#include "esphome/core/log.h"

namespace esphome
{
    namespace wr3223
    {

        static const char *const TAG = "wr3223_vent_speed_number";

        const char *WR3223VentSpeedNumber::get_command() const
        {
            switch (level_)
            {
            case 1:
                return WR3223Commands::L1;
            case 2:
                return WR3223Commands::L2;
            case 3:
                return WR3223Commands::L3;
            default:
                return nullptr;
            }
        }

        void WR3223VentSpeedNumber::setup()
        {
            const char *cmd = get_command();
            if (cmd == nullptr || parent_ == nullptr || parent_->connector_ == nullptr)
                return;
            parent_->connector_->send_request(cmd, [this, cmd](char *resp, bool ok)
                                              {
                if (ok) {
                    int val = WR3223Helper::to_int(resp, true);
                    this->publish_state(val);
                } else {
                    ESP_LOGW(TAG, "Failed to read initial value for %s", cmd);
                } });
        }

        void WR3223VentSpeedNumber::control(float value)
        {
            const char *cmd = get_command();
            if (cmd == nullptr || parent_ == nullptr || parent_->connector_ == nullptr)
                return;
            int val = static_cast<int>(value);
            std::string data = std::to_string(val);
            parent_->connector_->send_write_request(cmd, data, [this, val](char *, bool ok)
                                                    {
            ESP_LOGD(TAG, "Write %d result %d", val, ok);
            if (ok)
                this->publish_state(val); });
        }

    } // namespace wr3223
} // namespace esphome