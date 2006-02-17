/*!	\file cFichierObjetInterfaceStandard.h
	\brief En-tête et implémentation inline classe de fichier de description d'objets d'interface standards
*/

#ifndef SYNTHESE_CFICHIEROBJETINTERFACESTANDARD_H
#define SYNTHESE_CFICHIEROBJETINTERFACESTANDARD_H

/** @addtogroup m11
	@{ */

//! @name Eléments d'interface
//@{
#define ELEMENTSINTERFACEFORMAT 			"elementsinterface"		//!< Format du fichier des éléments d'interface
#define ELEMENTSINTERFACEEXTENSION 		".elementinterface.per"	//!< Extenstion des fichiers d'élément d'interface

/** Insertion de texte brut
	@code T texte @endcode
	@param texte Texte qui sera affiché directement par le module d'interface
*/
#define ELEMENTSINTERFACEFORMATLIGNETexte							0


/** Inclusion d'élément d'interface déjà décrit
	@code > numero parametres @endcode
	@param numero Numéro de l'élément d'interface (voir @ref interface)
	@param parametres Succession de paramètres (nombre défini par l'élément inclus, non contrôlé) :
		- @c Pxx : Paramètre numéro xx de l'élément courant
		- @c {...} : Texte brut entre les accolades
		- @c Axx
*/
#define ELEMENTSINTERFACEFORMATLIGNEInclusion						1


/** Insertion d'un objet de la bibliothèque
	@code O numero parametres @endcode
	@param numero Numéro de l'élément d'interface (voir @ref ointref)
	@param parametres Succession de paramètres (nombre défini par l'élément inclus, non contrôlé) :
		- @c Pxx : Paramètre numéro xx de l'élément courant
		- @c {...} : Texte brut entre les accolades
		- @c Axx : Attribut de l'objet de données courant si existant
*/
#define ELEMENTSINTERFACEFORMATLIGNEObjet							2


/** Insertion d'un paramètre
	@code P numer @endcode
	@param numero Numéro du paramètre inséré (voir @ref interface)
*/
#define ELEMENTSINTERFACEFORMATLIGNEParametre						3


/** Insertion conditionnelle
	@code ? condition resultat_vrai resultat_faux @endcode
	@param condition Condition testée :
		- @c Pxx : Paramètre numéro xx de l'élément courant
		- @c Axx
	@param resultat_vrai Résultat inséré si test vrai
		- @c {...} : Texte brut
		- @c Pxx : Paramètre numéro xx de l'élément courant
		- @c Oxx
		- @c Axx
		- @c Gxx
	@param resultat_faux Résultat intéré si test faux (facultatif : défaut = rien)
		- @c {...} : Texte brut
		- @c Pxx : Paramètre numéro xx de l'élément courant
		- @c Oxx
		- @c Axx
		- @c Gxx
*/
#define ELEMENTSINTERFACEFORMATLIGNEIfThenElse						4


/** Insertion d'un attribut de l'objet de données courant
	@code A numero @endcode
	@param numero Numéro d'attribut (voir @ref interface)
*/
#define ELEMENTSINTERFACEFORMATLIGNEAttribut						5


/** Branchement à une ligne numérotée
	@code G numero @endcode
	@param numero Numéro de ligne à atteindre (doit se situer plus bas dans le fichier)
*/
#define ELEMENTSINTERFACEFORMATLIGNEGoto							6


/** Numérotation de ligne
	@code L numero @endcode
	@param numero Numéro de ligne
*/
#define ELEMENTSINTERFACEFORMATLIGNELigne							7
#define ELEMENTSINTERFACEFORMATLIGNENombreFormats					8

#define ELEMENTSINTERFACEFORMATCOLONNENumero						0
#define ELEMENTSINTERFACEFORMATCOLONNECommande						1
#define ELEMENTSINTERFACEFORMATCOLONNECommandePourElementStatique	2
#define ELEMENTSINTERFACEFORMATCOLONNESNombreFormats				3
//@}

#include "cFichier.h"
#include "cInterface.h"
#include "cInterface_Objet_AEvaluer_PageEcran.h"



/**	Fichier décrivant des éléments d'interfaces
*/
class cFichierObjetInterfaceStandard : public cFichier
{
public:
	bool Charge(cInterface_Objet_AEvaluer_PageEcran&, const cInterface&);
	
	cFichierObjetInterfaceStandard(const cTexte& NomFichier, const cTexte& NomFichierFormats);
};
/** @} */


/** Constructeur
	\param __NomFichier Nom du fichier sans l'extension qui est imposée et automatiquement ajoutée
	\param __NomFichierFormats Nom du fichier contenant la description des formats de fichiers
	\author Hugues Romain
	\date 2005
*/
inline cFichierObjetInterfaceStandard::cFichierObjetInterfaceStandard(const cTexte& __NomFichier, const cTexte& __NomFichierFormats)
 : cFichier(cTexte(__NomFichier).Copie(ELEMENTSINTERFACEEXTENSION), new cFormatFichier(__NomFichierFormats, ELEMENTSINTERFACEFORMAT, ELEMENTSINTERFACEFORMATLIGNENombreFormats, ELEMENTSINTERFACEFORMATCOLONNESNombreFormats))
{ }


#endif
