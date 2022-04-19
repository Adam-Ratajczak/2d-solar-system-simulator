#include "SimulationClock.hpp"

std::ostream& operator<<(std::ostream& out, Util::SimulationClock::time_point const& time) {
    auto time_as_time_t = time.time_since_epoch().count();
    
    tm* time_as_tm = gmtime(&time_as_time_t);

    char buffer[256];
    strftime(buffer, 256, "%Y-%m-%d %H:%M:%S", time_as_tm);
    out << buffer;
    return out;
}

namespace Util::SimulationTime {

SimulationClock::time_point create(int year, int month, int day, int hour, int minute, int second) {
    tm time {
        .tm_sec = second,
        .tm_min = minute,
        .tm_hour = hour,
        .tm_mday = day,
        .tm_mon = month - 1,
        .tm_year = year - 1900,
        .tm_wday = 0,
        .tm_yday = 0,
        .tm_isdst = 0,
    };
    auto time_epoch = mktime(&time);
    return SimulationClock::time_point(SimulationClock::duration(time_epoch));
}

}
