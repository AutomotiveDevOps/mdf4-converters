#include "MDFTool.h"

#include "MdfFile.h"
#include "ProjectInformation.h"

using namespace mdf;

namespace mdf::tools::finalizer {

MDFTool::MDFTool() : ConverterInterface("mdftool") { overwrite = false; }

void MDFTool::configureParser(
    boost::program_options::options_description &opts) {
  opts.add_options()
      // Add option for overwriting.
      ("overwrite", boost::program_options::bool_switch()->default_value(false),
       "replace the input data with the output")

      // Add option for pattern to append.
      ("name,n",
       boost::program_options::value<std::string>()->default_value("_fin"),
       "name to append on processed data");
}

bool MDFTool::convert(boost::filesystem::path inputFilePath,
                      boost::filesystem::path outputFolder) {
  bool status = false;

  // Generate the new filename.
  auto fileNameWithoutExtension = outputFolder / inputFilePath.stem();
  fileNameWithoutExtension += appendName;
  auto fileNameWithExtension =
      fileNameWithoutExtension.replace_extension(inputFilePath.extension());

  // Attempt to open the input file.
  std::unique_ptr<MdfFile> mdfFile = MdfFile::Create(inputFilePath.string());

  if (mdfFile) {
    // Save as a new file.
    mdfFile->save(fileNameWithExtension.string());
  }

  // If overwrite is enabled, delete the old file and rename the saved file into
  // place.
  if (overwrite) {
    std::cout << "Overwriting" << std::endl;
    boost::filesystem::rename(fileNameWithExtension, inputFilePath);
  }

  status = true;

  return status;
}

Version MDFTool::getVersion() const { return mdf::tools::finalizer::version; }

tools::shared::ParseOptionStatus
MDFTool::parseOptions(boost::program_options::variables_map const &result) {
  appendName = result["name"].as<std::string>();

  return tools::shared::ParseOptionStatus::NoError;
}

} // namespace mdf::tools::finalizer
