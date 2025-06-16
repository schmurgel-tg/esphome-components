#pragma once

#include "esphome/core/component.h"
#include "wr3223.h"
#include "wr3223_constants.h"
#include "wr3223_mode_value_holder.h"
#include <vector>

namespace esphome
{
    namespace wr3223
    {

        class WR3223ModeControl
        {
        public:
            virtual void on_mode(WR3223ModeValueHolder *holder) = 0;
        };

        class WR3223ModeComponent : public PollingComponent
        {
        public:
            WR3223ModeComponent(WR3223 *parent, uint32_t update_interval,
                                WR3223ModeValueHolder *holder)
                : PollingComponent(update_interval), parent_(parent), holder_(holder) {}

            void setup() override;
            void update() override;

            WR3223ModeValueHolder *get_holder() const { return holder_; }

            void register_mode_control(WR3223ModeControl *control) { controls_.push_back(control); }

            void write_mode();

        protected:
            void notify_controls();

        private:
            WR3223 *parent_;
            WR3223ModeValueHolder *holder_;
            std::vector<WR3223ModeControl *> controls_;
        };

    } // namespace wr3223
} // namespace esphome