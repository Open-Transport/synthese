/*! \file cAlternance.h
	\brief En-t�te classe cAlternance
	*/

#ifndef SYNTHESE_CALTERNANCE_H
#define SYNTHESE_CALTERNANCE_H

#include "cTexte.h"

/*!	\brief Fournit un index modulo n

Cette classe est utilis�e en particulier pour g�n�rer une alternance de styles pour une application sur des lignes de fiche horaire par exemple.
*/
class cAlternance
{
	cTexte	_Message;	//!<Message � afficher avant le compteur
	short int	_Modulo;	//!< Valeur � laquelle le compteur doit �tre r�initialis� lors des incr�mentations
	short int _Valeur;	//!< Valeur de d�part
	
//	template <class T> friend T& operator<<(T&, const cAlternance&);
	
public:
	void Incrementer();
	cAlternance(const cTexte& __Message, char __Modulo);
};



/*! \brief Incr�mente le compteur et effectue la rotation en fin de modulo
	\author Hugues Romain
	\date 2000
*/
inline void cAlternance::Incrementer()
{
	if (_Valeur == _Modulo)
		_Valeur = 1;
	else
		_Valeur++;
}



/*!	\brief Affichage standard d'une Alternance
	\author Hugues Romain
	\date 2005
*/
/*
template <class T> 
inline T& operator<<(T& __flux, const cAlternance& __Obj)
{
	__flux << _Message << _Valeur;
	return __flux;
}
*/
	
#endif
