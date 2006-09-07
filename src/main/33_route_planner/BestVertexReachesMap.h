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

    typedef std::map<const synthese::env::Vertex*, JourneyLeg*> JourneyLegMap;
    typedef std::map<const synthese::env::Vertex*, synthese::time::DateTime> TimeMap;
    
    const synthese::env::AccessDirection _accessDirection;

    JourneyLegMap _bestJourneyLegMap;
    TimeMap _bestTimeMap;

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

    JourneyLeg* getBestJourneyLeg (const synthese::env::Vertex* vertex);

    //@}


    //! @name Update methods
    //@{
    void clear ();
    void insert (const synthese::env::Vertex* vertex, 
		 JourneyLeg* journeyLeg);
    
    void insert (const synthese::env::Vertex* vertex, 
		 const synthese::time::DateTime& dateTime,
		 bool propagateInConnectionPlace = true);

    //@}


 private:


};





}
}
#endif

