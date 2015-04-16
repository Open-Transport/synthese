#include "01_util/threads/ThreadAware.h"





namespace synthese
{
namespace util
{


    ThreadAware::ThreadAware ()
	: _ownerThread ()
    {
    }


    ThreadAware::~ThreadAware ()
    {
    }


    bool
    ThreadAware::insideOwnerThread () const
    {
	boost::thread current;
	return current == _ownerThread;
    }









}
}


