#include "wr3223_ventilation_select.h"
#include "esphome/core/log.h"
#include "wr3223_helper.h"

namespace esphome
{
    namespace wr3223
    {

        static const char *const TAG = "wr3223_vent_select";

        void WR3223VentilationLevelSelect::update()
        {
            if (this->parent_ == nullptr || this->parent_->connector_ == nullptr)
                return;

            this->parent_->connector_->send_request(
                WR3223Commands::LS,
                [this](char *resp, bool success)
                {
                    if (!success || resp == nullptr)
                    {
                        ESP_LOGW(TAG, "Failed to read LS value");
                        return;
                    }
                    int level = WR3223Helper::to_int(resp, true);
                    auto options = this->traits.get_options();
                    if (level < 0 || level >= static_cast<int>(options.size()))
                    {
                        ESP_LOGW(TAG, "Unexpected LS value: %d", level);
                        return;
                    }
                    this->publish_state(options[level]);
                });
        }

        void WR3223VentilationLevelSelect::control(const std::string &value)
        {
            auto idx = this->index_of(value);
            if (!idx.has_value())
            {
                ESP_LOGW(TAG, "Unknown ventilation option: %s", value.c_str());
                return;
            }
            int level = *idx;

            if (this->status_ != nullptr)
            {
                auto holder = this->status_->get_holder();
                if (holder != nullptr)
                {
                    holder->setVentilationLevel(level);
                    this->status_->write_status();
                }
            }

            this->publish_state(value);
        }

    } // namespace wr3223
} // namespace esphome