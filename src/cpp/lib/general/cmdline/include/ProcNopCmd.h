#pragma once
namespace tcs{
class NopCmd;

// command processor for Nop command
struct ProcNopCmd{
  void operator()(NopCmd const&cmd)const;
};

}
