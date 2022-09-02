#include "general/sysutils/fileutils.h"
#include "general/logtrace/throw.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;
namespace tcs{

// set an fd to non blocking
void setfdnonblock(int fd){
  int flags=fcntl(fd,F_GETFL,0);
  if(fcntl(fd,F_SETFL,flags|O_NONBLOCK)<0){
    THROW_TEXCEPT("setFdNonblock: failed setting fd in non-blocking mode: "<<strerror(errno));
  }
}
}
