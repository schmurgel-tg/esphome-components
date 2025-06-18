#include "wr3223_mode_component.h"
#include "esphome/core/log.h"
#include "wr3223_helper.h"
#include "wr3223_connector.h"

namespace esphome
{
    namespace wr3223
    {

        static const char *const TAG = "wr3223_mode_component";

        void WR3223ModeComponent::setup()
        {
            if (parent_ != nullptr)
                parent_->register_startup_listener(this);

            if (holder_ != nullptr)
                holder_->restore_state();

            notify_controls();
        }

        void WR3223ModeComponent::update()
        {
            // mit den regulären Updates warten wir bis das Startup abgeschlossen wurde
            if (parent_->is_startup_completed())
                write_mode();
        }

        void WR3223ModeComponent::on_startup()
        {
            if (holder_ != nullptr && !parent_->is_bedienteil_aktiv())
            {
                holder_->restore_state();
                notify_controls();
                write_mode();
            }
        }

        void WR3223ModeComponent::write_mode()
        {
            if (parent_ == nullptr || parent_->connector_ == nullptr || holder_ == nullptr)
                return;

            if (parent_->is_bedienteil_aktiv())
            {
                ESP_LOGW(TAG, "Bedienteil aktiv - Schreiben nicht moeglich, lese Modus.");
                parent_->connector_->send_request(
                    WR3223Commands::MD,
                    [this](char *resp, bool ok)
                    {
                        ESP_LOGD(TAG, "Mode readback: %s success=%d", resp, ok);
                        if (ok)
                        {
                            holder_->set_mode_status(resp);
                            notify_controls();
                        }
                    });
                return;
            }

            std::string data = std::to_string(holder_->get_mode_status());
            parent_->connector_->send_write_request(
                WR3223Commands::MD, data,
                [this](char *answer, bool success)
                {
                    ESP_LOGD(TAG, "Mode write response: %s success=%d", answer, success);
                    if (!success)
                    {
                        parent_->connector_->send_request(
                            WR3223Commands::MD,
                            [this](char *resp, bool ok)
                            {
                                ESP_LOGD(TAG, "Mode readback: %s success=%d", resp, ok);
                                if (ok)
                                {
                                    holder_->set_mode_status(resp);
                                    notify_controls();
                                }
                            });
                    }
                    else
                    {
                        notify_controls();
                    }
                });
        }

        void WR3223ModeComponent::notify_controls()
        {
            for (auto *ctrl : controls_)
            {
                if (ctrl != nullptr)
                    ctrl->on_mode(holder_);
            }
        }

    } // namespace wr3223
} // namespace esphome