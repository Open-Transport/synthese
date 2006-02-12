/** En-t�te classe Noeud de calcul d'itin�raires.
	@file RoutePlanningBound.h
*/

#ifndef SYNTHESE_ROUTEPLANNINGNODE_H
#define SYNTHESE_ROUTEPLANNINGNODE_H

#include <vector>
#include "temps.h"

class LogicalPlace;
class cArretPhysique;

/** Extr�mit� de calcul d'itin�raires.
*/
class RoutePlanningNode
{
private:

	//!	@name Donn�es
	//@{
		LogicalPlace* const								_logicalPlace;	//!< Lieu logique � l'origine de l'objet
		vector<pair<cArretPhysique*, cDureeEnMinutes> >	_pysicalStops;	//!< Points d'entr�e sur le calcul d'itin�raire
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