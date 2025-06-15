#pragma once

#include "esphome/core/component.h"
#include "wr3223.h"
#include "wr3223_constants.h"
#include "wr3223_status_value_holder.h"

namespace esphome
{
    namespace wr3223
    {

        class WR3223StatusComponent : public PollingComponent
        {
        public:
            WR3223StatusComponent(WR3223 *parent, uint32_t update_interval,
                                  WR3223StatusValueHolder *holder)
                : PollingComponent(update_interval), parent_(parent), holder_(holder) {}

            void setup() override;
            void update() override;

        private:
            void write_status();

            WR3223 *parent_;
            WR3223StatusValueHolder *holder_;
        };

    } // namespace wr3223
} // namespace esphome