#ifndef SYNTHESE_ENV_TEMPORARYPLACE_H
#define SYNTHESE_ENV_TEMPORARYPLACE_H


#include "Place.h"
#include <string>


namespace synthese
{
namespace env
{

    class Road;


/** Temporary place on the road network.

    Temporary means it is not a part of the graph definition and can just
    be used as an entry point on this graph.
    

 @ingroup m15
*/
class TemporaryPlace : public Place
{

private:

    const Road* _road;
    double _metricOffset;

protected:

public:

    TemporaryPlace (const Road* road, double metricOffset);

    virtual ~TemporaryPlace ();


    //! @name Getters/Setters
    //@{
    const Road* getRoad () const;
    double getMetricOffset () const;

    //@}

    //! @name Query methods
    //@{

    virtual void reachPhysicalStopAccesses (
	const AccessDirection& accessDirection,
	const AccessParameters& accessParameters,
	PhysicalStopAccessMap& result,
	const PhysicalStopAccess& currentAccess = PhysicalStopAccess ()) const;

    //@}
    
};


}
}

#endif 	    
