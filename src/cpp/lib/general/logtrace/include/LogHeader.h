#pragma once
#include <string>
#include <optional>
namespace tcs{

// singleton class for retrieving a log header
class LogHeader{
public:
  [[nodiscard]]static LogHeader&instance();
  [[nodiscard]]std::string const&getlogheader()const;
  void setlogheader(std::string const&setlogheader);
private:
  LogHeader()=default;
  std::optional<std::string>logheader_;
};
}
