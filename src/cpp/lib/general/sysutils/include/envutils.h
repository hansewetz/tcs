#pragma once
#include <string>
#include <optional>
namespace tcs{

// get value of env variable - if error, return nullopt and set an error in 'err'
std::optional<std::string>getenvvar(std::string const&env,std::string&err);

// get value of env variable - if error, throw exception
std::string getenvvar(std::string const&env);
}
