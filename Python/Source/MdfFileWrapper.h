#ifndef MDFSUPER_MDFFILEWRAPPER_H
#define MDFSUPER_MDFFILEWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include "MdfFile.h"

struct MdfFileWrapper : public Py::PythonClass<MdfFileWrapper> {
  MdfFileWrapper(Py::PythonClassInstance *self, Py::Tuple &args, Py::Dict &kwds );
  virtual ~MdfFileWrapper();
  static void init_type(void);

  Py::Object GetCANIterator();

  Py::Object getattro( const Py::String &name_ );
  int setattro( const Py::String &name_, const Py::Object &value );

protected:
  std::unique_ptr<mdf::MdfFile> backingFile;
};

#endif //MDFSUPER_MDFFILEWRAPPER_H
