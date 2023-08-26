#include <algorithm>
#include <iostream>
#include <dlfcn.h>
#include <string.h>
#include <vector>
#include <string>

#include "interfaces.h"

typedef void (*function_t)(void *);

struct ClassImpl {
  void* lib = nullptr;
  std::string class_name;
};

AbstractClass::AbstractClass() {
  pImpl = new ClassImpl();
}

AbstractClass::~AbstractClass() {
  if (pImpl->lib != nullptr) {
    dlclose(pImpl->lib);
  }
  delete pImpl;
}

std::string PathFromName(const std::string &name) {
  std::string copy = name;
  for (auto& c : copy) {
    if (c == ':') {
      c = '/';
    }
  }
  return std::string(std::getenv("CLASSPATH")) + "/" + copy + ".so";
}

std::string FullName(std::string &name) {
  size_t from = 0;
  size_t first_entry = 0;
  std::string sym_name;
  sym_name += "_ZN";
  while (true) {
    first_entry = name.find("::", from);
    if (first_entry == std::string::npos) {
      break;
    }
    sym_name += (std::to_string(first_entry - from) + name.substr(from, first_entry - from));
    from = first_entry + strlen("::");
  }
  sym_name += std::to_string(name.length() - from) + name.substr(from) + "C1Ev";
  return sym_name;
}

void *AbstractClass::newInstanceWithSize(size_t sizeofClass) {
  std::string sym_name = FullName(pImpl->class_name);
  function_t func = reinterpret_cast<function_t>(dlsym(pImpl->lib, sym_name.c_str()));
  void *obj = new int8_t[sizeofClass];
  func(obj);
  return obj;
}

struct ClassLoaderImpl {
  ClassLoaderError last_error = ClassLoaderError::NoError;
  std::vector<AbstractClass*> abstract_classes;
};

ClassLoader::ClassLoader() {
  pImpl = new ClassLoaderImpl();
}

ClassLoader::~ClassLoader() {
  for (auto ptr : pImpl->abstract_classes) {
    delete ptr;
  }
  delete pImpl;
}

ClassLoaderError ClassLoader::lastError() const {
  return pImpl->last_error;
}

AbstractClass *ClassLoader::loadClass(const std::string &fullyQualifiedName) {
  std::string class_path = PathFromName(fullyQualifiedName);
  int open_flag = RTLD_GLOBAL |  RTLD_NOW ;
  void *lib = dlopen(class_path.c_str(), open_flag);
  if (lib == nullptr) {
    pImpl->last_error = ClassLoaderError::LibraryLoadError;
    return nullptr;
  }

  pImpl->abstract_classes.push_back(new AbstractClass());
  pImpl->abstract_classes[pImpl->abstract_classes.size() - 1]->pImpl->class_name = fullyQualifiedName;
  pImpl->abstract_classes[pImpl->abstract_classes.size() - 1]->pImpl->lib = lib;

  pImpl->last_error = ClassLoaderError::NoError;
  return pImpl->abstract_classes[pImpl->abstract_classes.size() - 1];
}
