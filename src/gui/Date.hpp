#pragma once

#include <iostream>
#include <string>
const std::string month_name[12]{
    "Jan", "Feb", "Mar",
    "Apr", "May", "Jun",
    "Jul", "Aug", "Sep",
    "Oct", "Nov", "Dec"
};

const unsigned month_day_count[12]{
    31, 28, 31,
    30, 31, 30,
    31, 31, 30,
    31, 30, 31
};

class Date{
public:
    int day_count;
    Date(float year) : day_count(year * 365.25){};

    std::string to_string(){
        int years = day_count / 365.25;
        int days = day_count - years * 365.25;

        if(years < 0){
            days = -days;
        }

        int i = 0, sum = 0;
        while(sum < days) {
            sum += month_day_count[i];
            if(years % 4 == 0 && i == 2)
                sum++;
            i++;
        }
        i--;

        if(i == 12)
            i = 11;

        sum -= month_day_count[i];

        if(years % 4 == 0 && i == 2)
            sum--;

        days -= sum;

        if(i < 0)
            i = 0;

        if(years > 0)
            return std::to_string(days) + " " + month_name[i] + " " + std::to_string(years) + " AD";
        else
            return std::to_string(days) + " " + month_name[i] + " " + std::to_string(-years) + " BC";
    }
};
