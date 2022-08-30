#include <string>
#include <optional>
using namespace std;
namespace tcs{

// get value of env variable - if error, return nullopt and set an error in 'err'
std::optional<string>getenvvar(std::string const&env,std::string&err);

// get value of env variable - if error, throw exception
string getenvvar(std::string const&env);
}
