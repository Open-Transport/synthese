/** En-tête classe Noeud de calcul d'itinéraires.
	@file RoutePlanningBound.h
*/

#ifndef SYNTHESE_ROUTEPLANNINGNODE_H
#define SYNTHESE_ROUTEPLANNINGNODE_H

#include <vector>
#include "temps.h"

class LogicalPlace;
class cArretPhysique;

/** Extrémité de calcul d'itinéraires.
*/
class RoutePlanningNode
{
private:

	//!	@name Données
	//@{
		LogicalPlace* const								_logicalPlace;	//!< Lieu logique à l'origine de l'objet
		vector<pair<cArretPhysique*, cDureeEnMinutes> >	_pysicalStops;	//!< Points d'entrée sur le calcul d'itinéraire
	//@}

public:

	//!	@name Calculateurs
	//@{
		bool	includes(const cArretPhysique*) const;
	//@}

	//!	@name Modificateurs
	//@{
		void	addPhysicalStop(cArretPhysique*);
	//@}

	//!	@name Constructeur et destructeur
	//@{
		RoutePlanningNode(LogicalPlace*, bool);
		~RoutePlanningNode();
	//@}
};

#endif