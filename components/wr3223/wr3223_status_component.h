#pragma once

#include "esphome/core/component.h"
#include "wr3223.h"
#include "wr3223_constants.h"
#include "wr3223_status_value_holder.h"
#include <vector>

namespace esphome
{
    namespace wr3223
    {
        class WR3223StatusControl
        {
        public:
            virtual void on_status(WR3223StatusValueHolder *holder) = 0;
        };

        class WR3223StatusComponent : public PollingComponent, public WR3223StartUpListener
        {
        public:
            WR3223StatusComponent(WR3223 *parent, uint32_t update_interval,
                                  WR3223StatusValueHolder *holder)
                : PollingComponent(update_interval), parent_(parent), holder_(holder) {}

            void setup() override;
            void update() override;

            void on_startup() override;

            /// @brief Expose the internally used value holder
            WR3223StatusValueHolder *get_holder() const { return holder_; }

            /// @brief Register a control that depends on the status bits
            void register_status_control(WR3223StatusControl *control) { controls_.push_back(control); }

            /// @brief Write the current holder status to the device
            void write_status();

        protected:
            void notify_controls();

        private:
            WR3223 *parent_;
            WR3223StatusValueHolder *holder_;
            std::vector<WR3223StatusControl *> controls_;
        };

    } // namespace wr3223
} // namespace esphome