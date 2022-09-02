#pragma once
#include <unistd.h>
namespace tcs{

// wrappers around various system calls
// (throws exception if failure)
void eclose(int fd);
[[nodiscard]]ssize_t ewrite(int fd,void const*buf,size_t count);
[[nodiscard]]size_t eread(int fd,void*buf,size_t count);
[[nodiscard]]int edup(int fd); 
}
