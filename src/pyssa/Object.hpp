#pragma once

#include <Python.h>
#include <cstdio>
#include <iostream>
#include <string>

namespace PySSA {

class Object {
public:
    ~Object() {
        if (m_object)
            Py_DECREF(m_object);
    }

    Object(Object const& other)
        : m_object(other.m_object) {
        if (m_object)
            Py_INCREF(m_object);
    }

    Object& operator=(Object const& other) {
        if (this == &other)
            return *this;
        m_object = other.m_object;
        if (m_object)
            Py_INCREF(m_object);
        return *this;
    }

    // TODO: Move constructors

    // Creates a new Object without sharing a reference.
    static Object take(PyObject* object) {
        Object o;
        o.m_object = object;
        return o;
    }

    static Object create_tuple(Py_ssize_t size) {
        Object o;
        o.m_object = PyTuple_New(size);
        return o;
    }

    void set_tuple_object(Py_ssize_t i, Object const& object) {
        PyTuple_SetItem(m_object, i, object.m_object);
    }

    PyObject* python_object() { return m_object; }
    bool operator!() const { return !m_object; }
    void print() const {
        PyObject_Print(m_object, stdout, 0);
        puts("");
    }

    PyObject* leak_object() {
        auto object = m_object;
        m_object = nullptr;
        return object;
    }

private:
    Object() = default;

    PyObject* m_object {};
};

}
