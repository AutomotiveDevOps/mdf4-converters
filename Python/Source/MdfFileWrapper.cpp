#include "CallbackBuffer.h"
#include "MdfCANIteratorWrapper.h"
#include "MdfFileWrapper.h"

MdfFileWrapper::MdfFileWrapper(Py::PythonClassInstance *self, Py::Tuple &args, Py::Dict &kwds) :
    Py::PythonClass<MdfFileWrapper>::PythonClass(self, args, kwds) {

    // Look at the default argument.
    auto stream = std::make_shared<mdf::python::CallbackBuffer>(args.front());
    backingFile = mdf::MdfFile::Create(stream);
}

MdfFileWrapper::~MdfFileWrapper() = default;

Py::Object MdfFileWrapper::GetCANIterator() {

    return Py::asObject(new MdfCANIteratorWrapper(backingFile->getCANIterator()));

}

PYCXX_NOARGS_METHOD_DECL(MdfFileWrapper, GetCANIterator)

void MdfFileWrapper::init_type() {
    behaviors().name("mdfsimple.MdfFile");
    behaviors().doc("");
    behaviors().supportGetattro();
    behaviors().supportSetattro();

    PYCXX_ADD_NOARGS_METHOD(get_can_iterator, GetCANIterator, "Get iterator over CAN records");

    behaviors().readyType();
}

Py::Object MdfFileWrapper::getattro(const Py::String &name_) {
    return genericGetAttro(name_);
}

int MdfFileWrapper::setattro(const Py::String &name_, const Py::Object &value) {
    return genericSetAttro(name_, value);
}