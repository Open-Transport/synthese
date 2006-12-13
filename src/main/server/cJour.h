/*!	\file cJour.h
	\brief En-t�te, inline, et templates classe jour
*/

#ifndef SYNTHESE_CJOUR_H
#define SYNTHESE_CJOUR_H

using namespace std;

#include <iostream>


/** Jour
	@ingroup m04
*/
class cJour
{
	int		_Valeur;	//!< Jour (1 � 31)
	
	template <class charT, class Traits>
	friend basic_ostream<charT, Traits>& operator<<(
		basic_ostream<charT, Traits> &flux, const cMois& Obj);
public:
	//!	\name Modificateurs
	//@{
	cJour& operator=(const int);
	cJour& operator--(int);
	cJour& operator++(int);
	//@}

	//!	\name Accesseurs
	//@{
	int		Valeur() const;
	//@}
	
	//!	\name Constructeur
	//@{
	explicit cJour(int valeur = INCONNU) : _Valeur (valeur) {}
	//@}
};

inline int cJour::Valeur() const
{
	return _Valeur;
}

inline cJour& cJour::operator=(const int Valeur)
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

