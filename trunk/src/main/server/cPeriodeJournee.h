/*!	\file cPeriodeJournee.h
	\brief En-t�te classe p�riode de la journ�e
*/



#ifndef SYNTHESE_CPERIODEJOURNEE_H
#define SYNTHESE_CPERIODEJOURNEE_H

#include "Temps.h"
#include "cTexte.h"



/** P�riode de la journ�e
	@ingroup m04
	\author Hugues Romain
	\date 2005

Les objets p�riodes de la journ�e d�crivent pour une interface une tranche horaire sur laquelle s'effectuera le calcul de fiche horaire.
*/
class cPeriodeJournee
{
	cHeure	_HeureDebut;	//!< Heure du d�but de la p�riode
	cHeure	_HeureFin;	//!< Heure de fin de la p�riode
	cTexte	_Libelle;		//!< Libell� de la p�riode pour utilisation dans un formulaire par exemple
	
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
	\param __HeureDebut Heure du d�but de la p�riode de la journ�e
	\param __HeureFin Heure de fin de la p�riode de la journ�e
	\param __Libelle Libell� de l'objet
	\return true si les heures de d�but et de fin sont valides, false sinon
	
La modification des valeurs ne s'effectue que si l'ensemble des donn�es est valide (retour true)
*/
inline bool cPeriodeJournee::SetValeurs(const cHeure& __HeureDebut, const cHeure& __HeureFin, const cTexte& __Libelle)
{
	// Contr�le des entr�es
	if (	!__HeureDebut.OK() || !__HeureFin.OK())
		return false;
		
	// Affectation des valeurs
	_HeureDebut = __HeureDebut;
	_HeureFin = __HeureFin;
	_Libelle = __Libelle;
	
	// Sortie OK
	return true;
}



/*!	\brief Applique la p�riode de la journ�e aux dates fournies
	\param __MomentDebut L'objet date de d�but � modifier
	\param __MomentFin L'objet date de fin � modifier
	\param __MomentCalcul Moment du calcul
	\param __SolutionsPassees Filtre solutions pass�es (true = solutions pass�es affich�es)
	\return true si la plage temporelle appliqu�e est valide (cas d'invalidit� : p�riode demand�e anti�rueure au moment pr�sent et filtre solutions pass�es activ�)
	\author Hugues Romain
	\date 2005
	
L'application de la p�riode � deux dates effectue les actions suivantes :
 - V�rification de la validit� de la p�riode
 - Changement de l'heure � la date de d�but : d�but de p�riode ou heure du calcul si filtre solutions pass�es
 - Si l'heure de fin est inf�rieure � l'heure de d�but, alors ajout d'un jour � la date de fin
 - Changement de l'heure � la date de fin
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

	// Contr�le
	if (!__SolutionsPassees)
	{
		if (__MomentFin < __MomentCalcul)
			return false;
		if (__MomentDebut < __MomentCalcul)
			__MomentDebut = __MomentCalcul;
	}

	return true;
}
	


/*!	\brief Accesseur libell�
	\return Le libell� de l'objet
	\author Hugues Romain
	\date 2005
*/
inline const cTexte& cPeriodeJournee::Libelle() const
{
	return _Libelle;
}

#endif
