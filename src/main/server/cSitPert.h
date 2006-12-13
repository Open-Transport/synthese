/*-------------------------------------*
|                                     |
|  APDOS / APISAPI - SYNTHESE v0.5    |
|  © Hugues Romain 2000/2001          |
|  CSitPert.cpp                       |
|  Header Classe Situation perturbée  |
|                                     |
*-------------------------------------*/

#ifndef SYNTHESE_CSITPERT_H
 #define SYNTHESE_CSITPERT_H


#include "04_time/DateTime.h"
#include <string>


/*! \brief Situation perturbée
@ingroup m15
*/
class cSitPert
{
        synthese::time::DateTime _DateDeclaration;
        std::string _Motif;

    public:

        // Constructeur
        cSitPert( const std::string& newMotif );
};

#endif

