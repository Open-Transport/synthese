
#include "RoutePlanningNode.h"
#include "LogicalPlace.h"

#include <assert.h>


using namespace synmap;
using namespace std;


/** Constructeur.
    @param logicalPlace Lieu logique donnant lieu à la fabrication du noeud
    @param pedestrianApproach Prise en compte des arrêts physiques joignables à pied par le réseau de voirie
 
    Le constructeur remplit la liste des arrêts physiques du noeud
 
    @tod
*/
RoutePlanningNode::RoutePlanningNode(LogicalPlace* logicalPlace,
                                     double maxApproachDistance,   // in meters
                                     double approachSpeed)       // in meters/second
  : _logicalPlace(logicalPlace)
{

  if (distanceMax == 0)
    {
      // Just take physical stops contained in the logical place
      const PhysicalStopsMap& physicalStops = logicalPlace->getPhysicalStops ();
      for (PhysicalStopsMap::const_iterator iter = (physicalStops.begin ());
	   iter != physicalStops.end (); ++iter) {
	
	double approachDuration = 0.0; // TODO .
	_physicalStops.push_back (make_pair (*iter, approachDuration));

      }
    }
  else
    {
      const cEnvironnement& env = logicalPlace->getEnvironment ();
      const map<size_t, Address*>& adresses = getAddresses ();
      for (map<size_t, Address*>::const_iterator iter = adresses.begin ();
	   iter != adresses.end (); ++iter)
        {
	  std::set
	    < std::pair<double, const PhysicalStop*> > reachableStops =
	    iter->second->findDistancesToPhysicalStops (distanceMax);

	  for (std::set
		 < std::pair<double, const PhysicalStop*> >::iterator
		 distanceAndStop (reachableStops.begin ());
	       distanceAndStop != reachableStops.end ();
	       ++distanceAndStop)
	    {

	      double approachDistance = distanceAndStop->first;
	      const LogicalPlace* reachableLogicalPlace = 
		env.getLogicalPlace (distanceAndStop->second->getLogicalPlaceId ());
		    
	      cArretPhysique* physicalStop = *(reachableLogicalPlace->
					       getPhysicalStops().find (distanceAndStop->second->getRank ()));

	      double approachDuration = approachDistance / approachSpeed; // seconds
		    
	      _physicalStops.push_back (
					make_pair (physicalStop, 
						   cDureeEnMinutes (approachDuration / 60.0)));
		    
	    }

	    
	}
	
      // Prise en compte des lieux inclus
      for (size_t  i=0; i<_logicalPlace->getAliasedLogicalPlaces().size(); i++)
	{
	  // ?????? On se moque des alias non ? ils ont exactement
	  // les memes donnees que this.
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
      
    
