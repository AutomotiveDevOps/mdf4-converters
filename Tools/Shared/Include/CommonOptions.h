#ifndef TOOLS_SHARED_COMMONOPTIONS_H
#define TOOLS_SHARED_COMMONOPTIONS_H

namespace mdf::tools::shared {

enum struct DisplayTimeFormat : char {
  UTC = 'u',
  LoggerLocalTime = 'l',
  PCLocalTime = 'p',
};

struct CommonOptions {
  DisplayTimeFormat displayTimeFormat;
  bool nonInteractiveMode;
};

} // namespace mdf::tools::shared

#endif // TOOLS_SHARED_COMMONOPTIONS_H
