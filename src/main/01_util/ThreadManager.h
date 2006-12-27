#ifndef SYNTHESE_UTIL_THREADMANAGER_H
#define SYNTHESE_UTIL_THREADMANAGER_H



#include "module.h"

#include <string>
#include <vector>
#include <iostream>



#include "Thread.h"



namespace synthese
{
namespace util
{




/** Thread manager.
@todo add thread group management.

@ingroup m01
*/
class ThreadManager
{
 public:


 private:

    std::vector<ThreadSPtr> _threads;  //!< Threads in group.

    ThreadManager ();

 protected:

 public:

    ~ThreadManager ();

    static ThreadManager* Instance ();

    void addThread (const ThreadSPtr& thread);

 private:

    
};




}

}
#endif

