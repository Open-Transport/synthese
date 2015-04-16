#include "ThreadGroup.h"



namespace synthese
{
namespace util
{



ThreadGroup::ThreadGroup ()
{

}



ThreadGroup::~ThreadGroup ()
{
}



void
ThreadGroup::addThread (const ThreadSPtr& thread)
{
    _threads.push_back (thread);
}





void
ThreadGroup::waitForAllReady () const
{
    waitAllInState (Thread::READY);
}



void
ThreadGroup::waitForAllStopped () const
{
    waitAllInState (Thread::STOPPED);
}



void
ThreadGroup::waitAllInState (const Thread::ThreadState& state) const
{
    while (1)
    {
	bool allInRightState = true;
	for (std::vector<ThreadSPtr>::const_iterator it = _threads.begin ();
	     it != _threads.end (); ++it)
	{
	    if ((*it)->getState () != state)
	    {
		allInRightState = false;
		break;
	    }
	}
	if (allInRightState) break;
	Thread::Sleep (5);
    }
}







}
}

