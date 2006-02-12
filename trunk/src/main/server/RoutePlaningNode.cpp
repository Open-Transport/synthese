
#include "RoutePlanningNode.h"
#include "LogicalPlace.h"

/** Constructeur.
	@param logicalPlace Lieu logique donnant lieu à la fabrication du noeud
	@param pedestrianApproach Prise en compte des arrêts physiques joignables à pied par le réseau de voirie

	Le constructeur remplit la liste des arrêts physiques du noeud

	@tod
*/
RoutePlanningNode::RoutePlanningNode(LogicalPlace* logicalPlace, bool pedestrianApproach)
: _logicalPlace(logicalPlace)
{
	// Prise en compte des arrêts physiques du lieu logique
	for (size_t i=0; i<_logicalPlace->getPysicalStops().size(); i++)
	{
	}

	// Prise en compte des lieux inclus
	for (size_t  i=0; i<_logicalPlace->getAliasedLogicalPlaces().size(); i++)
	{

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