#pragma once

#include "esphome/components/select/select.h"
#include "esphome/core/component.h"
#include "wr3223.h"
#include "wr3223_constants.h"
#include "wr3223_mode_component.h"

namespace esphome
{
    namespace wr3223
    {

        class WR3223ModeSelect : public select::Select,
                                 public Component,
                                 public WR3223ModeControl
        {
        public:
            WR3223ModeSelect(WR3223 *parent, WR3223ModeComponent *mode)
                : parent_(parent), mode_(mode) {}

            void setup() override;
            float get_setup_priority() const override { return setup_priority::DATA; }

        protected:
            void control(const std::string &value) override;
            void on_mode(WR3223ModeValueHolder *holder) override;

            WR3223 *parent_;
            WR3223ModeComponent *mode_;
        };

    } // namespace wr3223
} // namespace esphome