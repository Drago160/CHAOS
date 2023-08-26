#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <Python.h>

static PyObject*
factor_out(PyObject *module, PyObject *args) {
  if (PyTuple_Size(args) != 1) {
    PyErr_BadArgument();
    return NULL;
  }
  PyObject *py_result = PyList_New(0);
  PyObject* py_number = PyTuple_GetItem(args, 0);
  size_t num = PyLong_AsSize_t(py_number);
  if (!PyLong_Check(py_number)) {
    PyErr_BadArgument();
    return NULL;
  }
  if (num < 2) {
    PyErr_BadArgument();
    return NULL;
  }

  size_t i = 2;
  while (i <= num) {
    PyObject* new_num = PyLong_FromSize_t(i);
    if (num % i == 0) {
      num /= i;
      PyList_Append(py_result, new_num);
      continue;
    }
    ++i;
  }
  PyObject* prime_label = PyUnicode_FromString("Prime!");
  if (PyList_Size(py_result) == 1) {
    return prime_label;
  }
  return py_result;
}

PyMODINIT_FUNC PyInit_primes() {
  static PyMethodDef methods[] = {
      {"factor_out",
       factor_out,
       METH_VARARGS,
       "factorization"},

      {NULL, NULL, 0, NULL}
  };

  static PyModuleDef moduleDef = {
      PyModuleDef_HEAD_INIT,
      "primes",
      "prime funcs",
      -1,
      methods
  };
  return PyModule_Create(&moduleDef);
}
