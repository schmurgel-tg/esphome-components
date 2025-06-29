#include "wr3223_number_control.h"
#include "wr3223_helper.h"
#include "wr3223_connector.h"
#include "esphome/core/log.h"

namespace esphome
{
    namespace wr3223
    {

        static const char *const TAG = "wr3223_number_control";

        WR3223NumberControl::WR3223NumberControl(WR3223 *parent, int level)
            : parent_(parent), command_(nullptr), use_float_(false)
        {
            switch (level)
            {
            case 1:
                command_ = WR3223Commands::L1;
                break;
            case 2:
                command_ = WR3223Commands::L2;
                break;
            case 3:
                command_ = WR3223Commands::L3;
                break;
            default:
                command_ = nullptr;
                break;
            }
        }

        void WR3223NumberControl::setup()
        {
            if (command_ == nullptr || parent_ == nullptr || parent_->connector_ == nullptr)
                return;
            parent_->connector_->send_request(command_, [this](char *resp, bool ok)
                                              {
                if (ok)
                {
                    float val = use_float_ ? WR3223Helper::to_float(resp)
                                           : static_cast<float>(WR3223Helper::to_int(resp, true));
                    this->publish_state(val);
                }
                else
                {
                    ESP_LOGW(TAG, "Failed to read initial value for %s", command_);
                } });
        }

        void WR3223NumberControl::control(float value)
        {
            if (command_ == nullptr || parent_ == nullptr || parent_->connector_ == nullptr)
                return;

            std::string data;
            if (use_float_)
            {
                data = WR3223Helper::to_string(value, 1);
            }
            else
            {
                int val = static_cast<int>(value);
                data = std::to_string(val);
            }

            parent_->connector_->send_write_request(command_, data, [this, value](char *, bool ok)
                                                    {
            ESP_LOGD(TAG, "Write %f result %d", value, ok);
                if (ok)
                    this->publish_state(value); });
        }

    } // namespace wr3223
} // namespace esphome