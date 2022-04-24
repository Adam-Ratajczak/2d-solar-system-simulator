#include "History.hpp"

#include <cassert>
#include <iostream>

History::History(unsigned segments, Entry first_entry) : m_segments(segments), m_first_entry(first_entry){
    m_entry_list.push_back(first_entry);
    m_side = 0;
}

std::optional<History::Entry> History::move_forward(const Entry entry) {
    if(m_side == 0){
        m_entry_list.push_back(entry);

        if(m_entry_list.size() > m_segments)
            m_entry_list.pop_front();
        return {};
    }else{
        if(m_entry_list.size() > 0){
            auto value = m_entry_list.front();
            m_entry_list.pop_front();

            return value;
        }else {
            m_side = 0;
            m_entry_list.push_back(entry);

            return {};
        }
    }
}

std::optional<History::Entry> History::move_backward(const Entry entry) {
    if(m_side == 1){
        m_entry_list.push_front(entry);

        if(m_entry_list.size() > m_segments)
            m_entry_list.pop_back();
        return {};
    }else{
        if(m_entry_list.size() > 0){
            auto value = m_entry_list.back();
            m_entry_list.pop_back();

            return value;
        }else {
            m_side = 1;
            m_entry_list.push_front(entry);

            return {};
        }
    }
}

void History::reset() {
    m_entry_list.clear();
    m_entry_list.push_back(m_first_entry);
    m_side = 0;
}
