/*! \file cSite.h
	\brief En-t�te classe cSite
	*/

#ifndef SYNTHESE_CSITE_H
#define SYNTHESE_CSITE_H

class cSite;
class cInterface_Objet_Connu_ListeParametres;
class cEnvironnement;

#include "Temps.h"
#include "cTexte.h"
#include "cInterface.h"
#include "cCalculItineraire.h"
#include <iostream>

/** Gestion des sites Internet clients
	@ingroup m51
	\author Hugues Romain
	\date 2005
*/
class cSite
{
	//!	\name Propri�t�s
	//@{
	cTexte 				vClef;				//!< Cl� du site � fournir en URL
	cTexte  			vIdentifiant;		//!< Identifiant du site
	cDate   			vDateDebut;			//!< Date de d�but de validit� du site
	cDate   			vDateFin;			//!< Date de fin de validit� du site
	cTexte				_URLClient;			//!< URL du binaire client (pour fabrication de liens)
	//@}

	//!	\name Param�tres
	//@{
	cEnvironnement*		_Environnement;		//!< Environnement li� au site
	const cInterface*	_Interface;			//!< Interface li�e au site
	//@}

	//!	\name Filtres
	//@{
	bool				vResaEnLigne;		//!< R�servation en ligne autoris�e sur ce site
	bool				_SolutionsPassees;	//!< Affichage des solutions pass�es (d�faut = false)
	//@}
		
public:
	//! \name Constructeur
	//@{
	cSite(const cTexte& clef);
	//@}
	
	//! \name Modificateurs
	//@{
	void	setIdentifiant		(const cTexte& id);
	bool	SetEnvironnement	(cEnvironnement*);
	bool	SetInterface		(const cInterface*);
	void	setDateDebut		(const cDate& dateDebut);
	void	setDateFin			(const cDate& dateFin);
	bool	setResaEnLigne		(const bool valeur);
	bool	setURLClient		(const cTexte&);
	bool	setSolutionsPassees	(bool);
	//@}

	//! \name Accesseurs
	//@{
	const cTexte&			getClef()				const;
	cEnvironnement*	getEnvironnement()		const;
	const cTexte&			getIdentifiant()		const;
	const cInterface*		getInterface()			const;
	const cDate& 			getDateDebut()			const;
	const cDate& 			getDateFin()			const;
	const cTexte&			getURLClient()			const;
	bool					ResaEnLigne()			const;
	bool					getSolutionsPassees()	const;
	//@}

	//! \name Calculateurs
	//@{	
	bool				valide()																const;
	cCalculateur*		CalculateurLibre()														const;
	void				Affiche(ostream&, tIndex __IndexObjet, const cInterface_Objet_Connu_ListeParametres&
								, const void* __Objet=NULL)									const;
	
	//@}
};

#endif
