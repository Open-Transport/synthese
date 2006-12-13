/*------------------------------------*
|                                    |
|  APDOS / APISAPI - SYNTHESE v0.4   |
|  � Hugues Romain 2000/2002         |
|  CAxe.cpp                          |
|  Impl�mentation Classes Axe        |
|                                    |
*------------------------------------*/

#include "cAxe.h"

/*! \brief Constructeur
 \author Hugues Romain
 \date 2001
*/
cAxe::cAxe( bool newLibre, const std::string& newCode, const std::string& newNomFichier, bool newAutorise )
    : vNomFichier (newNomFichier)
      , vCode (newCode)
{
    vLibre = newLibre;
    vAutorise = newAutorise;
}





