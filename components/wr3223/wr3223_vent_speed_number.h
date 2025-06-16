#pragma once

#include "esphome/components/number/number.h"
#include "esphome/core/component.h"
#include "wr3223.h"
#include "wr3223_constants.h"

namespace esphome
{
    namespace wr3223
    {

        class WR3223VentSpeedNumber : public number::Number, public Component
        {
        public:
            WR3223VentSpeedNumber(WR3223 *parent, int level) : parent_(parent), level_(level) {}

            void setup() override;
            float get_setup_priority() const override { return setup_priority::DATA; }

        protected:
            void control(float value) override;
            const char *get_command() const;

            WR3223 *parent_;
            int level_;
        };

    } // namespace wr3223
} // namespace esphome