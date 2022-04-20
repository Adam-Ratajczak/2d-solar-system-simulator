#pragma once

#include "math/Vector3.hpp"
#include <vector>

class History {
    struct Entry {
        Vector3 pos, vel;
    };

    struct Node {
        Node* prev = nullptr;
        Node* next = nullptr;

        std::vector<Entry> entry;
        size_t vector_pos = 0;
        size_t vector_size = 0;

        Node(Node* _prev, Node* _next, size_t _size) {
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

        ~Node() {
            if (next != nullptr)
                delete next;
        }
    };

    Node* m_root = nullptr;
    Node* m_current = nullptr;
    int m_segments = 0;

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
    bool on_edge() const { return m_edge; }

    Entry get_entry() const;
    void push_back(const Entry entry);
    void push_front();
    void reset();
    void reset_future_entries();
    void change_current(Entry entry);
    Entry first_entry() const { return m_first_entry; }

    ~History();
};
