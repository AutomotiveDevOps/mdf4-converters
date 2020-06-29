#ifndef MDFSUPER_MODULE_H
#define MDFSUPER_MODULE_H

#ifdef Py_DEBUG
#undef Py_DEBUG
#endif

#ifdef _DEBUG
#undef _DEBUG
#define NDEBUG
#endif

#include "CXX/Objects.hxx"
#include "CXX/Extensions.hxx"

struct Module : public Py::ExtensionModule<Module>{
  Module();
  virtual ~Module();

  Py::Object CreateMdfFile( const Py::Tuple &args, const Py::Dict &kwds );

};


#endif //MDFSUPER_MODULE_H
