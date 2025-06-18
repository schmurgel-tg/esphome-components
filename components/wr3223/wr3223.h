#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "wr3223_connector.h"
#include "wr3223_relais_component.h"

namespace esphome
{
    namespace wr3223
    {

        class WR3223 : public PollingComponent
        {
        public:
            WR3223(uart::UARTComponent *parent, uint32_t update_interval = 5000)
                : PollingComponent(update_interval) {}

            void setup() override;
            void update() override;
            void dump_config() override;

            void set_connector(WR3223Connector *connector); 
            void set_relais_component(WR3223RelaisComponent *component); 

            bool is_bedienteil_aktiv();

            WR3223Connector *connector_{nullptr};
            WR3223RelaisComponent *relais_component_{nullptr};

        private:
            /// @brief nach einem stromausfall oder ähnlichem, verhalten wir uns anders im
            /// ersten Update
            bool fresh_start_{true};
        };

    } // namespace wr3223
} // namespace esphome
