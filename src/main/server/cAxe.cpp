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
cAxe::cAxe( bool newLibre, const cTexte& newCode, const cTexte& newNomFichier, bool newAutorise )
{
    vLibre = newLibre;
    vAutorise = newAutorise;
    vCode.Vide();
    vCode << newCode;
    vNomFichier.Vide();
    vNomFichier << newNomFichier;
}




