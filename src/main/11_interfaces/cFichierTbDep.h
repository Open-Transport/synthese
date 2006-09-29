#ifndef SYNTHESE_CFICHIERTBDEP_H
#define SYNTHESE_CFICHIERTBDEP_H


#include "cFichier.h"

//#include "cFichierObjetInterfaceStandard.h"




//! @name Format du fichier tbdep
//@{
#define TBDEPFORMAT "tbdep"
#define TBDEPEXTENSION "tbdep.per"

#define TBDEPFORMATLIGNEEnvironnement			0
#define TBDEPFORMATLIGNEInterface				1
#define TBDEPFORMATLIGNETitre					2
#define TBDEPFORMATLIGNEOriginesSeulement		3
#define TBDEPFORMATLIGNEPointArret				4
#define TBDEPFORMATLIGNEQuai					5
#define TBDEPFORMATLIGNEDestinationAffichee		6
#define TBDEPFORMATLIGNEDestinationSelection	7
#define TBDEPFORMATLIGNENombreDeparts			8
#define TBDEPFORMATLIGNENumeroPanneau			9
#define TBDEPFORMATLIGNENombreFormats			10

#define TBDEPFORMATCOLONNEStandard 				0
#define TBDEPFORMATCOLONNEMethode 				1
#define TBDEPFORMATCOLONNECode					2
#define TBDEPFORMATCOLONNENombreFormats			3
//@}




class cFichierTbDep : public cFichier
{
public:
	bool	Charge();
	cFichierTbDep(const cTexte& NomFichier, const cTexte& NomFichierFormats)
		: cFichier(cTexte(NomFichier).Copie(TBDEPEXTENSION), new cFormatFichier(NomFichierFormats, TBDEPFORMAT, TBDEPFORMATLIGNENombreFormats, TBDEPFORMATCOLONNENombreFormats))
		{ _CheminFichierFormats = NomFichierFormats; }
};




#endif
