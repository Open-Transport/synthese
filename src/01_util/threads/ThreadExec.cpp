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
ThreadExec::loops (int nbLoops)
{
    for (int i=0; i<nbLoops; ++i)
    {
	loop ();
    }
}






}
}

