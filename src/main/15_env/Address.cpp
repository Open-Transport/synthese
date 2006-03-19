#include "Address.h"

namespace synthese
{
namespace env
{



Address::Address (int rank,
		  const std::string& name,
		  const LogicalPlace* logicalPlace,
		  const Road* road, 
		  double metricOffset)
    : Gateway (rank, name, logicalPlace)
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

