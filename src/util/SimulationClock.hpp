#pragma once

#include <chrono>
#include <ctime>
#include <iostream>
#include <ratio>

namespace Util {

class SimulationClock {
public:
    using rep = time_t;
    using period = std::ratio<1, 1>; // 1 second
    using duration = std::chrono::duration<rep, period>;
    using time_point = std::chrono::time_point<SimulationClock>;
};

namespace SimulationTime {

// This expects data in human-readable units
// E.g if you want to create 1970-02-15 you call create(1920, 2, 15, 0, 0, 0).
// Maybe it is obvious but C library uses another format.
SimulationClock::time_point create(int year, int month, int day, int hour = 0, int minute = 0, int second = 0);

}

// We need at least 8 bytes to represent our simulation times!
static_assert(sizeof(time_t) == 8);

}

std::ostream& operator<<(std::ostream& out, Util::SimulationClock::time_point const& time);
