#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <Python.h>
#include <frameobject.h>

char* ReadFromFile(char* fpath) {
  FILE* file = fopen(fpath, "r");
  char inp= fgetc(file);
  size_t cnt = 0;
  char* buf = calloc(sizeof(char), BUFSIZ * 128);
  while (inp != EOF) {
    buf[cnt++] = inp;
    inp = fgetc(file);
  }
  fclose(file);
  return buf;
}

void Complete(PyObject* commands) {
  if (PyList_Size(commands) == 0) {
    return;
  }
  PyList_SetSlice(commands, 0, 1, NULL); // equal to pop_front
}


int GetCommand(PyObject* commands) {
  if (PyList_Size(commands) == 0) {
    return 42;
  }
  return PyLong_AsLong(PyTuple_GetItem(PyList_GetItem(commands, 0), 0));
}

int depth = 0;
int neccesary_depth = 0;
int need_main = 1;

void Print(PyObject* commands) {
  char command[128] = {};
  uint32_t* expr = PyUnicode_AsUCS4Copy(PyTuple_GetItem(PyList_GetItem(commands, 0), 1));
  sprintf(command, "print(repr(%ls))", expr);
  PyRun_String(command, 256, PyEval_GetGlobals(), PyEval_GetLocals());
}

void FullPrint(PyObject* commands) {
  while (GetCommand(commands) == 2) {
    Print(commands);
    Complete(commands);
  }
}

int trace(PyObject* commands, PyFrameObject* frame, int event, PyObject* t) {
  if (PyList_Size(commands) == 0) {
    return 0;
  }

  if (event == PyTrace_CALL) {
    if (need_main == 1) {
      need_main = 0;
      return 0;
    }
    if (neccesary_depth != depth) {
      return 0;
    }
    ++depth;
    int curr_command = GetCommand(commands);
    if (curr_command == 1) { // step
      Complete(commands);
      neccesary_depth++;
    } else if (curr_command == 2) {
      FullPrint(commands);
    } else if (curr_command == 0) {
      Complete(commands);
    }
    return 0;
  }

  if (event == PyTrace_LINE) { // handle line
    if (neccesary_depth != depth) {
      return 0;
    }
    long curr_command = GetCommand(commands); // current action
    if (curr_command == 0 || curr_command == 1) { // next
      Complete(commands);
    } else if (curr_command == 2) {
      FullPrint(commands);
    }
  }

  if (event == PyTrace_RETURN && depth != 0) { // return
    if (neccesary_depth != depth) {
      depth--;
      return 0;
    }
    int curr_command = GetCommand(commands);
    if (curr_command == 0 || curr_command == 1) { // next
      Complete(commands);
    } else if (curr_command == 2) {
      FullPrint(commands);
    }
    depth--;
    neccesary_depth--;
  }

  return 0;
}

PyObject* BuildTup(PyObject* num, PyObject* expr) {
  PyObject* tuple = PyTuple_New(2);
  PyTuple_SetItem(tuple, 1, expr);
  PyTuple_SetItem(tuple, 0, num);
  return tuple;
}

void InitArgs(int n, char** argv) {
  wchar_t** py_args = PyMem_Malloc(sizeof(char*) * (n-3));
  for (int i = 0; i < n-3; i++) {
    py_args[i] = Py_DecodeLocale(argv[i + 3], NULL);
  }
  PySys_SetArgv(n-3, py_args);
  PyMem_Free(py_args);
}

void ParseCommandFile(char* cmds, PyObject* lst) {
  char* cmds_lines = ReadFromFile(cmds);
  PyObject* lines = PyUnicode_Splitlines(PyUnicode_FromString(cmds_lines), 0);
  for (size_t i = 0; i < PyList_Size(lines); ++i) { // iter over line in lines
    PyObject* line = PyList_GetItem(lines, i); // lines[i]

    if (PyUnicode_Contains(line, PyUnicode_FromString("step"))) {
      PyList_Append(lst, BuildTup(PyLong_FromLong(1), NULL));
      continue;
    }

    else if (PyUnicode_Contains(line, PyUnicode_FromString("next"))) {
      PyList_Append(lst, BuildTup(PyLong_FromLong(0), NULL));
      continue;

    } else if (PyUnicode_Contains(line, PyUnicode_FromString("print"))) {
      PyObject* splits = PyUnicode_Split(line, PyUnicode_FromString("print"), -1); // sep print and expression
      PyList_Append(lst, BuildTup(PyLong_FromLong(2), PyList_GetItem(splits, 1))); // add expression as second param

    } else if (PyUnicode_Contains(line, PyUnicode_FromString("continue"))) {
      return;
    }

  }
}

int main(int argc, char** argv) {
  Py_Initialize();

  if (argc <= 2) {
    PyErr_BadArgument();
  }

  PyObject* lst = PyList_New(0);
  ParseCommandFile(argv[1], lst);


  PyEval_SetTrace(trace, lst);
  PyRun_SimpleString(ReadFromFile(argv[2]));

  Py_Finalize();
  return 0;
}
