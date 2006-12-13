/*! \file cAlternance.cpp
\brief Implémentation classe cAlternance
*/

#include "cAlternance.h"




/*! \brief Constructeur
 \author Hugues Romain
 \date 2000
 \param __Message Message à afficher avant le compteur
 \param __Modulo Valeur à laquelle le compteur doit être réinitialisé lors des incrémentations
 */
cAlternance::cAlternance( const std::string& __Message, char __Modulo )
    : _Message (__Message)
{
    _Modulo = __Modulo;
    _Valeur = 1;
}


