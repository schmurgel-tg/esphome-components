#include "wr3223_helper.h"
#include <bitset>
#include <iomanip>
#include <sstream>
#include "esphome/core/log.h"

namespace esphome
{
    namespace wr3223
    {

        static const char *const TAG = "wr3223_helper";

        float WR3223Helper::to_float(const char *data)
        {
            if (data == nullptr)
            {
                ESP_LOGW(TAG, "Null pointer passed to to_float");
                return 0.0f;
            }

            char *end = nullptr;
            float value = strtof(data, &end);
            if (end == data)
            {
                ESP_LOGW(TAG, "Unable to parse float from '%s'", data);
                return 0.0f;
            }
            return value;
        }

        float WR3223Helper::to_float(const std::string &data)
        {
            return to_float(data.c_str());
        }

        int WR3223Helper::to_int(const char *data, bool positiv)
        {
            if (data == nullptr)
            {
                ESP_LOGW(TAG, "Null pointer passed to to_int");
                return 0;
            }

            char *end = nullptr;
            long value = strtol(data, &end, 10);
            if (end == data)
            {
                ESP_LOGW(TAG, "Unable to parse int from '%s'", data);
                return 0;
            }
            int result = static_cast<int>(value);
            return (positiv && result < 0) ? -result : result;
        }

        int WR3223Helper::to_int(const std::string &data, bool positiv)
        {
            return to_int(data.c_str(), positiv);
        }

        std::string WR3223Helper::to_string(float number, unsigned int precision)
        {
            std::stringstream stream;
            stream << std::fixed << std::setprecision(precision) << number;
            return stream.str();
        }

        std::string WR3223Helper::to_bitwise(int number)
        {
            return std::bitset<8>(number).to_string();
        }

        std::string WR3223Helper::to_hex_string(uint8_t *data, int length)
        {
            std::stringstream ss;
            ss << std::hex << std::setfill('0');
            for (int i = 0; i < length; i++)
                ss << ":" << std::setw(2) << static_cast<unsigned>(data[i]);
            return ss.str();
        }

        std::string WR3223Helper::to_hex_string(char *data, int length)
        {
            std::stringstream ss;
            ss << std::hex << std::setfill('0');
            for (int i = 0; i < length; i++)
                ss << ":" << std::setw(2) << static_cast<unsigned>(data[i]);
            return ss.str();
        }

    } // namespace wr3223
} // namespace esphome
