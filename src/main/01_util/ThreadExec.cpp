#include "ThreadExec.h"



namespace synthese
{
namespace util
{



ThreadExec::ThreadExec () 
{
    
}
    


ThreadExec::~ThreadExec () 
{
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






}
}
