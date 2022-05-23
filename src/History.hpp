#pragma once

#include <EssaGUI/util/Vector3.hpp>
#include <list>
#include <optional>
#include <vector>

class History {
    struct Entry {
        Vector3 pos, vel;
    };

    std::list<Entry> m_entry_list;

    bool m_side;

    const unsigned m_segments;
    const Entry m_first_entry;

public:
    History(unsigned segments, Entry first_entry);

    History(History const&) = delete;
    History& operator=(History const&) = delete;
    History(History&&) = delete;
    History& operator=(History&&) = delete;

    std::optional<Entry> move_forward(const Entry entry);
    std::optional<Entry> move_backward(const Entry entry);

    Entry first_entry() const{return m_first_entry;}

    void reset();
};
