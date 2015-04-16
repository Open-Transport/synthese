#include "ThreadManager.h"
#include "ManagedThread.h"

#include "01_util/Log.h"




#include <iostream>

namespace synthese
{
namespace util
{


bool ThreadManager::_MonothreadEmulation (false);



ThreadManager::ThreadManager (bool monothreadEmulation)
{

}



ThreadManager::~ThreadManager ()
{
}



bool
ThreadManager::GetMonothreadEmulation ()
{
    return _MonothreadEmulation;
}



void
ThreadManager::SetMonothreadEmulation (bool monothreadEmulation)
{
    _MonothreadEmulation = monothreadEmulation;
}



void
ThreadManager::add (ManagedThread* thread, bool autoRespawn)
{
    ManagedThreadEntry mt;
    mt.thread = thread;
    mt.nbLoopsPrev = 0;
    mt.nbLoopsPrevPrev = 0;

    _threads.insert (std::make_pair (thread->getName (), mt));
    _names.push_back (thread->getName ());

    if (_MonothreadEmulation)
    {
	mt.thread->execInitialize ();
    }
    else
    {
	mt.thread->start ();
	mt.thread->waitForReadyState ();
    }

}



void
ThreadManager::remove (ManagedThread* thread)
{
    _threads.erase (thread->getName ());
}





void
ThreadManager::checkForDeadThreads ()
{
    for (std::map<std::string, ManagedThreadEntry>::iterator it = _threads.begin ();
	 it != _threads.end (); ++it)
    {
	ManagedThreadEntry& mt = it->second;
	if (mt.thread->getAutoRespawn () == false) continue;

	unsigned long nbLoops = mt.thread->getNbLoops ();

	// If it is two times (two times because of unsigned long ranging)
	// that nbLoops has not increased,
	// the thread is dead.
	if ((nbLoops != 0) &&
	    (mt.nbLoopsPrev == nbLoops) &&
	    (mt.nbLoopsPrevPrev == mt.nbLoopsPrev))
	{
	    mt.nbLoopsPrev = 0;
	    mt.nbLoopsPrevPrev = 0;

	    mt.thread->respawn ();
	}
	else
	{
	    mt.nbLoopsPrevPrev = mt.nbLoopsPrev;
	    mt.nbLoopsPrev = nbLoops;
	}
    }
}



ThreadManager*
ThreadManager::Instance ()
{
    static ThreadManager* manager = new ThreadManager ();
    return manager;

}




void
ThreadManager::run ()
{


    if (_MonothreadEmulation)
    {
	Log::GetInstance ().info ("Running ThreadManager monothreaded...");
	while (true)
	{
	    for (std::vector<std::string>::const_iterator it = _names.begin ();
		 it != _names.end (); ++it)
	    {
		std::map<std::string, ManagedThreadEntry>::iterator it2
		    = _threads.find (*it);

		if (it2 == _threads.end ()) continue;

		ManagedThreadEntry& mt = it2->second;
		mt.thread->execLoop ();
	    }
	}
    }
    else
    {
 	Log::GetInstance ().info ("Running ThreadManager multithreaded...");
	while (true)
	{
	    Thread::Sleep (500);
	    checkForDeadThreads ();
	}
    }


}




}
}

