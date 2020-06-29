#include "Module.h"
#include "MdfFileWrapper.h"
#include "MdfCANIteratorWrapper.h"
#include "MdfCANRecordWrapper.h"
#include "PythonLogger.h"

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>

PYCXX_USER_EXCEPTION_STR_ARG(SimpleError)

Module::Module() : Py::ExtensionModule<Module>("mdfsimple") {
    // Map logging from boost to python.
    mdf::python::setupLogging();

    MdfFileWrapper::init_type();
    MdfCANIteratorWrapper::init_type();
    MdfCANRecordWrapper::init_type();

    add_keyword_method("make_instance", &Module::CreateMdfFile, "");

    initialize("");

    SimpleError::init(*this);
}

Py::Object Module::CreateMdfFile(const Py::Tuple &args, const Py::Dict &kwds) {
    Py::Callable class_type(MdfFileWrapper::type());

    Py::Object result = Py::None();

    try {
        result = Py::PythonClassObject<MdfFileWrapper>(class_type.apply(args, kwds));
    } catch (std::exception &e) {
        std::cout << e.what();
    }

    return result;
}

Module::~Module() = default;
