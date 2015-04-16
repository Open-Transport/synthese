#include "ManagedThread.h"
#include "ThreadManager.h"
#include "ThreadException.h"

#include "01_util/Log.h"



namespace synthese
{

namespace util
{


ManagedThread::ManagedThread (ThreadExec* exec,
			      const std::string& name,
			      int loopDelay,
			      bool autoRespawn)
: Thread (exec, name, loopDelay)
  , _autoRespawn (autoRespawn)
{
    // Be careful, construction not finished, this is not polymorphic...
    ThreadManager::Instance ()->add (this);
}





ManagedThread::~ManagedThread ()
{
    ThreadManager::Instance ()->remove (this);
}






bool
ManagedThread::getAutoRespawn () const
{
    return _autoRespawn;
}



void
ManagedThread::respawn ()
{
    // If the thread died, it must be while its state was started or paused.
    // We do not respawn the thread otherwise (we could but it is considered
    // harmful right now to respawn a thread if its init failed).
    if (getState () <= INIT)
	throw ThreadException ("Thread was not started. Respawn forbidden.");

    Log::GetInstance ().warn ("Thread " + getName () +  " has died unexpectedly ! Respawning...");

    setState (NOT_STARTED);
    start ();
}













}

}
