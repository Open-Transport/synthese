
#include "Address.h"

#include "ConnectionPlace.h"
#include "Edge.h"

namespace synthese
{
namespace env
{



Address::Address (const uid& id,
		  const ConnectionPlace* connectionPlace,
		  int rankInConnectionPlace,
		  const Road* road, 
		  double metricOffset,
		  double x,
		  double y)
    : synthese::util::Registrable<uid,Address> (id)
    , Vertex (connectionPlace, rankInConnectionPlace, x, y)
    , _road (road)
    , _metricOffset (metricOffset)
{

}


Address::Address (const uid& id,
		  const Road* road, 
		  double metricOffset,
		  double x,
		  double y)
    : synthese::util::Registrable<uid,Address> (id)
    , Vertex (0, -1, x, y)
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








}
}

