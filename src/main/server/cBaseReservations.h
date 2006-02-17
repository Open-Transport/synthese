
#ifndef SYNTHESE_CBASERESERVATIONS_H
#define SYNTHESE_CBASERESERVATIONS_H

class cBaseReservation;

#include "cReservation.h"
#include "cServeurBaseDeDonnees.h"

/** Base de réservations pour transport à la demande
	@ingroup m37
*/
class cBaseReservation
{
	//!	\name Paramètres
	//@{
	cServeurBaseDeDonnees*	_Serveur;			//!< Serveur de base de données à utiliser
	cTexte					_TableReservations;	//!< Nom de la table des réservations
	cTexte					_TableCompteur;		//!< Nom de la table des compteurs de réservations
	cTexte					_NomChampID;		//!< Nom du champ d'identification de la réservation
	//@}
	
public:
	//!	name Calculateurs
	//@{
	bool							EnregistrerReservation(cReservation&)			const;
	bool							LireReservation(cReservation&)					const;
	cTableauDynamique<cReservation>	LireReservations(const cTrain*, const cMoment&) const;
	//@}
};

#endif
