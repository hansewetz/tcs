#pragma once
namespace tcs{
class DummyCmd;

// command processor for Dummy command
struct ProcDummyCmd{
  void operator()(DummyCmd const&cmd)const;
};

}

