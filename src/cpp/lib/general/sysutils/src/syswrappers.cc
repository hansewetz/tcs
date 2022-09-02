#include "general/sysutils/envutils.h"
#include "general/logtrace/throw.h"
#include <string.h>
#include <stdlib.h>
using namespace std;
namespace tcs{

// close an fd
void eclose(int fd){
  int stat;
  while((stat=close(fd))<0&&errno==EINTR);
  if(stat<0)THROW_TEXCEPT("eclose: close failed on fd: "<<fd<<": "<<strerror(errno));
}
// write to fd
ssize_t ewrite(int fd,void const*buf,size_t count){
  int wstat;
  while((wstat=::write(fd,buf,count))<0&&errno==EINTR);
  if(wstat<0)THROW_TEXCEPT("error writing in ewrite(): "<<strerror(errno)<<", errno: "<<errno);
  return wstat;
}
// read from fd
size_t eread(int fd,void*buf,size_t count){
  int rstat;
  while((rstat=::read(fd,buf,count))<0&&errno==EINTR);
  if(rstat<0)THROW_TEXCEPT("error reading in eread(): "<<strerror(errno));
  return rstat;
}
// dup an fd
int edup(int fd){
  int dstat;
  while((dstat=dup(fd))<0&&errno==EINTR);
  if(dstat<0)THROW_TEXCEPT("error reading in edup(): "<<strerror(errno));
  return dstat;
}
}
