#include "cFichierInterfaces.h"





/*!	\brief Chargement du fichier d'interface
	\author Hugues Romain
	\date 2002
	\return true si le fichier a été trouvé, false sinon.
*/
bool cFichierInterfaces::Charge()
{
	// Variables
	tIndex NombreInterfacesReel = 0;
	bool PasTermineSousSection = true;
	bool PasTermineElement=false;

	// Tampon
	cTexte Tampon(TAILLETAMPON, true);
	cTexte Tampon2(TAILLETAMPON, true);
	
	// Objets courants
	cInterface* curInterface=NULL;

	// Allocation immédiate de l'espace suffisant
	if (!Synthese.TableauInterfaces().AlloueSiBesoin(NumeroMaxElement()+1))
	{
// 		Base.Erreur("Echec de l'allocation du tableau des interfaces", _Chemin, "", "");
		return false;
	}
	
	if (!Ouvrir())
	{
// 		Base.Erreur("Impossible d'ouvrir le fichier", _Chemin, "", "");
		return false;
	}

	Tampon.Vide();
	
	// Boucle sur les Sous Sections
	while (PasTermineSousSection)
	{
		if (ProchaineSection(Tampon, INTERFACESFORMATLIGNEInterface))
		{
			curInterface = new cInterface();
			if (Synthese.Enregistre(curInterface, Tampon.GetNombre(0, 1)) == INCONNU)
			{
// 				Base.Erreur("Echec enregistrement interface", _Chemin, Tampon, "");
				Tampon.Vide();
			}
			else
			{
				NombreInterfacesReel++;
				PasTermineElement = true;
			}
		}
		else
			PasTermineSousSection = false;

		// Boucle sur les éléments d'une interface
		while (PasTermineElement)
		{
			switch (LireLigneFormat(Tampon))
			{
			case INTERFACESFORMATLIGNEElement:
			{
				Tampon2 = _Format->Extrait(Tampon, INTERFACESFORMATCOLONNEChemin);
				Tampon2.fAdresseComplete(_Chemin);
				cFichierObjetInterfaceStandard __Fichier(Tampon2, _CheminFichierFormats);
				
				__Fichier.Charge(curInterface->Element(_Format->GetNombre(Tampon, INTERFACESFORMATCOLONNENumero)), *curInterface);
				//	Base.Erreur("Enregistrement élément d'interface", TXT(curInterface->Index()), Tampon, "");
				break;
			}
			
			case INTERFACESFORMATLIGNEPeriode:
			{
				cPeriodeJournee* __Periode = new cPeriodeJournee;
				if (__Periode->SetValeurs(cHeure() = _Format->Extrait(Tampon, INTERFACESFORMATCOLONNEPeriodeDebut)
								, cHeure() = _Format->Extrait(Tampon, INTERFACESFORMATCOLONNEPeriodeFin)
								, _Format->Extrait(Tampon, INTERFACESFORMATCOLONNEPeriodeLibelle)
								))
					curInterface->AddPeriode(__Periode);
				break;
			}
			
			case INTERFACESFORMATLIGNELibelleJour:
				curInterface->SetLibelleJour(_Format->GetNombre(Tampon, INTERFACESFORMATCOLONNELibelleJourIndex)
							, _Format->Extrait(Tampon, INTERFACESFORMATCOLONNELibelleJourLibelle)
							);
				break;
				
			case INTERFACESFORMATLIGNELibelleMois:
				curInterface->SetLibelleMois(_Format->GetNombre(Tampon, INTERFACESFORMATCOLONNELibelleMoisIndex)
							, _Format->Extrait(Tampon, INTERFACESFORMATCOLONNELibelleMoisLibelle)
							);
				break;

			case INTERFACESFORMATLIGNEPrefixeAlerte:
				curInterface->SetPrefixeAlerte(_Format->GetNombre(Tampon, INTERFACESFORMATCOLONNEPrefixeAlerteNiveau)
					, _Format->Extrait(Tampon, INTERFACESFORMATCOLONNEPrefixeAlerteTexte)
					);
				break;
				
			case -TYPEVide:
				Tampon.Vide();
			case -TYPESousSection:
				PasTermineElement = false;
			}
		}
	}

	return(true);
}



