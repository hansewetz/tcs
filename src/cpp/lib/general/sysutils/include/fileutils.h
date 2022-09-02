#pragma once
#include <cstddef>
namespace tcs{

// max of file descriptors
constexpr static const std::size_t MAX_FD_OPEN=1024;

// set an fd to non blocking
// (throws exception if an error)
void setfdnonblock(int fd);
}
