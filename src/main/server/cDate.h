/*!	\file cDate.h
	\brief En-t�te, inline, et templates classe date
*/

#ifndef SYNTHESE_CDATE_H
#define SYNTHESE_CDATE_H

#include <iostream>
#include "Temps.h"
#include "cAnnee.h"
#include "cMois.h"
#include "cJour.h"

/** Date
	@ingroup m04
*/
class cDate
{
	cJour  	_Jour;	//!<Jour
	cMois  	_Mois;	//!< Mois
	cAnnee 	_Annee;	//!< Ann�e
	
public:
	//! \name Accesseurs
	//@{
	tAnnee	Annee()	const;
	tJour	Jour()	const;
	tMois	Mois()	const;
	//@}
	
	//!	\name Modificateurs
	//@{
	/** Modification de la date � partir de trois entiers, sans contr�le des valeurs.
		\param Jour Nouvelle valeur pour le jour
		\param Mois Nouvelle valeur pour le Mois
		\param Annee Nouvelle valeur pour l'ann�e
		\return L'objet modifi�
		\author Hugues Romain
		\date 2001-2005

	Valeurs possibles pour les trois param�tres :
		- nombre entier : la valeur est prise telle quelle, sans contr�le total de validit� (une date comme le 31/2/1650 est accept�e).
		- commande TEMPS_ACTUEL ('A') = identique � la date actuelle selon le syst�me d'exploitation du serveur (penser � le synchroniser avec un serveur ntp)
		- commande TEMPS_MAX ('M') = identique � la date maximale absolue 31/12/9999
		- commande TEMPS_MIN ('m') = identique � la date minimale absolue 1/1/1
		- commande TEMPS_IDEM ('I') = ex�cution de la commande pass�e pour le jour. Si le jour est d�crit par une valeur, alors l'�l�ment n'est pas modifi�.
		- commande TEMPS_INCONNU ('?') = identique � la date inconnue (cod�e -1/-1/-1)
		- commande TEMPS_INCHANGE ('_') = la valeur n'est pas modifi�e

	La commande par d�faut du premier param�tre est TEMPS_ACTUEL
	La commande par d�faut des deux autres est TEMPS_IDEM
	Toute commande pass�e non comprise est interpr�t�e comme TEMPS_INCHANGE

	De ce fait, des utilisations courantes de la fonction en d�coulent :
		- utilisation sans param�tre : date fix�e � la date du jour
		- utilisation avec un param�tre de commande : date enti�rement fix�e selon la commande (ex : setDate(TEMPS_MAX) => 31/12/9999)
		- utilisation avec trois param�tres de valeur : date fix�e � un jour pr�cis (ex : setDate(2, 10, 2006) => 2/10/2006
		- utilisation avec trois param�tres h�t�rog�nes : permet d'obtenir des commande sp�ciales (ex : setDate(15, TEMPS_ACTUEL, TEMPS_ACTUEL) => le 15 du mois en cours)
	 
	La relation suivante est v�rifi�e : \f$ TEMPS_{INCONNU} \leq TEMPS_{MIN} \leq TEMPS_{ACTUEL} \leq TEMPS_{MAX} \f$
	*/
	cDate&	setDate(tJour Jour=TEMPS_ACTUEL, tMois Mois=TEMPS_IDEM, tAnnee Annee=TEMPS_IDEM);
	//void	ExtraitDate	(const cTexte& Tampon, const cEnvironnement* Env, size_t Position=0);
	cDate&	operator ++ (int);
	cDate&	operator -- (int);
	cDate&	operator += (tDureeEnJours JoursSupp);
	cDate&	operator -= (tDureeEnJours);
	cDate&	operator =  (const cTexte&);
	cDate&	operator =  (const cMoment&);
	//@}
		
	//! \name Calculateurs
	//@{
	int				operator <  (const cDate&)		const;
	int				operator <= (const cDate&)		const;
	int				operator <= (const cMoment&)	const;
	int				operator >  (const cDate&)		const;
	int				operator == (const cDate&)		const;
	int				operator != (const cDate&)		const;
 	tDureeEnJours	operator -	(const cDate&)		const;
	char 			JourSemaine()					const;
	bool			AnneeEstInconnue() 				const;
	bool			OK()							const;
	bool			EstInconnue()					const;
	//@}
	
