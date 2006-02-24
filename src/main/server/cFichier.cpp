/*!	\file cFichier.cpp
	\brief Impl�mentation classe fichier
*/

#define SYNTHESE_CFICHIER_CPP

#include "SYNTHESE.h"
#include "cFichier.h"
#include "cFichiersPourSYNTHESE.h"
#include "cFichierPourEnvironnement.h"
#include "cFichierObjetInterfaceStandard.h"

#include "cInterface_Objet_Element_Parametre_DonneeEnvironnement.h"
#include "cInterface_Objet_Element_Bibliotheque.h"
#include "cInterface_Objet_AEvaluer_PageEcran.h"

#include "cInterface_Objet_Element_Goto.h"
#include "cInterface_Objet_Element_Ligne.h"

#include "cTableauAffichageSelectifDestinations.h"

#include "cArretPhysique.h"
#include "LogicalPlace.h"
#include "cCommune.h"
#include "cPeriodeJournee.h"
#include "cEnvironnement.h"

extern SYNTHESE Synthese;

/*!	\brief Constructeur
	\param CheminFichier Chemin d'acc�s au fichier
*/
cFichier::cFichier(const cTexte& CheminFichier, const cFormatFichier* FormatFichier)
{
	_Chemin << CheminFichier;
	_Format = FormatFichier;
}


/*!	\brief Destructeur
	\author Hugues Romain
	\date 2005
*/
cFichier::~ cFichier()
{
	Fermer();
	delete _Format;
}


/*!	\todo Verifier le fonctionnement de cette fonction
*/
TypeSection cFichier::LireLigne(cTexte& Tampon)
{
	int Car;			// R�ceptionne la valeur de get.
	TypeSection TS = TYPEVide;	// Pas encore de ligne utile rencontr�e.
	tIndex vPosition = 0;

	Tampon.Vide();
	Tampon.setTampon();
	
	while (1)
	{
		// Lecture du premier caract�re de la ligne
		Car = _Fichier.get();

		// Lecture du premier caract�re - Type de section
		switch (Car)
		{
		case -1:
			if (!Tampon.Taille())
				return(TYPEVide);
			else
				return(TS);

		case TYPESection:
		case TYPESousSection:
		case TYPELien:
		case TYPECommentaire:
			TS = Car;
			break;
		default:
			TS = TYPEAutre;
		}

		// D�tection des lignes d'espaces
		while (Car == ' ')
		{
			Tampon.SetElement(' ', vPosition);
			vPosition++;

			Car = _Fichier.get();

			if ((Car == '\n') || (Car == -1) || (Car =='\r'))
			{
				vPosition = 0;
				break;
			}
		}

		// D�tection des caract�res CONTROL-M
		while (Car == '\r')
		{
			Car = _Fichier.get();
		}

		// Lecture de la ligne
		while ((Car != '\n') && (Car != '\r') && (Car != -1))
		{
			if (TS != TYPECommentaire)
			{
				Tampon.SetElement(Car, vPosition);
				vPosition++;
			}
			Car = _Fichier.get();
		}

		// Chaine non nulle
		if (vPosition)
		{
			while (Tampon[vPosition-1] == ' ')
				vPosition--;
			Tampon.Vide(vPosition);
			
			vPosition = 0;
			return(TS);
		}

		// Fin de fichier atteinte sans caract�re stock�
		if (Car == -1)
			return(TYPEVide);
	}
}



/*!	\brief Parcourt le fichier pour obtenir le num�ro maximal d'�l�ment
	\return Le plus grand num�ro d'objet si applicable au format. Il s'agit du plus grand num�ro pr�sent sous la forme [xxxx
	\param Position position du num�ro dans le label (d�faut = 1 : juste apr�s le [. Si Position = INCONNU (-1) alors on ne fait que compter les objets)
*/
int cFichier::NumeroMaxElement(int Position)
{
	int NumeroMax = 0;
	int curNumero;
	TypeSection TS;
	cTexte Tampon;

	ifstream __Fichier;
	__Fichier.open(_Chemin.Texte());
	
//	if (!Ouvrir())
//		return false;
		
	while (true)
	{
		TS = Tampon.LireLigne(__Fichier);
		if (TS == TYPEVide)
			break;
		
		if (TS == TYPESousSection)
		{
			if (Position != INCONNU)
			{
				curNumero = Tampon.GetNombre(6, Position);
				if (curNumero > NumeroMax)
					NumeroMax = curNumero;
			}
			else
				NumeroMax++;
		}
	}	
	__Fichier.close();
	return NumeroMax;
}


