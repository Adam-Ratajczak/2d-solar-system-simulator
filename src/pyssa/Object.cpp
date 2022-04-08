#include "Object.hpp"

namespace PySSA {

Object Object::create_none() {
    Object o;
    o.m_object = Py_None;
    Py_INCREF(o.m_object);
    return o;
}

Object Object::create_string(std::string const& str) {
    Object o;
    o.m_object = PyUnicode_FromString(str.c_str());
    return o;
}

Object Object::create_int(int v) {
    Object o;
    o.m_object = PyLong_FromLong(v);
    return o;
}

Object Object::create_double(double v) {
    Object o;
    o.m_object = PyFloat_FromDouble(v);
    return o;
}

void Object::set_attribute(Object const& name, Object const& value) {
    PyObject_SetAttr(m_object, name.m_object, value.m_object);
}

int Object::as_int() const {
    return PyLong_AsLong(m_object);
}

std::string Object::str() const {
    return PyUnicode_AsUTF8(PyObject_Str(m_object));
}

}
