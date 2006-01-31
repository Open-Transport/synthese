/*!	\file cAlternance.cpp
	\brief Impl�mentation classe cAlternance
*/

#include "cAlternance.h"
#include "Temps.h"



/*! \brief Constructeur
	\author Hugues Romain
	\date 2000
	\param __Message Message � afficher avant le compteur
	\param __Modulo Valeur � laquelle le compteur doit �tre r�initialis� lors des incr�mentations
	*/
cAlternance::cAlternance(const cTexte& __Message, char __Modulo)
{
	_Message << __Message;
	_Modulo = __Modulo;
	_Valeur = 1;
}

