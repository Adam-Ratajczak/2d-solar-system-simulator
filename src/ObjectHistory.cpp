#include "ObjectHistory.hpp"
#include "util/SimulationClock.hpp"

void ObjectHistory::clear_history(unsigned long to_index) {
    m_entries.resize(std::min(to_index, m_entries.size()));
    m_pos = to_index - 1;
}

bool ObjectHistory::set_time(Util::SimulationClock::time_point time) {
    m_pos = 0;

    for (auto& e : m_entries) {
        if (e->creation_date() > time)
            return true;
        m_pos++;
    }
    return false;
}

void ObjectHistory::push_to_entry(std::unique_ptr<Object> obj) {
    obj->reset_history();
    m_entries.push_back(std::move(obj));
}

std::unique_ptr<Object> ObjectHistory::pop_from_entries() {
    auto object = std::move(m_entries.back());
    m_entries.pop_back();
    return object;
}