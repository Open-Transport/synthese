/**	@file cMois.h
	\brief En-t�te, inline, et templates classe dur�e en minutes
*/

#ifndef SYNTHESE_CMOIS_H
#define SYNTHESE_CMOIS_H

using namespace std;

#include <iostream>

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
