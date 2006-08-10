#include "cFichierInterfaces.h"

#include "cFichierObjetInterfaceStandard.h"


#include "04_time/HourPeriod.h"
#include "04_time/Hour.h"

using synthese::time::HourPeriod;
using synthese::time::Hour;
using synthese::interfaces::Interface;



/*!	\brief Chargement du fichier d'interface
  \author Hugues Romain
  \date 2002
  \return true si le fichier a été trouvé, false sinon.
*/
bool cFichierInterfaces::Charge(synthese::interfaces::Interface::Registry& interfaces)
{
    // Variables
    tIndex NombreInterfacesReel = 0;
    bool PasTermineSousSection = true;
    bool PasTermineElement=false;

    // Tampon
    cTexte Tampon(TAILLETAMPON, true);
    cTexte Tampon2(TAILLETAMPON, true);
	
    // Objets courants
    Interface* curInterface=NULL;

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
	    curInterface = new Interface (Tampon.GetNombre(0, 1));
	    interfaces.add (curInterface);

	    NombreInterfacesReel++;
	    PasTermineElement = true;
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
		Hour startHour;
		startHour = std::string (_Format->Extrait(Tampon, INTERFACESFORMATCOLONNEPeriodeDebut).Texte ());
		Hour endHour;
		endHour = std::string (_Format->Extrait(Tampon, INTERFACESFORMATCOLONNEPeriodeFin).Texte ());

		HourPeriod* __Periode = new HourPeriod (_Format->Extrait(Tampon, INTERFACESFORMATCOLONNEPeriodeLibelle).Texte (), startHour, endHour);

		curInterface->AddPeriode(__Periode);
		break;
	    }
			
	    case INTERFACESFORMATLIGNELibelleJour:
		curInterface->SetLibelleJour(_Format->GetNombre(Tampon, INTERFACESFORMATCOLONNELibelleJourIndex)
					     , _Format->Extrait(Tampon, INTERFACESFORMATCOLONNELibelleJourLibelle).Texte ()
		    );
		break;
				
	    case INTERFACESFORMATLIGNELibelleMois:
		curInterface->SetLibelleMois(_Format->GetNombre(Tampon, INTERFACESFORMATCOLONNELibelleMoisIndex)
					     , _Format->Extrait(Tampon, INTERFACESFORMATCOLONNELibelleMoisLibelle).Texte ()
		    );
		break;

	    case INTERFACESFORMATLIGNEPrefixeAlerte:
		curInterface->SetPrefixeAlerte(_Format->GetNombre(Tampon, INTERFACESFORMATCOLONNEPrefixeAlerteNiveau)
					       , _Format->Extrait(Tampon, INTERFACESFORMATCOLONNEPrefixeAlerteTexte).Texte ()
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



