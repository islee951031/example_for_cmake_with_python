#ifndef __PYTHON_WRAPS__
#define __PYTHON_WRAPS__

#include <string>
#include <stdexcept>

#define PY_SSIZE_T_CLEAN
#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

namespace PyWraps {
    namespace Error {
        class PyObjectNotFound : public std::exception {
            public:
            virtual const char* what() const noexcept override {return "Error: Cannot find PyObject";}
        };

        class PyModuleNotFound : public std::exception {
            public:
            virtual const char* what() const noexcept override {return "Error: Cannot find Module";}
        };

        class PyOjectConvertError : public std::exception {
            public:
            virtual const char* what() const noexcept override {return "Error: Cannot convert PyObject";}
        };

        class PyMemAllocError : public std::exception {
            public:
            virtual const char* what() const noexcept override {return "Error: Fail to allocate memory";}
        };

        class PyObjectCallError : public std::exception {
            public:
            virtual const char* what() const noexcept override {return "Error: Fail to call object";}
        };
    }

    class PyCallable {
        private:
        PyObject* pObject;

        public:
        PyCallable(PyObject* pModule, std::string name);
        ~PyCallable() {Py_DECREF(pObject);}
        PyObject* operator()(PyObject** pValue);
        PyObject* operator()(PyObject** pValue, PyObject* args);
        PyObject* operator()(PyObject** pValue, PyObject* args, PyObject* pKwargs);
    };
    
};

#endif