#include <coroutine>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <thread>
#include <queue>
#include <set>
#include <mutex>
#include <atomic>
#include <iostream>
#include <unistd.h>
using namespace  std;

/*
  [] a Task object is returned from coroutine function maketask()
  [] the 'co_await schedule(s)' will register the coroutine with the scheduler
  [] when 'await_suspend() is called on the awaiter, the function sets a pointer in the promise object to point to the scheduler
  [] the scheduler is running multiple threads which will resume scheduled coroutines
  [] when final_suspend() is called on the promise_type, the function deregister the coroutine with the scheduler

  We could also have left the promise_type out of the scheduling business
  [] add a new awaiter: 'leavescheduler(Scheduler)'
  [] at end of coroutine function do: 'co_await leavescheduler(s)'
  [] await_suspend in the awaiter returns true so we won't suspend
  [] await_resume in the awaiter deregisters the coroutine handle from the scheduler

  We can keep both options so that a coroutine can deregister from scheduler at any time as opposed to only at the end
*/

// keep track of alive tasks we have
// NOTE!
static atomic<size_t>nalivecoroutines=0;

// some typedefs
using handle_t=coroutine_handle<>;
using queue_t=queue<handle_t>;

// forward decl.
struct Task;

// simple scheduler
// (runs until 'stop()' is called on scheduler object)
class Scheduler{
public:
  // ctor
  Scheduler(size_t nthreads):nthreads_(nthreads){
  }
  // stop scheduler
  void stop(){
    stop_=true;
  }
  // add coroutine to scheduling
  void schedule(handle_t h){
    unique_lock<mutex>lck(mtx_);
    q_.push(h);
  }
  // coroutine calls this when it is no longer part of scheduling
  // (we won't generate an error if we try to deregister a non existing handle)
  void leave(handle_t h){
    // NOTE! do we need this
    // unique_lock<mutex>lck(mtx_);
  }
  // run until 'stop' flag is set
  void operator()(){
    // create a pool of threads that runs the tasks
    vector<thread>threads;
    for(size_t i=0;i<nthreads_;++i){
      threads.push_back(move(thread(&Scheduler::thread_func,this)));
    }
    // join with threads
    for(size_t i=0;i<nthreads_;++i){
      threads[i].join();
    }
  }
private:
  // thread function
  void thread_func(){
    while(!stop_){
      optional<handle_t>handle;
      try{
        // get task to run (if any)
        unique_lock<mutex>lck(mtx_);
        if(!q_.empty()){
          handle=q_.front();
          q_.pop();
        }
        else{
          this_thread::yield();
        }
      }
      catch(...){
        stop_=true;
      }
      // if we found a task then go ahead and run it
      if(handle){
        // resume task on this thread from the thread pool
        handle.value().resume();
      }
    }
  }
  // private data
  size_t nthreads_;
  atomic<bool>stop_{false};    // scheduler stop flag
  queue_t q_;                  // queue of coroutine waiting to be scheduled
  mutable mutex mtx_;          // mutex protecting scheduling queue
};

// PROMISE TYPE:
template<typename TASK>
struct simple_promise_type{
  Scheduler*sp_;

  TASK get_return_object(){return TASK{};}
  void return_void(){}

  // do not automatically suspend initially
  // (we let coroutine task decide to register with scheduler)
  std::suspend_never initial_suspend(){return {};}

  // do not automatically suspend finally
  // (in final suspend we de-register from scheduler since there is nothing more to execute)
  std::suspend_never final_suspend()noexcept(true){
    sp_->leave(coroutine_handle<simple_promise_type>::from_promise(*this));
    return {};
  }
  void unhandled_exception(){}
};

// AWAITER: to register with scheduler
auto schedule(Scheduler&s){
  struct awaitable{
    Scheduler&s_;

    // we are never ready to resume immediately when we are about to suspend
    bool await_ready(){return false;}

    // schedule coroutine for execution right after suspension
    // (since this is implemented as a void function, we will always go ahead with suspension)
    void await_suspend(coroutine_handle<simple_promise_type<Task>>h){
      h.promise().sp_=&s_;
      s_.schedule(h);
    }
    // we have no return value for co_await call
    void await_resume(){
      // NOTE! implement return value as #of ns we have not executed for
      //       we need to track the time between the await_suspend() call and this point (now)
    }
  };
  return awaitable{s};
}
// AWAITER: to deregister from scheduler
// (not using it in the code)
auto leavescheduler(Scheduler&s){
  struct awaitable{
    handle_t handle_;    // set in await_suspend() is called
    Scheduler&s_;        // set in ctor

    bool await_ready(){return false;}
    bool await_suspend(handle_t h){handle_=h;return false;} // note: we return false so we don't suspend
    void await_resume(){s_.leave(handle_);}                 // before resuming we deregister with scheduler
  };
  return awaitable{handle_t{},s};   // return awaiter with null handle (we don't have the handle yet) and a reference to
}
// COROUTINE FUNCTION: return object (future
struct Task{
  using promise_type=simple_promise_type<Task>;
};

// test coroutine function
Task maketask(string id,Scheduler&s){
  ++nalivecoroutines;
  cout<<"(1) task: "<<id<<" first suspend ..."<<endl;
  co_await schedule(s);
  cout<<"(2) task: "<<id<<" second suspend ..."<<endl;
  sleep(1);
  co_await schedule(s);
  cout<<"(3) task: "<<id<<" co_return ..."<<endl;
  sleep(2);
  co_await leavescheduler(s); // not needed since 'promise.final_suspend()' will deregister with the scheduler
  --nalivecoroutines;
}
// main test program 
int main(){
  // some constants
  size_t ntasks{10};
  size_t nthreads{3};

  // scheduler for coroutine objects
  Scheduler schd(nthreads);

  // create coroutine tasks - each task will register coroutine with scheduler
  for(int i=0;i<ntasks;++i){
    maketask("t"s+to_string(i),schd);
  }
  // all Task objects are destroyed when reaching this point
  // however, coroutine states still exist and can be resumed

  // resume tasks while we still have tasks that can be resumed
 std::jthread jt([&schd]{schd();});

  // wait until we have no more tasks in scheduler
  while(nalivecoroutines){
    cout<<">>>main<<< alive coroutine count: "<<nalivecoroutines<<endl;
    sleep(1);
  }
  cout<<">>>main<<< stopping scheduler ..."<<endl;
  schd.stop();
  cout<<">>>main<<< stopped scheduler"<<endl;
}
