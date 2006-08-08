#include "PassThroughDescription.h"

#include "15_env/LineStop.h"
#include "15_env/Path.h"

#include <assert.h>


using synthese::env::ConnectionPlace;
using synthese::env::LineStop;

namespace synthese
{
namespace departurestable
{


PassThroughDescription::PassThroughDescription (const synthese::env::LineStop* lineStop,
						const synthese::time::DateTime& theoreticalMoment,
						int serviceNumber)
    : _lineStop (lineStop)
    , _theoreticalMoment (theoreticalMoment)
    , _realMoment (theoreticalMoment)
    , _serviceNumber (serviceNumber)
{
    
}
    


const std::vector<synthese::env::ConnectionPlace*>& 
PassThroughDescription::getDisplayedConnectionPlaces () const
{
    return _displayedConnectionPlaces;
}


const synthese::env::LineStop* 
PassThroughDescription::getLineStop () const
{
    return _lineStop;
}


const synthese::time::DateTime& 
PassThroughDescription::getTheoreticalMoment () const
{
    return _theoreticalMoment;
}



const synthese::time::DateTime& 
PassThroughDescription::getRealMoment () const
{
    return _realMoment;
}


int 
PassThroughDescription::getServiceNumber () const
{
    return _serviceNumber;
}



const synthese::env::ConnectionPlace* 
PassThroughDescription::getDisplayedTerminus () const
{
    return _displayedConnectionPlaces.back ();
}



synthese::time::DateTime
PassThroughDescription::getTerminusArrivalMoment () const
{
    synthese::time::DateTime terminusArrivalMoment;
    const LineStop* terminus = dynamic_cast<const LineStop*> (_lineStop->getParentPath ()->getEdges ().back ());
    assert (terminus != 0);
    terminus->calculateArrival (*_lineStop, _serviceNumber, _realMoment, terminusArrivalMoment);
    return terminusArrivalMoment;
}




}
}
