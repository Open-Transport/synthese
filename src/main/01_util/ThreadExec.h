#ifndef SYNTHESE_UTIL_THREADEXEC_H
#define SYNTHESE_UTIL_THREADEXEC_H


#include "module.h"

#include <string>
#include <iostream>


#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>


namespace synthese
{
namespace util
{




/** Abstract class defining thread execution in terms of initialization
    and thread atomic loop.

@ingroup m01
*/
class ThreadExec
{
 public:


 private:

 protected:

 public:

    ThreadExec ();
    ~ThreadExec ();

    virtual void initialize ();
    virtual void loop () = 0;
    virtual void finalize ();

    void loop (int nbLoops);

 private:
    
};




}

}
#endif
