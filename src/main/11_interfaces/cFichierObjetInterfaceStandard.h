/*!	\file cFichierObjetInterfaceStandard.h
	\brief En-t�te et impl�mentation inline classe de fichier de description d'objets d'interface standards
*/

#ifndef SYNTHESE_CFICHIEROBJETINTERFACESTANDARD_H
#define SYNTHESE_CFICHIEROBJETINTERFACESTANDARD_H

/** @addtogroup m11
	@{ */

//! @name El�ments d'interface
//@{
#define ELEMENTSINTERFACEFORMAT 			"elementsinterface"		//!< Format du fichier des �l�ments d'interface
#define ELEMENTSINTERFACEEXTENSION 		".elementinterface.per"	//!< Extenstion des fichiers d'�l�ment d'interface

/** Insertion de texte brut
	@code T texte @endcode
	@param texte Texte qui sera affich� directement par le module d'interface
*/
#define ELEMENTSINTERFACEFORMATLIGNETexte							0


/** Inclusion d'�l�ment d'interface d�j� d�crit
	@code > numero parametres @endcode
	@param numero Num�ro de l'�l�ment d'interface (voir @ref interface)
	@param parametres Succession de param�tres (nombre d�fini par l'�l�ment inclus, non contr�l�) :
		- @c Pxx : Param�tre num�ro xx de l'�l�ment courant
		- @c {...} : Texte brut entre les accolades
		- @c Axx
*/
#define ELEMENTSINTERFACEFORMATLIGNEInclusion						1


/** Insertion d'un objet de la biblioth�que
	@code O numero parametres @endcode
	@param numero Num�ro de l'�l�ment d'interface (voir @ref ointref)
	@param parametres Succession de param�tres (nombre d�fini par l'�l�ment inclus, non contr�l�) :
		- @c Pxx : Param�tre num�ro xx de l'�l�ment courant
		- @c {...} : Texte brut entre les accolades
		- @c Axx : Attribut de l'objet de donn�es courant si existant
*/
#define ELEMENTSINTERFACEFORMATLIGNEObjet							2


/** Insertion d'un param�tre
	@code P numer @endcode
	@param numero Num�ro du param�tre ins�r� (voir @ref interface)
*/
#define ELEMENTSINTERFACEFORMATLIGNEParametre						3


/** Insertion conditionnelle
	@code ? condition resultat_vrai resultat_faux @endcode
	@param condition Condition test�e :
		- @c Pxx : Param�tre num�ro xx de l'�l�ment courant
		- @c Axx
	@param resultat_vrai R�sultat ins�r� si test vrai
		- @c {...} : Texte brut
		- @c Pxx : Param�tre num�ro xx de l'�l�ment courant
		- @c Oxx
		- @c Axx
		- @c Gxx
	@param resultat_faux R�sultat int�r� si test faux (facultatif : d�faut = rien)
		- @c {...} : Texte brut
		- @c Pxx : Param�tre num�ro xx de l'�l�ment courant
		- @c Oxx
		- @c Axx
		- @c Gxx
*/
#define ELEMENTSINTERFACEFORMATLIGNEIfThenElse						4


/** Insertion d'un attribut de l'objet de donn�es courant
	@code A numero @endcode
	@param numero Num�ro d'attribut (voir @ref interface)
*/
#define ELEMENTSINTERFACEFORMATLIGNEAttribut						5


/** Branchement � une ligne num�rot�e
	@code G numero @endcode
	@param numero Num�ro de ligne � atteindre (doit se situer plus bas dans le fichier)
*/
#define ELEMENTSINTERFACEFORMATLIGNEGoto							6


/** Num�rotation de ligne
	@code L numero @endcode
	@param numero Num�ro de ligne
*/
#define ELEMENTSINTERFACEFORMATLIGNELigne							7
#define ELEMENTSINTERFACEFORMATLIGNENombreFormats					8

#define ELEMENTSINTERFACEFORMATCOLONNENumero						0
#define ELEMENTSINTERFACEFORMATCOLONNECommande						1
#define ELEMENTSINTERFACEFORMATCOLONNECommandePourElementStatique	2
#define ELEMENTSINTERFACEFORMATCOLONNESNombreFormats				3
//@}

#include "cFichier.h"
#include "Interface.h"
#include "cInterface_Objet_AEvaluer_PageEcran.h"



/**	Fichier d�crivant des �l�ments d'interfaces
*/
class cFichierObjetInterfaceStandard : public cFichier
{
public:
	bool Charge(cInterface_Objet_AEvaluer_PageEcran&, const synthese::interfaces::Interface&);
	
	cFichierObjetInterfaceStandard(const cTexte& NomFichier, const cTexte& NomFichierFormats);
};
/** @} */




#endif
