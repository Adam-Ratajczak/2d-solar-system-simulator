#include "ObjectHistory.hpp"

void ObjectHistory::add_entry(Entry entry){
    m_entries.push_back(entry);
    m_pos++;
    clear_history(m_pos);
}

ObjectHistory::Entry ObjectHistory::get_entry()const{
    unsigned i = 1;
    for(auto& e : m_entries){
        if(i == m_pos)
            return e;
    }
    return {};
}

void ObjectHistory::clear_history(unsigned long to_index){
    while (m_entries.size() > to_index)
        m_entries.pop_back();
    m_cache.resize(std::min(to_index, m_cache.size()));
    m_pos = to_index;
}

bool ObjectHistory::set_time(unsigned int time){
    m_pos = m_cache.size();

    for(auto& e : m_entries){
        if(e.date > time)
            return true;
        m_pos++;
    }
    return false;
}

void ObjectHistory::push_to_cache(std::unique_ptr<Object>& obj){
    m_cache.push_back(std::move(obj));
}

std::unique_ptr<Object>& ObjectHistory::pop_from_cache(){
    m_obj = std::move(m_cache.back());
    m_cache.pop_back();

    return m_obj;
}