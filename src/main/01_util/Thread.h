#ifndef SYNTHESE_UTIL_THREAD_H
#define SYNTHESE_UTIL_THREAD_H


#include "module.h"

#include <string>
#include <iostream>

#include <boost/thread/thread.hpp>
#include <boost/shared_ptr.hpp>

#include "ThreadExec.h"


namespace synthese
{
namespace util
{

 class ThreadExec;
 class ThreadManager;




/** Base class for thread creation and control.

    Implementation note : a copy of the ThreadExec object is kept for latter
    cloning. Mainly because it is too dangerous to deal with shared_ptr destruction
    and threading all together.

@ingroup m01
*/
class Thread
{
 public:
    

    typedef enum { NOT_STARTED = 0, INIT, READY, PAUSED, STOPPED } ThreadState;

 
    static const std::string DEFAULT_NAME_PREFIX;
    static int _NbThreads;
    
 protected:

    std::string _name;
    boost::shared_ptr<ThreadExec> _exec;

    int _loopDelay;  //!< Loop delay in milliseconds

 private:
    
    boost::shared_ptr<ThreadState> _state;
    boost::shared_ptr<unsigned long> _nbLoops; //!< Number of loops executed by this thread 
    boost::shared_ptr<boost::mutex> _stateMutex; 
    boost::shared_ptr<boost::mutex> _nbLoopsMutex; 
    boost::shared_ptr<boost::mutex> _execMutex; 


 protected:

 public:

    /** Constructs a Thread object given a ThreadExec pointer.
     */
    Thread (ThreadExec* exec, const std::string& name = "", int loopDelay = 5);

    virtual ~Thread ();

    const std::string& getName () const;


    void start ();
    void pause ();
    void resume ();
    void stop ();

    void operator()();

    //Thread& operator=(const Thread& ref);

    static void Sleep (int ms);
//    static void Yield ();

    ThreadState getState () const;

    void waitForState (const Thread::ThreadState& state) const;
    void waitForReadyState () const;

 protected:
    
    void setState (ThreadState state);

 private:

    unsigned long getNbLoops () const;

    void execInitialize ();
    void execLoop ();

    friend class ThreadManager;

};


 typedef boost::shared_ptr<Thread> ThreadSPtr;


}

}
#endif

