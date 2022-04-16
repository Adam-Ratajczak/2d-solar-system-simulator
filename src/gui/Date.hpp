#pragma once

#include <iostream>
#include <string>
const std::string month_name[12] {
    "Jan", "Feb", "Mar",
    "Apr", "May", "Jun",
    "Jul", "Aug", "Sep",
    "Oct", "Nov", "Dec"
};

const unsigned month_day_count[12] {
    31, 28, 31,
    30, 31, 30,
    31, 31, 30,
    31, 30, 31
};

class Date {
    int m_day_count;
public:
    Date(float year)
        : m_day_count(year * 365.25) {};

    std::string to_string() const{
        int years = m_day_count / 365.25;
        int days = m_day_count - years * 365.25;

        if (years < 0) {
            days = -days;
        }

        int i = 1, sum = 0;
        while (sum < days) {
            // FIXME: modulo is a quick fix for an overflow
            sum += month_day_count[i % 12];
            if (years % 4 == 0 && i == 2)
                sum++;
            i++;
        }
        i--;

        if (i == 12)
            i = 11;

        // FIXME: modulo is a quick fix for an overflow
        sum -= month_day_count[i % 12];

        if (years % 4 == 0 && i == 2)
            sum--;

        days -= sum;

        if (i < 0)
            i = 0;

        if (years > 0)
            return std::to_string(days) + " " + month_name[i] + " " + std::to_string(years) + " AD";
        else
            return std::to_string(days) + " " + month_name[i] + " " + std::to_string(-years) + " BC";
    }

    int get_int() const{
        return m_day_count;
    }

    void set_date(float date){
        m_day_count = date * 365.25;
    }

    void move_forward(unsigned days = 1){
        m_day_count += days;
    }

    void move_backward(unsigned days = 1){
        m_day_count -= days;
    }
};
