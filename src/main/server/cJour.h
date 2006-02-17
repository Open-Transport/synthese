/*!	\file cJour.h
	\brief En-t�te, inline, et templates classe jour
*/

#ifndef SYNTHESE_CJOUR_H
#define SYNTHESE_CJOUR_H

using namespace std;

#include <iostream>
#include "Temps.h"

/** Jour
	@ingroup m04
*/
class cJour
{
	tJour		_Valeur;	//!< Jour (1 � 31)
	
	template <class charT, class Traits>
	friend basic_ostream<charT, Traits>& operator<<(
		basic_ostream<charT, Traits> &flux, const cMois& Obj);
public:
	//!	\name Modificateurs
	//@{
	cJour& operator=(const tJour);
	cJour& operator--(int);
	cJour& operator++(int);
	//@}

	//!	\name Accesseurs
	//@{
	tJour		Valeur() const;
	//@}
	
	//!	\name Constructeur
	//@{
	explicit cJour(tJour valeur = INCONNU) : _Valeur (valeur) {}
	//@}
};

inline tJour cJour::Valeur() const
{
	return _Valeur;
}

inline cJour& cJour::operator=(const tJour Valeur)
{
	_Valeur = Valeur;
	return *this;
}

inline cJour& cJour::operator--(int)
{
	_Valeur--;
	return *this;
}
	
inline cJour& cJour::operator++(int)
{
	_Valeur++;
	return *this;
}
	

#endif
