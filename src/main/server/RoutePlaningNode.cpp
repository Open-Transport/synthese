
#include "RoutePlanningNode.h"
#include "LogicalPlace.h"
#include "cArretPhysique.h"
#include "map/Address.h"

#include <assert.h>


using namespace synmap;
using namespace std;


/** Constructeur.
    @param logicalPlace Lieu logique donnant lieu à la fabrication du noeud
    @param pedestrianApproach Prise en compte des arrêts physiques joignables à pied par le réseau de voirie
 
    Le constructeur remplit la liste des arrêts physiques du noeud
 
    @todo voir si on garde la distance ou la durée ou les deux
*/
RoutePlanningNode::RoutePlanningNode(LogicalPlace* logicalPlace,
                                     double maxApproachDistance,   // in meters
                                     double approachSpeed)       // in meters/second
  : _logicalPlace(logicalPlace)
{

  // take physical stops contained in the logical place
  const LogicalPlace::PhysicalStopsMap& physicalStops = logicalPlace->getPhysicalStops ();
  for (LogicalPlace::PhysicalStopsMap::const_iterator iter = (physicalStops.begin ());
       iter != physicalStops.end (); ++iter) {
    cArretPhysique* physicalStop = iter->second;
    double approachDuration = 0.0; // TODO .
    
	addAccessPoint(physicalStop, approachDuration);
    
   
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

	      double approachDuration = approachDistance / approachSpeed; // seconds

		  addAccessPoint((NetworkAccessPoint*) distanceAndAddress->second, approachDuration);
		    
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
	    std::pair<NetworkAccessPoint*, double> accessPointWithDistance
	      (( iter->second), approachDuration);

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
bool RoutePlanningNode::includes(const NetworkAccessPoint* accessPoint) const
{
	return _accessPointsWithDistance.find(accessPoint) != _accessPointsWithDistance.end();
}

/** Addition of network access point, with approach duration.
*/
void RoutePlanningNode::addAccessPoint(const NetworkAccessPoint* accessPoint, const tDureeEnMinutes& approachDuration)
{
	_accessPointsWithDistance[accessPoint] = approachDuration;
}