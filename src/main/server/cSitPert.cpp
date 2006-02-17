/*---------------------------------------------*
 |                                             |
 |  APDOS / APISAPI - SYNTHESE v0.5            |
 |  © Hugues Romain 2000/2001                  |
 |  CSitPert.cpp                               |
 |  Implémentation Classe Situation perturbée  |
 |                                             |
 *---------------------------------------------*/

#include "cSitPert.h"
 
cSitPert::cSitPert(const cTexte& newMotif)
{
	_Motif << newMotif;
}
