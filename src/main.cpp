#include <iostream>
#include <string>
#include <array>
#include <filesystem>
namespace fs = std::filesystem;

#include <wchar.h>
#include<conio.h>

#define PY_SSIZE_T_CLEAN
#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#include "Python_Wraps.h"

/// @brief Initialized python interpreter on isolated mode
/// @param program program name using PyDecodeLocale
/// @return PyStatus
PyStatus init_python(const wchar_t* program) {
    //API Reference https://docs.python.org/ko/3/c-api/init_config.html#python-path-configuration
    
    //get python absolute path
    fs::path python_path = fs::canonical(fs::path("./python"));
    
    PyStatus status;
    PyConfig config;
    PyConfig_InitIsolatedConfig(&config);
    try {
        //set program name
        status = PyConfig_SetString(&config, &config.program_name, program);
        if(PyStatus_Exception(status)) throw status;

        // Read all configuration at once to verify
        status = PyConfig_Read(&config);
        if(PyStatus_Exception(status)) throw status;

        //set python_home
        status = PyConfig_SetString(&config, &config.home, python_path.wstring().c_str());
        if(PyStatus_Exception(status)) throw status;

        //set sys.path explictly
        config.module_search_paths_set = 1;
        std::array<fs::path, 4> path_arr = {fs::path("./python/DLLs"), fs::path("./python/Lib"), fs::path("./python/Lib/site-packages"), fs::path("./python-scripts")};
        for(auto iter : path_arr) {
            status = PyWideStringList_Append(&config.module_search_paths, fs::canonical(iter).wstring().c_str());
            if(PyStatus_Exception(status)) throw status;
        }

        status = Py_InitializeFromConfig(&config);
        PyConfig_Clear(&config);
        return status;
    }
    catch(PyStatus& status) {
        PyConfig_Clear(&config);
        return status;
    }
}

int main(int argc, char* argv[]) {
    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if(program == NULL) {
        std::cerr << "Fatal Error: cannot decode argv[0]" << std::endl;
        exit(1);
    }
    init_python(program);
    PyRun_SimpleString("import sys");
    #ifdef _DEBUG
    std::cout << "======sys.path======" << std::endl;
    PyRun_SimpleString("print(sys.path)");
    #endif

    PyRun_SimpleString("print(sys.version)");
    PyRun_SimpleString("print(sys.executable)");

    std::cout << "\n======numpy test======" << std::endl;
    PyRun_SimpleString("import numpy as np");
    std::cout << "print(np.array([1, 2, 3]))" << std::endl;
    PyRun_SimpleString("print(np.array([1, 2, 3]))");

    std::cout << "\n======Call functions in my_module.py======" <<std::endl;
    //https://docs.python.org/3/c-api/import.html
    //https://docs.python.org/3/c-api/call.html#object-calling-api
    //https://docs.python.org/ko/3/extending/embedding.html
    PyObject *pModule = nullptr;
    PyObject *pArgs = nullptr;
    PyObject *pValue = nullptr;
    try {
        std::cout << "import my_module" << std::endl;
        pModule = PyImport_ImportModule("my_module");
        if(!pModule) throw PyWraps::Error::PyModuleNotFound();

        std::cout << "excute say_hello" << std::endl;
        PyWraps::PyCallable fsay_hello(pModule, "say_hello");
        fsay_hello(&pValue);

        std::cout << "create tuple for args" << std::endl;
        pArgs = PyTuple_New(2);
        if(!pArgs) throw PyWraps::Error::PyMemAllocError();

        std::cout << "add 2.0 on tuple" << std::endl;
        pValue = PyFloat_FromDouble(2.0);
        if(!pValue) throw PyWraps::Error::PyOjectConvertError();
        PyTuple_SetItem(pArgs, 0, pValue); //pValue reference stolen here
        pValue = nullptr;
        
        std::cout << "add 3.0 on tuple" << std::endl;
        pValue = PyFloat_FromDouble(3.0);
        if(!pValue) throw PyWraps::Error::PyOjectConvertError();
        PyTuple_SetItem(pArgs, 1, pValue); //pValue reference stolen here
        pValue = nullptr;

        std::cout << "excute mutiple" << std::endl;
        PyWraps::PyCallable fmultiple(pModule, "multiple");
        fmultiple(&pValue, pArgs);
        if(!pValue) throw PyWraps::Error::PyObjectCallError();
        std::cout << "Result of Call: " << PyFloat_AsDouble(pValue) << std::endl;
        Py_DECREF(pValue);
        Py_DECREF(pArgs);
        Py_DECREF(pModule);
    }
    catch(std::exception& e) {
        std::cout << e.what() << std::endl;
        if(pValue) {
            Py_DECREF(pValue);
            pValue = nullptr;
        }
        if(pArgs) {
            Py_DECREF(pArgs);
            pArgs = nullptr;
        }
        if(pModule) {
            Py_DECREF(pModule);
            pArgs = nullptr;
        }

    }
    
    if(Py_FinalizeEx() < 0) {
        exit(120);
    }
    PyMem_RawFree(program);
    std::cout << "\nPress any key to exit" << std::endl;
    _getch();
    return 0;
}