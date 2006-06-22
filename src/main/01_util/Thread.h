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


/** Base class for thread creation and control.
@ingroup m01
*/
class Thread
{
 public:
    

 private:

    static const std::string DEFAULT_NAME_PREFIX;
    static int _NbThreads;
    
    const std::string _name;
    ThreadExecPtr _exec;
    boost::thread* _thread;
    const int _loopDelay;  //!< Loop delay in milliseconds

 protected:

 public:

    /** Constructs a Thread object given a ThreadExec pointer.
	Destruction of the ThreadExec will be handled automatically
	and must never be done manually.
     */
    Thread (ThreadExec* exec, const std::string& name = "", int loopDelay = 5);
    ~Thread ();

    const std::string& getName () const;

    void start ();
    void pause ();
    void resume ();
    void stop ();

    void operator()();

    static void Sleep (int ms);
//    static void Yield ();


 private:
    

};



}

}
#endif
