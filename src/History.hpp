#pragma once

#include "math/Vector3.hpp"
#include <vector>

class History {
    struct Node {
        Node* prev;
        Node* next;

        std::vector<Vector3> entry;
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
    int m_len, m_pos, m_segments;

    bool m_edge = true;

public:
    History(unsigned segments, Vector3 first_entry);

    History(History const&) = delete;
    History& operator=(History const&) = delete;
    History(History&&) = delete;
    History& operator=(History&&) = delete;

    bool move_forward();
    bool move_backward();
    bool on_edge() const{return m_edge;}

    Vector3 get_entry() const;
    Vector3 get_entry_from_prev(unsigned index) const;
    void push_back(const Vector3 entry);
    void push_front(const Vector3 entry);

    ~History();
};
