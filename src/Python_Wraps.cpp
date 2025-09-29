#include "Python_Wraps.h"

PyWraps::PyCallable::PyCallable(PyObject *pModule, std::string name) {
    pObject = PyObject_GetAttrString(pModule, name.c_str());
    if(!pObject || !PyCallable_Check(pObject)) throw PyWraps::Error::PyObjectNotFound();
}

PyObject *PyWraps::PyCallable::operator()(PyObject **pValue) {
    *pValue = PyObject_CallNoArgs(pObject);
    return *pValue;
}

PyObject *PyWraps::PyCallable::operator()(PyObject **pValue, PyObject *pArgs) {
    *pValue = PyObject_CallObject(pObject, pArgs);
    return *pValue;
}

PyObject *PyWraps::PyCallable::operator()(PyObject **pValue, PyObject *pArgs, PyObject *pKwargs) {
    *pValue = PyObject_Call(pObject, pArgs, pKwargs);
    return *pValue;
}
