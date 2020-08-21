#ifndef MDFSUPER_MDFFILEWRAPPER_H
#define MDFSUPER_MDFFILEWRAPPER_H

#include <CXX/Extensions.hxx>
#include <CXX/Objects.hxx>

#include "MdfFile.h"

/**
 * MdfFileWrapper is wrapping the MdfFile interface using the new style in
 * PyCXX. The new style interface was chosen since this object is going to be
 * constructed from Python only.
 */
struct MdfFileWrapper : public Py::PythonClass<MdfFileWrapper> {
  MdfFileWrapper(Py::PythonClassInstance *self,
                 Py::Tuple &args,
                 Py::Dict &kwds);

  ~MdfFileWrapper() override;

  static void init_type();

  Py::Object GetCANIterator();

  Py::Object GetFirstMeasurement();

  Py::Object GetDataFrame();

  Py::Object getattro(const Py::String &name_) override;

  int setattro(const Py::String &name_, const Py::Object &value) override;

protected:
  std::unique_ptr<mdf::MdfFile> backingFile;
};

#endif // MDFSUPER_MDFFILEWRAPPER_H
