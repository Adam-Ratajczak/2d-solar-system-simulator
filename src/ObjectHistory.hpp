#pragma once

#include "Object.hpp"
#include <memory>
#include <vector>
class ObjectHistory{
    struct Entry{
        Object* obj;
        int date;
    };

    std::list<Entry> m_entries;
    std::vector<std::unique_ptr<Object>> m_cache;

    unsigned m_pos = 0;
    std::unique_ptr<Object> m_obj;

public:
    ObjectHistory() = default;
    void add_entry(Entry entry);
    Entry get_entry()const;

    unsigned get_pos() const { return m_pos; }
    void clear_history(unsigned long to_index);
    bool set_time(unsigned time);

    void push_to_cache(std::unique_ptr<Object>& obj);
    std::unique_ptr<Object>& pop_from_cache();

};
