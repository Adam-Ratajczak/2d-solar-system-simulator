#include "History.hpp"
#include <iostream>

History::History(unsigned segments, Vector3 first_entry) : m_segments(segments), m_first_entry(first_entry){
    m_root = new Node(nullptr, nullptr, m_segments);
    m_current = m_root;
    m_current->entry[0] = first_entry;
    m_current->vector_size++;
    m_len = 0;
}

bool History::move_forward(){
    m_current->vector_pos++;
    m_pos++;

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
    m_pos--;
    
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

Vector3 History::get_entry() const{
    // std::cout << m_current->vector_pos << " " << m_current->entry.size() << "\n";
    return m_current->entry[m_current->vector_pos];
}

Vector3 History::get_entry_from_prev(unsigned index) const{
    if(index > m_pos)
        return m_root->entry[0];
    auto temp = m_current;

    for(unsigned i = 0; i < index / m_segments; i++)
        temp = temp->prev;
    
    return temp->entry[index % m_segments];
}

void History::push_back(const Vector3 entry){
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
    while(m_root->next != nullptr){
        m_root = m_root->next;
        delete m_root->prev;
    }

    delete m_root;
}
