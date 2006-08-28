#ifndef SYNTHESE_ROUTEPLANNER_ROUTEPLANNER_H
#define SYNTHESE_ROUTEPLANNER_ROUTEPLANNER_H


#include <vector>
#include <map>



#include "15_env/Place.h"




namespace synthese
{

namespace time
{
    class DateTime;
}


namespace env
{
    class Vertex;
}



namespace routeplanner
{

    class Journey;

    typedef std::vector<Journey*> JourneyVector;
    typedef enum {DEPARTURE_FIRST, ARRIVAL_FIRST} PlanningOrder;



/** Route planner class.


@ingroup m33
*/
class RoutePlanner
{
 public:

 private:

    typedef std::map<synthese::env::Vertex*, Journey*> BestJourneyMap;

    mutable BestJourneyMap _bestJourneys;  //!< Map storing best journey to access a vertex.
    
    const synthese::env::Place* _origin;  //<! Origin place for route planning.
    const synthese::env::Place* _destination;  //!< Destination place for route planning.

    const synthese::env::AccessParameters _accessParameters;
    
    const PlanningOrder _planningOrder;  //!< Define planning sequence.
    
    const synthese::time::DateTime& _journeySheetStartTime;  //!< Start time of schedule sheet.
    const synthese::time::DateTime& _journeySheetEndTime;    //!< End time of schedule sheet.


 public:


    RoutePlanner (const synthese::env::Place* origin,
		  const synthese::env::Place* destination,
		  const synthese::env::AccessParameters& accessParameters,
		  const PlanningOrder& planningOrder,
		  const synthese::time::DateTime& journeySheetStartTime,
		  const synthese::time::DateTime& journeySheetEndTime);
    
    ~RoutePlanner ();


    //! @name Getters/Setters
    //@{

    const synthese::env::Place* getOrigin () const;
    const synthese::env::Place* getDestination () const;

    //@}

    //! @name Query methods
    //@{


    /**
       @param vertices
       @param desiredTime Desired time.
       @param accessDirection
       @param currentJourney Journey currently being built.
       @param maxDepth Maximum recursion depth.
       @param accessDirection
       @param accessDirection
       @param searchAddresses Whether or not to search for addresses.
       @param searchPhysicalStops Whether or not to search for physicalStops.
       @param strictTime Must the departure time be strictly equal to desired time ?
     */
    JourneyVector integralSearch (const synthese::env::VertexAccessMap& vertices, 
				  const synthese::time::DateTime& desiredTime,
				  const synthese::env::AccessDirection& accessDirection,
				  Journey* currentJourney,
				  int maxDepth,
				  bool searchAddresses, 
				  bool searchPhysicalStops,
				  bool strictTime) const;


    Journey* findBestJourney (const synthese::env::VertexAccessMap& vertices, 
			      const synthese::time::DateTime& desiredTime,
			      const synthese::env::AccessDirection& accessDirection,
			      bool strictTime, 
			      bool optim) const;


    Journey* computeRoutePlanning (const synthese::time::DateTime& desiredTime) const;


    JourneyVector elaborateJourneySheet () const;


    //@}



    //! @name Update methods
    //@{
    
    //@}



};





}
}
#endif

