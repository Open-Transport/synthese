
#include "Address.h"

#include "ConnectionPlace.h"
#include "Edge.h"

namespace synthese
{
namespace env
{



Address::Address (const uid& id,
		  const AddressablePlace* place,
		  const Road* road, 
		  double metricOffset,
		  double x,
		  double y)
    : synthese::util::Registrable<uid,Address> (id)
    , Vertex (place, x, y)
    , _road (road)
    , _metricOffset (metricOffset)
{

}





Address::~Address()
{
}




const Road* 
Address::getRoad() const
{
    return _road;
}




double 
Address::getMetricOffset () const
{
    return _metricOffset;
}



bool 
Address::isAddress () const
{
    return true;
}



bool 
Address::isPhysicalStop () const
{
    return false;
}





const uid& 
Address::getId () const
{
    return synthese::util::Registrable<uid,Address>::getKey();
}





}
}


