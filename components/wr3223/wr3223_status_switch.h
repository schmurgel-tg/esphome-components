#pragma once

#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"
#include "wr3223_status_component.h"

namespace esphome
{
    namespace wr3223
    {

        class WR3223StatusSwitch : public switch_::Switch,
                                   public Component,
                                   public WR3223StatusControl
        {
        public:
            void set_status_component(WR3223StatusComponent *status) { status_ = status; }

            void setup() override;
            void write_state(bool state) override;
            void on_status(WR3223StatusValueHolder *holder) override;

        protected:
            WR3223StatusComponent *status_{nullptr};
            virtual void apply_state(WR3223StatusValueHolder *holder, bool state) = 0;
            virtual bool current_state(WR3223StatusValueHolder *holder) = 0;
        };

        class WR3223HeatPumpSwitch : public WR3223StatusSwitch
        {
        protected:
            void apply_state(WR3223StatusValueHolder *holder, bool state) override
            {
                holder->setHeatPumpOn(state);
            }
            bool current_state(WR3223StatusValueHolder *holder) override
            {
                return holder->getHeatPumpOnStatus();
            }
        };

        class WR3223AdditionalHeatingSwitch : public WR3223StatusSwitch
        {
        protected:
            void apply_state(WR3223StatusValueHolder *holder, bool state) override
            {
                holder->setAdditionalHeatingOn(state);
            }
            bool current_state(WR3223StatusValueHolder *holder) override
            {
                return holder->getAdditionalHeatingOnStatus();
            }
        };

        class WR3223CoolingSwitch : public WR3223StatusSwitch
        {
        protected:
            void apply_state(WR3223StatusValueHolder *holder, bool state) override
            {
                holder->setCoolingOn(state);
            }
            bool current_state(WR3223StatusValueHolder *holder) override
            {
                return holder->getCoolingOnStatus();
            }
        };

    } // namespace wr3223
} // namespace esphome