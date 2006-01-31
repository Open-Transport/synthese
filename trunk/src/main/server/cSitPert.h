/*-------------------------------------*
 |                                     |
 |  APDOS / APISAPI - SYNTHESE v0.5    |
 |  � Hugues Romain 2000/2001          |
 |  CSitPert.cpp                       |
 |  Header Classe Situation perturb�e  |
 |                                     |
 *-------------------------------------*/
 
 #ifndef SYNTHESE_CSITPERT_H
 #define SYNTHESE_CSITPERT_H
 
#include "Temps.h"
#include "cTexte.h"

  
 /*!	\brief Situation perturb�e
 */
class cSitPert
{
	cMoment	_DateDeclaration;
	cTexte	_Motif;
	
public:
	
	// Constructeur
	cSitPert(const cTexte& newMotif);
};

#endif
