#ifndef MDFSUPER_CALLBACKBUFFER_H
#define MDFSUPER_CALLBACKBUFFER_H

#include <istream>
#include <memory>

#include "CXX/Objects.hxx"

namespace mdf::python {

    struct CallbackBuffer : std::streambuf {
        CallbackBuffer(Py::Object obj, std::size_t bufferSize = 4096);

        int_type underflow() override;
        pos_type seekoff(off_type, std::ios_base::seekdir, std::ios_base::openmode) override;
        pos_type seekpos(pos_type, std::ios_base::openmode) override;

    private:
        Py::Object obj;
        std::unique_ptr<char*> buffer;

        traits_type::pos_type absolutePosition;
        std::streamoff bufferOffset;
        std::size_t bufferSize;
        char* bufferPointer;
    };

    void exportCallbackBuffer();

}

#endif //MDFSUPER_CALLBACKBUFFER_H
