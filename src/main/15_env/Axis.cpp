#include "Axis.h"

namespace synthese
{
namespace env
{

Axis::Axis (const std::string& id,
	    bool free, bool authorized)
    : synthese::util::Registrable<std::string,Axis> (id)
    , _free (free)
    , _authorized (authorized)
{

}


Axis::~Axis ()
{

}



bool 
Axis::isFree () const
{
    return _free;
}


bool 
Axis::isAuthorized () const
{
    return _authorized;
}


}
}
