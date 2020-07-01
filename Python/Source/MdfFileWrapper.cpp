#include "MdfFileWrapper.h"

#include <boost/predef.h>

#include "CallbackBuffer.h"
#include "MdfCANIteratorWrapper.h"
#include "PythonLogger.h"

static Py::Object createNumpyArray(std::size_t size, std::string const& dtype);

template<typename T>
static T* getDataPointer(Py::Object column);

MdfFileWrapper::MdfFileWrapper(Py::PythonClassInstance *self, Py::Tuple &args, Py::Dict &kwds) :
    Py::PythonClass<MdfFileWrapper>::PythonClass(self, args, kwds) {
    // Check the arguments. If only one is present, it should match the IFileInterface. If two are present, the second
    // should match the targeted cache size.
    if(args.size() == 0) {
        throw Py::ValueError("Expected at least one argument, but got none");
    } else if(args.size() > 2) {
        std::stringstream ss;
        ss << "Expected at most two arguments, but got " << args.size();
        throw Py::ValueError(ss.str());
    }

    // Extract first argument.
    Py::Object fileHandle = args.front();

    // Check for a second argument.
    std::shared_ptr<mdf::python::CallbackBuffer> stream;

    try {
        if (args.size() > 1) {
            Py::Long cacheSize(args[1]);

            stream = std::make_shared<mdf::python::CallbackBuffer>(fileHandle, cacheSize);
        } else {
            stream = std::make_shared<mdf::python::CallbackBuffer>(fileHandle);
        }
    } catch (std::exception &e) {
        throw Py::RuntimeError(e.what());
    }


    try {
        backingFile = mdf::MdfFile::Create(stream);
    } catch(std::exception &e) {
        // Re-throw in Python.
        throw Py::RuntimeError(e.what());
    }

    if(!backingFile) {
        throw Py::RuntimeError("Could not parse input data");
    }
}

MdfFileWrapper::~MdfFileWrapper() = default;

Py::Object MdfFileWrapper::GetCANIterator() {
    return Py::asObject(new MdfCANIteratorWrapper(backingFile->getCANIterator()));
}

PYCXX_NOARGS_METHOD_DECL(MdfFileWrapper, GetCANIterator)

Py::Object MdfFileWrapper::GetFirstMeasurement() {
    Py::Long result;

    std::chrono::nanoseconds value = backingFile->getFirstMeasurement();

    result = value.count();

    return static_cast<Py::Object>(result);
}

PYCXX_NOARGS_METHOD_DECL(MdfFileWrapper, GetFirstMeasurement)

