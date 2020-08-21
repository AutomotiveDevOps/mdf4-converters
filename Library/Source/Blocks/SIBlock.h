#ifndef MDFSIMPLECONVERTERS_SIBLOCK_H
#define MDFSIMPLECONVERTERS_SIBLOCK_H

#include "MDBlock.h"
#include "MdfBlock.h"

namespace mdf {

enum struct SIBlockBusType : uint8_t { CAN = 0x02u, LIN = 0x03u };

SIBlockBusType operator&(SIBlockBusType const &lhs, SIBlockBusType const &rhs);

struct SIBlock : MdfBlock {
  [[nodiscard]] SIBlockBusType getBusType() const;
  [[nodiscard]] std::shared_ptr<MDBlock> getComment() const;

protected:
  bool load(std::shared_ptr<std::streambuf> stream) override;
  bool saveBlockData(uint8_t *dataPtr) override;

private:
  uint8_t busType;
  uint8_t flags;
  uint8_t type;
};

} // namespace mdf

#endif // MDFSIMPLECONVERTERS_SIBLOCK_H
