#include "History.hpp"

History::History(unsigned segments, Entry first_entry) : m_segments(segments){
    m_root = new Node(nullptr, nullptr, m_segments);
    m_current = m_root;
    m_current->entry[0] = first_entry;
    m_current->vector_size++;
}

bool History::move_forward(){
    if(m_current->vector_pos < m_current->vector_size - 1){
        m_current->vector_pos++;
        return true;
    }

    if(m_current->vector_size == m_current->vector_pos - 1){
        if(m_current->next != nullptr){
            m_current = m_current->next;
            return true;
        }
    }

    return false;
}

bool History::move_backward(){
    if(m_current->vector_pos > 0){
        m_current->vector_pos--;
        return true;
    }

    if(m_current->vector_size == 0){
        if(m_current->prev != nullptr){
            m_current = m_current->prev;
            return true;
        }
    }

    return false;
}

Entry History::get_entry() const{
    return m_current->entry[m_current->vector_pos];
}

void History::push_back(const Entry entry){
    if(!move_forward()){
        if(m_current->vector_size < m_segments){
            m_current->vector_pos++;
            m_current->vector_size++;
            m_current->entry[m_current->vector_pos] = entry;
        }else{
            m_current->next = new Node(m_current, nullptr, m_segments);
            m_current = m_current->next;
            m_current->entry[0] = entry;
            m_current->vector_size++;
            m_len++;
        }
    }
}

void History::push_front(const Entry entry){
    if(!move_backward()){
        if(m_current->vector_size < m_segments){
            for(unsigned i = 0; i < m_current->vector_size; i++){
                m_current->entry[i + 1] = m_current->entry[i];
            }
            m_current->entry[0] = entry;
            m_current->vector_size++;
        }else{
            m_current->prev = new Node(m_current, nullptr, m_segments);
            m_current = m_current->prev;
            m_current->entry[0] = entry;
            m_current->vector_size++;
            m_len++;
            m_root = m_current;
        }
    }
}

History::~History(){
    while(m_root->next != nullptr){
        m_root = m_root->next;
        delete m_root->prev;
    }

    delete m_root;
}
