#include "Environment.hpp"

#include "../World.hpp"
#include "Object.hpp"
#include "TupleParser.hpp"

#include <Python.h>
#include <filesystem>
#include <fstream>
#include <iostream>

namespace PySSA {

static Environment* s_the {};

Environment& Environment::the() {
    assert(s_the);
    return *s_the;
}

Environment::Environment(World& world, SimulationView& sv)
    : m_world(world)
    , m_simulation_view(sv) {
    assert(!s_the);
    s_the = this;
    initialize_environment();
    Py_Initialize();

    m_globals = PyDict_New();
    m_locals = PyDict_New();

    // Do "import pyssa"
    PyDict_SetItemString(m_globals, "pyssa", PyImport_ImportModule("pyssa"));
}

Environment::~Environment() {
    Py_DECREF(m_globals);
    Py_DECREF(m_locals);
    Py_FinalizeEx();
    s_the = nullptr;
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

Object Environment::eval_string(std::string const& str) {
    std::cout << "EVAL >>> " << str;
    // Try eval first
    Object code_object = Object::take(Py_CompileString(str.c_str(), "<stdin>", Py_eval_input));
    if (!code_object) {
        PyErr_Clear();
        // If eval doesn't compile, maybe file will work (but result will be None)
        code_object = Object::take(Py_CompileString(str.c_str(), "<stdin>", Py_file_input));
        if (!code_object)
            return {};
    }
    return Object::take(PyEval_EvalCode(code_object.leak_object(), m_globals, m_locals));
}

std::vector<std::string> Environment::generate_exception_message() const {
    // https://stackoverflow.com/questions/1418015/how-to-get-python-exception-text
    PyObject* type;
    PyObject* value;
    PyObject* traceback;
    PyErr_Fetch(&type, &value, &traceback);
    PyErr_NormalizeException(&type, &value, &traceback);
    PyErr_Clear();
    auto traceback_mod = Object::take(PyImport_ImportModule("traceback"));
    if (!traceback_mod)
        return { "traceback :(" };
    Object formatted_list;
    if (!traceback) {
        auto format_exception_only = traceback_mod.get_attribute("format_exception_only");
        if (!format_exception_only)
            return { "format_exception_only :(" };
        formatted_list = format_exception_only.call(Object::tuple(Object::share(type), Object::share(value)));
    }
    else {
        auto format_exception = traceback_mod.get_attribute("format_exception");
        if (!format_exception)
            return { "format_exception_only :(" };
        formatted_list = format_exception.call(Object::tuple(Object::share(type), Object::share(value), Object::share(traceback)));
    }
    std::vector<std::string> result;
    auto list = formatted_list.as_list();
    for (auto& element : list.value()) {
        result.push_back(element.str());
    }
    return result;
}

PyObject* tupleparser_test(PyObject*, PyObject* args) {
    Object args_py = Object::share(args);
    ::Object* o1 {};
    ::Object* o2 {};
    ::Object* o3 {};
    if (!parse_arguments(args_py, "O!O!O!", Arg::CheckedType<::Object> { o1 }, Arg::CheckedType<::Object> { o2 }, Arg::CheckedType<::Object> { o3 }))
        return nullptr;
    std::cout << o1->name() << "," << o2->name() << "," << o3->name() << std::endl;
    Py_RETURN_NONE;
}

void Environment::initialize_environment() {
    std::cout << "init python env" << std::endl;

    // TODO: Wrap these in nice C++ APIs.
    PyImport_AppendInittab("pyssa", []() {
        static PyMethodDef pyssa_methods[] = {
            { "test", [](PyObject*, PyObject*) { return Object::empty_tuple(5).leak_object(); }, METH_NOARGS, "Return the number of arguments received by the process." },
            { "tupleparser_test", tupleparser_test, METH_VARARGS, "TupleParser Test" },
            { NULL, NULL, 0, NULL }
        };

        static PyModuleDef pyssa_module = {
            PyModuleDef_HEAD_INIT, "pyssa", NULL, -1, pyssa_methods,
            NULL, NULL, NULL, NULL
        };
        auto module = PyModule_Create(&pyssa_module);
        PyModule_AddObject(module, "test2", Object::create("TEST").leak_object());
        PyModule_AddObject(module, "world", Environment::the().m_world.wrap().leak_object());
        PyModule_AddObject(module, "sv", Environment::the().m_simulation_view.wrap().leak_object());
        PyModule_AddType(module, &::Object::type_object());
        return module;
    });
}

}