Py::Object MdfFileWrapper::GetDataFrame() {
    BOOST_LOG_SEV(pythonLogger::get(), severity_level::trace) << "DataFrameLog";
    // Extract an iterator to ensure the file is finalized and sorted.
    auto iterator = backingFile->getCANIterator();

    // If file info is not yet loaded, force it to load here.
    backingFile->loadFileInfo();

    // Load required modules.
    Py::Module np("numpy");
    Py::Module pd("pandas");
    Py::Module datetime("datetime");

    // Get the number of CAN records.
    std::size_t numberOfRecords = backingFile->getFileInfo().CANMessages;

    // Create numpy arrays for all CAN columns.
    auto columnTimeStamp = createNumpyArray(numberOfRecords, "datetime64[ns]");
    auto columnBusChannel = createNumpyArray(numberOfRecords, "uint8");
    auto columnID = createNumpyArray(numberOfRecords, "uint32");
    auto columnIDE = createNumpyArray(numberOfRecords, "bool");
    auto columnDLC = createNumpyArray(numberOfRecords, "uint8");
    auto columnDataLength = createNumpyArray(numberOfRecords, "uint8");
    auto columnDir = createNumpyArray(numberOfRecords, "bool");
    auto columnBRS = createNumpyArray(numberOfRecords, "bool");
    auto columnEDL = createNumpyArray(numberOfRecords, "bool");
    auto columnDataBytes = createNumpyArray(numberOfRecords, "object");

    // Access the raw data pointers for all CAN columns.
    // NOTE: datetime64[ns] is represented as a floating point and not an integer in numpy.
    auto columnTimeStamp_ptr = getDataPointer<double>(columnTimeStamp);
    auto columnBusChannel_ptr = getDataPointer<uint8_t>(columnBusChannel);
    auto columnID_ptr = getDataPointer<uint32_t>(columnID);
    auto columnIDE_ptr = getDataPointer<bool>(columnIDE);
    auto columnDLC_ptr = getDataPointer<uint8_t>(columnDLC);
    auto columnDataLength_ptr = getDataPointer<uint8_t>(columnDataLength);
    auto columnDir_ptr = getDataPointer<bool>(columnDir);
    auto columnBRS_ptr = getDataPointer<bool>(columnBRS);
    auto columnEDL_ptr = getDataPointer<bool>(columnEDL);
    // NOTE: The raw pointer access to databytes is not present, since this an array of pointers to objects.

    // Iterate over all records, copying the data into the correct columns.
    Py::Tuple dataBytesArg;
    Py::Long ctr(0);

    for(auto const& record: iterator) {
        // Raw data, copy directly.
        *columnTimeStamp_ptr++ = record.TimeStamp.count();
        *columnBusChannel_ptr++ = record.BusChannel;
        *columnID_ptr++ = record.ID;
        *columnIDE_ptr++ = record.IDE;
        *columnDLC_ptr++ = record.DLC;
        *columnDataLength_ptr++ = record.DataLength;
        *columnDir_ptr++ = record.Dir;
        *columnBRS_ptr++ = record.BRS;
        *columnEDL_ptr++ = record.EDL;

        // Databytes is vector like, copy as a list.
        Py::List dataBytes(record.DataBytes.size());
        for(int i = 0; i < record.DataBytes.size(); ++i) {
            dataBytes.setItem(i, Py::Long(record.DataBytes[i]));
        }

        dataBytesArg = Py::TupleN(ctr++, dataBytes);
        columnDataBytes.callMemberFunction("__setitem__", dataBytesArg);

        // NOTE: Code for copying as bytes kept for reference.
        // auto ptr = reinterpret_cast<char const*>(record.DataBytes.data());
        // dataBytesArg = Py::TupleN(Py::Long(ctr), Py::Bytes(ptr, record.DataBytes.size()));
        // columnDataBytes.callMemberFunction("__setitem__", dataBytesArg);
    }

    // Create index from the timestamp column.
    Py::Dict indexArgs;
    auto const timezone = datetime.getAttr("timezone");
    auto const utc = timezone.getAttr("utc");
    indexArgs["tz"] = utc;
    Py::Object index = pd.callMemberFunction("DatetimeIndex", Py::TupleN(columnTimeStamp), indexArgs);
    index.setAttr("name", Py::String("TimeStamp"));

    // Create a pandas data frame with the index.
    Py::Dict dataFrameArgs;
    dataFrameArgs["index"] = index;
    Py::Object dataFrame = pd.callMemberFunction("DataFrame", Py::Tuple(), dataFrameArgs);

    // Merge the columns into the data frame.
    Py::Tuple insertArgs;
    int columnCounter = 0;

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("BusChannel"), columnBusChannel);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("ID"), columnID);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("IDE"), columnIDE);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("DLC"), columnDLC);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("DataLength"), columnDataLength);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("Dir"), columnDir);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("EDL"), columnEDL);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("BRS"), columnBRS);
    dataFrame.callMemberFunction("insert", insertArgs);

    insertArgs = Py::TupleN(Py::Long(columnCounter++), Py::String("DataBytes"), columnDataBytes);
    dataFrame.callMemberFunction("insert", insertArgs);

    return static_cast<Py::Object>(dataFrame);
}

PYCXX_NOARGS_METHOD_DECL(MdfFileWrapper, GetDataFrame)

void MdfFileWrapper::init_type() {
    behaviors().name("mdf_iter.MdfFile");
    behaviors().doc("Read-only representation of a MDF file containing bus logging data");
    behaviors().supportGetattro();
    behaviors().supportSetattro();

    // Check if the requirements for reading out into pandas data frames are present.
    auto const numpyIsPresent = PyImport_ImportModule("numpy");
    auto const pandasIsPresent = PyImport_ImportModule("pandas");
    auto const datetimeIsPresent = PyImport_ImportModule("datetime");

    BOOST_LOG_SEV(pythonLogger::get(), severity_level::trace) << "Checking for requirements for export to data frames";
    bool toDataFrameRequirementsPresent = true;

    if(numpyIsPresent == nullptr) {
        BOOST_LOG_SEV(pythonLogger::get(), severity_level::trace) << "numpy is not present";
        toDataFrameRequirementsPresent = false;
    }
    if(pandasIsPresent == nullptr) {
        BOOST_LOG_SEV(pythonLogger::get(), severity_level::trace) << "pandas is not present";
        toDataFrameRequirementsPresent = false;
    }
    if(datetimeIsPresent == nullptr) {
        BOOST_LOG_SEV(pythonLogger::get(), severity_level::trace) << "datetime is not present";
        toDataFrameRequirementsPresent = false;
    }

    if(toDataFrameRequirementsPresent) {
        BOOST_LOG_SEV(pythonLogger::get(), severity_level::trace) << "Enabling data frame export";

        PYCXX_ADD_NOARGS_METHOD(
            get_data_frame,
            GetDataFrame,
            "Get data frame over CAN records"
        );
    }

    PYCXX_ADD_NOARGS_METHOD(
        get_first_measurement,
        GetFirstMeasurement,
        "Get the timestamp of the first measurement in the data"
        );

    PYCXX_ADD_NOARGS_METHOD(
        get_can_iterator,
        GetCANIterator,
        "Get iterator over CAN records"
        );

    behaviors().readyType();
}

