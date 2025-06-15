#include "wr3223_status_component.h"
#include "esphome/core/log.h"
#include "wr3223_helper.h"

namespace esphome
{
    namespace wr3223
    {

        static const char *const TAG = "wr3223_status_component";

        void WR3223StatusComponent::setup()
        {
            if (holder_ != nullptr)
            {
                holder_->restore_state_sw();
            }
            write_status();
        }

        void WR3223StatusComponent::update() { write_status(); }

        void WR3223StatusComponent::write_status()
        {
            if (parent_ == nullptr || parent_->connector_ == nullptr ||
                holder_ == nullptr)
                return;
            std::string data = WR3223Helper::to_bitwise(holder_->getSwStatus());
            parent_->connector_->send_write_request(
                WR3223Commands::SW, data,
                [this](char *answer, bool success)
                {
                    ESP_LOGD(TAG, "Status write response: %s success=%d", answer, success);

                    parent_->connector_->send_request(
                        WR3223Commands::SW,
                        [this](char *resp, bool ok)
                        {
                            ESP_LOGD(TAG, "Status readback: %s success=%d", resp, ok);
                            if (ok && holder_->setSWStatus(resp))
                                holder_->save_state_sw();
                        });
                });
        }

    } // namespace wr3223
} // namespace esphome