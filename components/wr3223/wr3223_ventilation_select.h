#pragma once

#include "esphome/components/select/select.h"
#include "esphome/core/component.h"
#include "wr3223.h"
#include "wr3223_constants.h"
#include "wr3223_status_component.h"

namespace esphome
{
    namespace wr3223
    {

        class WR3223VentilationLevelSelect : public select::Select, public PollingComponent
        {
        public:
            WR3223VentilationLevelSelect(WR3223 *parent, uint32_t update_interval, WR3223StatusComponent *status)
                : PollingComponent(update_interval), parent_(parent), status_(status) {}

            void setup() override {}
            void update() override;
            float get_setup_priority() const override { return setup_priority::DATA; }

        protected:
            void control(const std::string &value) override;

            WR3223 *parent_;
            WR3223StatusComponent *status_;
        };

    } // namespace wr3223
} // namespace esphome