Py::Object MdfFileWrapper::getattro(const Py::String &name_) {
    return genericGetAttro(name_);
}

int MdfFileWrapper::setattro(const Py::String &name_, const Py::Object &value) {
    return genericSetAttro(name_, value);
}

static Py::Object createNumpyArray(std::size_t size, std::string const& dtype) {
    Py::Module np("numpy");

    return np.callMemberFunction("empty", Py::TupleN(Py::Long(size), np.getAttr(dtype)));
}

template<typename T>
static T* getDataPointer(Py::Object column) {
    // See https://stackoverflow.com/a/11266170
    Py::Dict columnID_arrayInterface = column.getAttr("__array_interface__");
    if(columnID_arrayInterface.isNull() || columnID_arrayInterface.isNone()) {
        throw Py::RuntimeError("Could not get numpy pointer");
    }

    // Extract the type information.
    Py::String columnID_arrayInterface_typestr = columnID_arrayInterface.getItem("typestr");
    if(columnID_arrayInterface_typestr.isNull() || columnID_arrayInterface_typestr.isNone()) {
        throw Py::RuntimeError("Could not get numpy pointer type information");
    }

    std::string const ptrType = columnID_arrayInterface_typestr.as_std_string();

    if(ptrType.length() != 3) {
        throw Py::RuntimeError("Could not get numpy pointer type information");
    }

    char endian = ptrType[0];
    char dataType = ptrType[1];
    char byteCount = ptrType[2];

    // Check endian.
    if constexpr(BOOST_ENDIAN_LITTLE_BYTE) {
        if (endian == '>') {
            throw Py::RuntimeError("numpy array does not use native endian format");
        }
    } else {
        if (endian == '<') {
            throw Py::RuntimeError("numpy array does not use native endian format");
        }
    }

    // Check the type.
    if constexpr(std::is_same_v<T, bool>) {
        if (dataType != 'b') {
            throw Py::RuntimeError("numpy array does not match the expected type");
        }
    } else if constexpr(std::is_floating_point_v<T>) {
        if (dataType != 'f') {
            throw Py::RuntimeError("numpy array does not match the expected type");
        }
    } else if constexpr(std::is_integral_v<T>) {
        if constexpr(std::is_unsigned_v<T>) {
            if (dataType != 'u') {
                throw Py::RuntimeError("numpy array does not match the expected type");
            }
        } else {
            if (dataType != 'i') {
                throw Py::RuntimeError("numpy array does not match the expected type");
            }
        }
    }

    // Ensure the number of bytes matches.
    if constexpr(sizeof(T) == 1) {
        if (byteCount != '1') {
            throw Py::RuntimeError("numpy array does not use the expected number of bytes");
        }
    } else if constexpr(sizeof(T) == 2) {
        if (byteCount != '2') {
            throw Py::RuntimeError("numpy array does not use the expected number of bytes");
        }
    } else if constexpr(sizeof(T) == 4) {
        if (byteCount != '4') {
            throw Py::RuntimeError("numpy array does not use the expected number of bytes");
        }
    } else if constexpr(sizeof(T) == 8) {
        if (byteCount != '8') {
            throw Py::RuntimeError("numpy array does not use the expected number of bytes");
        }
    }

    Py::Tuple columnID_arrayInterface_data = columnID_arrayInterface.getItem("data");
    if(columnID_arrayInterface_data.isNull() || columnID_arrayInterface_data.isNone()) {
        throw Py::RuntimeError("Could not get numpy pointer part 2");
    }

    Py::Long ptrAddress = static_cast<Py::Long>(columnID_arrayInterface_data.getItem(0));

    T* address = reinterpret_cast<T*>(ptrAddress.as_unsigned_long_long());

    return address;
}
