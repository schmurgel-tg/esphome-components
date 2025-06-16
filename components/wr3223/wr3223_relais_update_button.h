#pragma once

#include "esphome/components/button/button.h"
#include "esphome/core/component.h"
#include "wr3223_relais_component.h"

namespace esphome
{
    namespace wr3223
    {

        class WR3223RelaisUpdateButton : public button::Button, public Component
        {
        public:
            explicit WR3223RelaisUpdateButton(WR3223RelaisComponent *relais)
                : relais_(relais) {}

            void dump_config() override;

        protected:
            void press_action() override;
            WR3223RelaisComponent *relais_{nullptr};
        };

    } // namespace wr3223
} // namespace esphome