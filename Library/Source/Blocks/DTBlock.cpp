#include "DTBlock.h"

#include <streambuf>

#include <boost/endian.hpp>

namespace be = boost::endian;

namespace mdf {

    constexpr MdfHeader DTBlockHeader = {
        .blockType = MdfBlockType_DT,
        .blockSize = 24,
        .linkCount = 0
    };

    DTBlock::DTBlock() {
        header = DTBlockHeader;
    }

    bool DTBlock::load(std::shared_ptr<std::streambuf> stream_) {
        bool result = false;
        stream = stream_;

        result = true;

        return result;
    }

}
