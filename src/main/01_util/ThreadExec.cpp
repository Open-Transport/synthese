#include "ThreadExec.h"



namespace synthese
{
namespace util
{



ThreadExec::ThreadExec () 
: _state (NOT_STARTED)
, _stateMutex (new boost::mutex ())
{
    
}
    


ThreadExec::~ThreadExec () 
{
    delete _stateMutex;
}
    



void 
ThreadExec::initialize ()
{
}


void 
ThreadExec::finalize ()
{
}


 
void 
ThreadExec::loop (int nbLoops)
{
    for (int i=0; i<nbLoops; ++i)
    {
	loop ();
    }
}



ThreadExec::ThreadState 
ThreadExec::getState () const
{
    boost::mutex::scoped_lock lock (*_stateMutex);
    return _state;
}


void 
ThreadExec::setState (ThreadState state)
{
    boost::mutex::scoped_lock lock (*_stateMutex);
    _state = state;
}





}
}
