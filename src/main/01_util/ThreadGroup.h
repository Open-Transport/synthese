#ifndef SYNTHESE_UTIL_THREADGROUP_H
#define SYNTHESE_UTIL_THREADGROUP_H



#include "module.h"

#include <string>
#include <vector>
#include <iostream>



#include "Thread.h"



namespace synthese
{
namespace util
{




/** Group of threads.

@ingroup m01
*/
class ThreadGroup
{
 public:


 private:

    std::vector<ThreadSPtr> _threads;  //!< Threads in group.

 protected:

 public:

    ThreadGroup ();
    ~ThreadGroup ();

    void addThread (const ThreadSPtr& thread);

    void waitForAllReady () const;
    void waitForAllStopped () const;


 private:

    void waitAllInState (const Thread::ThreadState& state) const;
    
};




}

}
#endif

