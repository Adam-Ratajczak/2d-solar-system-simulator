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

Object Object::get_attribute(Object const& name) {
    return Object::take(PyObject_GetAttr(m_object, name.m_object));
}

void Object::set_attribute(Object const& name, Object const& value) {
    PyObject_SetAttr(m_object, name.m_object, value.m_object);
}

Object Object::call(Object const& args, Object const& kwargs) const {
    return Object::take(PyObject_Call(m_object, args.share_object(), kwargs.share_object()));
}

int Object::as_int() const {
    return PyLong_AsLong(m_object);
}

std::vector<Object> Object::as_list() const {
    Py_ssize_t size = PyList_Size(m_object);
    std::vector<Object> list;
    list.resize(size);
    for(Py_ssize_t s = 0; s < size; s++)
        list[s] = Object::share(PyList_GetItem(m_object, s));
    return list;
}

std::string Object::str() const {
    return PyUnicode_AsUTF8(PyObject_Str(m_object));
}

std::string Object::repr() const {
    return PyUnicode_AsUTF8(PyObject_Repr(m_object));
}

}
