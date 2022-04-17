#include "History.hpp"
#include <iostream>

History::History(unsigned segments, Entry first_entry) : m_segments(segments), m_first_entry(first_entry){
    if(m_segments < 1000)
        m_segments = 1000;
    m_root = new Node(nullptr, nullptr, m_segments);
    m_current = m_root;
    m_current->entry[0] = first_entry;
    m_current->vector_size++;
}

bool History::move_forward(){
    m_current->vector_pos++;

    if(m_current->vector_pos < m_current->vector_size - 1){
        m_edge = false;
        return true;
    }else if(m_current->vector_pos >= m_segments){
        if(m_current->next != nullptr){
            m_current = m_current->next;
            m_current->vector_pos = 0;

            m_edge = false;
            return true;
        }
    }

    m_edge = true;
    return false;
}

bool History::move_backward(){
    m_current->vector_pos--;
    
    if(m_current->vector_pos >= 0){

        m_edge = false;
        return true;
    }else{
        if(m_current->prev != nullptr){
            m_current = m_current->prev;
            m_current->vector_pos = m_segments - 1;

            m_edge = false;
            return true;
        }
    }

    m_edge = true;
    return false;
}

History::Entry History::get_entry() const{
    // std::cout << m_current->vector_pos << " " << m_current->entry.size() << "\n";
    return m_current->entry[m_current->vector_pos];
}

void History::reset_future_entries(){
    if(m_current->next != nullptr)
        delete m_current->next;
    m_current->next = nullptr;

    m_current->vector_size = m_current->vector_pos + 1;
    m_edge = true;
    // std::cout << m_current->next << " " << m_current << " " << m_current->vector_size << " " << m_current->vector_pos << "\n";
}

void History::push_back(const Entry entry){
    if(!move_forward()){
        if(m_current->vector_size < m_segments){
            m_current->entry[m_current->vector_pos] = entry;
        }else if(m_current->next == nullptr){
            m_current->next = new Node(m_current, nullptr, m_segments);
            m_current = m_current->next;
            m_current->entry[m_current->vector_pos] = entry;
        }else {
            m_edge = false;
        }
        m_current->vector_size++;
    }
}

void History::push_front(){
    move_backward();

    if(m_current->vector_pos < 0)
        m_current->vector_pos = 0;
}

void History::reset(){
    m_current = m_root;
    m_root->vector_pos = 0;
}

History::~History(){
    delete m_root;
}