	template <class T> T& toStringAvec0(T& Objet, tLangue CodeLangue=LANGUECode) const
	{
		if (CodeLangue == LANGUECode)
			Objet << TXT2(Jour(),2) << "/" << TXT2(Mois(),2) << "/" << TXT(Annee());
		else
		{
			Objet << TXT2(Jour(),2) << " ";
			LibelleMois(Objet, _Mois, CodeLangue);
			Objet << " " << TXT(Annee());
		}
		return(Objet);
	}

	/*template <class T> T& toXML(T& Objet) const
	{
		Objet << " jour=\"" << TXT(Jour()) << "\" mois=\"" << TXT(Mois()) << "\" annee=\"" << TXT(Annee()) << "\"";
		return(Objet);
	}*/
	//@}
	
	cDate() { _Jour = INCONNU; _Mois = INCONNU; _Annee = INCONNU; }
	~cDate() {}
}; 



/*!	\brief Affichage standard d'une date
	\author Hugues Romain
	\date 2005
*/
template <class T> 
inline T& operator<<(T& flux, const cDate& Obj)
{
	flux << Obj.Jour() << "/" << Obj.Mois() << "/" << Obj.Annee();
	return flux;
}



/*!	\brief Codage texte interne d'une date
	\author Hugues Romain
	\date 2005
*/
template <> 
inline cTexteCodageInterne& operator<< <cTexteCodageInterne> (cTexteCodageInterne& flux, const cDate& Obj)
{
	flux << cTexte(8).Copie(Obj.Annee(), 4).Copie(Obj.Mois(), 2).Copie(Obj.Jour(), 2);
	return flux;
}



inline tJour cDate::Jour() const
{
	return(_Jour.Valeur());
}

inline tMois cDate::Mois() const
{
	return(_Mois.Valeur());
}

inline tAnnee cDate::Annee() const
{
	return(_Annee.Valeur());
}

inline int cDate::operator == (const cDate& AutreDate) const
{
	return	_Annee.Valeur() == AutreDate._Annee.Valeur() 
		&&	_Mois.Valeur() == AutreDate._Mois.Valeur() 
		&&	_Jour.Valeur() == AutreDate._Jour.Valeur();
}

inline int cDate::operator < (const cDate& AutreDate) const
{
	return	_Annee.Valeur() < AutreDate._Annee.Valeur() 
		||	_Annee.Valeur() == AutreDate._Annee.Valeur() 
			&&	(	_Mois.Valeur() < AutreDate._Mois.Valeur() 
				||	_Mois.Valeur() == AutreDate._Mois.Valeur() 
					&&	_Jour.Valeur() < AutreDate._Jour.Valeur()
				);
}

inline int cDate::operator <= (const cDate& AutreDate) const
{
	return	_Annee.Valeur() < AutreDate._Annee.Valeur()
		||	_Annee.Valeur() == AutreDate._Annee.Valeur() 
			&&	(	_Mois.Valeur() < AutreDate._Mois.Valeur() 
				||	_Mois.Valeur() == AutreDate._Mois.Valeur() 
					&&	_Jour.Valeur() <= AutreDate._Jour.Valeur()
				);
}

inline int cDate::operator > (const cDate& AutreDate) const
{
	return	_Annee.Valeur() > AutreDate._Annee.Valeur()
		|| 	_Annee.Valeur() == AutreDate._Annee.Valeur()
			&& 	(	_Mois.Valeur() > AutreDate._Mois.Valeur() 
				|| 	_Mois.Valeur() == AutreDate._Mois.Valeur() 
					&& _Jour.Valeur() > AutreDate._Jour.Valeur()
				);
}



/*!	\brief Contr�le de la validit� des valeurs entr�es dans l'objet
	\return true si l'objet d�crit un jour existant
	\author Hugues Romain
*/
inline bool cDate::OK() const
{
	return	_Annee.Valeur() >=0 
		&&	_Mois.Valeur() > 0
		&&	_Mois.Valeur() <= MOIS_PAR_AN
		&&	_Jour.Valeur() > 0 
		&&	_Jour.Valeur() <= _Mois.NombreJours(_Annee);
}

inline int cDate::operator != (const cDate& Op2) const
{
	return _Annee.Valeur() != Op2._Annee.Valeur() 
		|| _Mois.Valeur() != Op2._Mois.Valeur() 
		|| _Jour.Valeur() != Op2._Jour.Valeur();
}

inline bool cDate::AnneeEstInconnue() const
{
	return _Annee.Valeur() == INCONNU;
}

inline bool cDate::EstInconnue() const
{
	return _Annee.Valeur() == INCONNU && _Mois.Valeur() == INCONNU && _Jour.Valeur() == INCONNU;
}

#endif
