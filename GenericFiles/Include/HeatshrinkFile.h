#ifndef MDFSIMPLECONVERTERS_HEATSHRINKFILE_H
#define MDFSIMPLECONVERTERS_HEATSHRINKFILE_H

#include <string>

namespace mdf {
enum class IsCompressedFileStatus : int {

};

bool isCompressedFile(std::string const &fileName);
bool decompressFile(std::string const &inputFile,
                    std::string const &outputFile);
} // namespace mdf

#endif // MDFSIMPLECONVERTERS_HEATSHRINKFILE_H
