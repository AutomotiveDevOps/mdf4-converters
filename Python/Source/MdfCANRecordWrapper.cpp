#include "MdfCANRecordWrapper.h"

MdfCANRecordWrapper::MdfCANRecordWrapper(mdf::CANRecord const& data) : record(data) {
  //
}

MdfCANRecordWrapper::~MdfCANRecordWrapper() = default;

void MdfCANRecordWrapper::init_type() {
  behaviors().name( "mdfsimple.CANRecord" );
  behaviors().doc( "" );
  behaviors().supportRepr();

  behaviors().readyType();
}

Py::Object MdfCANRecordWrapper::repr() {
  std::ostringstream s_out;
  s_out << record;

  return static_cast<Py::Object>(Py::String(s_out.str()));
}