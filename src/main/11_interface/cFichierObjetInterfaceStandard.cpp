
#include "cFichierObjetInterfaceStandard.h"

#include "cInterface_Objet_Element_Parametre_DonneeEnvironnement.h"
#include "cInterface_Objet_Element_Bibliotheque.h"
#include "cInterface_Objet_AEvaluer_PageEcran.h"
#include "cInterface_Objet_Element_Goto.h"
#include "cInterface_Objet_Element_Ligne.h"


/** Constructeur
	\param __NomFichier Nom du fichier sans l'extension qui est imposée et automatiquement ajoutée
	\param __NomFichierFormats Nom du fichier contenant la description des formats de fichiers
	\author Hugues Romain
	\date 2005
*/
cFichierObjetInterfaceStandard::cFichierObjetInterfaceStandard(const cTexte& __NomFichier, const cTexte& __NomFichierFormats)
 : cFichier(cTexte(__NomFichier).Copie(ELEMENTSINTERFACEEXTENSION), new cFormatFichier(__NomFichierFormats, ELEMENTSINTERFACEFORMAT, ELEMENTSINTERFACEFORMATLIGNENombreFormats, ELEMENTSINTERFACEFORMATCOLONNESNombreFormats))
{ }






/*! \brief Chargement d'un fichier d'élément d'interface
	\author Hugues Romain
	\date 2002-2005
*/
bool cFichierObjetInterfaceStandard::Charge(cInterface_Objet_AEvaluer_PageEcran& __Objet, const cInterface& __Interface)
{
	// Variables
	bool PasTermineElementElement = true;
	
	// Tampon
	cTexte Tampon(TAILLETAMPON, true);

	// LOG
	//Tampon << "Ouverture du fichier de gestion des interfaces " << _Chemin;
	//Base.Message(Tampon, 2);
	
	if (!Ouvrir())
	{
	//	Base.Erreur("Impossible d'ouvrir le fichier", _Chemin, "", "");
		return false;
	}

	while (PasTermineElementElement)
	{
		switch (LireLigneFormat(Tampon))
		{
		case ELEMENTSINTERFACEFORMATLIGNETexte:
			__Objet.Chaine(new cInterface_Objet_Element_Parametre_TexteConnu(_Format->Extrait(Tampon, ELEMENTSINTERFACEFORMATCOLONNECommandePourElementStatique)));
			break;

		case -TYPELien:
		{
			cInterface_Objet_AEvaluer_ListeParametres __ParametresDonnees;
			__ParametresDonnees.InterpreteTexte(cInterface_Objet_AEvaluer_ListeParametres(), _Format->Extrait(Tampon, ELEMENTSINTERFACEFORMATCOLONNECommande));
			__Objet.InclureCopie(__Interface[_Format->GetNombre(Tampon, INTERFACESFORMATCOLONNENumero)], __ParametresDonnees);
		}
			break;

		case ELEMENTSINTERFACEFORMATLIGNEObjet:
		{
			cInterface_Objet_Element_Bibliotheque* __EI = new cInterface_Objet_Element_Bibliotheque(_Format->GetNombre(Tampon, INTERFACESFORMATCOLONNENumero), _Format->Extrait(Tampon, ELEMENTSINTERFACEFORMATCOLONNECommande));
			__Objet.Chaine(__EI);
		}
			break;

		case ELEMENTSINTERFACEFORMATLIGNEParametre:
			__Objet.Chaine(
				new cInterface_Objet_Element_Parametre_TexteAEvaluer(
					_Format->GetNombre(Tampon, INTERFACESFORMATCOLONNENumero)
				)
			);
			break;

		case ELEMENTSINTERFACEFORMATLIGNEIfThenElse:
			__Objet.Chaine(
				new cInterface_Objet_Element_TestConditionnel(
					_Format->Extrait(Tampon, ELEMENTSINTERFACEFORMATCOLONNECommandePourElementStatique)
				)
			);
			break;
			
		case ELEMENTSINTERFACEFORMATLIGNEAttribut:
			__Objet.Chaine(
				new cInterface_Objet_Element_Parametre_DonneeEnvironnement(
					_Format->GetNombre(Tampon, ELEMENTSINTERFACEFORMATCOLONNENumero)
				)
			);
			break;

		case ELEMENTSINTERFACEFORMATLIGNEGoto:
			__Objet.Chaine(
				new cInterface_Objet_Element_Goto(
					_Format->GetNombre(Tampon, ELEMENTSINTERFACEFORMATCOLONNENumero)
				)
			);
			break;
		
		case ELEMENTSINTERFACEFORMATLIGNELigne:
			__Objet.Chaine(
				new cInterface_Objet_Element_Ligne(
					_Format->GetNombre(Tampon, ELEMENTSINTERFACEFORMATCOLONNENumero)
				)
			);
			break;
		
		case -TYPEVide:
			Tampon.Vide();
		case -TYPESousSection:
			Fermer();
			PasTermineElementElement = false;
		}
	}
	
	return true;
}
