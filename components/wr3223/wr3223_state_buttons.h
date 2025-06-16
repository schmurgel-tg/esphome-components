#pragma once

#include "esphome/components/button/button.h"
#include "esphome/core/component.h"
#include "wr3223_mode_component.h"
#include "wr3223_status_component.h"

namespace esphome
{
    namespace wr3223
    {
        class WR3223SaveStateButton : public button::Button, public Component
        {
        public:
            WR3223SaveStateButton(WR3223ModeComponent *mode, WR3223StatusComponent *status)
                : mode_(mode), status_(status) {}

            void dump_config() override;

        protected:
            void press_action() override;
            WR3223ModeComponent *mode_{nullptr};
            WR3223StatusComponent *status_{nullptr};
        };

        class WR3223RestoreStateButton : public button::Button, public Component
        {
        public:
            WR3223RestoreStateButton(WR3223ModeComponent *mode, WR3223StatusComponent *status)
                : mode_(mode), status_(status) {}

            void dump_config() override;

        protected:
            void press_action() override;
            WR3223ModeComponent *mode_{nullptr};
            WR3223StatusComponent *status_{nullptr};
        };

    } // namespace wr3223
} // namespace esphome