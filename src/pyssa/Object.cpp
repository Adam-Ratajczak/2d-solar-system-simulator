#include "Object.hpp"

namespace PySSA {

Object Object::none() {
    Object o;
    o.m_object = Py_None;
    Py_INCREF(o.m_object);
    return o;
}

Object Object::create(Util::UString const& str) {
    Object o;
    o.m_object = PyUnicode_FromString(str.encode().c_str());
    return o;
}

Object Object::create(int v) {
    Object o;
    o.m_object = PyLong_FromLong(v);
    return o;
}

Object Object::create(double v) {
    Object o;
    o.m_object = PyFloat_FromDouble(v);
    return o;
}

Object Object::create(Util::DeprecatedVector3d const& vector) {
    auto tuple = PySSA::Object::empty_tuple(3);
    tuple.set_tuple_item(0, PySSA::Object::create(vector.x()));
    tuple.set_tuple_item(1, PySSA::Object::create(vector.y()));
    tuple.set_tuple_item(2, PySSA::Object::create(vector.z()));
    return tuple;
}

Object Object::create(Util::Color const& color) {
    auto tuple = PySSA::Object::empty_tuple(3);
    tuple.set_tuple_item(0, PySSA::Object::create(color.r));
    tuple.set_tuple_item(1, PySSA::Object::create(color.g));
    tuple.set_tuple_item(2, PySSA::Object::create(color.b));
    return tuple;
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

std::optional<Util::UString> Object::as_string() const {
    Py_ssize_t size;
    auto data = PyUnicode_AsUTF8AndSize(m_object, &size);
    assert(size >= 0);
    return Util::UString { { data, static_cast<size_t>(size) } };
}

std::optional<int> Object::as_int() const {
    auto value = PyLong_AsLong(m_object);
    if (value == -1 && PyErr_Occurred())
        return {};
    return value;
}

std::optional<double> Object::as_double() const {
    auto value = PyFloat_AsDouble(m_object);
    if (value == -1 && PyErr_Occurred())
        return {};
    return value;
}

std::optional<std::vector<Object>> Object::as_list() const {
    if (!PyList_Check(m_object)) {
        PyErr_SetString(PyExc_TypeError, "Object is not a list instance");
        return {};
    }
    Py_ssize_t size = PyList_Size(m_object);
    std::vector<Object> list;
    list.resize(size);
    for (Py_ssize_t s = 0; s < size; s++)
        list[s] = Object::share(PyList_GET_ITEM(m_object, s));
    return list;
}

std::optional<std::vector<Object>> Object::as_tuple() const {
    if (!PyTuple_Check(m_object)) {
        PyErr_SetString(PyExc_TypeError, "Object is not a tuple instance");
        return {};
    }
    Py_ssize_t size = PyTuple_Size(m_object);
    std::vector<Object> list;
    list.resize(size);
    for (Py_ssize_t s = 0; s < size; s++)
        list[s] = Object::share(PyTuple_GET_ITEM(m_object, s));
    return list;
}

std::optional<Util::DeprecatedVector3d> Object::as_vector() const {
    auto object_as_tuple = as_tuple();
    if (!object_as_tuple.has_value())
        return {};
    auto tuple = object_as_tuple.value();
    if (tuple.size() != 3) {
        PyErr_SetString(PyExc_TypeError, "Tuple must have 3 elements to be a vector");
        return {};
    }
    auto x = tuple[0].as_double();
    if (!x.has_value()) {
        PyErr_SetString(PyExc_TypeError, "'x' coordinate must be a float");
        return {};
    }
    auto y = tuple[1].as_double();
    if (!y.has_value()) {
        PyErr_SetString(PyExc_TypeError, "'y' coordinate must be a float");
        return {};
    }
    auto z = tuple[2].as_double();
    if (!z.has_value()) {
        PyErr_SetString(PyExc_TypeError, "'z' coordinate must be a float");
        return {};
    }
    return Util::DeprecatedVector3d(x.value(), y.value(), z.value());
}

std::optional<Util::Color> Object::as_color() const {
    auto object_as_tuple = as_tuple();
    if (!object_as_tuple.has_value())
        return {};
    auto tuple = object_as_tuple.value();
    if (tuple.size() != 3) {
        PyErr_SetString(PyExc_TypeError, "Tuple must have 3 elements to be a vector");
        return {};
    }
    auto r = tuple[0].as_int();
    if (!r.has_value()) {
        PyErr_SetString(PyExc_TypeError, "'r' component must be an int");
        return {};
    }
    auto g = tuple[1].as_int();
    if (!g.has_value()) {
        PyErr_SetString(PyExc_TypeError, "'g' component must be an int");
        return {};
    }
    auto b = tuple[2].as_int();
    if (!b.has_value()) {
        PyErr_SetString(PyExc_TypeError, "'b' component must be an int");
        return {};
    }
    if (r.value() < 0 || r.value() > 255) {
        PyErr_Format(PyExc_ValueError, "'r' component must be in range <0;255>, %d given", r);
        return {};
    }
    if (g.value() < 0 || g.value() > 255) {
        PyErr_Format(PyExc_ValueError, "'g' component must be in range <0;255>, %d given", g);
        return {};
    }
    if (b.value() < 0 || b.value() > 255) {
        PyErr_Format(PyExc_ValueError, "'b' component must be in range <0;255>, %d given", b);
        return {};
    }
    return Util::Color { static_cast<uint8_t>(r.value()), static_cast<uint8_t>(g.value()), static_cast<uint8_t>(b.value()) };
}

Util::UString Object::str() const {
    Py_ssize_t size;
    auto data = PyUnicode_AsUTF8AndSize(PyObject_Str(m_object), &size);
    assert(size >= 0);
    return Util::UString { { data, static_cast<size_t>(size) } };
}

Util::UString Object::repr() const {
    Py_ssize_t size;
    auto data = PyUnicode_AsUTF8AndSize(PyObject_Repr(m_object), &size);
    assert(size >= 0);
    return Util::UString { { data, static_cast<size_t>(size) } };
}

}
