#pragma once
#include <string>
#include <vector>
#include <optional>
namespace tcs{

// ---- (begin) high level cmds

// execute a command and capture stdout in a string
[[nodiscard]]std::string shellcmd(std::string const&cmd);

// parse child exit status retrieved from 'waitpid'
// (return nullopt of no error, else if error return error string)
[[nodiscard]]std::optional<std::string>parseChildExitStatus(int stat);

// ---- (end) high level cmds




// ---- (begin) low level cmds

// spawn child process setting up stdout and stdin as a pipe
void spawnCaptureStdinout(std::string const&file,std::vector<std::string>args,int&fdchild,int&cpid,bool diewhenparentdies);

// ---- (low) low level cmds
}
