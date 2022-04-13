#pragma once

#include "math/Vector3.hpp"
#include <vector>

struct Entry {
    Vector3 pos, vel;
};

class History {
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
    };

    Node* m_root;
    Node* m_current;
    unsigned m_len, m_pos, m_segments;

    bool m_edge = true;

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
    void push_front(const Entry entry);

    ~History();
};
