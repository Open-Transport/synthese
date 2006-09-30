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
	@ingroup m15
	
    Temporary means it is not a part of the graph definition and can just
    be used as an entry point on this graph.
	
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

    VertexAccess getVertexAccess (const AccessDirection& accessDirection,
				  const AccessParameters& accessParameters,
				  const Vertex* destination,
				  const Vertex* origin = 0) const;
    
    void getImmediateVertices (VertexAccessMap& result, 
			       const AccessDirection& accessDirection,
			       const AccessParameters& accessParameters,
			       const Vertex* origin = 0,
			       bool returnAddresses = true,
			       bool returnPhysicalStops = true) const;

    //@}
    
};


}
}

#endif 	    
