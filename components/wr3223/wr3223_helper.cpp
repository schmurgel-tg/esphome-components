#include "wr3223_helper.h"
#include <bitset>
#include <iomanip>
#include <sstream>

namespace esphome {
namespace wr3223 {

float WR3223Helper::to_float(const char *data) {      
    std::string fs(data);
    return std::stof(fs);
}

float WR3223Helper::to_float(const std::string &data) {      
    return std::stof(data);
}

int WR3223Helper::to_int(const char *data, bool positiv) {                  
    int result = static_cast<int>(to_float(data));
    return (positiv && result < 0) ? -result : result;
}

int WR3223Helper::to_int(const std::string &data, bool positiv) {                  
    int result = static_cast<int>(to_float(data));
    return (positiv && result < 0) ? -result : result;
}

std::string WR3223Helper::to_string(float number, unsigned int precision) {      
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << number;
    return stream.str();
}

std::string WR3223Helper::to_bitwise(int number) {    
    return std::bitset<8>(number).to_string();
}

std::string WR3223Helper::to_hex_string(uint8_t *data, int length) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < length; i++)            
        ss << ":" << std::setw(2) << static_cast<unsigned>(data[i]);
    return ss.str();
}

std::string WR3223Helper::to_hex_string(char *data, int length) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < length; i++)            
        ss << ":" << std::setw(2) << static_cast<unsigned>(data[i]);
    return ss.str();
}

}  // namespace wr3223
}  // namespace esphome
