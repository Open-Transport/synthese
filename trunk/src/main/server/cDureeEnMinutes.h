/*!	\file cDureeEnMinutes.h
	\brief En-t�te, inline, et templates classe dur�e en minutes
*/

#ifndef SYNTHESE_CDUREEENMINUTES_H
#define SYNTHESE_CDUREEENMINUTES_H

using namespace std;

#include "Temps.h"

/** Dur�e en minutes
	@ingroup m04
*/
class cDureeEnMinutes
{
	tDureeEnMinutes	_Valeur;	//!< Dur�e en minutes (peut �tre n�gative)

public:
	//!	\name Accesseurs
	//@{
	tDureeEnMinutes Valeur() const;
	//@}
	
	//!	\name Modificateurs
	//@{
	cDureeEnMinutes& operator+=(int);
	cDureeEnMinutes& operator+=(const cDureeEnMinutes&);
	cDureeEnMinutes& operator-=(int);
	cDureeEnMinutes& operator-=(const cDureeEnMinutes&);
	cDureeEnMinutes& operator=(int);
	cDureeEnMinutes& operator--(int);
	//@}
	
	//!	\name Calculateurs
	//@{
	int operator>(const cDureeEnMinutes&)	const;
	int operator>=(const cDureeEnMinutes&)	const;
	int operator<(const cDureeEnMinutes&)	const;
	int operator!=(const cDureeEnMinutes&)	const;
	//@}
	
	//!	\name Constructeur et destructeur
	//@{
	explicit cDureeEnMinutes(tDureeEnMinutes Valeur=0) { _Valeur = Valeur; }
	//@}
};



/*!	\brief Acc�s � la valeur
*/
inline tDureeEnMinutes cDureeEnMinutes::Valeur() const
{
	return _Valeur;
}

inline cDureeEnMinutes& cDureeEnMinutes::operator+=(int Valeur)
{
	_Valeur += Valeur;
	return *this;
}


inline cDureeEnMinutes& cDureeEnMinutes::operator-=(int __Valeur)
{
	_Valeur -= __Valeur;
	return *this;
}

inline cDureeEnMinutes& cDureeEnMinutes::operator+=(const cDureeEnMinutes& Valeur)
{
	return *this += Valeur.Valeur();
}

inline cDureeEnMinutes& cDureeEnMinutes::operator-=(const cDureeEnMinutes& Valeur)
{
	return *this -= Valeur.Valeur();
}

inline cDureeEnMinutes& cDureeEnMinutes::operator--(int)
{
	_Valeur--;
	return *this;
}

inline cDureeEnMinutes& cDureeEnMinutes::operator=(int Valeur)
{
	_Valeur = Valeur;
	return *this;
}

inline int cDureeEnMinutes::operator !=(const cDureeEnMinutes& __Objet) const
{
	return _Valeur != __Objet.Valeur();
}

inline int cDureeEnMinutes::operator>(const cDureeEnMinutes& Obj) const
{
	return Valeur() > Obj.Valeur();
}

inline int cDureeEnMinutes::operator>=(const cDureeEnMinutes& Obj) const
{
	return Valeur() >= Obj.Valeur();
}

inline int cDureeEnMinutes::operator<(const cDureeEnMinutes& Obj) const
{
	return Valeur() < Obj.Valeur();
}



/*!	\brief Affichage standard d'une dur�e
	\return Si la dur�e est inf�rieure � une heure, alors il est �crit la dur�e en minutes sinon il est �crit la dur�e en heures et minutes
	\param flux Le flux sur lequel �crire
	\param Obj La dur�e � afficher
	\todo SUPPRIMER CETTE FONCTION UTILISER L'INTERFACE
	\author Hugues Romain
	\date 2005
*/
template <class T> 
inline T& operator<<(T& flux, const cDureeEnMinutes& Obj)
{
	if (Obj.Valeur() < MINUTES_PAR_HEURE)
		flux << Obj.Valeur() << "min";
	else
		flux << Obj.Valeur() / 60 << "h" << TXT2(Obj.Valeur() % 60, 2);
	return flux;
}

#endif
