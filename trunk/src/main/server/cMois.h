/**	@file cMois.h
	\brief En-t�te, inline, et templates classe dur�e en minutes
*/

#ifndef SYNTHESE_CMOIS_H
#define SYNTHESE_CMOIS_H

using namespace std;

#include <iostream>
#include "Temps.h"

/** Mois de l'ann�e
	@ingroup m04
*/
class cMois
{
	tMois	_Valeur;	//!< Mois de l'ann�e (1=Janvier ... 12=D�cembre)
	
	template <class charT, class Traits>
	friend basic_ostream<charT, Traits>& operator<<(
		basic_ostream<charT, Traits> &flux, const cMois& Obj);
public:

	//!	\name Constructeur et destructeur
	//@{
	explicit cMois(tMois valeur = INCONNU) : _Valeur (valeur) {}
	//@}
	
	//!	\name Modificateurs
	//@{
	cMois& operator=(const tMois);
	cMois& operator--(int);
	cMois& operator++(int);
	//@}

	//!	\name Accesseurs
	//@{
	tMois	Valeur() const;
	//@}
	
	//!	\name Calculateurs
	//@{
	tJour		NombreJours(const cAnnee&) 									const;
	tJour		NombresJoursJusquaFinMois(tJour, const cAnnee&)							const;
	tDureeEnJours	NombresJourJusquAMois(const cAnnee& anneeDep, const cMois& moisArr, const cAnnee& anneeArr) 	const;
	bool		OK()												const;
	//@}

	//!	\name Sorties
	//@{
	template <class T> T& toString(T& Objet, tLangue CodeLangue) const
	{
		switch (CodeLangue)
		{
		case LANGUEFrancais:
			switch (_Valeur)
			{
			case 1: Objet << "Janvier"; break;
			case 2: Objet << "F�vrier"; break;
			case 3: Objet << "Mars"; break;
			case 4: Objet << "Avril"; break;
			case 5: Objet << "Mai"; break;
			case 6: Objet << "Juin"; break;
			case 7: Objet << "Juillet"; break;
			case 8: Objet << "Ao�t"; break;
			case 9: Objet << "Septembre"; break;
			case 10: Objet << "Octobre"; break;
			case 11: Objet << "Novembre"; break;
			case 12: Objet << "D�cembre"; break;
			}
			break;
	
		case LANGUEAnglais:
			switch (_Valeur)
			{
			case 1: Objet << "January"; break;
			case 2: Objet << "February"; break;
			case 3: Objet << "March"; break;
			case 4: Objet << "April"; break;
			case 5: Objet << "May"; break;
			case 6: Objet << "June"; break;
			case 7: Objet << "July"; break;
			case 8: Objet << "August"; break;
			case 9: Objet << "September"; break;
			case 10: Objet << "October"; break;
			case 11: Objet << "November"; break;
			case 12: Objet << "December"; break;
			}
			break;
			
		case LANGUECode:
			Objet << _Valeur;
			break;
		}
		return(Objet);
	}
	//@}
	
};

/*!	\brief Nombre de jours dans un mois
	\param NumeroAnnee Ann�e
	\return Le nombre de jours dans le mois de l'objet, en tenant �ventuellement compte du caract�re bissextile de l'ann�e
*/
inline tJour cMois::NombreJours(const cAnnee& Annee) const
{
	switch (_Valeur)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		return(31);
	case 4:
	case 6:
	case 9:
	case 11:
		return(30);
	case 2:
		if (Annee.Bissextile())
			return(29);
		else
			return(28);
	}
	return(0);
}

/*!	\brief Nombre de jours restant dans le mois
	\param Jour Num�ro du jour actuel
	\param Annee Objet ann�e repr�sentant l'ann�e actuelle
	\return Le nombre de jours restant dans le mois sans compter le jour actuel
*/
inline tJour cMois::NombresJoursJusquaFinMois(tJour Jour, const cAnnee& Annee) const
{
	return (NombreJours(Annee) - Jour);
}

/*!	\brief Accesseur valeur
*/
inline tMois cMois::Valeur() const
{
	return _Valeur;
}

/*!	\brief Op�rateur de modification depuis un entier
*/
inline cMois& cMois::operator=(const tMois Valeur)
{
	_Valeur = Valeur;
	return *this;
}

/*!	\brief Op�rateur de d�cr�mentation
*/
inline cMois& cMois::operator--(int)
{
	_Valeur--;
	return *this;
}

/*!	\brief Op�rateur d'incr�mentation
*/
inline cMois& cMois::operator++(int)
{
	_Valeur++;
	return *this;
}

inline tDureeEnJours cMois::NombresJourJusquAMois(const cAnnee& anneeDep, const cMois& moisArr, const cAnnee& anneeArr) const
{
	tDureeEnJours calcul=0;
	cMois tMoisDep = *this;
	cAnnee tAnneeDep = anneeDep;
	while (tAnneeDep.Valeur()<anneeArr.Valeur() || tMoisDep.Valeur() < (moisArr.Valeur()-1))
	{
		tMoisDep++;
		if (tMoisDep.Valeur() > MOIS_PAR_AN)
		{
			tMoisDep=1;
			tAnneeDep++;
		}
		
		calcul = calcul + tMoisDep.NombreJours(tAnneeDep);
	}
	
	return calcul;
}

/*
template <class charT, class Traits> basic_ostream<charT, Traits>& operator<<(
		basic_ostream<charT, Traits> &flux, const cMois& Obj)
		{
			typename basic_ostream<charT, Traits>::sentry init(flux);
			if (init)
			{
				flux << Obj.Valeur();
			}
			return(flux);
		}

template <class Traits> basic_ostream<cTexteCodageInterne, Traits>& operator<<(
		basic_ostream<cTexteCodageInterne, Traits> &flux, const cMois& Obj)
		{
			typename basic_ostream<cTexteCodageInterne, Traits>::sentry init(flux);
			if (init)
			{
				flux << TXT2(Obj.Valeur(), 2);
			}
			return(flux);
		}
*/

template <class T>
inline T& operator<<(T& flux, const cMois& Obj)
{
	flux << Obj.Valeur();
	return flux;
}

template <>
inline cTexteCodageInterne& operator<< <cTexteCodageInterne>(cTexteCodageInterne& flux, const cMois& Obj)
{
	flux << TXT2(Obj.Valeur(), 2);
	return flux;
}

#endif
