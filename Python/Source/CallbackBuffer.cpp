#include "CallbackBuffer.h"

#include "PythonLogger.h"

namespace mdf::python {

    CallbackBuffer::CallbackBuffer(Py::Object obj, std::size_t bufferSize) :
        obj(std::move(obj)),
        absolutePosition(0),
        bufferOffset(0),
        bufferSize(bufferSize) {
        // TODO:
        // Ensure that the object has the required properties.

        // Setup buffer.
        buffer = ::std::make_unique<char*>(new char[bufferSize]);
        bufferPointer = *buffer.get();

        // Reset pointers.
        setg(bufferPointer, bufferPointer, bufferPointer);
    }

    CallbackBuffer::traits_type::int_type CallbackBuffer::underflow() {
        // Create bytes buffer.
        PyObject* pythonBuffer = PyMemoryView_FromMemory(reinterpret_cast<char *>(eback()), bufferSize, 0x200);
        Py::Object wrappedBuffer = Py::asObject(pythonBuffer);
        Py::Long bufferSizeObj(bufferSize);


        // Call in python.
        Py::Object result = obj.callMemberFunction("read", Py::TupleN(wrappedBuffer, bufferSizeObj));
        Py::Long bytesRead(result);

        // Update pointers.
        setg(bufferPointer, bufferPointer, bufferPointer + bytesRead);

        if(bytesRead == 0) {
            return traits_type::eof();
        } else {
            return traits_type::to_int_type( *buffer.get()[0]);
        }
    }

    CallbackBuffer::traits_type::pos_type CallbackBuffer::seekoff(
        off_type offset,
        std::ios_base::seekdir direction,
        std::ios_base::openmode mode) {

        // Early exit for tellg() calls.
        if(offset == 0 && direction == std::ios_base::cur) {
            pos_type result(bufferOffset + (gptr() - eback()));
            return result;
        }

        // Can the seek be performed in the current buffer?
        if(direction == std::ios::beg) {
            // Calculate relative position.
            /*auto rel = offset - bufferOffset;

            if(rel < (egptr() - eback())) {
                // Can be done with a movement of the pointers on the already read data.

                setg(bufferPointer + rel, bufferPointer + rel, egptr());
            } else */{
                // Don't optimize. Defer call and set buffers to empty.
                Py::Long argOffset(offset);
                Py::Long argDirection(direction);

                Py::Object bytesReadObj = obj.callMemberFunction("seek", Py::TupleN(argOffset, argDirection));
                Py::Long bufferOffsetRes(bytesReadObj);
                bufferOffset = bufferOffsetRes;

                setg(bufferPointer, bufferPointer, bufferPointer);
            }
        } else if(direction == std::ios::cur) {
            // Determine which way to seek.
            std::ptrdiff_t available = 0;

            if(offset > 0) {
                available = egptr() - gptr();
            } else if (offset < 0) {
                available = eback() - gptr();
            }

            if(offset > available) {
                setg(bufferPointer, bufferPointer, bufferPointer);
            } else {
                setg(*buffer.get(), gptr() + offset, egptr());
            }
        } else if(direction == std::ios::end) {
            // Don't optimize. Defer call and set buffers to empty.
            Py::Long argOffset(offset);
            Py::Long argDirection(direction);

            Py::Object bytesReadObj = obj.callMemberFunction("seek", Py::TupleN(argOffset, argDirection));
            Py::Long bufferOffsetRes(bytesReadObj);
            bufferOffset = bufferOffsetRes;

            setg(bufferPointer, bufferPointer, bufferPointer);
        }

        // Calculate new absolute position from buffer position and relative position within the buffer.
        pos_type result(bufferOffset + (gptr() - eback()));

        return result;
    }

    CallbackBuffer::traits_type::pos_type CallbackBuffer::seekpos(pos_type offset, std::ios_base::openmode mode) {
        return seekoff(offset, std::ios::beg, mode);
    }

    void exportCallbackBuffer() {

    }

}
