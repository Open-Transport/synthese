#include "cFichierSites.h"



/*! 	\brief Chargement du fichier des sites
	\param Base La classe principale de l'application SYNTHESE où enregistrer les données lues
	\return true si le chargment est fait sans erreur majeure, sinon false
	\warning Le chargement des sites doit intervenir après celui des environnements et des interfaces
	\todo Supprimer les sites mal formés
*/
bool cFichierSites::Charge()
{
	// Variables locales
	tIndex NombreSitesCharges = 0;
	bool PasTermineSousSection = true;
	bool PasTermineElement=false;

	// Tampon
	cTexte Tampon(TAILLETAMPON, true);

	// Objets courants
	cSite* curSite=NULL;

	// Allocation immédiate de l'espace suffisant
	if (!Synthese.TableauSites().AlloueSiBesoin(NumeroMaxElement()+1))
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
			cTexte clefSite = Tampon.Extrait(_Format->GetColonnePosition(SITESFORMATCOLONNEClef),_Format->GetColonneLargeur(SITESFORMATCOLONNEClef));
			curSite = new cSite(clefSite);
			if (Synthese.Enregistre(curSite) == INCONNU)
			{
				Synthese.FichierLogBoot().Ecrit(MESSAGE_SITES_PBENREG, Tampon);
				Tampon.Vide();
			}
			else
			{
				PasTermineElement = true;
				NombreSitesCharges++;
			}
		}
		else
			PasTermineSousSection = false;

		// Boucle sur les propriétés d'un élément
		while (PasTermineElement)
		{
			switch (LireLigneFormat(Tampon))
			{
				case SITESFORMATLIGNEEnv:
					if (!curSite->SetEnvironnement(*Synthese.TableauEnvironnements().GetElement(_Format->GetNombre(Tampon, SITESFORMATCOLONNEStandard))))
						Synthese.FichierLogBoot().Ecrit(MESSAGE_SITES_PBENV, curSite->getClef(), Tampon);
					break;
				
				case SITESFORMATLIGNEInterface:
					if (!curSite->SetInterface(Synthese.GetInterface(_Format->GetNombre(Tampon, SITESFORMATCOLONNEStandard))))
						Synthese.FichierLogBoot().Ecrit(MESSAGE_SITES_PBINT, curSite->getClef(), Tampon);
					break;

				case SITESFORMATLIGNEIdentifiant:
					curSite->setIdentifiant(Tampon.Extrait(_Format->GetColonnePosition(SITESFORMATCOLONNEStandard)));
					break;
				
				case SITESFORMATLIGNEDateDebut:
				{
					const cTexte txtDate = _Format->Extrait(Tampon, SITESFORMATCOLONNEDate);

					cDate dateDebut;
					//si date non valide, on ne permet pas le site
					if (txtDate.Taille()<=0 || txtDate[0]==' ')
						dateDebut.setDate(TEMPS_MAX);
					else
						dateDebut = txtDate;

					curSite->setDateDebut(dateDebut);

					break;
				}

				case SITESFORMATLIGNEDateFin:
				{
					const cTexte txtDate = _Format->Extrait(Tampon, SITESFORMATCOLONNEDate);

					cDate dateFin;
					if (txtDate.Taille()<=0 || txtDate[0]==' ')
						dateFin.setDate(TEMPS_MIN);
					else
						dateFin = txtDate;

					curSite->setDateFin(dateFin);

					break;
				}
				
				case SITESFORMATLIGNEResaEnLigne:
					curSite->setResaEnLigne(_Format->GetNombre(Tampon, SITESFORMATCOLONNEResaEnLigne) > 0);
					break;

				case SITESFORMATLIGNEURLClient:
					curSite->setURLClient(_Format->Extrait(Tampon, SITESFORMATCOLONNEURLClient));
					break;

				case SITESFORMATLIGNESolutionsPassees:
					curSite->setSolutionsPassees(_Format->GetNombre(Tampon, SITESFORMATCOLONNESolutionsPassees) > 0);
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


