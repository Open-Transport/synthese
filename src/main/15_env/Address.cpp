#include "Address.h"

namespace synthese
{
namespace env
{



Address::Address (const ConnectionPlace* connectionPlace,
		  int rankInConnectionPlace,
		  const Road* road, 
		  double metricOffset)
    : Vertex (connectionPlace, rankInConnectionPlace)
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

