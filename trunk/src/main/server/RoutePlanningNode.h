/** En-tête classe Noeud de calcul d'itinéraires.
 @file RoutePlanningBound.h
*/

#ifndef SYNTHESE_ROUTEPLANNINGNODE_H
#define SYNTHESE_ROUTEPLANNINGNODE_H

#include <map>
#include "Temps.h"

class LogicalPlace;
class NetworkAccessPoint;
class cArretPhysique;


/** Noeud de calcul d'itinéraires.

	Cette classe permet de stocker un lieu sur lequel peut être lancée une occurence de la récursion, ou bien le lieu à atteindre.

	Le lieu est constitué par :
		- les points d'accès atteignables instantanément sur le lieu décrit
		- les points d'accès atteignables moyennant une marche à pied, dont la longueur est fournie

	Ce lieu utilisé comme amorce du calcul contient le parcours d'approche.
	Lorsque ce lieu est utilisé au milieu du calcul, il ne contient que les arrêts physiques immédiats.
*/
class RoutePlanningNode
{
public:

	/** Network access point / distance map */
	typedef map<const NetworkAccessPoint*, double> AccessPointsWithDistance;

private:

    //! @name Données
    //@{
		LogicalPlace* const        _logicalPlace; //!< Lieu logique à l'origine de l'objet
		AccessPointsWithDistance _accessPointsWithDistance; //!< Points d'entrée sur le calcul d'itinéraire
		tDureeEnMinutes			_approachDurationAmplitude;	//!< Difference between highest et lowest approch duration
    //@}

public:

	//! @name Accesseurs
	//@{
		const AccessPointsWithDistance& getAccessPoints() const { return _accessPointsWithDistance; }
		const tDureeEnMinutes& getApproachDurationAmplitude() const { return _approachDurationAmplitude; }
	//@}

    //! @name Calculateurs
    //@{
		bool includes(const NetworkAccessPoint*) const;
		const tDureeEnMinutes& leastApproachDuration() const;
    //@}

    //! @name Modificateurs
    //@{
		void addAccessPoint(const NetworkAccessPoint*, const tDureeEnMinutes&);
    //@}

    //! @name Constructeur et destructeur
    //@{
    RoutePlanningNode(LogicalPlace*, 
		      double maxApproachDistance,  
		      double approachSpeed);

    ~RoutePlanningNode();
    //@}
};

#endif
