#include "cFichierTbDep.h"


#include "cTableauAffichageSelectifDestinations.h"


/*! 	\brief Chargement du fichier des sites
  \param Base La classe principale de l'application SYNTHESE où enregistrer les données lues
  \return true si le chargment est fait sans erreur majeure, sinon false
  \warning Le chargement des sites doit intervenir après celui des environnements et des interfaces
  \todo Supprimer les sites mal formés
*/
bool cFichierTbDep::Charge()
{
    // Variables locales
//	tIndex NombreTbCharges = 0;
    bool PasTermineSousSection = true;
    bool PasTermineElement=false;

    // Tampon
    cTexte Tampon(TAILLETAMPON, true);

    // Objets courants
    cTableauAffichage*						__TbDep			= NULL;
    cTableauAffichageSelectifDestinations*	__TbDepSelect	= NULL;

    // Allocation immédiate de l'espace suffisant
    if (!Synthese.TableauTableauxAffichage().AlloueSiBesoin(NumeroMaxElement()+1))
    {
	Synthese.FichierLogBoot().Ecrit(MESSAGE_SITES_PBALLOC, _Chemin);
	return false;
    }
	
    Tampon.Vide();
    if (!Ouvrir())
    {
	Synthese.FichierLogBoot().Ecrit(MESSAGE_SITES_PBOUVRIR, _Chemin);
	return false;
    }

    // Boucle sur les Sous Sections
    while (PasTermineSousSection)
    {
	if (ProchaineSection(Tampon, TYPESousSection))
	{
	    cTexte __Code = _Format->Extrait(Tampon, TBDEPFORMATCOLONNECode);
	    cTexte __Methode = _Format->Extrait(Tampon, TBDEPFORMATCOLONNEMethode);
	    switch(__Methode[0])
	    {
	    case 'S':
		__TbDepSelect = new cTableauAffichageSelectifDestinations(__Code);
		__TbDep = (cTableauAffichage*) __TbDepSelect;
		break;

	    case 'C':
		__TbDepSelect = NULL;
		__TbDep = new cTableauAffichage(__Code);
		break;
	    }
	    if (Synthese.Enregistre(__TbDep) == INCONNU)
	    {
		Synthese.FichierLogBoot().Ecrit(MESSAGE_SITES_PBENREG, Tampon);
		Tampon.Vide();
	    }
	    else
	    {
		PasTermineElement = true;
//				NombreSitesCharges++;
	    }
	}
	else
	    PasTermineSousSection = false;

	// Boucle sur les propriétés d'un élément
	while (PasTermineElement)
	{
	    switch (LireLigneFormat(Tampon))
	    {
	    case TBDEPFORMATLIGNEEnvironnement:
		if (!__TbDep->SetEnvironnement(*Synthese.TableauEnvironnements().GetElement(_Format->GetNombre(Tampon, TBDEPFORMATCOLONNEStandard))))
		    Synthese.FichierLogBoot().Ecrit(MESSAGE_SITES_PBENV, Tampon, Tampon);
		break;
				
	    case TBDEPFORMATLIGNEInterface:
		if (!__TbDep->SetInterface(Synthese.GetInterface(_Format->GetNombre(Tampon, TBDEPFORMATCOLONNEStandard))))
		    Synthese.FichierLogBoot().Ecrit(MESSAGE_SITES_PBINT, Tampon, Tampon);
		break;

	    case TBDEPFORMATLIGNETitre:
		__TbDep->SetTitre(_Format->Extrait(Tampon, TBDEPFORMATCOLONNEStandard));
		break;

	    case TBDEPFORMATLIGNEOriginesSeulement:
		__TbDep->SetOriginesSeulement(_Format->GetNombre(Tampon, TBDEPFORMATCOLONNEStandard) >0);
		break;
				
	    case TBDEPFORMATLIGNEPointArret:
		__TbDep->SetPointArret(__TbDep->getEnvironnement()->GetPointArret(_Format->GetNombre(Tampon, TBDEPFORMATCOLONNEStandard)));
		break;

	    case TBDEPFORMATLIGNEQuai:
		__TbDep->AddQuaiAutorise(_Format->GetNombre(Tampon, TBDEPFORMATCOLONNEStandard));
		break;

	    case TBDEPFORMATLIGNEDestinationAffichee:
		__TbDep->AddDestinationAffichee(__TbDep->getEnvironnement()->GetPointArret(_Format->GetNombre(Tampon, TBDEPFORMATCOLONNEStandard)));
		break;

	    case TBDEPFORMATLIGNEDestinationSelection:
		if (__TbDepSelect != NULL)
		{
		    __TbDepSelect->AddDestination(__TbDep->getEnvironnement()->GetPointArret(_Format->GetNombre(Tampon, TBDEPFORMATCOLONNEStandard)));
		}
		break;

	    case TBDEPFORMATLIGNENombreDeparts:
		__TbDep->SetNombreDeparts(_Format->GetNombre(Tampon, TBDEPFORMATCOLONNEStandard));
		break;

	    case TBDEPFORMATLIGNENumeroPanneau:
		__TbDep->SetNumeroPanneau(_Format->GetNombre(Tampon, TBDEPFORMATCOLONNEStandard));
		break;

	    case -TYPEVide:
		Tampon.Vide();
	    case -TYPESousSection:
	    {
/*					if (!curSite->getNumeroEnv())
					{
					Base.Erreur("Définition de l'environnement manquante", curSite->getClef(), "", "08002");
						
					}	
					else if(!curSite->getNumeroInterface())
					{
					Base.Erreur("Définition de l'interface manquante", curSite->getClef(), "", "08003");
					}
*/
		PasTermineElement = false;
	    }
	    }
	}
    }
    Fermer();

    return true;
}


