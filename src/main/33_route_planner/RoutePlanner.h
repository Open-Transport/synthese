#ifndef SYNTHESE_ROUTEPLANNER_ROUTEPLANNER_H
#define SYNTHESE_ROUTEPLANNER_ROUTEPLANNER_H


#include <deque>
#include <vector>
#include <map>

#include "04_time/DateTime.h"


#include "15_env/Place.h"
#include "15_env/VertexAccessMap.h"

#include "BestVertexReachesMap.h"
#include "JourneyLegComparator.h"




namespace synthese
{

namespace time
{
    class DateTime;
}


namespace env
{
    class Edge;
    class Path;
    class Vertex;
    class Service;
    class SquareDistance;
}



namespace routeplanner
{

    class Journey;
    class JourneyLeg;

    typedef std::vector<Journey> JourneyVector;
    typedef enum {DEPARTURE_FIRST, ARRIVAL_FIRST} PlanningOrder;



/** Route planner class.


@ingroup m33
*/
class RoutePlanner
{
 public:

 private:

    typedef const synthese::env::Edge* (synthese::env::Edge::*PtrEdgeStep) () const;

    typedef enum { USE_ROADS, DO_NOT_USE_ROADS } UseRoads;
    typedef enum { USE_LINES, DO_NOT_USE_LINES } UseLines;
    typedef enum { SEARCH_ADDRESSES, DO_NOT_SEARCH_ADDRESSES } SearchAddresses;
    typedef enum { SEARCH_PHYSICALSTOPS, DO_NOT_SEARCH_PHYSICALSTOPS } SearchPhysicalStops;


    const synthese::env::Place* _origin;  //<! Origin place for route planning.
    const synthese::env::Place* _destination;  //!< Destination place for route planning.

    synthese::env::VertexAccessMap _originVam;
    synthese::env::VertexAccessMap _destinationVam;

    const synthese::env::AccessParameters _accessParameters;
    
    const PlanningOrder _planningOrder;  //!< Define planning sequence.


    synthese::time::DateTime _minDepartureTime;  //!< Min departure time.
    synthese::time::DateTime _maxArrivalTime;  //!< Max arrival time.

    const synthese::time::DateTime _journeySheetStartTime;  //!< Start time of schedule sheet.
    const synthese::time::DateTime _journeySheetEndTime;    //!< End time of schedule sheet.

    const synthese::time::DateTime _calculationTime;    //!< Time of calculation (initialized to current time)

    int _previousContinuousServiceDuration;  //!< Journey duration in previously found continuous service.

    synthese::time::DateTime _previousContinuousServiceLastDeparture;  //!< End time of validity range of previously found continuous service.
    
    BestVertexReachesMap _bestDepartureVertexReachesMap;  //!< 
    BestVertexReachesMap _bestArrivalVertexReachesMap;  //!< 

    JourneyLegComparator _journeyLegComparatorForBestArrival;
    JourneyLegComparator _journeyLegComparatorForBestDeparture;
    
    RoutePlanner (const synthese::env::Place* origin,
		  const synthese::env::Place* destination,
		  const synthese::env::AccessParameters& accessParameters,
		  const PlanningOrder& planningOrder,
		  const synthese::time::DateTime& journeySheetStartTime,
		  const synthese::time::DateTime& journeySheetEndTime);
    

 public:


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
				  const Journey* currentJourney,
				  int maxDepth,
				  SearchAddresses searchAddresses, 
				  SearchPhysicalStops searchPhysicalStops,
				  UseRoads useRoads,
				  UseLines useLines,
				  bool strictTime = false);


    void findBestJourney (Journey& result,
			  const synthese::env::VertexAccessMap& vam, 
			  const synthese::env::AccessDirection& accessDirection,
			  const Journey& currentJourney,
			  bool strictTime, 
			  bool optim);
	
	
    void computeRoutePlanningDepartureArrival (Journey& result,
					       const synthese::env::VertexAccessMap& ovam,
					       const synthese::env::VertexAccessMap& dvam);

    JourneyVector computeJourneySheetDepartureArrival ();

 private:

    bool areAxisContraintsFulfilled (const synthese::env::Path* path, 
				     const Journey& journey) const;

    bool isPathCompliant (const synthese::env::Path* path) const;

    bool isServiceCompliant (const synthese::env::Service* service) const;

    bool isVertexUseful (const synthese::env::Vertex* vertex,
			 const synthese::time::DateTime& dateTime,
			 const synthese::env::AccessDirection& accessDirection,
			 synthese::env::SquareDistance& sqd) const;
    
    bool evaluateArrival (const synthese::env::Edge* arrivalEdge,
			  const synthese::time::DateTime& departureMoment,
			  const synthese::env::Edge* departureEdge,
			  const synthese::env::Service* service,
			  std::deque<JourneyLeg*>& journeyPart,
			  const Journey& currentJourney,
			  bool strictTime,
			  int continuousServiceRange);
	
    //@}



    //! @name Update methods
    //@{
    
    //@}



};





}
}
#endif

