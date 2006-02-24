/*! \file cSite.h
	\brief En-t�te classe cSite
	*/

#ifndef SYNTHESE_CSITE_H
#define SYNTHESE_CSITE_H

class cSite;
class cInterface_Objet_Connu_ListeParametres;
class cEnvironnement;
class cInterface;

#include "cDate.h"
#include <iostream>
#include <string>

/** Gestion des sites Internet clients
	@ingroup m10
	\author Hugues Romain
	\date 2005
*/
class cSite
{
	//!	\name Propri�t�s
	//@{
	const std::string	vClef;				//!< Cl� du site � fournir en URL
	std::string  			vIdentifiant;		//!< Identifiant du site
	cDate   			vDateDebut;			//!< Date de d�but de validit� du site
	cDate   			vDateFin;			//!< Date de fin de validit� du site
	std::string				_URLClient;			//!< URL du binaire client (pour fabrication de liens)
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
	cSite(const std::string& clef);
	//@}
	
	//! \name Modificateurs
	//@{
	void	setIdentifiant		(const std::string& id);
	bool	SetEnvironnement	(cEnvironnement*);
	bool	SetInterface		(const cInterface*);
	void	setDateDebut		(const cDate& dateDebut);
	void	setDateFin			(const cDate& dateFin);
	bool	setResaEnLigne		(const bool valeur);
	bool	setURLClient		(const std::string&);
	bool	setSolutionsPassees	(bool);
	//@}

	//! \name Accesseurs
	//@{
	const std::string&			getClef()				const;
	cEnvironnement*	getEnvironnement()		const;
	const std::string&			getIdentifiant()		const;
	const cInterface*		getInterface()			const;
	const cDate& 			getDateDebut()			const;
	const cDate& 			getDateFin()			const;
	const std::string&			getURLClient()			const;
	bool					ResaEnLigne()			const;
	bool					getSolutionsPassees()	const;
	//@}

	//! \name Calculateurs
	//@{	
	bool				valide()																const;
	void				Affiche(ostream&, tIndex __IndexObjet, const cInterface_Objet_Connu_ListeParametres&
								, const void* __Objet=NULL)									const;
	
	//@}
};

#endif
