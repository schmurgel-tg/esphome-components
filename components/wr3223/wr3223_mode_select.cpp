#include "wr3223_mode_select.h"
#include "esphome/core/log.h"
#include "wr3223_helper.h"

namespace esphome
{
    namespace wr3223
    {

        static const char *const TAG = "wr3223_mode_select";

        void WR3223ModeSelect::setup()
        {
            if (this->mode_ != nullptr)
            {
                this->mode_->register_mode_control(this);
                auto holder = this->mode_->get_holder();
                if (holder != nullptr)
                    this->on_mode(holder);
            }
        }

        void WR3223ModeSelect::control(const std::string &value)
        {
            auto idx = this->index_of(value);
            if (!idx.has_value())
            {
                ESP_LOGW(TAG, "Unknown mode option: %s", value.c_str());
                return;
            }
            int mode = *idx;

            if (this->mode_ != nullptr)
            {
                auto holder = this->mode_->get_holder();
                if (holder != nullptr)
                {
                    holder->set_mode(mode);
                    this->mode_->write_mode();
                }
            }

            this->publish_state(value);
        }

        void WR3223ModeSelect::on_mode(WR3223ModeValueHolder *holder)
        {
            if (holder == nullptr)
                return;

            int mode = holder->get_mode();
            auto options = this->traits.get_options();
            if (mode < 0 || mode >= static_cast<int>(options.size()))
                return;

            this->publish_state(options[mode]);
        }

    } // namespace wr3223
} // namespace esphome