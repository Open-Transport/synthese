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



Thread::Thread (ThreadExec& exec, const std::string& name, int loopDelay)
: _name ((name == "") ? DEFAULT_NAME_PREFIX + Conversion::ToString (_NbThreads++) : name)
, _exec (exec)
, _thread (0)
, _loopDelay (loopDelay)
{
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
    if (_exec.getState () != ThreadExec::NOT_STARTED) throw ThreadException ("Thread was already started.");
    _thread = new boost::thread (*this);
}




void 
Thread::stop ()
{
    if (_exec.getState () == ThreadExec::STOPPED) return;
    _exec.setState (ThreadExec::STOPPED);
}



void 
Thread::pause ()
{
    if (_exec.getState () == ThreadExec::STOPPED) throw ThreadException ("Thread was stopped.");
    _exec.setState (ThreadExec::PAUSED);
}


void 
Thread::resume ()
{
    if (_exec.getState () != ThreadExec::PAUSED) throw ThreadException ("Thread is not paused.");
    _exec.setState (ThreadExec::READY);
}






void 
Thread::operator()()
{
    try
    {
	_exec.setState (ThreadExec::INIT);
	Log::GetInstance ().info ("Initializing thread " + _name +  "...");
	_exec.initialize ();
	_exec.setState (ThreadExec::READY);
	Log::GetInstance ().info ("Thread " + _name +  " is ready.");
	
	while (_exec.getState () != ThreadExec::STOPPED) 
	{
	    if (_exec.getState () != ThreadExec::PAUSED) _exec.loop ();
	    Sleep (_loopDelay);
	}
	Log::GetInstance ().info ("Finalizing thread " + _name + "...");
	_exec.finalize ();
	Log::GetInstance ().info ("Thread " + _name +  " is stopped.");
    }
    catch (std::exception& ex)
    {
	Log::GetInstance ().error ("Thread " + _name +  " has crashed.", ex);
    } 

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













}

}
