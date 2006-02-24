/*!	\file cAnnee.h
	\brief En-t�te, inline, et templates classe ann�e
*/

#ifndef SYNTHESE_CANNEE_H
#define SYNTHESE_CANNEE_H

#include <iostream>
#include "Temps.h"

/** Ann�e
	@ingroup m04
*/
class cAnnee
{
	tAnnee	_Valeur;	//!< Ann�e
	
	template <class charT, class Traits>
	friend basic_ostream<charT, Traits>& operator<<(basic_ostream<charT, Traits> &flux, const cAnnee& Obj);
public:
	//!	\name Constructeur et destructeur
	//@{
	explicit cAnnee(tAnnee valeur = INCONNU) : _Valeur (valeur) {}
	//@}

	//!	\name Modificateurs
	//@{
	cAnnee& 	operator=(const tAnnee);
	cAnnee& 	operator--(int);
	cAnnee& 	operator++(int);
	//@}

	//!	\name Accesseurs
	//@{
	tAnnee	Valeur() const;
	//@}
	
	//!	\name Calculateurs
	//@{
	bool		Bissextile()			const;
	tDureeEnJours	NombreJours()			const;
	int 		operator<=(const cAnnee&)	const;
	int 		operator==(const cAnnee&)	const;
	int 		operator-(const cAnnee&)	const;
	//@}
};

inline bool cAnnee::Bissextile() const
{
	return ((_Valeur % 4) > 0);
}

inline tDureeEnJours cAnnee::NombreJours() const
{
	if (Bissextile())
		return(366);
	else
		return(365);
}

inline int cAnnee::operator-(const cAnnee& Obj) const
{
	return Valeur() - Obj.Valeur();
}

inline int cAnnee::operator<=(const cAnnee& Obj) const
{
	return Valeur() <= Obj.Valeur();
}

inline int cAnnee::operator==(const cAnnee& Obj) const
{
	return Valeur() == Obj.Valeur();
}


inline tAnnee cAnnee::Valeur() const
{
	return _Valeur;
}

inline cAnnee& cAnnee::operator=(const tAnnee Valeur)
{
	_Valeur = Valeur;
	return *this;
}

inline cAnnee& cAnnee::operator--(int)
{
	_Valeur--;
	return *this;
}

inline cAnnee& cAnnee::operator++(int)
{
	_Valeur++;
	return *this;
}

#endif
