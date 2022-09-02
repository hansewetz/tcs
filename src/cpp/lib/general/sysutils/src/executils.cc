#include "general/sysutils/executils.h"
#include "general/sysutils/syswrappers.h"
#include "general/sysutils/fileutils.h"
#include "general/logtrace/throw.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <sys/socket.h>

using namespace std;
namespace tcs{

// execute a command and capture stdout in a string
// (if failure, set error in 'err' and return nullopt)
string shellcmd(string const&cmd){
  // NOTE! have not gone through this yet ...
  // ...

  // some calls may throw an exception
  // (make sure we catch exception and set an error string and return nullopt)
//  try{
  int fdchild;
  int cpid;
  string file="/usr/bin/bash";
  vector<string>args={"bash","-c"};
  args.push_back(cmd);
  spawnCaptureStdinout(file,args,fdchild,cpid,true);

  // read data from pipe ...
  stringstream str;
  char c;
  while(eread(fdchild,&c,1)==1)str<<c;    // NOTE! should handle error here
  eclose(fdchild);

  // wait for child and get exit status code
  int stat;
  if(waitpid(cpid,&stat,0)!=cpid)THROW_TEXCEPT("failed waiting for child process, err: "<<strerror(errno));

  // get exit status
  auto exitstaterr=parseChildExitStatus(stat);
  if(exitstaterr)THROW_TEXCEPT(exitstaterr.value());

  // no errors
  string ret=str.str();
  if(ret.length()&&ret[ret.length()-1]=='\n')ret=ret.substr(0,ret.length()-1);
  return ret;
}
// spawn child process setting up stdout and stdin as a pipe
void spawnCaptureStdinout(string const&file,vector<string>args,int&fdchild,int&cpid,bool diewhenparentdies){

// NOTE! have not gone through this yet ...
// ...

// NOTE! make sure we either have an exxx(...) or check all system call return values
// ...

  // create pipe between child and parent
  int fds[2];   // fds[0]: parent, fds[1]: child
  int stat=socketpair(AF_LOCAL,SOCK_STREAM,0,fds);
  if(stat<0)THROW_TEXCEPT("failed creating socketpair pipe: "s+strerror(errno));

  // fork child process
  int pid=fork();
  if(pid==0){ // child
    // die if parent dies so we won't become a zombie
// NOTE! should be able to select signal ... (?)
    if(diewhenparentdies&&prctl(PR_SET_PDEATHSIG,SIGHUP)<0){
      THROW_TEXCEPT("failed call to prctl(...): "<<strerror(errno));
    }
    // dup stdin/stdout ---> pipe
    eclose(0);        // close stdin/stdout
    eclose(1);        // ...
    (void)edup(fds[1]);      // dup pipe --> stdin/stdout
    (void)edup(fds[1]);      // ...

    // close fds we no longer need
    eclose(fds[0]);   // close pipe fds
    eclose(fds[1]);   // ...

    // setup for calling execv 
    // (we don't care if we leak memory since we'll overlay process using exec)
    char*const*tmpargs=(char*const*)malloc((args.size()+1)*sizeof(char*const*));
    if(tmpargs==0)THROW_TEXCEPT("failed call to malloc(...): "<<strerror(errno));

    char**ptmpargs=const_cast<char**>(tmpargs);
    for(std::size_t i=0;i<args.size();++i)ptmpargs[i]=const_cast<char*>(args[i].c_str());
    ptmpargs[args.size()]=0;

    // execute child process
    if(execvp(file.c_str(),tmpargs)<0){
      THROW_TEXCEPT("failed executing execl: "<<strerror(errno));
    }else{
      // should never get here
      THROW_TEXCEPT("failed executing execl: "<<strerror(errno));
    }
  }else
  if(pid>0){ // parent
    eclose(fds[1]);                  // close child side fd
    fdchild=fds[0];                  // set return parameters and return child pid
    cpid=pid;                        // set child pid for caller (caller might want to wit for child)
  }else{
    // fork failed
    THROW_TEXCEPT("failed fork: "<<strerror(errno));
  }
}
// parse child exit status retrieved from 'waitpid'
optional<string>parseChildExitStatus(int stat){
  // check if child terminated normally
  if(WIFEXITED(stat)){
    int exitcode=WEXITSTATUS(stat);
    if(exitcode==0){
      return nullopt;
    }else{
      return string("exit status: ")+to_string(exitcode);
    }
  }else
  if(WIFSIGNALED(stat)){
    if(WCOREDUMP(stat)){
      int signal=WTERMSIG(stat);
      return string("child coredumped from signal: ")+to_string(signal);
    }else{
      int signal=WTERMSIG(stat);
      return string("child terminated from signal: ")+to_string(signal);
    }
  }else{
    return "child terminated for unknown reason";
  }
}
}
