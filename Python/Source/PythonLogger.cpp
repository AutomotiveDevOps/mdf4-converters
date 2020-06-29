#include "PythonLogger.h"

#include <memory>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/keywords/severity.hpp>
#include <boost/log/sinks.hpp>

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", severity_level);

BOOST_LOG_GLOBAL_LOGGER_INIT(pythonLogger, boost::log::sources::severity_logger<severity_level>)
{
    boost::log::sources::severity_logger<severity_level> lg;
    return lg;
}

namespace mdf::python {



    typedef boost::log::sinks::synchronous_sink<PythonLogger> python_sink_t;

    bool setupLogging() {
        // Add synchronous sink for logged messages.
        boost::shared_ptr<python_sink_t> pythonSink = boost::make_shared<python_sink_t>();

        // Register the new sink in the logging core.
        boost::log::core::get()->add_sink(pythonSink);

        BOOST_LOG_SEV(pythonLogger::get(), severity_level::trace) << "Logger initialized";

        return true;
    }

    PythonLogger::PythonLogger() {
        // Attempt to find the required python module.
        pythonLogger = PyImport_ImportModule("logging");

        if(pythonLogger == NULL) {
            // TODO: Error out.
        }
    }

    void PythonLogger::consume(const boost::log::record_view &rec) {
        // Extract the severity.
        auto a = rec[severity];
        severity_level severity = a.get();
        Py::Long loggerLevel;

        switch(severity) {
            case trace:
                // Fallthrough
            case debug:
                loggerLevel = 40;
                break;
            case info:
                loggerLevel = 40;
                break;
            case warning:
                loggerLevel = 40;
                break;
            case error:
                loggerLevel = 40;
                break;
            case fatal:
                loggerLevel = 50;
                break;
            default:
                loggerLevel = 0;
                break;
        }

        // Extract the message.
        std::string loggingMessage = *rec[boost::log::expressions::smessage];

        // Delegate to python logging module.
        Py::String msg(loggingMessage);
        Py::String log_name("log");

        PyObject_CallMethodObjArgs(pythonLogger, log_name.ptr(), loggerLevel.ptr(), msg.ptr(), NULL);
    }

}
