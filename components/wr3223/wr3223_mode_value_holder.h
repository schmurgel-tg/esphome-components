#pragma once

#include "esphome/core/preferences.h"
#include "wr3223_helper.h"

namespace esphome
{
    namespace wr3223
    {

        enum WR3223EnumModus : uint8_t
        {
            AUS = 0b00000000,
            SOMMER_BETRIEB = 0b00000001,
            SOMMER_ABLUFT = 0b00000010,
            WINTER_BETRIEB = 0b00000011,
            HANDBETRIEB_WINTERBETRIEB = 0b00000100,

            ABSENKUNG_ON = 0b01000000,
            NACHHEIZREGISTER_ON = 0b10000000,

            MODE_MASK = 0b00000111,
            STATUS_MASK = 0b11000111
        };

        class WR3223ModeValueHolder
        {
        public:
            WR3223ModeValueHolder()
                : pref_(global_preferences->make_preference<int>(0x02)) {}

            void restore_state() { pref_.load(&mode_value_); }
            void save_state() { pref_.save(&mode_value_); }

            void set_mode(int mode)
            {
                mode_value_ &= ~WR3223EnumModus::MODE_MASK;
                mode_value_ |= (mode & WR3223EnumModus::MODE_MASK);
            }

            int get_mode() const { return mode_value_ & WR3223EnumModus::MODE_MASK; }

            bool set_mode_status(const char *read)
            {
                if (read == nullptr)
                    return false;
                int value = WR3223Helper::to_int(read, true);
                mode_value_ = value & WR3223EnumModus::STATUS_MASK;
                return true;
            }

            int get_mode_status() const { return mode_value_; }

        private:
            ESPPreferenceObject pref_;
            int mode_value_{0};
        };

    } // namespace wr3223
} // namespace esphome