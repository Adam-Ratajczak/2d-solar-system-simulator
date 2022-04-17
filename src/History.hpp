#pragma once

#include "math/Vector3.hpp"
#include <vector>

class History {
    struct Entry{
        Vector3 pos, vel;
    };

    struct Node {
        Node* prev;
        Node* next;

        std::vector<Entry> entry;
        int vector_pos, vector_size;

        Node(Node* _prev, Node* _next, unsigned _size) {
            prev = _prev;
            next = _next;

            entry.resize(_size);
            vector_pos = 0;
            vector_size = 0;
        }

        Node(Node const&) = delete;
        Node& operator=(Node const&) = delete;
        Node(Node&&) = delete;
        Node& operator=(Node&&) = delete;

        ~Node(){
            if(next != nullptr)
                delete next;
        }
    };

    Node* m_root;
    Node* m_current;
    int m_segments;

    bool m_edge = true;
    Entry m_first_entry;

public:
    History(unsigned segments, Entry first_entry);

    History(History const&) = delete;
    History& operator=(History const&) = delete;
    History(History&&) = delete;
    History& operator=(History&&) = delete;

    bool move_forward();
    bool move_backward();
    bool on_edge() const{return m_edge;}

    Entry get_entry() const;
    void push_back(const Entry entry);
    void push_front();
    void reset();
    void reset_future_entries();
    Entry first_entry() const{return m_first_entry;}

    ~History();
};
