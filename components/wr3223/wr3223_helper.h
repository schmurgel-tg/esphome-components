#pragma once

#include <string>

namespace esphome {
namespace wr3223 {

class WR3223Helper {
public:
    static float to_float(const char *data);
    static float to_float(const std::string &data);
    static int to_int(const char *data, bool positiv = false);
    static int to_int(const std::string &data, bool positiv = false);
    static std::string to_string(float number, unsigned int precision = 1);
    static std::string to_bitwise(int number);
    static std::string to_hex_string(uint8_t *data, int length);
    static std::string to_hex_string(char *data, int length);
};

}  // namespace wr3223
}  // namespace esphome
