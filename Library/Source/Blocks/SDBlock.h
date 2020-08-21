#ifndef MDFSIMPLECONVERTERS_SDBLOCK_H
#define MDFSIMPLECONVERTERS_SDBLOCK_H

#include "IDataBlock.h"
#include "MdfBlock.h"

namespace mdf {

struct SDBlock : MdfBlock, IDataBlock {
  SDBlock();
};

} // namespace mdf

#endif // MDFSIMPLECONVERTERS_SDBLOCK_H
