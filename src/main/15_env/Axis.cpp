#include "Axis.h"

namespace synthese
{
namespace env
{

Axis::Axis (const uid& id,
	    const std::string& name,
	    bool free, bool authorized)
    : synthese::util::Registrable<uid,Axis> (id)
      , _name (name)
      , _free (free)
      , _authorized (authorized)
{

}


Axis::~Axis ()
{

}



const std::string& 
Axis::getName () const
{
    return _name;
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
