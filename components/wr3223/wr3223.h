#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include "wr3223_connector.h"
#include "wr3223_relais_component.h"
#include <vector>

namespace esphome
{
    namespace wr3223
    {

        class WR3223StartUpListener
        {
        public:
            virtual void on_startup() = 0;
        };

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

            void register_startup_listener(WR3223StartUpListener *listener) { startup_listeners_.push_back(listener); }

            void on_relais_update();

            bool is_bedienteil_aktiv();

            /// @brief liefert ob der Statup Prozess abgeschlossen wurde
            /// @return 
            bool is_startup_completed() { return fresh_start_ == false; }

            WR3223Connector *connector_{nullptr};
            WR3223RelaisComponent *relais_component_{nullptr};

        private:
            /// @brief nach einem stromausfall oder ähnlichem, verhalten wir uns anders im
            /// ersten Update
            bool fresh_start_{true};
            std::vector<WR3223StartUpListener *> startup_listeners_{};
        };

    } // namespace wr3223
} // namespace esphome
