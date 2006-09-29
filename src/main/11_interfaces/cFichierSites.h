#ifndef SYNTHESE_CFICHIERSITES_H
#define SYNTHESE_CFICHIERSITES_H


#include "cFichier.h"
// #include "cFichierObjetInterfaceStandard.h"


//! @name Format du fichier Sites
//@{
#define SITESFORMAT "sites"
#define SITESEXTENSION "sites.per"
#define SITESFORMATLIGNEEnv 					0
#define SITESFORMATLIGNEInterface 				1
#define SITESFORMATLIGNEIdentifiant 			2
#define SITESFORMATLIGNEDateDebut 				3
#define SITESFORMATLIGNEDateFin 				4
#define SITESFORMATLIGNEResaEnLigne				5
#define SITESFORMATLIGNEURLClient				6
#define SITESFORMATLIGNESolutionsPassees		7
#define SITESFORMATLIGNENombreFormats			8

#define SITESFORMATCOLONNEStandard 				0
#define SITESFORMATCOLONNEClef		 			1
#define SITESFORMATCOLONNEDate		 			2
#define SITESFORMATCOLONNEURLClient				3
#define SITESFORMATCOLONNESolutionsPassees		4
#define SITESFORMATCOLONNEResaEnLigne			5
#define SITESFORMATCOLONNENombreFormats			6
//@}


class cFichierSites : public cFichier
{
public:
    bool Charge();
    
    cFichierSites(const cTexte& CheminFichier, const cTexte& NomFichierFormats)
	: cFichier(cTexte(CheminFichier).Copie(SITESEXTENSION), 
		   new cFormatFichier(NomFichierFormats, 
				      SITESFORMAT, 
				      SITESFORMATLIGNENombreFormats, 
				      SITESFORMATCOLONNENombreFormats))
	{ 
	}
};



#endif
