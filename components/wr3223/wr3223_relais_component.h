#pragma once

#include "esphome/core/component.h"
#include "wr3223_connector.h"
#include "wr3223_constants.h"
#include <map>
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome
{
  namespace wr3223
  {

    class WR3223RelaisComponent : public PollingComponent
    {
    public:
      WR3223RelaisComponent(WR3223Connector *connector, uint32_t update_interval)
          : PollingComponent(update_interval), connector_(connector) {}

      void setup() override;
      void update() override;

      void register_relais_sensor(int flag, binary_sensor::BinarySensor *sensor);

      bool is_bedienteil_aktiv() const { return bedienteil_aktiv_; }

    private:
      WR3223Connector *connector_;
      void process_response(char *response);
      std::map<int, binary_sensor::BinarySensor *> relais_sensors_;
      bool bedienteil_aktiv_{true};
    };

  } // namespace wr3223
} // namespace esphome
