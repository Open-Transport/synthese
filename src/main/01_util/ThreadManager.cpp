#include "ThreadManager.h"



namespace synthese
{
namespace util
{



ThreadManager::ThreadManager () 
{
    
}
    


ThreadManager::~ThreadManager () 
{
}
    


void 
ThreadManager::addThread (const ThreadSPtr& thread)
{
    _threads.push_back (thread);
}




ThreadManager*
ThreadManager::Instance ()
{
    static ThreadManager* manager = new ThreadManager ();
    return manager;

}





}
}

