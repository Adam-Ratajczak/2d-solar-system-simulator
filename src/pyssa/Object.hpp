#pragma once

#include <Python.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <utility>

namespace PySSA {

class Object {
public:
    Object() = default;

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

    Object(Object&& other)
        : m_object(std::exchange(other.m_object, nullptr)) { }

    Object& operator=(Object&& other) {
        if (this == &other)
            return *this;
        m_object = std::exchange(other.m_object, nullptr);
        return *this;
    }

    // Create a new Object without sharing a reference (take ownership).
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

    static Object create_string(std::string const&);
    static Object create_int(int);

    void set_tuple_item(Py_ssize_t i, Object const& object) {
        PyTuple_SetItem(m_object, i, object.m_object);
    }

    // Get access to a Python object without transferring ownership.
    PyObject* python_object() const { return m_object; }

    bool operator!() const { return !m_object; }

    void print() const {
        PyObject_Print(m_object, stdout, 0);
        puts("");
    }

    // Stop owning the object. After that, it's the user responsibility
    // to manage object lifetime.
    PyObject* leak_object() {
        auto object = m_object;
        m_object = nullptr;
        return object;
    }

    void set_attribute(Object const& name, Object const& value);

    int as_int() const;

private:
    PyObject* m_object {};
};

}
