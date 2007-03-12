#ifndef SYNTHESE_UTIL_MANAGEDTHREAD_H
#define SYNTHESE_UTIL_MANAGEDTHREAD_H

#include "module.h"

#include "Thread.h"


namespace synthese
{
namespace util
{




/**
@ingroup m01
*/
class ManagedThread : public Thread
{
 public:
    

 private:

    const bool _autoRespawn;

 protected:

 public:


    ManagedThread (ThreadExec* exec, 
		   const std::string& name = "", 
		   int loopDelay = 5, 
		   bool autoRespawn = false);

    ~ManagedThread ();

    bool getAutoRespawn () const;

    void respawn ();

 private:
    

};



}

}
#endif

