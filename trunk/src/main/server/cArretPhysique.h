/*------------------------------------*
 |                                    |
 |  APDOS / APISAPI - SYNTHESE v0.6   |
 |  � Hugues Romain 2000/2001         |
 |  cArretPhysique.h                  |
 |  Header Classes ArretPhysique      |
 |                                    |
 *------------------------------------*/

#ifndef SYNTHESE_CQUAI_H
#define SYNTHESE_CQUAI_H

typedef short int tNumeroVoie;
class cArretPhysique;

#include "cTexte.h"
#include "cPhoto.h"
#include "Point.h"

/*! \brief Arr�ts physiques (quais, arr�ts de bus, etc.)
	@ingroup m05
*/
class cArretPhysique : public NetworkAccessPoint
{

public:
	
	//!	\name Constructeur
	//@{
	cArretPhysique();
	//@}
};



#endif
