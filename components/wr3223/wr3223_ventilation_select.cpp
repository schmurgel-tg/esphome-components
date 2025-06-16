#include "wr3223_ventilation_select.h"
#include "esphome/core/log.h"
#include "wr3223_helper.h"

namespace esphome
{
    namespace wr3223
    {

        static const char *const TAG = "wr3223_vent_select";

        void WR3223VentilationLevelSelect::setup()
        {
            if (this->status_ != nullptr)
            {
                this->status_->register_status_control(this);
                auto holder = this->status_->get_holder();
                if (holder != nullptr)
                {
                    this->on_status(holder);
                }
            }
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

        void WR3223VentilationLevelSelect::on_status(WR3223StatusValueHolder *holder)
        {
            if (holder == nullptr)
                return;

            int level = holder->getVentilationLevel();
            auto options = this->traits.get_options();
            if (level < 0 || level >= static_cast<int>(options.size()))
                return;

            this->publish_state(options[level]);
        }

    } // namespace wr3223
} // namespace esphome