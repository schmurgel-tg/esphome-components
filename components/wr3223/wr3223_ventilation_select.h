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

        class WR3223VentilationLevelSelect : public select::Select,
                                             public Component,
                                             public WR3223StatusControl
        {
        public:
            WR3223VentilationLevelSelect(WR3223 *parent, WR3223StatusComponent *status)
                : parent_(parent), status_(status) {}

            void setup() override;            
            float get_setup_priority() const override { return setup_priority::DATA; }

        protected:
            void control(const std::string &value) override;
            void on_status(WR3223StatusValueHolder *holder) override;

            WR3223 *parent_;
            WR3223StatusComponent *status_;
        };

    } // namespace wr3223
} // namespace esphome