bool cFichier::ProchaineSection(cTexte& Tampon, TypeSection TS)
{
	TypeSection tempTS = Tampon.TypeSectionTampon();
	if (tempTS == TYPEVide)
		tempTS = LireLigne(Tampon);

	while (1)
	{
		if (tempTS == TS)
			return true;
		if (tempTS == TYPEVide)
			return false;
		tempTS = LireLigne(Tampon);
	}
}



bool cFichier::RechercheSection(cTexte& Tampon, const cTexte& Intitule, char CarEntreeSection)
{
	while (ProchaineSection(Tampon, CarEntreeSection))
	{
		if (Tampon.Compare(Intitule, 0, 1))
			return true;
		Tampon.Vide();
	}
	return false;
}


/*!	\brief Ouverture du fichier si besoin et positionnement au d�but
*/
bool cFichier::Ouvrir()
{
	if (_Fichier.is_open())
		_Fichier.seekg(0, ios::beg);
	else
		_Fichier.open(_Chemin.Texte());
	return _Fichier.is_open();
}

void cFichier::Fermer()
{
	if (_Fichier.is_open())
		_Fichier.close();
}


/*! 	\brief Chargement du fichier des sites
	\param Base La classe principale de l'application SYNTHESE o� enregistrer les donn�es lues
	\return true si le chargment est fait sans erreur majeure, sinon false
	\warning Le chargement des sites doit intervenir apr�s celui des environnements et des interfaces
	\todo Supprimer les sites mal form�s
*/
bool cFichierSites::Charge()
{
	// Variables locales
	bool PasTermineSousSection = true;
	bool PasTermineElement=false;

	// Tampon
	cTexte Tampon(TAILLETAMPON, true);

	// Objets courants
	cSite* curSite=NULL;

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
			curSite = new cSite(string(
				Tampon.Extrait(_Format->GetColonnePosition(SITESFORMATCOLONNEClef),_Format->GetColonneLargeur(SITESFORMATCOLONNEClef)).Texte()
				));
			if (Synthese.Enregistre(curSite))
				Synthese.FichierLogBoot().Ecrit(MESSAGE_SITES_PBENREG, Tampon);	// Warning ID already in use
			PasTermineElement = true;
		}
		else
			PasTermineSousSection = false;

		// Boucle sur les propri�t�s d'un �l�ment
		while (PasTermineElement)
		{
			switch (LireLigneFormat(Tampon))
			{
				case SITESFORMATLIGNEEnv:
					if (!curSite->SetEnvironnement(Synthese.GetEnvironnement(_Format->GetNombre(Tampon, SITESFORMATCOLONNEStandard))))
						Synthese.FichierLogBoot().Ecrit(MESSAGE_SITES_PBENV, curSite->getClef(), Tampon);
					break;
				
				case SITESFORMATLIGNEInterface:
					if (!curSite->SetInterface(Synthese.GetInterface(_Format->GetNombre(Tampon, SITESFORMATCOLONNEStandard))))
						Synthese.FichierLogBoot().Ecrit(MESSAGE_SITES_PBINT, curSite->getClef(), Tampon);
					break;

				case SITESFORMATLIGNEIdentifiant:
					curSite->setIdentifiant(string(Tampon.Extrait(_Format->GetColonnePosition(SITESFORMATCOLONNEStandard)).Texte()));
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
					curSite->setURLClient(string(_Format->Extrait(Tampon, SITESFORMATCOLONNEURLClient).Texte()));
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
						Base.Erreur("D�finition de l'environnement manquante", curSite->getClef(), "", "08002");
						
					}	
					else if(!curSite->getNumeroInterface())
					{
						Base.Erreur("D�finition de l'interface manquante", curSite->getClef(), "", "08003");
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



/*! 	\brief Chargement du fichier des sites
	\param Base La classe principale de l'application SYNTHESE o� enregistrer les donn�es lues
	\return true si le chargment est fait sans erreur majeure, sinon false
	\warning Le chargement des sites doit intervenir apr�s celui des environnements et des interfaces
	\todo Supprimer les sites mal form�s
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
			string __Code(_Format->Extrait(Tampon, TBDEPFORMATCOLONNECode).Texte());
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
			if (Synthese.Enregistre(__TbDep))
				Synthese.FichierLogBoot().Ecrit(MESSAGE_SITES_PBENREG, Tampon);	// Warning ID already in use
			PasTermineElement = true;
		}
		else
			PasTermineSousSection = false;

		// Boucle sur les propri�t�s d'un �l�ment
		while (PasTermineElement)
		{
			switch (LireLigneFormat(Tampon))
			{
				case TBDEPFORMATLIGNEEnvironnement:
					if (!__TbDep->SetEnvironnement(Synthese.GetEnvironnement(_Format->GetNombre(Tampon, TBDEPFORMATCOLONNEStandard))))
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
				
				case TBDEPFORMATLIGNEArretLogique:
					__TbDep->SetArretLogique(__TbDep->getEnvironnement()->getLogicalPlace(_Format->GetNombre(Tampon, TBDEPFORMATCOLONNEStandard)));
					break;

				case TBDEPFORMATLIGNEArretPhysique:
					__TbDep->AddArretPhysiqueAutorise(_Format->GetNombre(Tampon, TBDEPFORMATCOLONNEStandard));
					break;

				case TBDEPFORMATLIGNEDestinationAffichee:
					__TbDep->AddDestinationAffichee(__TbDep->getEnvironnement()->getLogicalPlace(_Format->GetNombre(Tampon, TBDEPFORMATCOLONNEStandard)));
					break;

				case TBDEPFORMATLIGNEDestinationSelection:
					if (__TbDepSelect != NULL)
					{
						__TbDepSelect->AddDestination(__TbDep->getEnvironnement()->getLogicalPlace(_Format->GetNombre(Tampon, TBDEPFORMATCOLONNEStandard)));
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
						Base.Erreur("D�finition de l'environnement manquante", curSite->getClef(), "", "08002");
						
					}	
					else if(!curSite->getNumeroInterface())
					{
						Base.Erreur("D�finition de l'interface manquante", curSite->getClef(), "", "08003");
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




/*!	\brief Chargement des donn�es environnements
	\return true si le chargment est fait sans erreur majeure, sinon false
*/
bool cFichierEnvironnements::Charge()
{
	// Variables locales
	cTexte					vNomFichierSites;
	bool PasTermineSousSection = true;
	bool PasTermineElement=false;

	// Tampon
	cTexte Tampon(TAILLETAMPON, true);

	// Objets courants
	cEnvironnement* __Environnement=NULL;

	if (!Ouvrir())
	{
// 		Base.Erreur("Impossible d'ouvrir le fichier", _Chemin, "", "");
		return false;
	}

	Tampon.Vide();
	
	// Boucle sur les Sous Sections
	while (PasTermineSousSection)
	{
		tIndex numeroEnv=0;
		if (ProchaineSection(Tampon, TYPESousSection))
		{
			numeroEnv = _Format->GetNombre(Tampon, ENVSFORMATCOLONNENumero);
			PasTermineElement = true;
		}
		else
		{
			PasTermineSousSection = false;
		}

		// Boucle sur les propri�t�s d'un �l�ment
		while (PasTermineElement)
		{
			switch (LireLigneFormat(Tampon))
			{
				case ENVSFORMATLIGNERepertoire:
				{
					__Environnement = new cEnvironnement(
						Tampon.Extrait(_Format->GetColonnePosition(ENVSFORMATLIGNERepertoire)).fAdresseComplete(_Chemin).Copie(SEPARATEUR_REPERTOIRE_TXT).Texte()
						, string(_CheminFichierFormats.Texte())
						, numeroEnv
					);
					Synthese.Enregistre(__Environnement);
					break;
				}

				case -TYPEVide:
					Tampon.Vide();
				case -TYPESousSection:
					PasTermineElement = false;
			}
		}
	}

	Fermer();
	
	return true;
}





/*!	\brief Chargement du fichier d'interface
	\author Hugues Romain
	\date 2002
	\return true si le fichier a �t� trouv�, false sinon.
*/
bool cFichierInterfaces::Charge()
{
	// Variables
	bool PasTermineSousSection = true;
	bool PasTermineElement=false;

	// Tampon
	cTexte Tampon(TAILLETAMPON, true);
	cTexte Tampon2(TAILLETAMPON, true);
	
	// Objets courants
	cInterface* curInterface=NULL;

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
			curInterface = new cInterface(Tampon.GetNombre(0, 1));
			if (Synthese.Enregistre(curInterface))
			{
// 				Base.Erreur("Echec enregistrement interface", _Chemin, Tampon, "");
			}
			PasTermineElement = true;
		}
		else
			PasTermineSousSection = false;

		// Boucle sur les �l�ments d'une interface
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
				//	Base.Erreur("Enregistrement �l�ment d'interface", TXT(curInterface->Index()), Tampon, "");
				break;
			}
			
			case INTERFACESFORMATLIGNEPeriode:
			{
				cPeriodeJournee* __Periode = new cPeriodeJournee(string(_Format->Extrait(Tampon, INTERFACESFORMATCOLONNEPeriodeLibelle).Texte())
								, cHeure() = _Format->Extrait(Tampon, INTERFACESFORMATCOLONNEPeriodeDebut)
								, cHeure() = _Format->Extrait(Tampon, INTERFACESFORMATCOLONNEPeriodeFin)
								);
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




/*! \brief Chargement d'un fichier d'�l�ment d'interface
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


/*!	\brief Fonction de chargement de donn�es de calendriers de circulation
	\return true si le chargement s'est effectu� avec succ�s, false sinon
	\author Hugues Romain
	\date 2001-2005
*/
bool cFichierJoursCirculation::Charge(cEnvironnement* __Environnement)
{
	ifstream		Fichier;
	ifstream		Fichier2;
	cJC*			JCEncours=NULL;
	TypeSection	TS = TYPEVide;
	bool			PasTermine = true;
	cTexte		Tampon(TAILLETAMPON, true);
	

	// Ouverture
	if (!Ouvrir())
	{
// 		__Environnement->Erreur("Impossible d'ouvrir le fichier JC", "", _Chemin, "");
		return false;
	}
	
	// Service
	LireLigne(Tampon);
	if (!Tampon.Compare(JCFORMATLIGNEService,  8)) // A completer
	{
// 		__Environnement->Erreur("Ann�es non document�es.", "", Tampon, "06003");
		Fermer();
		return false;
	}
	//if (Chargement)
	//{
		__Environnement->SetDatesService(Tampon.GetNombre(4, 8), Tampon.GetNombre(4, 13));
	//}
/*	if (vPremiereAnnee > vDerniereAnnee)
	{
		Erreur("Ann�es incorrectes.", "", Tampon, "06004");
		Fichier.close();
		return(false);
	}
*/
	TS = LireLigne(Tampon);
	
	while (PasTermine)
	{
		if (TS == JCFORMATLIGNECalendrier)
		{
			//if (Chargement)
			//{
			JCEncours = new cJC(__Environnement->PremiereAnnee(), __Environnement->DerniereAnnee(), Tampon.GetNombre(4, 1), string(Tampon.Extrait(5).Texte()));
			__Environnement->Enregistre(JCEncours);

			//}
			//else
			//	JCEncours = __Environnement->getJC(NumeroJC);

			TS = RemplirJC(*JCEncours, cJC::InclusionType_POSITIVE, Tampon, __Environnement);
			if (TS==TYPEError)
				return false;
		}
		else
		{
			if (TS == TYPEVide)
				PasTermine=false;

			TS = Tampon.LireLigne(Fichier);
		}
	}

	Fermer();

	return true;
}

TypeSection cFichierJoursCirculation::RemplirJC(cJC& JC, cJC::InclusionType Sens, cTexte& Tampon, cEnvironnement* __Environnement)
{
	while (true)
	{
		if (LireLigne(Tampon) == TYPEVide)
			return TYPEVide;
		else if (Tampon[0]=='[')
			return TYPESousSection;
		else
		{
			if (!JCExecuterCommande(JC, Sens, Tampon, __Environnement))
				return TYPEError;	
		}
	}
}



/*!	\brief Remplissage d'un calendrier de circulation d'apr�s une commande
	\param JC Le calendrier � modifier
	\param Sens Le sens de la modification (Positif ('+') : Les jours d�crits sont ajout�s au calendrier, Negatif ('-') : les jours d�crits sont retir�s du calendrier
	\param Tampon Texte contenant la commande
	\author Hugues Romain
	\date 2001-2005

Lorsque des jours de circulation sont ajout�s, les variables contenant les premi�res et derni�res dates de circulation de l'environnement sont mises � jour le cas �ch�ant (voir cEnvironnement::SetDateMinReelle() et cEnvironnement::SetDateMaxReelle())
*/
bool cFichierJoursCirculation::JCExecuterCommande(cJC& JC, cJC::InclusionType Sens, cTexte& Tampon, cEnvironnement* __Environnement)
{
	cFichierJoursCirculation* Fichier2 = NULL;
	cTexte	 NomFichier2;
	cDate	 Date1;
	cDate	 Date2;
	tJour	 Pas;
	cJC::InclusionType	 CurSens = Sens;

	switch (Tampon[0])
	{
	case JCFORMATLIGNECategorie:
		//if (Chargement)
		JC.setCategorie((cJC::Category) Tampon.GetNombre(3,1));
		break;

	case 'I':
		break;

	case JCFORMATLIGNESuppressionDate:
		CurSens = (Sens == cJC::InclusionType_POSITIVE) ? cJC::InclusionType_NEGATIVE : cJC::InclusionType_POSITIVE;
	
	case JCFORMATLIGNEAjoutDate:
		switch (Tampon[1])
		{
		case JCFORMATLIGNEAjoutCalendrier:
			if (__Environnement->GetJC(Tampon.GetNombre(4,2)))
			{
				//if (Chargement)
					__Environnement->GetJC(Tampon.GetNombre(4,2))->SetInclusionToMasque(JC, CurSens);
				//else
				//	JCUtile[Tampon.GetNombre(4,2)] = true;
			}
			else
			{}
// 				__Environnement->Erreur("Num�ro de JC inclus incorrect", TXT(JC.Index()), Tampon, "06009");
			break;

		case JCFORMATLIGNEAjoutFichier:
			NomFichier2.Copie(Tampon.Extrait(2), false);
			NomFichier2.fAdresseComplete(_Chemin);
			NomFichier2 << JCEXTENSION;
			
			Fichier2 = new cFichierJoursCirculation(NomFichier2);
			
			
			if (!Fichier2->Ouvrir())
			{
// 				__Environnement->Erreur("*** ERREUR Impossible d'ouvrir le fichier", Fichier2->_Chemin, "", "06009");	
				cout << "*** ERREUR Impossible d'ouvrir le fichier" << Fichier2->_Chemin << "\n";
				return false;					
			}
			
			Fichier2->RemplirJC(JC, CurSens, Tampon, __Environnement);
			Fichier2->Fermer();
			break;

		default:
			//if (Chargement)
			//{
				Date1 = Tampon.Extrait(1, 8);
				if (Tampon.Taille() > 9 && Tampon[9] == '/')
				{
					Date2 = Tampon.Extrait(10);
					if (Tampon.Taille() > 18 && Tampon[18] == '/')
						Pas = Tampon.GetNombre(2,19);
					else
						Pas = 1;
					
					if (JC.SetCircule(Date1, Date2, CurSens, Pas))
					{
						// Si ajout de dates, mise � jour �ventuel des premi�res et derni�res dates circul�es
						if (CurSens == cJC::InclusionType_POSITIVE)
						{
							__Environnement->SetDateMinReelle(Date1);
							__Environnement->SetDateMaxReelle(Date2);
						}
					}
					else
					{}
// 						__Environnement->Erreur("Dates incorrectes", TXT(JC.Index()), Tampon, "06007");
					
					
				}
				else
				{
					if (JC.SetCircule(Date1, CurSens))
					{
						// Si ajout de dates, mise � jour �ventuel des premi�res et derni�res dates circul�es
						if (CurSens == cJC::InclusionType_POSITIVE)
						{
							__Environnement->SetDateMinReelle(Date1);
							__Environnement->SetDateMaxReelle(Date1);
						}
					}
					else
					{}
// 						__Environnement->Erreur("Date invalide", TXT(JC.Index()), Tampon, "06006");
				}
			//}
		}
		break;

	default:
// 		__Environnement->Erreur("Commande inconnue", TXT(JC.Index()), Tampon, "06005");
		return false;
	}
	
	return true;
}

/*!	\brief Lecture d'une ligne du fichier et renvoi de l'identificateur du champ
*/
int cFichier::LireLigneFormat(cTexte& Texte)
{
	return _Format->LireFichier(_Fichier, Texte);
}



bool cFichierPointsArret::Charge(cEnvironnement* __Environnement)
{
	// Variables
	bool PasTermineSousSection = true;
	bool PasTermineElement = false;
	tCategorieDistance iCategorieDistance;
	LogicalPlace::tNiveauCorrespondance curNiveauCorrespondance = LogicalPlace::CorrInterdite;;
	// Tampon
	cTexte Tampon(TAILLETAMPON, true);
	cTexte TamponCalcul(TAILLETAMPON, true);

	// Objets courants
	LogicalPlace* curArretLogique = NULL;
	cArretPhysique* curArretPhysique = NULL;
	tIndex curNumeroArretPhysique;
	size_t mainDesignation;

	if (!Ouvrir())
	{
		//cout << "*** ERREUR Impossible d'ouvrir le fichier " << vNomFichierPA.Texte() << "\n";
		return false;
	}

	// Boucle sur les Sous Sections
	while (PasTermineSousSection)
	{
		if (ProchaineSection(Tampon, PAFORMATLIGNEArretLogique))
		{

			PasTermineElement = true;
			switch (Tampon[1])
			{
			case 'C': curNiveauCorrespondance = LogicalPlace::CorrAutorisee;			break;
			case 'Q': curNiveauCorrespondance = LogicalPlace::CorrRecommandeeCourt;	break;
			case 'R': curNiveauCorrespondance = LogicalPlace::CorrRecommandee;		break;
			case 'S': curNiveauCorrespondance = LogicalPlace::CorrInterdite;			break;
			default:
				curNiveauCorrespondance = LogicalPlace::CorrInterdite;
// 				Synthese.FichierLogBoot().Ecrit("Point d'Arr�t mal d�clar�", Tampon);
				//! \todo Standardiser la sortie log
			}

			curArretLogique = new LogicalPlace(Tampon.GetNombre(0,2), curNiveauCorrespondance);
			__Environnement->Enregistre(curArretLogique);

			if (curArretLogique == NULL)
			{
				Tampon.Vide();
				PasTermineElement = false;
			}

			mainDesignation = true;

		}
		else
			PasTermineSousSection = false;

		// Boucle sur les �l�ments d'un PA
		while (PasTermineElement)
		{
			switch (LireLigneFormat(Tampon))
			{
			case PAFORMATLIGNEGpsx:
				curArretLogique->setX(_Format->GetNombre(Tampon, PAFORMATCOLONNEStandard));
				break;

			case PAFORMATLIGNEAlerte:
				{
					cTexte messageAlerte = _Format->ExtraitComplet(Tampon, PAFORMATCOLONNEAlerte);
					curArretLogique->getAlertForSettings().setMessage(string(messageAlerte.Texte()));
					
					break;
				}

			case PAFORMATLIGNEAlerteDebut:
				{
					const cTexte dateDebut = _Format->Extrait(Tampon, PAFORMATCOLONNEDateAlerte);

					cMoment momentDebut;
					if (dateDebut.Taille()<=0 || dateDebut[0]==' ')
						momentDebut.setMoment('m', 'm', __Environnement->PremiereAnnee());
					else
						momentDebut = dateDebut;
					curArretLogique->getAlertForSettings().setMomentDebut(momentDebut);

					break;
				}

			case PAFORMATLIGNEAlerteFin:
				{
					const cTexte dateFin = _Format->Extrait(Tampon, PAFORMATCOLONNEDateAlerte);

					cMoment momentFin;
					if (dateFin.Taille()<=0 || dateFin[0] ==' ')
						momentFin.setMoment('M', 'M', __Environnement->DerniereAnnee());
					else
						momentFin = dateFin;
					curArretLogique->getAlertForSettings().setMomentFin(momentFin);

					break;
				}

			case PAFORMATLIGNEGpsy:
				curArretLogique->setY(_Format->GetNombre(Tampon, PAFORMATCOLONNEStandard));
				break;

			case PAFORMATLIGNEDescription:
				{
					cCommune* commune;

					tIndex Position = Tampon.RechercheOccurenceGauche('(', 1
						, _Format->GetColonnePosition(PAFORMATCOLONNEDesignationLibelle)	);

					int designationType = _Format->GetNombre(Tampon, PAFORMATCOLONNEDesignationType);

 					if (!Position)
					{}
// 						Erreur("D�signation de point d'arr�t mal d�clar�e", TXT(curPA->Index()), Tampon, "02003");
//							Synthese.FichierLogBoot().Ecrit(MESSAGE_GARE_DESIGNATION_INCORRECTE, TXT(curArretLogique->Index()), Tampon);
					else
					{

						// All places virtual stop
						if (designationType == 2)
						{
							/** @todo ARRET TOUT LIEU NON GERE : le signaler avec un autre truc que la désignation */
						}

						// Normal logical stop
						if (designationType == 0 || designationType == 1)
						{
							LogicalPlace* designation;

							string townName = string(
								Tampon.Extrait(
									_Format->GetColonnePosition(PAFORMATCOLONNEDesignationLibelle)
									, Position - _Format->GetColonnePosition(PAFORMATCOLONNEDesignationLibelle)
								).Texte());
							string name = string(Tampon.Extrait(Position+1).Texte());

							if (!(commune = __Environnement->getTown(townName)))
							{
								commune = new cCommune(-1, townName);
								__Environnement->addTown(commune);
							}

							if (mainDesignation)
							{
								designation = curArretLogique;
								mainDesignation = false;
							}
							else
								designation = new LogicalPlace(curArretLogique);
							
							designation->setDesignation(commune, name);
						
							// Main stop in town
							if (designationType == 1)
							{
								commune->addToMainLogicalPlace(designation);
							}
						}
					}
				}
				break;

			case PAFORMATLIGNEHtml:
				//curPA->vHTML << Tampon+PositionColonne;
				break;

			case PAFORMATLIGNEArretPhysique:
				// Cr�ation du quai
				curNumeroArretPhysique = (tIndex) _Format->GetNombre(Tampon, PAFORMATCOLONNEIndexArretPhysique);
				curArretPhysique = new cArretPhysique(curArretLogique, curNumeroArretPhysique);
				
				// Matrice des correspondances et position du curseur de texte
				int Position;
				if (curArretLogique->CorrespondanceAutorisee() != LogicalPlace::CorrInterdite)
				{
					// Ecriture de chaque d�lai de correspondance
					for (size_t i=0; i<curArretLogique->getNetworkAccessPoints().size(); ++i)
						curArretLogique->setDelaiCorrespondance(curNumeroArretPhysique, i
							, _Format->GetNombre(Tampon, PAFORMATCOLONNEDelaiCorrespondance, i-1)	);
						
					// Position du pointeur apr�s la matrice de d�lais de correspondances
					Position = _Format->GetColonnePosition(PAFORMATCOLONNEDelaiCorrespondance, curArretLogique->getNetworkAccessPoints().size());
				}
				else // Sinon position du pointeur imm�diatement apr�s le num�ro du quai
					Position = _Format->GetColonnePosition(PAFORMATCOLONNEDelaiCorrespondance, 0);
				
				// Interpr�tation de la d�signation du quai
				/*!	\todo Int�grer ce type d'interpr�tation lin�aire dans une description type XML assist�e par un fichier de format
						Etape indispensable pour augmenter le type d'informations li�es au quai, notamment si longueur variable
				*/
				for(; Position < Tampon.Taille(); Position++)
				{
					if (Tampon[Position]=='#')
					{
						if (Tampon.Compare("GX", 2, Position+1)) // Coordonn�es X
						{
							curArretPhysique->setX((CoordonneeKMM) Tampon.GetNombre(7, Position+3));
							Position += 9;
						}
						if (Tampon.Compare("GY", 2, Position+1)) // Coordonn�es Y
						{
							curArretPhysique->setY((CoordonneeKMM) Tampon.GetNombre(7, Position+3));
							Position += 9;
						}
/*						if (Tampon.Compare("PH", 2, Position+1)) // Photo
						{
							curArretPhysique->setPhoto((cPhoto*) __Environnement->GetDocument(Tampon.GetNombre(6, Position+3)));
							Position += 9;
						}
						if (Tampon.Compare("APS", 3, Position+1)) // Compatibilit� ancienne donn�e (ignor�e)
						{
							Position += 9;
						}
						if (Tampon.Compare("RET", 3, Position+1)) // Compatibilit� ancienne donn�e (ignor�e)
						{
							Position += 6;
						}
*/					}
					else
					{
						// Le reste du texte correspond au nom du quai
						curArretPhysique->setNom(string(Tampon.Extrait(Position,1).Texte()));
					}
					curArretLogique->addNetworkAccessPoint(curArretPhysique, curNumeroArretPhysique);
				}
				break;

			case PAFORMATLIGNEPostScriptOD:
				curArretLogique->setDesignationOD(string(Tampon.Extrait(_Format->GetColonnePosition(PAFORMATCOLONNEStandard)).Texte()));
				break;

/*			case PAFORMATLIGNEPhoto:
				curArretLogique->addPhoto((cPhoto*) __Environnement->GetDocument(_Format->GetNombre(Tampon, PAFORMATCOLONNEStandard)));
				break;

			case PAFORMATLIGNEVMax:
				for (iCategorieDistance=0; iCategorieDistance!=NOMBREVMAX; iCategorieDistance++)
					curArretLogique->setVMax(iCategorieDistance
						, (tVitesseKMH) _Format->GetNombre(Tampon, PAFORMATCOLONNEVMax, iCategorieDistance)	);
				break;

			case PAFORMATLIGNENombreServices:
				curArretLogique->declServices(_Format->GetNombre(Tampon, PAFORMATCOLONNEStandard));
				break;

			case PAFORMATLIGNEService:
				curArretLogique->addService(Tampon[_Format->GetColonnePosition(PAFORMATCOLONNEServiceType)]
				, (cPhoto*) __Environnement->GetDocument(_Format->GetNombre(Tampon, PAFORMATCOLONNEServicePhoto))
					, _Format->Extrait(Tampon, PAFORMATCOLONNEServiceDesignation)	);
				break;
*/
			case PAFORMATLIGNEDesignation13:	//20 / D13
				curArretLogique->setDesignation13(string(Tampon.Extrait(_Format->GetColonnePosition(PAFORMATCOLONNEStandard)).Texte()));
				break;

			case PAFORMATLIGNEDesignation26:	//21 / D26
				curArretLogique->setDesignation26(string(Tampon.Extrait(_Format->GetColonnePosition(PAFORMATCOLONNEStandard)).Texte()));
				break;

			case -TYPEVide:
				Tampon.Vide();
			case -TYPESousSection:
				PasTermineElement = false;
// 						Erreur("ArretPhysique manquant", TXT(curPA->Index()), TXT(""), "02006");
			}
		}
	}

	Fermer();

	return true;
}

tIndex cFichier::NombreElementsAAllouer()
{
	return NumeroMaxElement() + 1;
}

bool TimeTablesFile::load()
{
		// Variables
	bool PasTermineSousSection = true;
	bool PasTermineElement=false;
	cEnvironnement* environment = NULL;
	cJC* JCBase = NULL;
	// Tampon
	cTexte Tampon(TAILLETAMPON, true);
	// Objets courants
	cIndicateurs* curIndicateur=NULL;

	// Ouverture du fichier
	ifstream Fichier;
	cTexte NomFichierComplet;
	NomFichierComplet << _Chemin << INDICATEURSEXTENSION;
	Fichier.open(NomFichierComplet.Texte());

	//SET PORTAGE LINUX
	if (!Fichier.is_open())
	{
		cout << "*** ERREUR Impossible d'ouvrir le fichier " << NomFichierComplet.Texte() << "\n";
		return false;
	}
	//END PORTAGE LINUX

	Tampon.LireLigne(Fichier);
	if (Tampon.Compare("ENV=", 4))
	{
		environment = Synthese.GetEnvironnement(Tampon.GetNombre(0,4));
	}
	if (!environment)
	{
		Fichier.close();
		return(false);
	}

	// Nombre d'�l�ments
	Tampon.LireLigne(Fichier);
	if (Tampon.Compare("JC=", 3))
	{
		JCBase = environment->GetJC(Tampon.GetNombre(0, 3));
	}
	if (!JCBase)
	{
		Fichier.close();
		return(false);
	}
// @todo RAJOUTER SECTION = LIVRE INDICATEUR, SOuS SECTION = TABLEAU
/*	// Boucle sur les Sous Sections
	while (PasTermineSousSection)
	{
		if (Tampon.ProchaineSection(Fichier, TYPESousSection))
		{
			curIndicateur = new cIndicateurs(Tampon.Extrait(1), environment);
			Enregistre(curIndicateur);
			PasTermineElement = true;
		}
		else
		{
			PasTermineElement = false;
			PasTermineSousSection = false;
		}

		// Boucle sur les propri�t�s d'un �l�ment
		while (PasTermineElement)
		{
			switch (_Format.LireFichier(Fichier, Tampon))
			{
			case INDICATEURSFORMATLIGNECP:
				curIndicateur->setCommencePage(true);
				break;

			case INDICATEURSFORMATLIGNEJC:
				curIndicateur->setJC(*GetJC(_Format.GetNombre(Tampon, INDICATEURSFORMATCOLONNEStandard)), *JCBase);
				break;

			case INDICATEURSFORMATLIGNEGare:
				curIndicateur->addArretLogique(
					getLogicalPlace(vFormatIndicateurs.GetNombre(_Format, INDICATEURSFORMATCOLONNEStandard)),
					(cGareLigne::tTypeGareLigneDA) Tampon[_Format.GetColonnePosition(INDICATEURSFORMATCOLONNEDepartArrivee)],
					(tTypeGareIndicateur) Tampon[_Format.GetColonnePosition(INDICATEURSFORMATCOLONNEObligatoire)]
					);
				break;

			//case INDICATEURSFORMATLIGNETxt:


			case -TYPEVide:
				Tampon.Vide();
			case -TYPESousSection:
				PasTermineElement = false;
			}
		}
	}

*/	Fichier.close();

	return(true);

}