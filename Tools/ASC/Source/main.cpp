#include "ASC_Exporter.h"
#include "ExecutableEntryPoint.h"

using namespace mdf::tools::asc;

int main(int argc, char *argv[]) {
  std::unique_ptr<ASC_Exporter> exporter(new ASC_Exporter());
  return programEntry(argc, argv, std::move(exporter));
}
