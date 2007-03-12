#ifndef SYNTHESE_UTIL_THREADMANAGER_H
#define SYNTHESE_UTIL_THREADMANAGER_H



#include "module.h"

#include <vector>
#include <string>
#include <map>
#include <iostream>




namespace synthese
{
namespace util
{


    class ManagedThread;


/** Thread manager.
@todo add thread group management.

@ingroup m01
*/
class ThreadManager
{
 public:


 private:

    static bool _MonothreadEmulation;

    typedef struct {
	ManagedThread* thread;
	unsigned long nbLoopsPrev;
	unsigned long nbLoopsPrevPrev;
    } ManagedThreadEntry;

    std::vector<std::string> _names;  //!< Thread names ordered by registration.
    std::map<std::string, ManagedThreadEntry> _threads;  //!< Managed threads.
    

    ThreadManager (bool monothreadEmulation = true);

 protected:

 public:

    ~ThreadManager ();

    static ThreadManager* Instance ();

    void run ();

    static bool GetMonothreadEmulation ();
    static void SetMonothreadEmulation (bool monothreadEmulation);

 private:

    void checkForDeadThreads ();

    void add (ManagedThread* thread, bool autoRespawn = false);
    void remove (ManagedThread* thread);

    friend class ManagedThread;

    
};




}

}
#endif

