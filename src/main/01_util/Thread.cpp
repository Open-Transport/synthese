#include "Thread.h"

#include "ThreadException.h"
#include "01_util/Log.h"
#include "01_util/Conversion.h"

#include <boost/thread/xtime.hpp>


using synthese::util::Log;
using synthese::util::Conversion;


namespace synthese
{
namespace util
{


 const std::string Thread::DEFAULT_NAME_PREFIX = "thread_";
 int Thread::_NbThreads = 0;



Thread::Thread (ThreadExec* exec, const std::string& name, int loopDelay)
: _name ((name == "") ? DEFAULT_NAME_PREFIX + Conversion::ToString (_NbThreads++) : name)
, _exec (exec)
, _thread (0)
, _loopDelay (loopDelay)
, _state (new ThreadState (NOT_STARTED))
, _stateMutex (new boost::mutex ())
{
    // Implementation note : _state and _stateMutex are shared pointers because
    // when the thread will be spawned, this object will be first copied
    // and then the operator() will be called on the copy.
    // We want to keep in sync the state of the Thread object used 'internally'
    // by the boost thread, and the one from the originally user created Thread object
    // (which is the one likeky to be asked for its state).

    // Moreover, this way, it is feasible to share the same ThreadExec between different
    // Thread objects. Of course, using proper thread-local variables and mutexes in ThreadExec 
    // derived classes is left to user responsability.

}
 

Thread::~Thread ()
{
    delete _thread;
}



const std::string& 
Thread::getName () const
{
    return _name;
}



void 
Thread::start ()
{
    if (getState () != NOT_STARTED) throw ThreadException ("Thread was already started.");
    _thread = new boost::thread (*this);

    // Now that ThreadExec pointer has been copied, reset this shared_ptr on thread exec
    // so that this object does not prevent destruction of ThreadExec.
    _exec.reset ();
}




void 
Thread::stop ()
{
    if (getState () == STOPPED) return;
    setState (STOPPED);
}



void 
Thread::pause ()
{
    if (getState () == STOPPED) throw ThreadException ("Thread was stopped.");
    setState (PAUSED);
}


void 
Thread::resume ()
{
    if (getState () != PAUSED) throw ThreadException ("Thread is not paused.");
    setState (READY);
}


/*
Thread&
Thread::operator=(const Thread& ref)
{
    if (&ref == this) return (*this);
    _name = ref._name;
    _exec = ref._exec;
    _thread = 0; // ?
    _loopDelay = ref._loopDelay;
    _state = ref._state;
    _stateMutex = ref._stateMutex;

    return (*this);
}

*/


void 
Thread::operator()()
{
    try
    {
	setState (INIT);
	Log::GetInstance ().info ("Initializing thread " + _name +  "...");
	_exec->initialize ();
	setState (READY);
	Log::GetInstance ().info ("Thread " + _name +  " is ready.");
	
	ThreadState state = getState ();
	while (state != STOPPED) 
	{
	    if (state != PAUSED) _exec->loop ();
	    Sleep (_loopDelay);
	    state = getState ();
	}
	Log::GetInstance ().info ("Thread " + _name +  " is stopped.");
    }
    catch (std::exception& ex)
    {
	Log::GetInstance ().error ("Thread " + _name +  " has crashed.", ex);
    } 

    // Finalization is done in ANY case even after a crash
    Log::GetInstance ().info ("Finalizing thread " + _name + "...");
    _exec->finalize ();
    Log::GetInstance ().info ("Finalization done. " + _name + " is dead.");

}





/*
void 
Thread::Yield ()
{
    boost::thread::yield ();
}
*/


void 
Thread::Sleep (int ms)
{
    boost::xtime xt;
    boost::xtime_get(&xt, boost::TIME_UTC);

    xt.sec += ms / 1000; 
    xt.nsec += (ms % 1000000) * 1000000; 
    
    if (xt.nsec >= 1000000000)
    {
	xt.sec += 1;
	xt.nsec = xt.nsec % 1000000000;
    }

    boost::thread::sleep(xt);
} 




Thread::ThreadState 
Thread::getState () const
{
    boost::mutex::scoped_lock lock (*_stateMutex);
    return *_state;
}



void 
Thread::setState (ThreadState state)
{
    boost::mutex::scoped_lock lock (*_stateMutex);
    *_state = state;
}


void 
Thread::waitForReadyState () const
{
    waitForState (Thread::READY);
}



void 
Thread::waitForState (const Thread::ThreadState& state) const
{
    while (1)
    {
	if (getState () == state)
	{
	    break;
	}
	Thread::Sleep (5);
    }
}









}

}

