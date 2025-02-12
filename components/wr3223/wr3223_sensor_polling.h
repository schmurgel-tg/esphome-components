#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "wr3223_types.h"
#include "wr3223.h" 
#include <vector>

namespace esphome {
namespace wr3223 {

class WR3223SensorPollingComponent : public PollingComponent {
 public:
  WR3223SensorPollingComponent(WR3223 *parent, uint32_t update_interval, sensor::Sensor *sensor, const char *command)
      : PollingComponent(update_interval), parent_(parent), cmd_pair(command, sensor) {}

  void setup() override;
  void update() override;

 private:
  WR3223 *parent_; // Zugriff auf die Hauptklasse WR3223
  CommandPair cmd_pair;
};

}  // namespace wr3223
}  // namespace esphome
