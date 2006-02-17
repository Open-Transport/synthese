/*!	\file cPeriodeJournee.h
	\brief En-tête classe période de la journée
*/



#ifndef SYNTHESE_CPERIODEJOURNEE_H
#define SYNTHESE_CPERIODEJOURNEE_H

#include "Temps.h"
#include "cTexte.h"



/** Période de la journée
	@ingroup m04
	\author Hugues Romain
	\date 2005

Les objets périodes de la journée décrivent pour une interface une tranche horaire sur laquelle s'effectuera le calcul de fiche horaire.
*/
class cPeriodeJournee
{
	cHeure	_HeureDebut;	//!< Heure du début de la période
	cHeure	_HeureFin;	//!< Heure de fin de la période
	cTexte	_Libelle;		//!< Libellé de la période pour utilisation dans un formulaire par exemple
	
public:
	//! \name Modificateurs
	//@{
	bool SetValeurs(const cHeure& __HeureDebut, const cHeure& __HeureFin, const cTexte& __Libelle);
	//@}
	
	//! \name Accesseur
	//@{
	const cTexte& Libelle()		const;
	//@}
	
	//!	\name Calculateur
	//@{
	bool AppliquePeriode(cMoment& __DateDebut, cMoment& __DateFin, const cMoment& __MomentCalcul, bool __SolutionsPassees) const;
	//@}
	
	//! \name Constructeur
	//@{
	cPeriodeJournee() {}
	~cPeriodeJournee() {}
	//@}
};



/*!	\brief Modificateur complet de l'objet
	\param __HeureDebut Heure du début de la période de la journée
	\param __HeureFin Heure de fin de la période de la journée
	\param __Libelle Libellé de l'objet
	\return true si les heures de début et de fin sont valides, false sinon
	
La modification des valeurs ne s'effectue que si l'ensemble des données est valide (retour true)
*/
inline bool cPeriodeJournee::SetValeurs(const cHeure& __HeureDebut, const cHeure& __HeureFin, const cTexte& __Libelle)
{
	// Contrôle des entrées
	if (	!__HeureDebut.OK() || !__HeureFin.OK())
		return false;
		
	// Affectation des valeurs
	_HeureDebut = __HeureDebut;
	_HeureFin = __HeureFin;
	_Libelle = __Libelle;
	
	// Sortie OK
	return true;
}



/*!	\brief Applique la période de la journée aux dates fournies
	\param __MomentDebut L'objet date de début à modifier
	\param __MomentFin L'objet date de fin à modifier
	\param __MomentCalcul Moment du calcul
	\param __SolutionsPassees Filtre solutions passées (true = solutions passées affichées)
	\return true si la plage temporelle appliquée est valide (cas d'invalidité : période demandée antiérueure au moment présent et filtre solutions passées activé)
	\author Hugues Romain
	\date 2005
	
L'application de la période à deux dates effectue les actions suivantes :
 - Vérification de la validité de la période
 - Changement de l'heure à la date de début : début de période ou heure du calcul si filtre solutions passées
 - Si l'heure de fin est inférieure à l'heure de début, alors ajout d'un jour à la date de fin
 - Changement de l'heure à la date de fin
*/
inline bool cPeriodeJournee::AppliquePeriode(
	cMoment& __MomentDebut, cMoment& __MomentFin
	, const cMoment& __MomentCalcul, bool __SolutionsPassees
) const {

	// Modifications
	if (_HeureFin <= _HeureDebut)
		__MomentFin.addDureeEnJours(1);
	__MomentFin = _HeureFin;
	__MomentDebut = _HeureDebut;

	// Contrôle
	if (!__SolutionsPassees)
	{
		if (__MomentFin < __MomentCalcul)
			return false;
		if (__MomentDebut < __MomentCalcul)
			__MomentDebut = __MomentCalcul;
	}

	return true;
}
	


/*!	\brief Accesseur libellé
	\return Le libellé de l'objet
	\author Hugues Romain
	\date 2005
*/
inline const cTexte& cPeriodeJournee::Libelle() const
{
	return _Libelle;
}

#endif
