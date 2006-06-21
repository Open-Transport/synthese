#ifndef SYNTHESE_UTIL_THREADEXEC_H
#define SYNTHESE_UTIL_THREADEXEC_H


#include "module.h"

#include <string>
#include <iostream>


#include <boost/thread/mutex.hpp>


namespace synthese
{
namespace util
{

/** Abstract class defining thread execution in terms of initialization
    and thread atomic loop.

@ingroup m01
*/
class ThreadExec
{
 public:

    typedef enum { NOT_STARTED, INIT, READY, PAUSED, STOPPED } ThreadState;

 private:

    ThreadState _state;
    mutable boost::mutex* _stateMutex; 

 protected:

    ThreadExec ();
    ~ThreadExec ();

 public:

    virtual void initialize ();
    virtual void loop () = 0;
    virtual void finalize ();

    void loop (int nbLoops);

    ThreadState getState () const;
    void setState (ThreadState state);

 private:
    
};



}

}
#endif
