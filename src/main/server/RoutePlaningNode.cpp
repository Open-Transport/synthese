
#include "RoutePlanningNode.h"
#include "LogicalPlace.h"
#include "cArretPhysique.h"
#include "map/Address.h"

#include <assert.h>


using namespace synmap;
using namespace std;


/** Constructor from a network access point (transfer)

	@param accessPoint Base access point from which getting the reachable access points
	@param forDeparture true if the base access point is an arrival and the reachable access points will be used as departure, else false
	@param maxApproachDistance Maximal approach distance, for integration of junctions (not yet implemented)
	@param approachSpeed Speed of approach, for integration of junctions (not yet implemented)

	@todo Junctions integration
	
	The list of network access points is made up of the reachable physical stops inside the logical place of the current access point.
	
	If the transfer is forbidden in the logicalplace, then the node will be empty
*/
RoutePlanningNode::RoutePlanningNode(const NetworkAccessPoint* accessPoint, const bool forDeparture
									 , const DistanceInMeters& maxApproachDistance, const SpeedInKmh& approachSpeed)
: _logicalPlace(accessPoint->getLogicalPlace())
{
	if (accessPoint->getLogicalPlace()->CorrespondanceAutorisee() != LogicalPlace::CorrInterdite)
	{
		// List of physical stops reachable by simple transfer in the logical place
		const LogicalPlace::PhysicalStopsMap& physicalStops = accessPoint->getLogicalPlace()->getPhysicalStops();
		for (LogicalPlace::PhysicalStopsMap::const_iterator iter = physicalStops.begin();
			iter != physicalStops.end();
			++iter)
		{
			tDureeEnMinutes transferDuration = _logicalPlace->AttenteCorrespondance( forDeparture ? accessPoint->getRankInLogicalPlace() : iter->second->getRankInLogicalPlace()
				, forDeparture ? iter->second->getRankInLogicalPlace() : accessPoint->getRankInLogicalPlace() );
			if (transferDuration != LogicalPlace::FORBIDDEN_TRANSFER_DELAY)
				addAccessPoint(iter->second, transferDuration, 0);
		}

		// TODO junctions
	}
}


/** Constructor from a logical place (beginning or end).

    @param logicalPlace Base logical place frow which getting the reachable access points
	@param forDeparture true if the base logical place is an arrival and the reachable access points will be used as departure, else false
	@param maxApproachDistance Maximal approach distance
	@param approachSpeed Speed of approach
 
    The list of physical stops is made up of the reachable ones inside the logical place, and other ones around the logical place reachable using the road network
*/
RoutePlanningNode::RoutePlanningNode(const LogicalPlace* logicalPlace
									, const bool forDeparture,
                                     const DistanceInMeters& maxApproachDistance,   
                                     const SpeedInKmh& approachSpeed)       
  : _logicalPlace(logicalPlace)
{

  // take physical stops contained in the logical place, which are considered equivalents and without approach
  const LogicalPlace::PhysicalStopsMap& physicalStops = logicalPlace->getPhysicalStops ();
  for (LogicalPlace::PhysicalStopsMap::const_iterator iter = (physicalStops.begin ());
       iter != physicalStops.end (); ++iter)
  {
    cArretPhysique* physicalStop = iter->second;
    addAccessPoint(physicalStop, 0, 0);
  }

  // search of other logical places reached with road network
  if (maxApproachDistance > 0)
    {
      const map<size_t, Address*>& adresses = logicalPlace->getAddresses ();
      for (map<size_t, Address*>::const_iterator iter = adresses.begin ();
	   iter != adresses.end (); ++iter)
        {
	  std::set
	    < std::pair<double, const Address*> > reachableAddresses =
	    iter->second->findDistancesToAddresses (maxApproachDistance);

	  for (std::set
		 < std::pair<double, const Address*> >::iterator
		 distanceAndAddress (reachableAddresses.begin ());
	       distanceAndAddress != reachableAddresses.end ();
	       ++distanceAndAddress)
	    {

	      double approachDistance = distanceAndAddress->first;
//	      const LogicalPlace* reachableLogicalPlace = 
//		env.getLogicalPlace (distanceAndStop->second->getLogicalPlaceId ());
		    
//	      cArretPhysique* physicalStop = *(reachableLogicalPlace->
//					       getPhysicalStops().find (distanceAndStop->second->getRank ()));

		  tDureeEnMinutes approachDuration = approachDistance / approachSpeed; // seconds

//		  addAccessPoint(distanceAndAddress->second, approachDuration, approachDistance);
		    
	    }

	    
	}
	
      // Prise en compte des lieux inclus
      for (vector<LogicalPlace*>::iterator included = _logicalPlace->getAliasedLogicalPlaces().begin();
	   included != _logicalPlace->getAliasedLogicalPlaces().end();
	     ++included
	   )
	{
	  const LogicalPlace::PhysicalStopsMap& physicalStops = (*included)->getPhysicalStops ();
	  

	  // take physical stops contained in the logical place
	  
	  for (LogicalPlace::PhysicalStopsMap::const_iterator iter = ( physicalStops.begin ());
	       iter != physicalStops.end (); ++iter) {
	
	    double approachDuration = 0.0; // TODO .
	   // std::pair<NetworkAccessPoint*, double> accessPointWithDistance
	   //   (( iter->second), approachDuration);

	  }
	  
	}
    }
}
    
      
/** Destructeur.
	
Détruit le lieu logique s'il est volatil
*/
RoutePlanningNode::~RoutePlanningNode()
{
  if (_logicalPlace->getVolatile())
    delete _logicalPlace;
}
      
    
/** Test of appartenance of a network access point.
	@param accessPoint Network access point
*/
bool RoutePlanningNode::includes(const cArretPhysique* const accessPoint) const
{
	return _data.find(accessPoint) != _data.end();
}

/** Addition of network access point, with approach duration.
*/
void RoutePlanningNode::addAccessPoint(const cArretPhysique* const accessPoint, const tDureeEnMinutes& approachDuration, const DistanceInMeters& approachDistance)
{
	_data[accessPoint] = ApproachDescription(approachDuration, approachDistance);
}