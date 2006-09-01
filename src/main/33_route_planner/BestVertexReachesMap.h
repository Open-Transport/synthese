#ifndef SYNTHESE_ROUTEPLANNER_BESTVERTEXREACHESMAP_H
#define SYNTHESE_ROUTEPLANNER_BESTVERTEXREACHESMAP_H


#include "module.h"

#include "04_time/DateTime.h"

#include "15_env/module.h"
#include "15_env/VertexAccessMap.h"


#include <map>


namespace synthese
{

namespace env
{
    class ConnectionPlace;
    class Vertex;
}


namespace routeplanner
{

    class JourneyLeg;


/** Best vertex reaches map class.


@ingroup m33
*/
class BestVertexReachesMap
{
 private:

    const synthese::env::AccessDirection _accessDirection;

    std::map<const synthese::env::Vertex*, const JourneyLeg*> _vertexMap;
    std::map<const synthese::env::ConnectionPlace*, synthese::time::DateTime> _connectionPlaceMap;


 public:


    BestVertexReachesMap (const synthese::env::AccessDirection& accessDirection);
    ~BestVertexReachesMap ();


    //! @name Getters/Setters
    //@{
    //@}


    //! @name Query methods
    //@{
    bool contains (const synthese::env::Vertex* vertex) const;

    const synthese::time::DateTime& 
	getBestTime (const synthese::env::Vertex* vertex, 
		     const synthese::time::DateTime& defaultValue) const;

    //@}


    //! @name Update methods
    //@{
    void insert (synthese::env::Vertex* vertex, const JourneyLeg* journeyLeg);

    void erase (synthese::env::Vertex* vertex);
    //@}


 private:


};





}
}
#endif

