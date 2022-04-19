#pragma once

#include <iostream>
#include <string>
const std::string month_name[12] {
    "Jan", "Feb", "Mar",
    "Apr", "May", "Jun",
    "Jul", "Aug", "Sep",
    "Oct", "Nov", "Dec"
};

const unsigned month_day[12] {
    31, 28, 31,
    30, 31, 30,
    31, 31, 30,
    31, 30, 31
};

class Date {
public:
    enum Era{
        AD,
        BC
    };
    Date(unsigned year, unsigned month, unsigned day, Era era = AD, unsigned hour = 0, unsigned minute = 0, unsigned second = 0)
        : m_year(year)
        , m_month(month)
        , m_day(day)
        , m_hour(hour)
        , m_minute(minute)
        , m_second(second)
        , m_era(era){}

    Date(int seconds);

    std::string to_full_date_string() const{
        std::string str = std::to_string(m_day) + " " + month_name[m_month] + " " + std::to_string(m_year);
        if(m_era == AD)
            str += " AD, ";
        else
            str += " BC, ";

        str += m_prepare_string(m_hour) + ":" + m_prepare_string(m_minute) + ":" + m_prepare_string(m_second) + " UTC";
        return str;
    }

    std::string to_date_string() const{
        std::string str = m_prepare_string(m_day) + "." + m_prepare_string(m_month + 1) + " " + m_prepare_string(m_year);
        if(m_era == AD)
            str += " AD, ";
        else
            str += " BC, ";

        str += m_prepare_string(m_hour) + ":" + m_prepare_string(m_minute) + ":" + m_prepare_string(m_second) + " UTC";
        return str;
    }

    int get_int() const{
        int result = 0;
        result += m_year * 365.25;
        for(unsigned i = 0; i < m_month; i++)
            result += month_day[i];
        result += m_day;

        return result;
    }

    void set_date(unsigned year, unsigned month, unsigned day, Era era = AD, unsigned hour = 0, unsigned minute = 0, unsigned second = 0){
        m_year = year;
        m_month = month;
        m_day = day;
        m_era = era;
        m_hour = hour;
        m_minute = minute;
        m_second = second;
    }

    unsigned secs() const{return m_second;}
    unsigned mins() const{return m_minute;}
    unsigned hrs() const{return m_hour;}
    unsigned ds() const{return m_day;}
    unsigned ms() const{return m_month;}
    unsigned yrs() const{return m_year;}

private:
    unsigned m_year, m_month, m_day, m_hour, m_minute, m_second;
    Era m_era;

    std::string m_prepare_string(unsigned val) const{
        if(val < 10)
            return "0" + std::to_string(val);
        return std::to_string(val);
    }
};

inline Date operator+(const Date& d1, const Date& d2){
    unsigned sec, min, hr, d, m, yr;
    sec = d1.secs() + d2.secs();
    min = d1.mins() + d2.mins();
    hr = d1.hrs() + d2.hrs();
    d = d1.ds() + d2.ds();
    m = d1.ms() + d2.ms();
    yr = d1.yrs() + d2.yrs();

    min += sec / 60;
    sec = sec % 60;
    hr += min / 60;
    min = min % 60;
    d += hr / 24;
    hr = hr % 24;
    yr += m / 12;
    m = m % 12;

    if(d > month_day[m]){
        d = d % month_day[m];
        m++;
    }

    yr += m / 12;
    m = m % 12;

    return Date(yr, m, d, Date::AD, hr, min, sec);
}

inline Date operator-(const Date& d1, const Date& d2){
    int sec, min, hr, d, m, yr;
    sec = d1.secs() - d2.secs();
    min = d1.mins() - d2.mins();
    hr = d1.hrs() - d2.hrs();
    d = d1.ds() - d2.ds();
    m = d1.ms() - d2.ms();
    yr = d1.yrs() - d2.yrs();

    min -= sec / 60;
    sec = sec % 60;
    hr -= min / 60;
    min = min % 60;
    d -= hr / 24;
    hr = hr % 24;

    if(m < 0){
        m = 11 - m;
        yr--;
    }

    if(d > month_day[m]){
        d = d % month_day[m];
        m--;
    }
    m = m % 12;

    sec = std::abs(sec);
    min = std::abs(min);
    hr = std::abs(hr);
    d = std::abs(d);
    m = std::abs(m);
    yr = std::abs(yr);

    return Date(yr, m, d, Date::AD, hr, min, sec);
}

inline Date operator+(const Date& d1, int s){

    return d1 + Date(s);
}

inline Date operator-(const Date& d1, int s){

    return d1 - Date(s);
}

inline Date& operator+=(Date& d1, const Date& d2){
    return d1 = d1 + d2;
}

inline Date& operator-=(Date& d1, const Date& d2){
    return d1 = d1 - d2;
}

inline Date& operator+=(Date& d1, const unsigned s){
    return d1 = d1 + s;
}

inline Date& operator-=(Date& d1, const unsigned s){
    return d1 = d1 - s;
}

inline Date::Date(int seconds){
    m_second = seconds % 60;
    seconds /= 60;
    m_minute = seconds % 60;
    seconds /= 60;
    m_hour = seconds % 24;
    seconds /= 24;
    unsigned m = 0;

    while (seconds > month_day[m]) {
        seconds -= month_day[m];
        m++;

        if(m == 12){
            m = 1;
            m_year++;
        }
    }

    m_month = m;
    m_day = seconds;

    // std::cout << m_second << " " << m_minute << " " << m_hour << " " << m_day << " " << m_month << " " << m_year << "\n";
}
