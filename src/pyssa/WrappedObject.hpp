#pragma once

#include "Object.hpp"

#include <set>
#include <sstream>

namespace PySSA {

// A TypeObject wrapper for specific T. Makes all Python objects
// of this type just handles, so that things they wrap aren't deleted
// by Python interpreter. Should be used for objects that we want to
// be accessible from Python scripts but cannot be created by them.
template<class T>
class WrappedObject {
public:
    struct PythonType {
        PyObject_HEAD;

        T* ptr;

        // Is the object created and should be removed by Python interpreter?
        bool owned_by_python;
    };

    virtual ~WrappedObject() {
        // Ensure that no Python code will try to access already
        // destructed object. The exception will be thrown in that
        // case.
        for (auto& wrapper : m_wrappers) {
            if (wrapper && !wrapper->owned_by_python)
                wrapper->ptr = nullptr;
        }
    }

    static constexpr bool CanBeCreatedFromPython = requires(Object const& o) {
        T::create_for_python(o, o);
        (char const*)T::PythonClassName;
    };

    static PyTypeObject& type_object();

    Object wrap() {
        auto type = &type_object();
        auto object = Object::take(PyObject_New(PyObject, type));
        auto& py_object = *(PythonType*)object.python_object();
        py_object.ptr = static_cast<T*>(this);
        py_object.owned_by_python = false;
        m_wrappers.insert((PythonType*)object.python_object());
        return object;
    }

    void python_stop_owning() {
        for (auto& wrapper : m_wrappers) {
            wrapper->owned_by_python = false;
        }
    }

    static T* get(Object const& object) {
        return ((PythonType*)object.python_object())->ptr;
    }

private:
    struct Funcs {
        Funcs() = default;
        Funcs(Funcs const&) = delete;
        Funcs& operator=(Funcs const&) = delete;
        Funcs(Funcs&&) = default;
        Funcs& operator=(Funcs&&) = default;
        std::vector<PyMethodDef> methods;
        std::vector<PyGetSetDef> getters_setters;
    };

protected:
    class TypeSetup {
    public:
        TypeSetup(Funcs& funcs, PyTypeObject& type_object)
            : m_funcs(funcs)
            , m_type_object(type_object) { }

        using Method = Object (T::*)(Object const& args, Object const& kwargs);
        using Getter = Object (T::*)() const;
        using Setter = bool (T::*)(Object const&);

        template<Method method>
        struct MethodWrapper {
            static PyObject* wrapper(PythonType* self, PyObject* args, PyObject* kwargs) {
                auto that = self->ptr;
                if (!that)
                    return PyErr_Format(PyExc_ReferenceError, "This %s instance was already destructed", T::PythonClassName);
                return (that->*method)(Object::share(args), Object::share(kwargs)).leak_object();
            }
        };

        template<Getter getter>
        struct GetterWrapper {
            static PyObject* wrapper(PythonType* self, void*) {
                auto that = self->ptr;
                if (!that)
                    return PyErr_Format(PyExc_ReferenceError, "This %s instance was already destructed", T::PythonClassName);
                return (that->*getter)().leak_object();
            }
        };

        template<Setter setter>
        struct SetterWrapper {
            static int wrapper(PythonType* self, PyObject* value, void*) {
                auto that = self->ptr;
                if (!that) {
                    PyErr_Format(PyExc_ReferenceError, "This %s instance was already destructed", T::PythonClassName);
                    return 1;
                }
                return (that->*setter)(Object::share(value)) ? 0 : 1;
            }
        };

        template<Method method>
        void add_method(char const* name, char const* doc = "") const {
            m_funcs.methods.push_back(PyMethodDef {
                .ml_name = name,
                .ml_meth = (PyCFunction)MethodWrapper<method>::wrapper,
                .ml_flags = METH_VARARGS | METH_KEYWORDS,
                .ml_doc = doc });
        }

        template<Getter getter, Setter setter>
        void add_attribute(const char* attr, char const* doc = "") const {
            m_funcs.getters_setters.push_back(PyGetSetDef {
                .name = attr,
                .get = (::getter)GetterWrapper<getter>::wrapper,
                .set = setter != nullptr ? (::setter)SetterWrapper<setter>::wrapper : nullptr,
                .doc = doc,
                .closure = nullptr });
        }

    private:
        Funcs& m_funcs;
        PyTypeObject& m_type_object;
    };

private:
    std::set<PythonType*> m_wrappers;

    static void setup_python_bindings_internal(PyTypeObject&);
};

template<class T>
void WrappedObject<T>::setup_python_bindings_internal(PyTypeObject& type) {
    static Funcs funcs = [&]() -> Funcs {
        Funcs funcs2;
        constexpr bool has_setup_python_bindings = requires(TypeSetup a) { T::setup_python_bindings(a); };
        static_assert(has_setup_python_bindings, "You need to define static void setup_python_bindings(FuncsAdder)");
        if constexpr (has_setup_python_bindings) {
            T::setup_python_bindings(TypeSetup { funcs2, type });
        }
        funcs2.methods.push_back(PyMethodDef { nullptr, nullptr, 0, nullptr });
        funcs2.getters_setters.push_back(PyGetSetDef { nullptr, nullptr, nullptr, nullptr, nullptr });
        return funcs2;
    }();
    type.tp_methods = funcs.methods.data();
    type.tp_getset = funcs.getters_setters.data();
    if constexpr (requires(std::ostream & out, T const& t) { out << t; }) {
        if (!type.tp_repr) {
            struct ReprWrapper {
                static PyObject* wrapper(PythonType* self) {
                    std::ostringstream oss;
                    oss << *self->ptr;
                    return PyUnicode_FromString(oss.str().c_str());
                }
            };
            type.tp_repr = (reprfunc)ReprWrapper::wrapper;
        }
    }
    if constexpr (CanBeCreatedFromPython) {
        // FIXME: There is a memory leak somewhere but I cannot reproduce it.
        type.tp_init = [](PyObject* self, PyObject* args, PyObject* kwargs) {
            auto object = (PythonType*)self;
            object->ptr = T::create_for_python(Object::share(args), Object::share(kwargs));
            object->ptr->m_wrappers.insert(object);
            if (!object->ptr)
                return -1;
            object->owned_by_python = true;
            return 0;
        };
        type.tp_dealloc = [](PyObject* self) {
            auto object = (PythonType*)self;
            object->ptr->m_wrappers.erase(object);
            if (object->owned_by_python)
                delete object->ptr;
            Py_TYPE(self)->tp_free(self);
        };
        type.tp_new = PyType_GenericNew;
    }
}

template<class T>
PyTypeObject& WrappedObject<T>::type_object() {
    static PyTypeObject type_object = []() {
        PyTypeObject type { PyVarObject_HEAD_INIT(nullptr, 0) };
        type.tp_name = T::PythonClassName;
        type.tp_doc = "TODO";
        type.tp_basicsize = sizeof(T);
        type.tp_itemsize = 0;
        type.tp_flags = Py_TPFLAGS_DEFAULT;
        setup_python_bindings_internal(type);
        return type;
    }();
    assert(PyType_Ready(&type_object) >= 0);
    return type_object;
}
}
