#include "WrappedObject.hpp"

namespace PySSA
{

PyTypeObject create_type_object(std::string const& name) {
    PyTypeObject type_object;
    memset(&type_object, 0, sizeof(PyTypeObject));
    type_object.tp_name = name.c_str();
    type_object.tp_basicsize = 
}

}
