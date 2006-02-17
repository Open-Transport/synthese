/** En-t�te classe Noeud de calcul d'itin�raires.
 @file RoutePlanningBound.h
*/

#ifndef SYNTHESE_ROUTEPLANNINGNODE_H
#define SYNTHESE_ROUTEPLANNINGNODE_H

#include <map>
#include "Temps.h"

class LogicalPlace;
class NetworkAccessPoint;
class cArretPhysique;


/** Noeud de calcul d'itin�raires.

	Cette classe permet de stocker un lieu sur lequel peut �tre lanc�e une occurence de la r�cursion, ou bien le lieu � atteindre.

	Le lieu est constitu� par :
		- les points d'acc�s atteignables instantan�ment sur le lieu d�crit
		- les points d'acc�s atteignables moyennant une marche � pied, dont la longueur est fournie

	Ce lieu utilis� comme amorce du calcul contient le parcours d'approche.
	Lorsque ce lieu est utilis� au milieu du calcul, il ne contient que les arr�ts physiques imm�diats.
*/
class RoutePlanningNode
{
public:

	/** Network access point / distance map */
	typedef map<const NetworkAccessPoint*, double> AccessPointsWithDistance;

private:

    //! @name Donn�es
    //@{
		LogicalPlace* const        _logicalPlace; //!< Lieu logique � l'origine de l'objet
		AccessPointsWithDistance _accessPointsWithDistance; //!< Points d'entr�e sur le calcul d'itin�raire
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
