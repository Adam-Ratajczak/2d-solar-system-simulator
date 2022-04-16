#pragma once

#include "Object.hpp"
#include <memory>
#include <vector>
class ObjectHistory {

    std::vector<std::unique_ptr<Object>> m_entries;

    unsigned m_pos = 0;

public:
    ObjectHistory() = default;

    unsigned get_pos() const { return m_pos; }
    void clear_history(unsigned long to_index);
    bool set_time(unsigned time);

    void push_to_entry(std::unique_ptr<Object> obj);
    std::unique_ptr<Object> pop_from_entries();
    std::unique_ptr<Object>& back() { return m_entries.back(); }

    unsigned size() const { return m_entries.size(); }
};
