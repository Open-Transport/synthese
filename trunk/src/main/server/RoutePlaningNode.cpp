
#include "RoutePlanningNode.h"
#include "LogicalPlace.h"

/** Constructeur.
	@param logicalPlace Lieu logique donnant lieu � la fabrication du noeud
	@param pedestrianApproach Prise en compte des arr�ts physiques joignables � pied par le r�seau de voirie

	Le constructeur remplit la liste des arr�ts physiques du noeud

	@tod
*/
RoutePlanningNode::RoutePlanningNode(LogicalPlace* logicalPlace, bool pedestrianApproach)
: _logicalPlace(logicalPlace)
{
	// Prise en compte des arr�ts physiques du lieu logique
	for (size_t i=0; i<_logicalPlace->getPysicalStops().size(); i++)
	{
	}

	// Prise en compte des lieux inclus
	for (size_t  i=0; i<_logicalPlace->getAliasedLogicalPlaces().size(); i++)
	{

	}
}

/** Destructeur.
	
	D�truit le lieu logique s'il est volatil
*/
RoutePlanningNode::~RoutePlanningNode()
{
	if (_logicalPlace->getVolatile())
		delete _logicalPlace;
}