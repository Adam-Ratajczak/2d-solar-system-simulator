#include "Environment.hpp"

#include "Object.hpp"
#include "methodobject.h"

#include <Python.h>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace PySSA {

Environment& Environment::the() {
    static Environment env;
    return env;
}

Environment::Environment() {
    initialize_environment();
    Py_Initialize();
}

Environment::~Environment() {
    Py_FinalizeEx();
}

bool Environment::run_script(std::string const& path) {
    std::cout << "!!!" << path << "!!!" << std::endl;
    std::ifstream file(path);
    if (!file.good()) {
        std::cerr << "failed to open file" << std::endl;
        return false;
    }

    size_t filesize = std::filesystem::file_size(path);
    std::string script;
    script.resize(filesize);
    if (!file.read(script.data(), filesize)) {
        std::cerr << "failed to read file" << std::endl;
        return false;
    }

    static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> wstring_converter;

    Py_SetProgramName(wstring_converter.from_bytes(path).c_str());
    PyRun_SimpleString(script.c_str());
    return true;
}

void Environment::initialize_environment() {
    std::cout << "init python env" << std::endl;

    // TODO: Wrap these in nice C++ APIs.
    PyImport_AppendInittab("pyssa", []() {
        static PyMethodDef pyssa_methods[] = {
            { "test", [](PyObject*, PyObject*) { return Object::create_tuple(5).leak_object(); }, METH_NOARGS, "Return the number of arguments received by the process." },
            { NULL, NULL, 0, NULL }
        };

        static PyModuleDef pyssa_module = {
            PyModuleDef_HEAD_INIT, "pyssa", NULL, -1, pyssa_methods,
            NULL, NULL, NULL, NULL
        };
        return PyModule_Create(&pyssa_module);
    });
}

}
