#include "LineStop.h"

#include "SquareDistance.h"
#include "Line.h"
#include "Service.h"
#include "ContinuousService.h"
#include "PhysicalStop.h"



namespace synthese
{
namespace env
{


LineStop::LineStop (const uid& id,
		    const Line* line,
		    int rankInPath,
		    bool isDeparture,
		    bool isArrival,		
		    double metricOffset,
		    const PhysicalStop* physicalStop)
    : synthese::util::Registrable<uid,LineStop> (id)
    , Edge (isDeparture, isArrival, line, rankInPath)
    , _metricOffset (metricOffset)
    , _physicalStop (physicalStop)
{

}






LineStop::~LineStop()
{

}






const Vertex* 
LineStop::getFromVertex () const
{
    return _physicalStop;
}





double
LineStop::getMetricOffset () const
{
    return _metricOffset;
}



    








bool 
LineStop::seemsGeographicallyConsistent (const LineStop& other) const
{
    double deltaMO; // meters
    if ( getMetricOffset () > other.getMetricOffset () )
        deltaMO = ( getMetricOffset () - other.getMetricOffset () ) / 1000;
    else
        deltaMO = ( other.getMetricOffset () - getMetricOffset () ) / 1000;

    int deltaGPS = SquareDistance ( 
	*getFromVertex (), 
	*other.getFromVertex () ).getDistance(); // kilometers

    if ( deltaMO > 10 * deltaGPS && deltaMO - deltaGPS > 1 )
    {
        return false;
    }
    if ( deltaMO < deltaGPS && deltaGPS - deltaMO > 1 )
    {
        return false;
    }

    return true;
    
}






void 
LineStop::setMetricOffset (double metricOffset)
{
    _metricOffset = metricOffset;
}




}
}
