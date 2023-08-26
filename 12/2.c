#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <Python.h>

int main() {
  Py_Initialize();

  PyRun_SimpleString("from sys import stdin");
  PyRun_SimpleString("matr = list()");

  PyRun_SimpleString("for line in stdin.readlines(): matr.append(line.split(\';\'))");

  PyRun_SimpleString("matr = list(map(lambda l: list(map(lambda x: x.replace(\'\\n\',\'\'), l)), matr))");
  PyRun_SimpleString("for t in range(0, 234):"
                     "\n  for i in range(0, len(matr)):"
                     "\n    for j in range(0, len(matr[i])):"
                     "\n      for k in range(len(matr)):"
                     "\n        for l in range(len(matr[k])):"
                     "\n          matr[k][l] = matr[k][l].replace(str(chr(ord('A')+j))+str(i+1), \'(\' + str(matr[i][j]) + \')\')"
                     "\n          matr[k][l] = matr[k][l].replace(\'=\', \'\')"
  );

  PyRun_SimpleString("for lst in matr:"
                     "\n  for i, elem in enumerate(lst):"
                     "\n    if (len(str(elem)) == 0):"
                     "\n      print(\'\', end = \'\')"
                     "\n    else:"
                     "\n      print(eval(elem), end = '')"
                     "\n    if (i != len(lst)-1):"
                     "\n      print(\';\', end = \'\')"
                     "\n  print()"
                     );

  Py_Finalize();
}
