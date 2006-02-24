/*!	\file cPeriodeJournee.h
	\brief En-tête classe période de la journée
*/



#ifndef SYNTHESE_CPERIODEJOURNEE_H
#define SYNTHESE_CPERIODEJOURNEE_H

#include "cMoment.h"
#include "cHeure.h"
#include <string>


/** Période de la journée
	@ingroup m04
	\author Hugues Romain
	\date 2005

Les objets périodes de la journée décrivent pour une interface une tranche horaire sur laquelle s'effectuera le calcul de fiche horaire.
*/
class cPeriodeJournee
{
	const cHeure	_HeureDebut;	//!< Heure du début de la période
	const cHeure	_HeureFin;	//!< Heure de fin de la période
	const std::string	_Libelle;		//!< Libellé de la période pour utilisation dans un formulaire par exemple
	size_t	_id;	//!< ID de la période dans l'interface

public:
	//!	@name Modificateurs
	//@{
//	void setId(const size_t& id) { _id = id; }
	//@}

	//! \name Accesseur
	//@{
	const std::string& Libelle()		const { return _Libelle; }
//	const size_t& getId() const { return _id; }
	//@}
	
	//!	\name Calculateur
	//@{
	bool AppliquePeriode(cMoment& __DateDebut, cMoment& __DateFin, const cMoment& __MomentCalcul, bool __SolutionsPassees) const;
	//@}
	
	//! \name Constructeur
	//@{
	cPeriodeJournee(const std::string name, const cHeure& startTime, const cHeure& endTime) : _Libelle(name), _HeureDebut(startTime), _HeureFin(endTime) {}
	~cPeriodeJournee() {}
	//@}
};




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
	

#endif
