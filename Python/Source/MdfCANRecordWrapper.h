#ifndef MDFSUPER_MDFCANRECORDWRAPPER_H
#define MDFSUPER_MDFCANRECORDWRAPPER_H

#include <CXX/Objects.hxx>
#include <CXX/Extensions.hxx>

#include <CANRecord.h>

struct MdfCANRecordWrapper : public Py::PythonExtension<MdfCANRecordWrapper> {
  MdfCANRecordWrapper(mdf::CANRecord const& data);
  virtual ~MdfCANRecordWrapper();
  static void init_type(void);

  Py::Object repr();

private:
  mdf::CANRecord record;
};


#endif //MDFSUPER_MDFCANRECORDWRAPPER_H
