/*! \file Environnement.cpp
	\brief Impl�mentation classe cEnvironnement : chargement de tous les fichiers
	\author Hugues Romain
	\date 2000-2001
*/

#include "cEnvironnement.h"
#include "cFormatFichier.h"
#include "cModaliteReservationEnLigne.h"
#include <string.h>
#include "cFichierXML.h"
#include "cCommune.h"

#include <iostream>



#ifdef UNIX
#include <pthread.h>
pthread_mutex_t mutex_environnement = PTHREAD_MUTEX_INITIALIZER;
#endif

/** Constructeur.
	\param __NombreCalculateurs Nombre d'espaces de calcul � allouer
	\author Hugues Romain
	\date 2000-2001

Les objets environnement sont initialis�s par les valeurs par d�faut suivantes :
 - Tableau des calendriers de circulation vide
 - Plage de date de couverture de l'environnement de taille nulle

Les �l�ments suivants sont d'ores et d�j� initialis�s :
 - Cr�ation d'espaces de calcul pour threads.
*/
cEnvironnement::cEnvironnement(int __NombreCalculateurs)
{
	// Plage de date de taille nulle
	vDateMin.setDate(TEMPS_MAX);
	vDateMax.setDate(TEMPS_MIN);
	
	// Cr�ation des espaces de calcul
	_NombreCalculateurs = __NombreCalculateurs;
	_Calculateur = new cCalculateur[_NombreCalculateurs];
}



/*!	\brief D�signation d'un espace de calcul libre d'utilisation pour un thread
	\return Pointeur vers l'espace de calcul d�sign�, NULL si aucun disponible
	\author Hugues Romain
	\date 2005
*/
cCalculateur* cEnvironnement::CalculateurLibre()
{
	// Parcours de la liste des espaces de calcul de l'environnement
	for (tIndex __Index = 0; __Index < _NombreCalculateurs; __Index++)
		if (_Calculateur[__Index].Prend())
			return &_Calculateur[__Index];
	
	// Retour echec
	return NULL;
}



/*!	\brief Chargement de toutes les donn�es � partir des fichiers
	\param __Chemin Partie commune des noms de fichiers de donn�es pr�sents dans la racine de l'environnement (laisser le / terminal du r�pertoire si pas de nom de fichier). A cette partie commune se rajoutent les extensions de chaque type de fichier
	\param __CheminFormats Chemin d'acc�s au fichier des formats de fichiers (habituellement __Chemin/../formats.per)
	\return true Si le chargement a �t� effectu� avec succ�s
	\author Hugues Romain
	\date 2000-2005
*/
bool cEnvironnement::Charge(const cTexte& __Chemin, const cTexte& __CheminFormats)
{
	cTexte __NomFichier;
	
	// Chargement des photos
	vNomFichierPhotos << __Chemin << PHEXTENSION;
	_FormatPhoto = new cFormatFichier(__CheminFormats, PHFORMAT, PHFORMATLIGNENombreFormats, PHFORMATCOLONNENombreFormats);
	if (!_FormatPhoto || !ChargeFichierPhotos())
	{
		envOk=false;
		return false;
	}

	// Chargement fichier des points d'arr�t
	__NomFichier = __Chemin;
	__NomFichier << PAEXTENSION;
	cFichierPointsArret __FichierPA(__NomFichier, __CheminFormats);
	if (!__FichierPA.Charge(this))
	{
		envOk=false;
		return false;
	}

	// Chargement fichier mat�riel
	vNomFichierMateriel << __Chemin << MATERIELSEXTENSION;
	_FormatMateriel = new cFormatFichier(__CheminFormats, MATERIELSFORMAT, MATERIELSFORMATLIGNENombreFormats, MATERIELSFORMATCOLONNESNombreFormats);
	if (!_FormatMateriel || !ChargeFichierMateriel())
	{
		envOk=false;
		return false;
	} 

	// Chargement fichier des jours de circulation
	__NomFichier = __Chemin;
	__NomFichier << JCFILEEXTENSION;
	cFichierJoursCirculation __FichierJC(__NomFichier);
	if (!__FichierJC.Charge(this))
	{
		envOk=false;
		return false;
	}

	// Chargement fichier des modalit�s de r�servation
	vNomFichierResa << __Chemin << RESASEXTENSION;
	_FormatResa = new cFormatFichier(__CheminFormats, RESASFORMAT, RESASFORMATLIGNENombreFormats, RESASFORMATCOLONNESNombreFormats);
	if (!_FormatResa || !ChargeFichierResa())
	{
		envOk=false;
		return false;
	}

	// Chargement fichier des prises en charges des v�los
	vNomFichierVelo << __Chemin << VELOSEXTENSION;
	_FormatVelo = new cFormatFichier(__CheminFormats, VELOSFORMAT, VELOSFORMATLIGNENombreFormats, VELOSFORMATCOLONNESNombreFormats);
	if (!_FormatVelo || !ChargeFichierVelo())
	{
		envOk=false;
		return false;
	}

	// Chargement fichier des prises en charges des personnes handicap�es
	vNomFichierHandicape << __Chemin << HANDICAPESEXTENSION;
	_FormatHandicape = new cFormatFichier(__CheminFormats, HANDICAPESFORMAT, HANDICAPESFORMATLIGNENombreFormats, HANDICAPESFORMATCOLONNESNombreFormats);
	if (!_FormatHandicape || !ChargeFichierHandicape())
	{
		envOk=false;
		return false;
	}

	// Chargement fichier des tarifications
	vNomFichierTarif << __Chemin << TARIFSEXTENSION;
	_FormatTarif = new cFormatFichier(__CheminFormats, TARIFSFORMAT, TARIFSFORMATLIGNENombreFormats, TARIFSFORMATCOLONNESNombreFormats);
	if (!_FormatTarif || !ChargeFichierTarif())
	{
		envOk=false;
		return false;
	}


	// Chargement fichier des r�seaux
	vNomFichierReseaux << __Chemin << RESEAUXEXTENSION;
	_FormatReseaux = new cFormatFichier(__CheminFormats, RESEAUXFORMAT, RESEAUXFORMATLIGNENombreFormats, RESEAUXFORMATCOLONNESNombreFormats);
	if (!_FormatReseaux	|| !ChargeFichierReseaux())
	{
		envOk=false;
		return false;
	}

	// Chargement fichier des horaires
	vPremiereLigne = NULL;
	_FormatHoraire = new cFormatFichier(__CheminFormats, HORAIRESFORMAT, HORAIRESFORMATLIGNENombreFormats, HORAIRESFORMATCOLONNENombreFormats);
	if (!_FormatHoraire	|| !ChargeFichierHoraires(__Chemin))
	{
		envOk=false;
		return false;
	}

	RemplitProchainAxe();
	RemplitCIL();

	// Chargement format Indicateurs
	_FormatIndicateurs = new cFormatFichier(__CheminFormats, INDICATEURSFORMAT, INDICATEURSFORMATLIGNENombreFormats, INDICATEURSFORMATCOLONNENombreFormats);
	if (!_FormatIndicateurs)
	{
		envOk = false;
		return false;
	}
	
	// Chargement des donnees carto
	vNomRepertoireCarto = std::string (__Chemin.Texte());
	vNomRepertoireCarto += std::string (CARTOEXTENSION);
    
	ChargeFichiersRoutes();
	
    
	// Mise � disposition des calculateurs
	for (int __i = 0; __i < _NombreCalculateurs; __i++)
  		_Calculateur[__i].setEnvironnement(this);

	return true;
}



/*! \brief Destructeur
	\author Hugues Romain
	\date 2000-2001
*/
cEnvironnement::~cEnvironnement()
{
	delete[] _Calculateur;

	for(std::vector<cModaliteReservation*>::iterator iter = vResa.begin(); 
		iter != vResa.end(); ++iter) {
	  if (*iter == 0) continue;
	  delete (*iter);
	}
	vResa.clear();
	
	for(std::vector<cReseau*>::iterator iter = vReseau.begin(); 
		iter != vReseau.end(); ++iter) {
	  if (*iter == 0) continue;
	  delete (*iter);
	}
	vReseau.clear();
	
	for(std::vector<cVelo*>::iterator iter = vVelo.begin(); 
		iter != vVelo.end(); ++iter) {
	  if (*iter == 0) continue;
	  delete (*iter);
	}
	vVelo.clear();
	
	for(std::vector<cHandicape*>::iterator iter = vHandicape.begin(); 
		iter != vHandicape.end(); ++iter) {
	  if (*iter == 0) continue;
	  delete (*iter);
	}
	vHandicape.clear();
	
	for(std::vector<cTarif*>::iterator iter = vTarif.begin(); 
		iter != vTarif.end(); ++iter) {
	  if (*iter == 0) continue;
	  delete (*iter);
	}
	vTarif.clear();
	
	for(std::vector<cMateriel*>::iterator iter = vMateriel.begin(); 
		iter != vMateriel.end(); ++iter) {
	  if (*iter == 0) continue;
	  delete (*iter);
	}
	vMateriel.clear();
	
	free(vIndicateurs);
	free(vPremiereLigne);
}



/*!	Recherche de commune par nom.
	\param name Nom a rechercher
	\param n Nombre de communes � retourner
	\return Liste de communes pouvant correspondre
	
	@todo HR terminer l'implémentation
*/
vector<cCommune*> cEnvironnement::searchTown(const std::string& name, size_t n) const
{
	// Recherche
	vector<Interpretor::Result> matches = _towns.search(name, n);

	// METTRE ICI UNE DISCUSSION SUR L'AMBIGUITE AVEC CRITERES METIER

	// Sortie
	vector<cCommune*> result;
	for (size_t i=0; i<matches.size(); i++)
		result.push_back(getCommune(matches[i]));
	return result;
}



/*!	Recherche de commune dans l'environnement avec création si non trouvée.
	\param Entree Nom de la commune � trouver
	\return La commune trouv�e ou cr��e
	\author Hugues Romain
	\date 2001-2006
*/
cCommune* cEnvironnement::GetCommuneAvecCreation(const std::string& name)
{
	for (Interpretor::Index i=0; i<_towns.size(); i++)
		if (getCommune(i)->getName() == name)
			return getCommune(i);

	return addTown(new cCommune(INCONNU, name));
}



/*!	\brief Accesseur point d'arr�t, avec cr�ation de point d'arr�t si index non existant
	\param __NiveauCorrespondance Type de correspondances autoris�es � l'arr�t
	\param __Index Num�ro de point d'arr�t
	\return Pointeur sur l'objet trouv� ou cr��
*/
LogicalPlace* cEnvironnement::GetGareAvecCreation(tNiveauCorrespondance __NiveauCorrespondance, tIndex __Index)
{
	// Recherche arr�t existant
	if (__Index != INCONNU || !GetArretLogique(__Index))
	{		
		// Correction �ventuelle de l'index
		if (__Index < 0)
			__Index = INCONNU;

		_ArretLogique.SetElement(new LogicalPlace(__Index, __NiveauCorrespondance), __Index);
	}

	return getArretLogique(__Index);
}




/*
cAccesPADe** cEnvironnement::CommuneTextToPA(tNumeroCommune NumeroCommune, char* Entree, size_t n)
{
	// Variables
	cCommune* curCommune = Commune[NumeroCommune];
	cAccesPADe** tbPADe = (cAccesPADe**) malloc((n+1)*sizeof(cAccesPADe*));
	size_t nbPA;
	size_t j;
	char* Tampon = (char*) malloc(TAILLETAMPON*sizeof(char));
	for (nbPA = 0; nbPA<=n; nbPA++)
		tbPADe[nbPA] = NULL;
	nbPA=0;

	// Pas d'entr�e = liste de tous les PA
	if (Entree[0]==0)
		for (cAccesPADe* curAccesPADe=curCommune->PremiereGA; curAccesPADe!=NULL; curAccesPADe=curAccesPADe->Suivant)
		{
			tbPADe[nbPA] = curAccesPADe;
			nbPA++;
			if (nbPA == n)
				break;
		}
	else // Entree: liste des PA correspondant
		for (size_t LongueurComparaison=strlen(Entree); LongueurComparaison>0; LongueurComparaison--)
		{
			for (cAccesPADe* curAccesPADe=curCommune->PremiereGA; curAccesPADe!=NULL; curAccesPADe=curAccesPADe->Suivant)
				if (CompareChaine(Entree, curAccesPADe->Nom(), LongueurComparaison))
				{
					for (j=0; j<nbPA; j++)
						if (tbPADe[j] == curAccesPADe)
							break;
					if (j == nbPA)
					{
						tbPADe[nbPA] = curAccesPADe;
						nbPA++;
						if (strlen(curAccesPADe->Nom()) == LongueurComparaison)
							return(tbPADe);
					}

					if (nbPA == n)
						return(tbPADe);
				}
			if ((nbPA == 1) && (LongueurComparaison == strlen(Entree)))
				return(tbPADe);
		}
	return(tbPADe);
}
*/
/*----------------*
 |                |
 |    Horaires    |
 |                |
 *----------------*/

 
/** Chargement d'un fichier d'horaires
*/	
bool cEnvironnement::ChargeFichierHoraires(const cTexte& NomFichier)
{
	// Variables
	bool PasTermineSection = true;
	bool PasTermineSousSection;
	bool PasTermineElement;
	int nAxes=0;

	// Ouverture du fichier
	ifstream Fichier;
	cTexte NomFichierComplet(NomFichier.Taille() + strlen(HORAIRESEXTENSION)); // A conserver pour les axes
	NomFichierComplet << NomFichier << HORAIRESEXTENSION;
	Fichier.open(NomFichierComplet.Texte());

	if (!Fichier.is_open())
	{
// 		vFichierLOG << "*** ERREUR Impossible d'ouvrir le fichier " << NomFichierComplet.Texte() << "\n";
		return false;
	}

	// Tampon
	cTexte Tampon(TAILLETAMPON, true);
	cTexte TamponAtt(TAILLETAMPON, true);
	cTexte TamponDernier(TAILLETAMPON, true);

	// Objets courants
	cGareLigne*		PileGLSansHoraires[NOMBREGARESPILE];
	cGareLigne*		DerniereGLAvecHoraires=NULL;
	int				TaillePileGLSansHoraire=0;
	cAxe*			curAxe=NULL;
	cLigne*			curLigne=NULL;
	cGareLigne*		curGareLigne=NULL;
	LogicalPlace*			curArretLogique=NULL;
	tIndex			NumeroArretLogiquePrecedent=0;
	bool				DeuxiemePassage = false;
	tBool3			PHCroissants=Indifferent;	//!< Inconnu
	cGareLigne*		lastGareLigne=NULL;
	tIndex		curNumeroVoie;
	tVitesseKMH		Vitesse;
	tDistanceHM		Distance;
	int				TailleTotalePileGLSansHoraire;
	bool				DernierPAHorairesSaisis=false;
	tNumeroService		iService; //, NombreServices;
	cHoraire			HeureDernier;
	cHoraire			HeurePremier;
	cFormatFichier&	vFormatHoraire = *_FormatHoraire;



	// Liens autoris�s en en-t�te uniquement
	while (Tampon.LireLigne(Fichier) == HORAIRESFORMATLIGNELien)
	{
		if (!ChargeFichierHoraires(Tampon.Extrait(1).fAdresseComplete(NomFichier).Copie(".")))
		{
			Fichier.close();
			return false;
		}
	}

	// Boucle sur les axes d'un fichier
	while (PasTermineSection)
	{
		if (Tampon.ProchaineSection(Fichier, HORAIRESFORMATLIGNEAxe) && Tampon.Taille() > 1)
		{
			switch (Tampon[1])
			{
			case '#':
 				curAxe = new cAxe(true, Tampon.Extrait(2), NomFichierComplet);
				break;

			case '�':
     			curAxe = new cAxe(false, Tampon.Extrait(2), NomFichierComplet, false);
				break;

			default:
 				curAxe = new cAxe(false, Tampon.Extrait(1), NomFichierComplet);
			}

			nAxes++;
			PasTermineSousSection=true;
		}
		else
		{
			PasTermineSection = false;
			PasTermineSousSection = false;
		}

		// Boucle sur les lignes d'un axe
		while (PasTermineSousSection)
		{
			if (Tampon.ProchaineSection(Fichier, HORAIRESFORMATLIGNELigne))
			{
				curLigne = new cLigne(Tampon.Extrait(1), curAxe);
				Enregistre(curLigne);

				TaillePileGLSansHoraire = 0;
				DerniereGLAvecHoraires = NULL;
				PHCroissants = Indifferent;
				lastGareLigne = NULL;
				PasTermineElement = true;
			}
			else
			{
				PasTermineElement = false;
				PasTermineSousSection = false;
			}
			
			
			
			
			
			
			// SET : Correction du probleme de coredump au demarrage
			// il faut verifier que chaque ligne a des horaires definis,sinon on sort en erreur
			DernierPAHorairesSaisis=false;
			
			// Boucle sur les �l�ments d'une ligne (de bus!!! ne pas confondre avec ligne du fichier)
			while (PasTermineElement)
			{
				switch (vFormatHoraire.LireFichier(Fichier, Tampon))
				{
				case HORAIRESFORMATLIGNEGirouette:
					curLigne->setGirouette(vFormatHoraire.ExtraitComplet(Tampon, HORAIRESFORMATCOLONNEHoraire));
					break;
					
/*				case HORAIRESFORMATLIGNEDocumentation:
					curLigne->setDocumentation(vFormatHoraire.Extrait(Tampon, HORAIRESFORMATCOLONNEHoraire));
					break;
					*/
					

				case HORAIRESFORMATLIGNEJoursCirculationLigne:
					break;

				case HORAIRESFORMATLIGNEReseau:
					curLigne->setReseau(vReseau[vFormatHoraire.GetNombre(Tampon, HORAIRESFORMATCOLONNEHoraire)]);
					break;

				case HORAIRESFORMATLIGNECartouche:
					curLigne->setLibelleSimple(vFormatHoraire.ExtraitComplet(Tampon, HORAIRESFORMATCOLONNEHoraire));
					break;

				case HORAIRESFORMATLIGNEStyle:
					curLigne->setStyle(vFormatHoraire.ExtraitComplet(Tampon, HORAIRESFORMATCOLONNEHoraire));
					break;

				case HORAIRESFORMATLIGNEImage:
					curLigne->setImage(vFormatHoraire.ExtraitComplet(Tampon, HORAIRESFORMATCOLONNEHoraire));
					break;

				case HORAIRESFORMATLIGNELibelleComplet:
					curLigne->setLibelleComplet(vFormatHoraire.ExtraitComplet(Tampon, HORAIRESFORMATCOLONNEHoraire));
					break;

				case HORAIRESFORMATLIGNEMateriel:
					curLigne->setMateriel(vMateriel[vFormatHoraire.GetNombre(Tampon, HORAIRESFORMATCOLONNEHoraire)]);
					break;

				case HORAIRESFORMATLIGNEResa:
					curLigne->setResa(vResa[vFormatHoraire.GetNombre(Tampon, HORAIRESFORMATCOLONNEHoraire)]);
					break;

				case HORAIRESFORMATLIGNEVelo:
					curLigne->setVelo(vVelo[vFormatHoraire.GetNombre(Tampon, HORAIRESFORMATCOLONNEHoraire)]);
					break;

				case HORAIRESFORMATLIGNEHandicape:
					curLigne->setHandicape(vHandicape[vFormatHoraire.GetNombre(Tampon, HORAIRESFORMATCOLONNEHoraire)]);
					break;

				case HORAIRESFORMATLIGNETarification:
				{
					if (vFormatHoraire.GetNombre(Tampon, HORAIRESFORMATCOLONNEHoraire) >= vNombreTarif)
					{
// 						Erreur("Tarif inexistant", "", Tampon, "03012");
					}
					else
					{
						curLigne->setTarif(vTarif[vFormatHoraire.GetNombre(Tampon, HORAIRESFORMATCOLONNEHoraire)]);
					}
					break;
				}

				case HORAIRESFORMATLIGNEAlerte:
				{
					cTexte messageAlerte = vFormatHoraire.ExtraitComplet(Tampon, HORAIRESFORMATCOLONNEAlerte);
					curLigne->setAlerteMessage(messageAlerte);

					break;
				}

				case HORAIRESFORMATLIGNEAlerteDebut:
				{
					const cTexte dateDebut = vFormatHoraire.Extrait(Tampon, HORAIRESFORMATCOLONNEDateAlerte);

					cMoment momentDebut;
					if (dateDebut.Taille()<=0 || dateDebut[0]==' ')
						momentDebut.setMoment('m', 'm', this->PremiereAnnee());
					else
						momentDebut = dateDebut;
					curLigne->setAlerteDebut(momentDebut);

					break;
				}

				case HORAIRESFORMATLIGNEAlerteFin:
				{
					const cTexte dateFin = vFormatHoraire.Extrait(Tampon, HORAIRESFORMATCOLONNEDateAlerte);

					cMoment momentFin;
					if (dateFin.Taille()<=0 || dateFin[0] ==' ')
						momentFin.setMoment('M', 'M', this->DerniereAnnee());
					else
						momentFin = dateFin;
					curLigne->setAlerteFin(momentFin);

					break;
				}

				case HORAIRESFORMATLIGNECodeIndicateur:
					curLigne->setNomPourIndicateur(vFormatHoraire.ExtraitComplet(Tampon, HORAIRESFORMATCOLONNEHoraire));
					break;

				case HORAIRESFORMATLIGNEAAfficher:
					curLigne->setAAfficherSurTableauDeparts(vFormatHoraire.GetNombre(Tampon, HORAIRESFORMATCOLONNEHoraire) == 1);
					break;

				case HORAIRESFORMATLIGNEAffIndicateurs:
					curLigne->setAAfficherSurIndicateurs(vFormatHoraire.GetNombre(Tampon, HORAIRESFORMATCOLONNEHoraire) == 1);
					break;

				case HORAIRESFORMATLIGNEJoursCirculationServices:
					for (iService=0; iService!=curLigne->NombreServices(vFormatHoraire.Longueur(Tampon, HORAIRESFORMATCOLONNEHoraire)); iService++)
						curLigne->setJC(iService, GetJC(vFormatHoraire.GetNombre(Tampon, HORAIRESFORMATCOLONNEHoraire, iService)));
					break;

				case HORAIRESFORMATLIGNEAttente:
					for (iService=0; iService!=curLigne->NombreServices(); iService++)
						curLigne->setAttente(iService, cDureeEnMinutes(vFormatHoraire.GetNombre(Tampon, HORAIRESFORMATCOLONNEHoraire, iService)));
					break;

				case HORAIRESFORMATLIGNEFin:
					for (iService=0; iService!=curLigne->NombreServices(vFormatHoraire.Longueur(Tampon, HORAIRESFORMATCOLONNEHoraire)); iService++)
						if (vFormatHoraire.GetColonnePosition(HORAIRESFORMATCOLONNEHoraire, iService) < Tampon.Taille() && Tampon[vFormatHoraire.GetColonnePosition(HORAIRESFORMATCOLONNEHoraire, iService)]!=' ')
						{
							HeureDernier = vFormatHoraire.Extrait(Tampon, HORAIRESFORMATCOLONNEHoraire, iService);
							curLigne->setServiceContinu(iService);
						}
					break;

				case HORAIRESFORMATLIGNENumero:
					for (iService=0; iService!=curLigne->NombreServices(); iService++)
						curLigne->setNumeroService(iService, vFormatHoraire.Extrait(Tampon, HORAIRESFORMATCOLONNEHoraire, iService));
					break;

				case HORAIRESFORMATLIGNECodeImportationSource:
#ifdef ClasseAdmin
					curLigne->setImportSource(vImportation[vFormatHoraire.GetNombre(Tampon, HORAIRESFORMATCOLONNEHoraire)]);
#endif
					break;

				case HORAIRESFORMATLIGNECodeBaseTrains:
#ifdef ClasseAdmin
					for (iService=0; iService!=curLigne->NombreServices(); iService++)
						curLigne->Train(iService).setCodeBaseTrains(vFormatHoraire.GetNombre(Tampon, HORAIRESFORMATCOLONNEHoraire, iService));
#endif
					break;

				case HORAIRESFORMATLIGNEDepot:
#ifdef ClasseAdmin
					curLigne->setDepot(GetArretLogique(vFormatHoraire.GetNombre(Tampon, HORAIRESFORMATCOLONNEHoraire)));
#endif
					break;

				// Enregistrement des horaires g�r� par des piles.
				// En plusieurs parties
				case HORAIRESFORMATLIGNEHoraires:
					// Partie 1 - Enregistrement de la gareligne et cr�ation des sp�cificit�s
					if (lastGareLigne != NULL && NumeroArretLogiquePrecedent == vFormatHoraire.GetNombre(Tampon, HORAIRESFORMATCOLONNEArretLogique) && lastGareLigne->TypeDA() == Arrivee && (tTypeGareLigneDA) vFormatHoraire.Extrait(Tampon, HORAIRESFORMATCOLONNETypeArretLogique)[0] == Depart)
					{
						curGareLigne->setTypeDA(Passage);
						curGareLigne->setPADepartSuivant();
						DeuxiemePassage = true;
					}
					else
					{
						curArretLogique = GetArretLogique(vFormatHoraire.GetNombre(Tampon, HORAIRESFORMATCOLONNEArretLogique));

						// Err 002: Test existence du point d'arret
						if (curArretLogique == NULL)
						{
// 							Erreur("Point d'arret inexistant", "", Tampon, "03002");
							break;
						}

						curNumeroVoie = (tIndex) vFormatHoraire.GetNombre(Tampon, HORAIRESFORMATCOLONNEArretPhysique);

						// Err 005: Test existence du quai
						if (curArretLogique->GetArretPhysique(curNumeroVoie) == NULL)
						{

// 							Erreur("ArretPhysique inexistant dans le point d'arr�t", "", Tampon, "03005");

							curNumeroVoie = 1;
						}

						// Allocation
						curGareLigne = curLigne->addGareLigne(NULL,
							(tDistanceHM) vFormatHoraire.GetNombre(Tampon, HORAIRESFORMATCOLONNEPointHectometrique),
							curNumeroVoie,
							(tTypeGareLigneDA) vFormatHoraire.Extrait(Tampon, HORAIRESFORMATCOLONNETypeArretLogique)[0],
							curArretLogique,
							Tampon.Taille() >= vFormatHoraire.GetColonnePosition(HORAIRESFORMATCOLONNEHoraire) + vFormatHoraire.GetColonneLargeur(HORAIRESFORMATCOLONNEHoraire) * curLigne->NombreServices() - 1,
							false
						);
						DeuxiemePassage = false;

						// Err 001: Controle de croissance des PH
						switch (PHCroissants)
						{
						case Indifferent:
							if (curLigne->PremiereGareLigne() != curGareLigne)
							{
								if (curGareLigne->PM() > lastGareLigne->PM())
									PHCroissants = Vrai;
								else
									PHCroissants = Faux;
							}
							break;

						case Vrai:
							if (curGareLigne->PM() < lastGareLigne->PM())
							{}
// 								Erreur("Erreur de croissance de point hectom�trique", "", Tampon, "03001");
							break;

						case Faux:
							if (curGareLigne->PM() > lastGareLigne->PM())
							{}
// 								Erreur("Erreur de croissance de point hectom�trique", "", Tampon, "03001");
							break;
						}

						// Err 006: Controle de coh�rence des PH avec les diff�rences de GPS
						if (lastGareLigne != NULL)
							if (!curGareLigne->CoherenceGeographique(*lastGareLigne))
							{}
// 								Erreur("Erreur de coh�rence g�ographique (trajet trop long)", "", Tampon, "03006");
					}


					// Service continu
					if (curLigne->PremiereGareLigne() == curGareLigne)
						for (iService = 0; iService != curLigne->NombreServices(); iService++)
							if (curLigne->EstCadence(iService))
							{
								HeurePremier = vFormatHoraire.Extrait(Tampon, HORAIRESFORMATCOLONNEHoraire, iService);
								curLigne->setAmplitudeServiceContinu(iService, HeureDernier - HeurePremier);
							}

					// Partie 2: Horaires Avec ou sans horaires
					if (curGareLigne->HorairesSaisis())
					{
						curGareLigne->setHoraires(Tampon, vFormatHoraire.GetColonnePosition(HORAIRESFORMATCOLONNEHoraire), vFormatHoraire.GetColonneLargeur(HORAIRESFORMATCOLONNEHoraire), DeuxiemePassage);
						
					  // ici controle des horaires (pas pris en compte, juste log) 
						if (!curGareLigne->controleHoraire(DerniereGLAvecHoraires))
						{
							cTexte Message;
							Message << "Ligne " << curLigne->getCode() << " PM " << curGareLigne->PM();
// 							Erreur("Erreur de donn�es horaires", curLigne->getCode(), Message, "03009");
						}

						// Controle de vitesse
						if (DerniereGLAvecHoraires != NULL)
							for (iService=0; iService!=curLigne->NombreServices(); iService++)
							{
								if (curGareLigne->PM() > DerniereGLAvecHoraires->PM())
									Distance = curGareLigne->PM() - DerniereGLAvecHoraires->PM();
								else
									Distance = DerniereGLAvecHoraires->PM() - curGareLigne->PM();

								if ((curGareLigne->getHoraireArriveePremier(iService) - DerniereGLAvecHoraires->getHoraireDepartPremier(iService)).Valeur() != 0)
								{
									Vitesse = Distance / (curGareLigne->getHoraireArriveePremier(iService) - DerniereGLAvecHoraires->getHoraireDepartPremier(iService)).Valeur();
									Vitesse *= 6;
									if (Vitesse > curGareLigne->Ligne()->Materiel()->VitesseMoyenneMax())
									{
// 										vFichierLOG << "Attention: V=" << Vitesse << "km/h (VMax=" << curGareLigne->Ligne()->Materiel()->VitesseMoyenneMax() << " km/h) entre " << DerniereGLAvecHoraires->ArretLogique()->Index() << " et " << curGareLigne->ArretLogique()->Index() << ", sur ";
// 										vFichierLOG << curGareLigne->Ligne()->getCode() << ", service N�" << iService << "\n";
									}
								}
							}

						// Calcul des horaires de GareLigne sans saisie
						TailleTotalePileGLSansHoraire = TaillePileGLSansHoraire;
						while (TaillePileGLSansHoraire != 0)
						{
							PileGLSansHoraires[TaillePileGLSansHoraire - 1]->ChaineAvecHoraireSuivant(*DerniereGLAvecHoraires, *curGareLigne, TaillePileGLSansHoraire - 1, TailleTotalePileGLSansHoraire);
							TaillePileGLSansHoraire--;
						}
						DerniereGLAvecHoraires = curGareLigne;
					}
					else if (!curGareLigne->HorairesSaisis())
					{
						// Err 003 ne doit pas commencer sans horaires
						if (lastGareLigne == NULL)
						{
// 							Erreur("Debut de ligne sans horaire", curLigne->getCode(), "", "03003");
							break;
						}

						// Controle d'alerte: ligne apparemment mal document�e
						if (Tampon.Taille() > vFormatHoraire.GetColonnePosition(HORAIRESFORMATCOLONNEHoraire) + 1)
						{
// 							vFichierLOG << "Attention: Horaires mal document�s ? Donn�es horaires �ventuelles ignor�es: ligne " << curLigne->getCode() << ", point arret" << curGareLigne->ArretLogique()->Index() << "\n";
						}
						PileGLSansHoraires[TaillePileGLSansHoraire] = curGareLigne;
						TaillePileGLSansHoraire++;
					}

					// Controle d'erreur: ne doit pas terminer sans horaires, stockage pour controle en fin de ligne
					DernierPAHorairesSaisis = curGareLigne->HorairesSaisis();
					
					NumeroArretLogiquePrecedent = curGareLigne->ArretLogique()->Index();
					lastGareLigne = curGareLigne;
					break;

				case -TYPEVide:
					// Controle d'erreur en sortie
					if (!DernierPAHorairesSaisis)
					{
// 						Erreur("Fin de ligne sans horaire", curLigne->getCode(), "", "03004");
						return false;
					}

					PasTermineElement = false;
					PasTermineSousSection = false;
					PasTermineSection = false;
					break;

				case -TYPESousSection:
					// Controle d'erreur en sortie
					if (!DernierPAHorairesSaisis)
					{
// 						Erreur("Fin de ligne sans horaire", curLigne->getCode(), "", "03004");
						return false;
					}
					PasTermineElement = false;
					break;

				case -TYPESection:
					// Controle d'erreur en sortie
					if (!DernierPAHorairesSaisis)
					{
// 						Erreur("Fin de ligne sans horaire", curLigne->getCode(), "", "03004");
						return false;
					}
					PasTermineSousSection = false;
					PasTermineElement = false;

				}

			}
		}
	}

	Fichier.close();

	return true;
//	vFichierLOG << nLignes << "L " << (short int) nTrains << " T\n";
}







void cEnvironnement::RemplitProchainAxe()
{
	cGareLigne* GLAModifier;
	cGareLigne* GLTest;

	for (tIndex iNumeroArretLogique = 0; iNumeroArretLogique < NombrePointsArret(); iNumeroArretLogique++)
		if (getArretLogique(iNumeroArretLogique) != NULL)
		{
			// Departs
			for (GLAModifier = getArretLogique(iNumeroArretLogique)->PremiereGareLigneDep(); GLAModifier!=NULL; )
			{
				for (GLTest = GLAModifier->PADepartSuivant(); GLTest != NULL && (GLTest->Ligne()->Axe() == GLAModifier->Ligne()->Axe() || !GLTest->Ligne()->Axe()->Autorise()); GLTest = GLTest->PADepartSuivant())
				{ }
				for (; GLAModifier != GLTest; GLAModifier = GLAModifier->PADepartSuivant())
					GLAModifier->setPADepartAxeSuivant(GLTest);
			}

			// Arrivees
			for (GLAModifier = getArretLogique(iNumeroArretLogique)->PremiereGareLigneArr(); GLAModifier!=NULL; )
			{
				for (GLTest = GLAModifier->PAArriveeSuivante(); GLTest!=NULL && (GLTest->Ligne()->Axe() == GLAModifier->Ligne()->Axe() || !GLTest->Ligne()->Axe()->Autorise()); GLTest = GLTest->PAArriveeSuivante())
				{ }
				for (; GLAModifier != GLTest; GLAModifier = GLAModifier->PAArriveeSuivante())
					GLAModifier->setPAArriveeAxeSuivant(GLTest);
			}

}

}




void cEnvironnement::RemplitCIL()
{
	for (cLigne* curLigne = vPremiereLigne; curLigne!=NULL; curLigne=curLigne->Suivant())
		curLigne->MajCirculation();
}







/*cSauvegarde* cEnvironnement::JCSauvegardeModifier(tNumeroJC NumeroNewJC, const cTexte& newIntitule)
{
	if (cJC* curJC = GetJC(NumeroNewJC))
	{
		if (!newIntitule.GetTaille())
			curJC->setIntitule(newIntitule);
		curJC->ReAlloueMasque();
		curJC->setCategorie();

		cSauvegarde* curSauvegarde = new cSauvegarde(vNomFichierJC);
		curSauvegarde->CopieJusqueA(cTexte(""), TXT2(NumeroNewJC, TAILLECODEJC));
		*curSauvegarde << "[" << TXT2(NumeroNewJC, TAILLECODEJC) << FINL;
		return(curSauvegarde);
	}
	else
		return(NULL);
}*/



/** Chargement du fichier des modalit�s de r�servation
	\author Hugues Romain
	\date 2001-2005
*/
bool cEnvironnement::ChargeFichierResa()
{
	// Variables
	size_t	NombreResaReel = 0;
	bool		PasTermineSousSection = true;
	bool		PasTermineElement=false;
	size_t	Taille;
	cFormatFichier&	vFormatResa = *_FormatResa;
	
	// Tampon
	cTexte Tampon(TAILLETAMPON, true);

	// Objets courants
	cModaliteReservation*			curResa=NULL;
	cModaliteReservationEnLigne*	curResaEnLigne=NULL;

	// Ouverture du fichier
	ifstream Fichier;
	Fichier.open(vNomFichierResa.Texte());

	if (!Fichier.is_open())
	{
		cout << "*** ERREUR Impossible d'ouvrir le fichier " << vNomFichierResa.Texte() << "\n";
// 		vFichierLOG << "*** ERREUR Impossible d'ouvrir le fichier " << vNomFichierResa.Texte() << "\n";
		return false;
	}
	
	// Nombre d'�l�ments
	Tampon.LireLigne(Fichier);
	if (Tampon.Compare(RESASFORMATLIGNENombre, 6))
	{
		vNombreResa = Tampon.GetNombre(0, 7);
		for (unsigned int i=0; i<vNombreResa; ++i) vResa.push_back (0); //dirty hack
	}
	else
	{
// 		Erreur("Nombre de modalit�s de r�servation non document�, chargement interrompu.", "", Tampon, "07001");
		return(false);
	}


	// Boucle sur les Sous Sections
	while (PasTermineSousSection)
	{
		if (Tampon.ProchaineSection(Fichier, RESASFORMATLIGNEReservation))
		{
			if (Tampon.GetNombre(1, 1))
			{
				curResaEnLigne = new cModaliteReservationEnLigne();
				curResa = (cModaliteReservation*) curResaEnLigne;
			}
			else
			{
				curResa = new cModaliteReservation();
				curResaEnLigne = NULL;
			}
			if (Enregistre(curResa, Tampon.GetNombre(0, 1)) != INCONNU)
			{
				NombreResaReel++;
				PasTermineElement = true;
			}
			else
			{
				delete curResa;
// 				Erreur("Num�ro de modalit� de r�servation trop grand", "", Tampon, "07002");
			}
		}
		else
			PasTermineSousSection = false;

		// Boucle sur les �l�ments d'une r�sa
		while (PasTermineElement)
		{
			switch (vFormatResa.LireFichier(Fichier, Tampon))
			{
			case RESASFORMATLIGNEType:
				if (!curResa->SetTypeResa(Tampon[vFormatResa.GetColonnePosition(RESASFORMATCOLONNEStandard)]))
				{}
// 					Erreur("Type de r�servation incorrect", TXT(curResa->Index()), Tampon, "07004");
				break;

			case RESASFORMATLIGNEDelaiMinMinutes:
				if (!curResa->SetDelaiMinMinutes(vFormatResa.GetNombre(Tampon, RESASFORMATCOLONNEStandard)))
				{}
// 					Erreur("D�lai minimal en minutes incorrect", TXT(curResa->Index()), Tampon, "07005");
				break;

			case RESASFORMATLIGNEDelaiMinHeureMax:
			{
				cHeure curHeure;
				curHeure = vFormatResa.Extrait(Tampon, RESASFORMATCOLONNEHeure);
				if (!curResa->SetDelaiMinHeureMax(curHeure))
				{}
// 					Erreur("D�lai min Heure max incorrect", TXT(curResa->Index()), Tampon, "07003");
				break;
			}
			
			case RESASFORMATLIGNEDelaiMinJours:
				if (!curResa->SetDelaiMinJours((tDureeEnJours) vFormatResa.GetNombre(Tampon, RESASFORMATCOLONNEStandard)))
				{}
// 					Erreur("D�lai minimal en jours incorrect", TXT(curResa->Index()), Tampon, "07006");
				break;

			case RESASFORMATLIGNEDelaiMaxJours:
				if (!curResa->SetDelaiMaxJours((tDureeEnJours) vFormatResa.GetNombre(Tampon, RESASFORMATCOLONNEStandard)))
				{}
// 					Erreur("D�lai maximal en jours incorrect", TXT(curResa->Index()), Tampon, "07007");
				break;

			case RESASFORMATLIGNEDoc:
				if (!curResa->SetDoc(vFormatResa.Extrait(Tampon, RESASFORMATCOLONNEStandard)))
				{}
// 					Erreur("Erreur de copie Documentation", TXT(curResa->Index()), Tampon, "07008");
				break;

			case RESASFORMATLIGNEPrix:
				if (!curResa->SetPrix((const float) vFormatResa.GetNombre(Tampon, RESASFORMATCOLONNEStandard)))
				{}
// 					Erreur("Prix incorrect", TXT(curResa->Index()), Tampon, "07009");
				break;

			case RESASFORMATLIGNETel:
				if (!curResa->SetTel(vFormatResa.Extrait(Tampon, RESASFORMATCOLONNEStandard)))
				{}
// 					Erreur("Erreur de copie Num�ro T�l�phone", TXT(curResa->Index()), Tampon, "07010");
				break;

			case RESASFORMATLIGNEHorairesTel:
				if (!curResa->SetHorairesTel(vFormatResa.Extrait(Tampon, RESASFORMATCOLONNEStandard)))
				{}
// 					Erreur("Erreur de copie Horaires T�l�phone", TXT(curResa->Index()), Tampon, "07011");
				break;

			case RESASFORMATLIGNEEMail:
				if (!curResaEnLigne || !curResaEnLigne->SetMail(vFormatResa.Extrait(Tampon, RESASFORMATCOLONNEStandard)))
				{}
// 					Erreur("Erreur de copie E-Mail", TXT(curResa->Index()), Tampon, "07012");
				break;

			case RESASFORMATLIGNEEMailCopie:
				if (!curResaEnLigne || !curResaEnLigne->SetMailCopie(vFormatResa.Extrait(Tampon, RESASFORMATCOLONNEStandard)))
				{}
// 					Erreur("Erreur de copie E-Mail CC", TXT(curResa->Index()), Tampon, "07023");
				break;

			case RESASFORMATLIGNESiteWeb:
				if (!curResa->SetSiteWeb(vFormatResa.Extrait(Tampon, RESASFORMATCOLONNEStandard)))
				{}
// 					Erreur("Erreur de copie Site web", TXT(curResa->Index()), Tampon, "07013");
				break;

			case RESASFORMATLIGNEReservationEnLigne:
				break;

			case RESASFORMATLIGNERELPrenom:
				if (!curResaEnLigne || !curResaEnLigne->SetRELPrenom((tBool3)vFormatResa.GetNombre(Tampon, RESASFORMATCOLONNEStandard)))
				{}
// 					Erreur("Erreur REL PreNom", TXT(curResa->Index()), Tampon, "07016");
				break;

			case RESASFORMATLIGNERELAdresse:
				if (!curResaEnLigne || !curResaEnLigne->SetRELAdresse((tBool3)vFormatResa.GetNombre(Tampon, RESASFORMATCOLONNEStandard)))
				{}
// 					Erreur("Erreur REL Adresse", TXT(curResa->Index()), Tampon, "07017");
				break;

			case RESASFORMATLIGNERELTelephone:
				if (!curResaEnLigne || !curResaEnLigne->SetRELTel((tBool3)vFormatResa.GetNombre(Tampon, RESASFORMATCOLONNEStandard)))
				{}
// 					Erreur("Erreur REL T�l�phone", TXT(curResa->Index()), Tampon, "07015");
				break;

			case RESASFORMATLIGNERELEMail:
				if (!curResaEnLigne || !curResaEnLigne->SetRELEMail((tBool3)vFormatResa.GetNombre(Tampon, RESASFORMATCOLONNEStandard)))
				{}
// 					Erreur("Erreur REL e-mail", TXT(curResa->Index()), Tampon, "07018");
				break;

			case RESASFORMATLIGNERELNumeroAbo:
				if (!curResaEnLigne || !curResaEnLigne->SetRELNumeroAbo((tBool3)vFormatResa.GetNombre(Tampon, RESASFORMATCOLONNEStandard)))
				{}
// 					Erreur("Erreur REL Numero abo", TXT(curResa->Index()), Tampon, "07019");
				break;

			case RESASFORMATLIGNEMax:
				if (!curResaEnLigne || !curResaEnLigne->setMax(vFormatResa.GetNombre(Tampon, RESASFORMATCOLONNEStandard)))
				{}
// 					Erreur("Erreur Nombre Max", TXT(curResa->Index()), Tampon, "07021");
				break;
				
			case RESASFORMATLIGNESeuil:
				Taille = vFormatResa.Longueur(Tampon, RESASFORMATCOLONNEHeure);
				for (size_t i = 0; i < Taille; i++)
					if (!curResaEnLigne || !curResaEnLigne->addSeuil(vFormatResa.GetNombre(Tampon, RESASFORMATCOLONNEHeure, i), Taille))
					{}
// 						Erreur("Erreur Seuil", TXT(curResa->Index()), Tampon, "07022");
				break;
				
			case RESASFORMATLIGNEReferenceALOrigine:
				if (!curResa->setReferenceEstLOrigine(vFormatResa.GetNombre(Tampon, RESASFORMATCOLONNEStandard) != 0))
				{}
// 					Erreur("Erreur Reference est l'origine", TXT(curResa->Index()), Tampon, "07023");
				break;

			case -TYPEVide:
				Tampon.Vide();
			case -TYPESousSection:
				PasTermineElement = false;
				break;

			default:
			{}
// 				Erreur("Commande inconnue", TXT(curResa->Index()), Tampon, "07020");
			}
		}
	}

	Fichier.close();

	return true;	
}



bool cEnvironnement::ChargeFichierHandicape()
{
	// Variables
	tIndex NombreHandicapeReel = 0;
	bool PasTermineSousSection = true;
	bool PasTermineElement=false;
	cFormatFichier& vFormatHandicape = *_FormatHandicape;

	// Tampon
	cTexte Tampon(TAILLETAMPON, true);
	// Objets courants
	cHandicape* curHandicape=NULL;

	// Ouverture du fichier
	ifstream Fichier;
	Fichier.open(vNomFichierHandicape.Texte());

	if (!Fichier.is_open())
	{
		cout << "*** ERREUR Impossible d'ouvrir le fichier " << vNomFichierHandicape.Texte() << "\n";
// 		vFichierLOG << "*** ERREUR Impossible d'ouvrir le fichier " << vNomFichierHandicape.Texte() << "\n";
		return false;
	}

	// Nombre d'�l�ments
	Tampon.LireLigne(Fichier);
	if (Tampon.Compare("Nombre=", 6))
	{
		vNombreHandicape = (tIndex) Tampon.GetNombre(0, 7);
        // Hugues, pourquoi +1 ??
		for (unsigned int i=0; i<vNombreHandicape+1; ++i) vHandicape.push_back (0); //dirty hack
	}
	else
	{
// 		vFichierLOG << "*** ERREUR Nombre de type handicap�s non document�, chargement interrompu.\n\n";
		Fichier.close();
		return(false);
	}

	// Boucle sur les Sous Sections
	while (PasTermineSousSection)
	{
		if (Tampon.ProchaineSection(Fichier, TYPESousSection))
		{
			curHandicape = new cHandicape;
			NombreHandicapeReel++;
			PasTermineElement = true;
			vHandicape[Tampon.GetNombre(0, 1)] = curHandicape;
		}
		else
			PasTermineSousSection = false;

		// Boucle sur les propri�t�s d'un �l�ment
		while (PasTermineElement)
		{
			switch (vFormatHandicape.LireFichier(Fichier, Tampon))
			{
			case HANDICAPESFORMATLIGNEType:
				curHandicape->setTypeHandicape((tBool3) Tampon[vFormatHandicape.GetColonnePosition(HANDICAPESFORMATCOLONNEStandard)]);
				break;

			case HANDICAPESFORMATLIGNEContenance:
				curHandicape->setContenance((tContenance) vFormatHandicape.GetNombre(Tampon, HANDICAPESFORMATCOLONNEStandard));
				break;

			case HANDICAPESFORMATLIGNEDoc:
				curHandicape->setDoc(vFormatHandicape.Extrait(Tampon, HANDICAPESFORMATCOLONNEStandard));
				break;

			case HANDICAPESFORMATLIGNEPrix:
				curHandicape->setPrix((tPrix) vFormatHandicape.GetNombre(Tampon, HANDICAPESFORMATCOLONNEStandard));
				break;

			case HANDICAPESFORMATLIGNEResa:
				curHandicape->setResa(vResa[vFormatHandicape.GetNombre(Tampon, HANDICAPESFORMATCOLONNEStandard)]);
				break;

			case -TYPEVide:
				Tampon.Vide();
			case -TYPESousSection:
				PasTermineElement = false;
			}
		}
	}

	Fichier.close();

	return(true);

}


bool cEnvironnement::ChargeFichierTarif()
{
	// Variables
	tNumeroTarif NombreTarifReel = 0;
	bool PasTermineSousSection = true;
	bool PasTermineElement=false;
	cFormatFichier& vFormatTarif = *_FormatTarif;

	// Tampon
	cTexte Tampon(TAILLETAMPON, true);
	// Objets courants
	cTarif* curTarif=NULL;

	// Ouverture du fichier
	ifstream Fichier;
	Fichier.open(vNomFichierTarif.Texte());

	if (!Fichier.is_open())
	{
		cout << "*** ERREUR Impossible d'ouvrir le fichier " << vNomFichierTarif.Texte() << "\n";
// 		vFichierLOG << "*** ERREUR Impossible d'ouvrir le fichier " << vNomFichierTarif.Texte() << "\n";
		return false;
	}

	// Nombre d'�l�ments
	Tampon.LireLigne(Fichier);
	if (Tampon.Compare("Nombre=", 6))
	{
		vNombreTarif = (tNumeroTarif) Tampon.GetNombre(0, 7);
		for (unsigned int i=0; i<vNombreTarif; ++i) vTarif.push_back (0); //dirty hack
	}
	else
	{
// 		vFichierLOG << "*** ERREUR Nombre de type de tarifications non document�, chargement interrompu.\n\n";
		return(false);
	}

	// Boucle sur les Sous Sections
	while (PasTermineSousSection)
	{
		if (Tampon.ProchaineSection(Fichier, TYPESousSection))
		{
			curTarif = new cTarif;
			if (Enregistre(curTarif, Tampon.GetNombre(0, 1)) != INCONNU)
			{
// 				vFichierLOG << "Ajout de tarif " << Tampon.GetNombre(0, 1) << "\n";
				NombreTarifReel++;
				PasTermineElement = true;
			}
			else
			{
				delete curTarif;
// 				Erreur("Num�ro de modalit� de tarification trop grand", "", Tampon, "07002");
			}
		}
		else
			PasTermineSousSection = false;

		// Boucle sur les propri�t�s d'un �l�ment
		while (PasTermineElement)
		{
			switch (vFormatTarif.LireFichier(Fichier, Tampon))
			{
			case TARIFSFORMATLIGNEType:
				curTarif->setTypeTarif((tTypeTarif) Tampon[vFormatTarif.GetColonnePosition(TARIFSFORMATCOLONNEStandard)]);
				break;

			case TARIFSFORMATLIGNELibelle:
			{
				cTexte libelle = vFormatTarif.ExtraitComplet(Tampon, TARIFSFORMATCOLONNEStandard);
				curTarif->setLibelle(libelle);
				break;
			}
			case -TYPEVide:
				Tampon.Vide();
			case -TYPESousSection:
				PasTermineElement = false;
			}
		}
	}

	Fichier.close();

	return true;
}


 
bool cEnvironnement::ChargeFichierVelo()
{
	// Variables
	tIndex NombreVeloReel = 0;
	bool PasTermineSousSection = true;
	bool PasTermineElement=false;
	cFormatFichier& vFormatVelo = *_FormatVelo;

	// Tampon
	cTexte Tampon(TAILLETAMPON, true);
	// Objets courants
	cVelo* curVelo=NULL;

	// Ouverture du fichier
	ifstream Fichier;
	Fichier.open(vNomFichierVelo.Texte());

	if (!Fichier.is_open())
	{
		cout << "*** ERREUR Impossible d'ouvrir le fichier " << vNomFichierVelo.Texte() << "\n";
// 		vFichierLOG << "*** ERREUR Impossible d'ouvrir le fichier " << vNomFichierVelo.Texte() << "\n";
		return false;
	}

	// Nombre d'�l�ments
	Tampon.LireLigne(Fichier);
	if (Tampon.Compare("Nombre=", 6))
	{
		vNombreVelo = (tIndex) Tampon.GetNombre(0, 7);
		for (unsigned int i=0; i<vNombreVelo; ++i) vVelo.push_back (0); //dirty hack
	}
	else
	{
// 		vFichierLOG << "*** ERREUR Nombre de v�los non document�, chargement interrompu.\n\n";
		return(false);
	}


	// Boucle sur les Sous Sections
	while (PasTermineSousSection)
	{
		if (Tampon.ProchaineSection(Fichier, TYPESousSection))
		{
			curVelo = new cVelo;
			NombreVeloReel++;
			PasTermineElement = true;
			vVelo[Tampon.GetNombre(0, 1)] = curVelo;
		}
		else
			PasTermineSousSection = false;

		// Boucle sur les propri�t�s d'un �l�ment
		while (PasTermineElement)
		{
			switch (vFormatVelo.LireFichier(Fichier, Tampon))
			{
			case VELOSFORMATLIGNEType:
				curVelo->setTypeVelo((tBool3) Tampon[vFormatVelo.GetColonnePosition(VELOSFORMATCOLONNEStandard)]);
				break;

			case VELOSFORMATLIGNEContenance:
				curVelo->setContenance((tContenance) vFormatVelo.GetNombre(Tampon, VELOSFORMATCOLONNEStandard));
				break;

			case VELOSFORMATLIGNEDoc:
				curVelo->setDoc(vFormatVelo.Extrait(Tampon, VELOSFORMATCOLONNEStandard));
				break;

			case VELOSFORMATLIGNEPrix:
				curVelo->setPrix((tPrix) vFormatVelo.GetNombre(Tampon, VELOSFORMATCOLONNEStandard));
				break;

			case VELOSFORMATLIGNEResa:
				curVelo->setResa(vResa[vFormatVelo.GetNombre(Tampon, VELOSFORMATCOLONNEStandard)]);
				break;

			case -TYPEVide:
				Tampon.Vide();
			case -TYPESousSection:
				PasTermineElement = false;
			}
		}
	}

	Fichier.close();

	return true;
}



bool cEnvironnement::ChargeFichierReseaux()
{
	// Variables
	tNumeroReseau NombreReseauxReel = 0;
	bool PasTermineSousSection = true;
	bool PasTermineElement=false;
	cFormatFichier& vFormatReseaux = *_FormatReseaux;

	// Tampon
	cTexte Tampon(TAILLETAMPON, true);
	// Objets courants
	cReseau* curReseau=NULL;

	// Ouverture du fichier
	ifstream Fichier;
	Fichier.open(vNomFichierReseaux.Texte());

	if (!Fichier.is_open())
	{
		cout << "*** ERREUR Impossible d'ouvrir le fichier " << vNomFichierReseaux.Texte() << "\n";
// 		vFichierLOG << "*** ERREUR Impossible d'ouvrir le fichier " << vNomFichierReseaux.Texte() << "\n";
		return false;
	}

	// Nombre d'�l�ments
	Tampon.LireLigne(Fichier);
	if (Tampon.Compare("Nombre=", 6))
	{
		vNombreReseaux = (tNumeroReseau) Tampon.GetNombre(0, 7);
		for (unsigned int i=0; i<vNombreReseaux; ++i) vReseau.push_back (0); //dirty hack
	}
	else
	{
// 		vFichierLOG << "*** ERREUR Nombre de r�seaux non document�, chargement interrompu.\n\n";
		return(false);
	}


	// Boucle sur les Sous Sections
	while (PasTermineSousSection)
	{
		if (Tampon.ProchaineSection(Fichier, TYPESousSection))
		{
			curReseau = new cReseau;
			NombreReseauxReel++;
			PasTermineElement = true;
			vReseau[Tampon.GetNombre(0, 1)] = curReseau;
		}
		else
			PasTermineSousSection = false;

		// Boucle sur les propri�t�s d'un �l�ment
		while (PasTermineElement)
		{
			switch (vFormatReseaux.LireFichier(Fichier, Tampon))
			{
			case RESEAUXFORMATLIGNENom:
				curReseau->setNom(vFormatReseaux.Extrait(Tampon, RESEAUXFORMATCOLONNEStandard));
				break;

			case RESEAUXFORMATLIGNEUrl:
				curReseau->setURL(vFormatReseaux.Extrait(Tampon, RESEAUXFORMATCOLONNEStandard));
				break;

			case RESEAUXFORMATLIGNEDoc:
				curReseau->setDoc(vFormatReseaux.Extrait(Tampon, RESEAUXFORMATCOLONNEStandard));
				break;

			case RESEAUXFORMATLIGNECartouche:
				break;

			case -TYPEVide:
				Tampon.Vide();
			case -TYPESousSection:
				PasTermineElement = false;
			}
		}
	}

	Fichier.close();

	return true;

}

bool cEnvironnement::ChargeFichierMateriel()
{
	// Variables
	tIndex NombreMaterielsReel = 0;
	bool PasTermineSousSection = true;
	bool PasTermineElement=false;
	tIndex NumeroMateriel;
	cFormatFichier& vFormatMateriel = *_FormatMateriel;
	// Tampon

	cTexte Tampon(TAILLETAMPON, true);
	// Objets courants
	cMateriel* curMateriel=NULL;

	// Ouverture du fichier
	ifstream Fichier;
	Fichier.open(vNomFichierMateriel.Texte());

	if (!Fichier.is_open())
	{
		cout << "*** ERREUR Impossible d'ouvrir le fichier " << vNomFichierMateriel.Texte() << "\n";
// 		vFichierLOG << "*** ERREUR Impossible d'ouvrir le fichier " << vNomFichierMateriel.Texte() << "\n";
		return false;
	}

	// Nombre d'�l�ments
	Tampon.LireLigne(Fichier);
	if (Tampon.Compare("Nombre=", 6))
	{
		vNombreMateriels = (tIndex) Tampon.GetNombre(0, 7);
		for (unsigned int i=0; i<vNombreMateriels; ++i) vMateriel.push_back (0); //dirty hack
	}
	else
	{
// 		Erreur("Nombre de materiels non document�, chargement interrompu.", "", Tampon, "05001");
		Fichier.close();
		return(false);
	}


	// Boucle sur les Sous Sections
	while (PasTermineSousSection)
	{
		if (Tampon.ProchaineSection(Fichier, TYPESousSection))
		{
			NumeroMateriel = Tampon.GetNombre(0, 1);
			if (NumeroMateriel < 0 || NumeroMateriel >= vNombreMateriels)
			{}
// 				Erreur("Numero de materiel trop grand", "", Tampon, "05002");
			else
			{
				curMateriel = new cMateriel(NumeroMateriel);
				vMateriel[curMateriel->Code()] = curMateriel;
				NombreMaterielsReel++;
				PasTermineElement = true;
			}
		}
		else
		{
			PasTermineElement = false;
			PasTermineSousSection = false;
		}

		// Boucle sur les propri�t�s d'un �l�ment
		while (PasTermineElement)
		{
			switch (vFormatMateriel.LireFichier(Fichier, Tampon))
			{
			case MATERIELSFORMATLIGNENom:
				curMateriel->setLibelleSimple(vFormatMateriel.ExtraitComplet(Tampon, MATERIELSFORMATCOLONNEStandard));
				break;

			case MATERIELFORMATLIGNEArticle:
				curMateriel->setArticle(vFormatMateriel.ExtraitComplet(Tampon, MATERIELSFORMATCOLONNEStandard));
				break;

			case MATERIELSFORMATLIGNEDoc:
				curMateriel->setDoc(vFormatMateriel.ExtraitComplet(Tampon, MATERIELSFORMATCOLONNEStandard));
				break;

			case MATERIELSFORMATLIGNEVitesse:
				curMateriel->setVitesseMoyenneMax((tVitesseKMH) vFormatMateriel.GetNombre(Tampon, MATERIELSFORMATCOLONNEStandard));
				break;

			case MATERIELSFORMATLIGNEIndicateur:
				curMateriel->setIndicateur(vFormatMateriel.ExtraitComplet(Tampon, MATERIELSFORMATCOLONNEStandard));
				break;

			case -TYPEVide:
				Tampon.Vide();
			case -TYPESousSection:
				PasTermineElement = false;
			}
		}
	}

	Fichier.close();

	return true;
}


/*! \brief Chargement du fichier des photos
	\author Hugues Romain
	\date 2002
*/
bool cEnvironnement::ChargeFichierPhotos()
{
	// Variables
	bool PasTermineSousSection = true;
	bool PasTermineElement=false;
	size_t Position;
	cFormatFichier& vFormatPhoto = *_FormatPhoto;

	// Tampon
	cTexte Tampon(TAILLETAMPON, true);
	// Objets courants
	cPhoto* curPhoto=NULL;

	// Ouverture du fichier
	ifstream Fichier;
	Fichier.open(vNomFichierPhotos.Texte());

	if (!Fichier.is_open())
	{
		cout << "*** ERREUR Impossible d'ouvrir le fichier " << vNomFichierPhotos.Texte() << "\n";
// 		vFichierLOG << "*** ERREUR Impossible d'ouvrir le fichier " << vNomFichierPhotos.Texte() << "\n";
		return false;
	}
	
	// Nombre d'�l�ments
	Tampon.LireLigne(Fichier);
	if (Tampon.Compare("Nombre=", 6))
	{
		
	}
	else
	{
// 		vFichierLOG << "*** ERREUR Nombre de photos non document�, chargement interrompu.\n\n";
		Fichier.close();
		return(false);
	}

	// Boucle sur les Sous Sections
	while (PasTermineSousSection)
	{
		if (Tampon.ProchaineSection(Fichier, TYPESousSection))
		{
			curPhoto = new cPhoto((tIndex) Tampon.GetNombre(0, 1));
			_Documents.SetElement(curPhoto, curPhoto->Index());
			PasTermineElement = true;
		}
		else
			PasTermineSousSection = false;

		// Boucle sur les propri�t�s d'un �l�ment
		while (PasTermineElement)
		{
			switch (vFormatPhoto.LireFichier(Fichier, Tampon))
			{
			case PHFORMATLIGNEDesignationGenerale:
				curPhoto->setDescriptionGenerale(vFormatPhoto.ExtraitComplet(Tampon, PHFORMATCOLONNEStandard));
				break;

			case PHFORMATLIGNEDesignationLocale:
				curPhoto->setDescriptionLocale(vFormatPhoto.ExtraitComplet(Tampon, PHFORMATCOLONNEStandard));
				break;

			case PHFORMATLIGNEZoneCliquableURL:
				{
					cTexteHTML __Alt;
					Position = Tampon.RechercheOccurenceGauche('#');
					__Alt << Tampon.Extrait
					(
						vFormatPhoto.GetColonnePosition(PHFORMATCOLONNEAltUrlMapUrl)
						, Position - vFormatPhoto.GetColonnePosition(PHFORMATCOLONNEAltUrlMapUrl) + 1
					);
					curPhoto->addMapPhoto
					(
						vFormatPhoto.Extrait(Tampon, PHFORMATCOLONNECoordonneesMap)
						, Tampon.Extrait(Position+1)
						, __Alt
					);
				}
				break;

			case PHFORMATLIGNEZoneCliquablePhoto:
				{
					cTexteHTML __Alt;
					__Alt << vFormatPhoto.Extrait(Tampon, PHFORMATCOLONNEAltMapPhoto);
					curPhoto->addMapPhoto
					(
						vFormatPhoto.Extrait(Tampon, PHFORMATCOLONNECoordonneesMap)
						, (tIndex) vFormatPhoto.GetNombre(Tampon, PHFORMATCOLONNENumeroPhotoLiee)
						, __Alt
					);
				}
				break;

			case PHFORMATLIGNENomFichier:
				curPhoto->SetURL(vFormatPhoto.ExtraitComplet(Tampon, PHFORMATCOLONNEStandard));
				break;

			case -TYPEVide:
				Tampon.Vide();
			case -TYPESousSection:
				PasTermineElement = false;
			}
		}
	}
	Fichier.close();
	
	return true;
}






// cEnvironnement - NomLibreUnique
// ____________________________________________________________________________
// Fournit un nom unique de ligne sous la forme
// NomBase/Cxx
// Si NomBase contient deja /Cxx, xx sera remplac� par un code donnant
// l'unicit�.
// Gere les noms de lignes termin�s par ___

// ERR: xx limit� � 99 voir si cela pose probleme

// ERR: Pas de controle de xx si /Cxx trouv�
// ____________________________________________________________________________
/*void cEnvironnement::NomLigneUnique(cTexte& NomBase) const
{
	int DernierNumeroTrouve;
	int CodeTeste;

	// Extraction du nom de la ligne
	if (NomBase.Compare("/C", 2, NomBase.GetTaille() - 4))
		DernierNumeroTrouve = NomBase.GetNombre(0, NomBase.GetTaille() - 2);
	else
	{
		DernierNumeroTrouve = 0;
		NomBase << "/C00";
	}

	// Parcours des lignes pour obtenir le bon num�ro
	for (cLigne* curLigne = PremiereLigne(); curLigne!=NULL; curLigne = curLigne->Suivant())
	{
		if (curLigne->getCode().GetTaille() == NomBase.GetTaille() && curLigne->getCode().Compare(NomBase, NomBase.GetTaille() - 2))
		{
			CodeTeste = curLigne->getCode().GetNombre(0, NomBase.GetTaille() - 2);
			if (CodeTeste > DernierNumeroTrouve)
				DernierNumeroTrouve = CodeTeste;
		}
	}

	// Ecriture du nom definitif
	DernierNumeroTrouve++;
	NomBase.vPosition = NomBase.GetTaille() - 2;
	NomBase.Copie(DernierNumeroTrouve, 2, false);
}*/
// � Hugues Romain 2002
// ____________________________________________________________________________







/*!	\brief Recherche d'une ligne � partir de son code
	\param CodeLigne Code � comparer
	\author Hugues Romain
	\date 2000
*/
cLigne* cEnvironnement::GetLigne(const cTexte& CodeLigne) const
{
	cLigne* curLigne = PremiereLigne();
	for (; curLigne != NULL; curLigne = curLigne->Suivant())
		if (curLigne->getCode().Compare(CodeLigne))
			break;
	return curLigne;
}

bool cEnvironnement::ChargeFichierIndicateurs()
{
	// Variables
	tNumeroIndicateur NombreIndicateursCharges = 0;
	bool PasTermineSousSection = true;
	bool PasTermineElement=false;
	cJC* JCBase = NULL;
	cFormatFichier& vFormatIndicateurs = *_FormatIndicateurs;
	// Tampon
	cTexte Tampon(TAILLETAMPON, true);
	// Objets courants
	cIndicateurs* curIndicateur=NULL;

	// Ouverture du fichier
	ifstream Fichier;
	cTexte NomFichierComplet;
	NomFichierComplet << vNomRepEnv << INDICATEURSEXTENSION;
	Fichier.open(NomFichierComplet.Texte());

	//SET PORTAGE LINUX
	if (!Fichier.is_open())
	{
		cout << "*** ERREUR Impossible d'ouvrir le fichier " << NomFichierComplet.Texte() << "\n";
		return false;
	}
	//END PORTAGE LINUX

	// Nombre d'�l�ments
	Tampon.LireLigne(Fichier);
	if (Tampon.Compare("Nombre=", 7))
	{
		vNombreIndicateurs = (tNumeroIndicateur) Tampon.GetNombre(0, 7);
		vIndicateurs = (cIndicateurs**) malloc(vNombreIndicateurs * sizeof(cIndicateurs*));
	}
	else
	{
		Fichier.close();
		return(false);
	}

	// Nombre d'�l�ments
	Tampon.LireLigne(Fichier);
	if (Tampon.Compare("JC=", 3))
	{
		JCBase = GetJC(Tampon.GetNombre(0, 3));
	}
	if (!JCBase)
	{
		Fichier.close();
		return(false);
	}

	// Boucle sur les Sous Sections
	while (PasTermineSousSection)
	{
		if (vNombreIndicateurs == NombreIndicateursCharges)
			break;
		if (Tampon.ProchaineSection(Fichier, TYPESousSection))
		{
			curIndicateur = new cIndicateurs(Tampon.Extrait(1), this);
			PasTermineElement = true;
			vIndicateurs[NombreIndicateursCharges] = curIndicateur;
			NombreIndicateursCharges++;
		}
		else
		{
			PasTermineElement = false;
			PasTermineSousSection = false;
		}

		// Boucle sur les propri�t�s d'un �l�ment
		while (PasTermineElement)
		{
			switch (vFormatIndicateurs.LireFichier(Fichier, Tampon))
			{
			case INDICATEURSFORMATLIGNECP:
				curIndicateur->setCommencePage(true);
				break;

			case INDICATEURSFORMATLIGNEJC:
				curIndicateur->setJC(*GetJC(vFormatIndicateurs.GetNombre(Tampon, INDICATEURSFORMATCOLONNEStandard)), *JCBase);
				break;

			case INDICATEURSFORMATLIGNEGare:
				curIndicateur->addArretLogique(
					getArretLogique(vFormatIndicateurs.GetNombre(Tampon, INDICATEURSFORMATCOLONNEStandard)),
					(tTypeGareLigneDA) Tampon[vFormatIndicateurs.GetColonnePosition(INDICATEURSFORMATCOLONNEDepartArrivee)],
					(tTypeGareIndicateur) Tampon[vFormatIndicateurs.GetColonnePosition(INDICATEURSFORMATCOLONNEObligatoire)]
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

	Fichier.close();

	return(true);
}


void cEnvironnement::ConstruitColonnesIndicateur(cIndicateurs* curIndicateur)
{
/*	// Declarations
	bool LigneSelectionnee;
	bool ZoneDepartTerminee;
	bool PassagePasse;
	cColonneIndicateurs* curCI;
	cGareIndicateurs* curGI;
	cGareIndicateurs* GIDepart=NULL;
	cGareLigne* curGareLigne=NULL;
	cGareLigne* GareLigneArrivee=NULL;
	cGareLigne* GareLigneDepart=NULL;
	cGareLigne* GareLigneDepart2=NULL;
	cLigne* curLigne;
	size_t NumeroGare;
	tNumeroService iNumeroService;
	curIndicateur->Reset();

	// A: Pr�paration des colonnes
	for (curLigne = vPremiereLigne; curLigne!=NULL; curLigne = curLigne->Suivant())
	{
		if (curLigne->AAfficherSurIndicateurs())
		{
			LigneSelectionnee = false;
			PassagePasse = false;

			// A1: S�lection de la ligne: Un d�part parmi les d�parts de l'indicateur
			for (curGI = curIndicateur->getPremiereGI(); curGI!=NULL; curGI = curGI->getSuivant())
			{
				if (curGI->EstDepart())
				{
					for (curGareLigne = curLigne->PremiereGareLigne(); curGareLigne!=NULL; curGareLigne = curGareLigne->Suivant())
					{
						if (curGareLigne->EstDepart() && curGareLigne->ArretLogique() == curGI->getArretLogique())
						{
							LigneSelectionnee = true;
							GareLigneDepart = curGareLigne;
							GareLigneDepart2 = curGareLigne;
							if (curGI->EstArrivee() || curGI->Obligatoire() == PassageSuffisant)
								PassagePasse = true;
							break;
						}
					}
					if (LigneSelectionnee)
					{
						GIDepart = curGI;
						break;
					}
				}
			}



			// A2: S�lection de la ligne: Une arriv�e apr�s le d�part

			if (LigneSelectionnee)
			{
				LigneSelectionnee = false;

				for (curGI = GIDepart->getSuivant(); curGI != NULL; curGI = curGI->getSuivant())
				{
					if ((curGI->EstArrivee() && (PassagePasse || curGI->Obligatoire()==PassageSuffisant)) || (curGI->EstDepart() && curGI->EstArrivee()))
					{
						for (curGareLigne = GareLigneDepart->getArriveeSuivante(); curGareLigne != NULL; curGareLigne = curGareLigne->getArriveeSuivante())
						{
							if (curGareLigne->ArretLogique() == curGI->getArretLogique())
							{
								LigneSelectionnee = true;
								break;
							}
						}
						if (LigneSelectionnee)
							break;
					}
				}
			}

			// A3: S�lection de la ligne par les jours de circulation (gain de temps)
			if (LigneSelectionnee)
				if (!curIndicateur->getJC().UnPointCommun(curLigne->GetCirculation()))
					LigneSelectionnee = false;


			// B Cr�ation de la donn�e
				// B1: Remplissage des colonnes

			if (LigneSelectionnee)
			{
				// Parcours colonne par colonne
				for (iNumeroService = 0; iNumeroService < curLigne->NombreServices(); iNumeroService++)
				{
					// Test de JC et rejet des cadences
					if (curLigne->GetTrain(iNumeroService)->getJC()->UnPointCommun(curIndicateur->getJC()) && !curLigne->GetTrain(iNumeroService)->EstCadence())
					{
						// Allocation
						// Renvoi: on stocke uniquement le JC pour un traitement ult�rieur
						// (pour permettre de fusionner deux colonnes, et pour gloabliser le code renvoi)
						curCI = new cColonneIndicateurs(curIndicateur->NombreGares(), curLigne, curLigne->GetTrain(iNumeroService)->getJC());

						// Passages
						NumeroGare = 0;
						ZoneDepartTerminee = false;
						GareLigneDepart = GareLigneDepart2;

						//SET PORTAGE LINUX
						//curCI->CopiePostScript("[");
						curCI->CopiePostScript(cTexte("["));
						//END PORTAGE

						for (curGI = curIndicateur->getPremiereGI(); curGI != NULL; curGI = curGI->getSuivant())
						{
							for (curGareLigne = GareLigneDepart; curGareLigne != NULL; curGareLigne = curGareLigne->Suivant())
							{
								if (curGareLigne->ArretLogique() == curGI->getArretLogique() && (curGareLigne->EstDepart() == curGI->EstDepart() || curGareLigne->EstArrivee() == curGI->EstArrivee()))
								{
									if (curGI->EstArrivee())
										ZoneDepartTerminee = true;
									if (curGareLigne->EstDepart() && curGI->EstDepart())
									{
										curCI->setColonne(NumeroGare, &curGareLigne->getHoraireDepartPremier(iNumeroService));
										GareLigneArrivee = curGareLigne;
										GareLigneDepart = curGareLigne->Suivant();
										break;
									}
									else
									{
										curCI->setColonne(NumeroGare, &curGareLigne->getHoraireArriveePremier(iNumeroService)); // IL Y AVAIT DEPART
										GareLigneArrivee = curGareLigne;
										GareLigneDepart = curGareLigne->Suivant();
										break;
									}
								}
							}
							if (curGareLigne == NULL)
								curCI->setColonne(NumeroGare, NULL);
							NumeroGare++;
						}

						curCI->CopiePostScript(cTexte("]"));

						if (GareLigneDepart2->ArretLogique() == curLigne->PremiereGareLigne()->ArretLogique())
							curCI->setOrigineSpeciale(Terminus);
						if (GareLigneArrivee->ArretLogique() == curLigne->PremiereGareLigne()->Destination()->ArretLogique())
							curCI->setDestinationSpeciale(Terminus);

						// B2: Stockage des colonnes et classement
						curIndicateur->Add(curCI, curLigne->GetTrain(iNumeroService)->getJC());

					}
				}
			}
		}
	}
	curIndicateur->ConstruitRenvois();
*/
}

bool cEnvironnement::ConstruitIndicateur()
{
	/*
//    char* Tampon = (char*) malloc(1000*sizeof(char));
//	cCommune* curCommune = NULL;
	char c;
	size_t NumeroColonne;
	signed int HDispo;
	signed int HRequis;
	size_t NombreTableaux;
	size_t NombreTableauxAEcrire;
	size_t NombreColonnes;
	bool RenvoisEcrits;
//	size_t NumeroPageAbsolu;
	size_t NumeroPageRelatif = 0;

	ofstream FichierSortie;
	// SET PORTAGE LINUX
	FichierSortie.open(NOMFICHIERINDICATEURS.Texte());
	if (!FichierSortie.is_open())
	{
		cout << "*** ERREUR Impossible d'ouvrir le fichier " << NOMFICHIERINDICATEURS << "\n";
		return false;
	}
	//END PORTAGE

	ifstream FichierModele;

	// SET PORTAGE LINUX
	FichierModele.open(NOMFICHIERMODELE.Texte());
	if (!FichierModele.is_open())
	{
		cout << "*** ERREUR Impossible d'ouvrir le fichier " << NOMFICHIERMODELE << "\n";
		return false;
	}
	//END PORTAGE LINUX

	while (!FichierModele.eof())
	{
		c = (char) FichierModele.get();
		if (FichierModele.eof())
			break;
		FichierSortie.put(c);
	}

	HDispo = IndicateursHaut;

	FichierSortie << "\n";

	for (tNumeroIndicateur iIndicateur=0; iIndicateur< vNombreIndicateurs; iIndicateur++)
	{
		if (vIndicateurs[iIndicateur]->CommencePage())
		{
			HDispo = IndicateursHaut;
			FichierSortie << "showpage\n";
		}

		// APPEL A B
		ConstruitColonnesIndicateur(vIndicateurs[iIndicateur]);



		// Comptage des �l�ments � �crire

		NombreColonnes = vIndicateurs[iIndicateur]->NombreColonnes();
		if (NombreColonnes == 0)
			FichierSortie << "newpath (" << vIndicateurs[iIndicateur]->getTitre() << " VIDE) 50 50 moveto show\n";
		else
		{
			NombreTableaux = ((NombreColonnes-1) / IndicateursNombreColonnesMax) + 1;
			if (vIndicateurs[iIndicateur]->NombreRenvois())
				RenvoisEcrits = false;
			else
				RenvoisEcrits = true;
			NumeroPageRelatif = 0;
			NumeroColonne = 0;

			// Ecriture jusqu'a avoir tout ecrit
			while (NombreTableaux != 0 || RenvoisEcrits == false)
			{
				// Tentative d'�criture totale
				if (vIndicateurs[iIndicateur]->NombreRenvois())
					HRequis = IndicateursMarge + IndicateursMarge + IndicateursDistanceTableauHaut + (IndicateursNombreLignesSupplementaires + vIndicateurs[iIndicateur]->NombreGares() + 1) * IndicateursHauteurLigne * NombreTableaux + (vIndicateurs[iIndicateur]->NombreRenvois()+1) * IndicateursHauteurLigne;
				else
					HRequis = IndicateursMarge + IndicateursMarge + IndicateursDistanceTableauHaut + (IndicateursNombreLignesSupplementaires + vIndicateurs[iIndicateur]->NombreGares() + 1) * IndicateursHauteurLigne * NombreTableaux;

				if (HRequis < HDispo)
				{
					vIndicateurs[iIndicateur]->EcritTableaux(HDispo, NumeroColonne, NombreTableaux, true, NumeroPageRelatif, FichierSortie);
					RenvoisEcrits = true;
					NombreTableaux = 0;
					HDispo -= (HRequis - IndicateursMarge);
				}
				else // Tentative d'�criture partielle
				{
					RenvoisEcrits = false;
					for (NombreTableauxAEcrire = NombreTableaux; NombreTableauxAEcrire != 0; NombreTableauxAEcrire--)
					{
						HRequis = IndicateursMarge + IndicateursMarge + IndicateursDistanceTableauHaut + (IndicateursNombreLignesSupplementaires + vIndicateurs[iIndicateur]->NombreGares() + 1) * IndicateursHauteurLigne * NombreTableauxAEcrire;
						if (HRequis < HDispo)
							break;
					}

					// Echec: nouvelle page
					if (NombreTableauxAEcrire == 0)
					{
						HDispo = IndicateursHaut;
						FichierSortie << "showpage\n";
					}
					else
					{
						if (NumeroPageRelatif == 0)
							NumeroPageRelatif = 1;
						vIndicateurs[iIndicateur]->EcritTableaux(HDispo, NumeroColonne, NombreTableauxAEcrire, false, NumeroPageRelatif, FichierSortie);
						NombreTableaux -= NombreTableauxAEcrire;
						HDispo -= (HRequis - IndicateursMarge);
						NumeroColonne += NombreTableauxAEcrire * IndicateursNombreColonnesMax;
						NumeroPageRelatif++;
					}
				}
			}
		}
	}
	// Validation derniere page
	FichierSortie << "showpage\n";

	FichierSortie.close();
	FichierModele.close();
*/
	return(true);
}









/*

// cEnvironnement - ConstruitLigne � partir de la liste de gares INUTILISE
// ____________________________________________________________________________
// L'axe n'est pas document�. Il DOIT �tre renseign� par la suite.
// ____________________________________________________________________________
cLigne* cEnvironnement::ConstruitLigne(LogicalPlace** tbGares, char* newCode, cMateriel* newMateriel)
{
	cLigne* curLigne = new cLigne(newCode, NULL, this);
	curLigne->Materiel = newMateriel;

	cGareLigne* DepartPrecedent = NULL;
	cGareLigne* DepartCorrespondancePrecedent = NULL;
	cGareLigne*		PileDep[NOMBREGARESPILE];
	int				TaillePileDep=0;

	for (int i=0; tbGares[i]!=NULL; i++)
	{
		cGareLigne* curGareLigne = new cGareLigne(curLigne, 0, 1, Passage, tbGares[i], DepartPrecedent, DepartCorrespondancePrecedent, true);
		if (i==0)
			curLigne->PremiereGareLigne = curGareLigne;
		if (DepartPrecedent != NULL)
			DepartPrecedent->ArriveeSuivante = curGareLigne;
		DepartPrecedent = curGareLigne;
		if (tbGares[i]->CorrespondanceAutorisee())
		{
			for (; TaillePileDep==0; TaillePileDep--)
				PileDep[TaillePileDep]->ArriveeCorrespondanceSuivante = curGareLigne;
			DepartCorrespondancePrecedent = curGareLigne;
		}
		TaillePileDep++;
		PileDep[TaillePileDep]=curGareLigne;
	}

	return(curLigne);
}

// � Hugues Romain 2002

// ____________________________________________________________________________
*/




cJC* cEnvironnement::GetJC(const tMasque* MasqueAIdentifer, const cJC& JCBase) const
{
	cJC* curJC;
	cJC* JCIdentifie = NULL;

	for (tIndex iNumeroJC = 0; iNumeroJC < _JC.Taille(); iNumeroJC++)
	{
		curJC = _JC[iNumeroJC];
		if (curJC != NULL)
//			if ((curJC->Categorie >= Categorie) && (TousPointsCommuns(curJC->JoursAnnee, Masque)))
			if (JCBase.TousPointsCommuns(*curJC, MasqueAIdentifer))
				if (JCIdentifie == NULL)
					JCIdentifie = curJC;
				else
					if (curJC->Categorie() < JCIdentifie->Categorie())
						JCIdentifie = curJC;
					else
						if (curJC->Categorie() == JCIdentifie->Categorie() && JCBase.Card(*curJC) < JCBase.Card(*JCIdentifie))
							JCIdentifie = curJC;
	}
	return(JCIdentifie);
}




/*!	\brief M�thode d'enregistrement d'un calendrier de circulation
	\param JCAAjouter Le calendrier � ajouter � l'environnement
	\param Code L'index � donner � l'objet (INCONNU = attribution automatique de l'index au premier index libre connu)
	\return L'index attribu� � l'objet dans l'environnement (INCONNU = echec de l'ajout)
	\todo Placer une section critique pour un usage temps r�el
	\author Hugues Romain
	\date 2003-2005

Pour rappel si besoin, anciens param�tres suppl�mentaires (code en commentaires) :
 Sauvegarde        : Vrai  -> effectue la sauvegarde m�moire morte
 ContenuSauvegarde : Vide  -> sauvegarde les jours circul�s un par un dans un
                              sous-fichier.
                     Sinon -> sauvegarde le contenu de la variable sur le
                              fichier
*/
tIndex cEnvironnement::Enregistre(cJC* JCAAjouter, tIndex Code)
{
	// Se r�server la main
#ifdef UNIX
	pthread_mutex_lock( &mutex_environnement );
#endif
	 
	// D�termination du num�ro du JC
	if (Code == INCONNU)
		Code = ProchainNumeroJC();

	// Cr�ation du lien dans le tableau des pointeurs de l'environnement
	Code = _JC.SetElement(JCAAjouter, Code);
	
	// Test lien effectu� avec succ�s
	if (Code == INCONNU)
		return INCONNU;
	
	// MAJ du code dans l'objet
	JCAAjouter->setIndex(Code);

	// Sauvegarde m�moire morte
/*	if (Sauvegarde)
	{
		// Ouverture fichier
		cSauvegarde* curSauvegarde = new cSauvegarde(vNomFichierJC);
		curSauvegarde->CopieJusqueA(Code, 4);

		// Sauvegarde en tete JC
		*curSauvegarde << FINL;
		*curSauvegarde << "[" << TXT2(JCAAjouter->Code(), TAILLECODEJC) << JCAAjouter << FINL;

		// Contenu
		// SET PORTAGE LINUX
		//if (ContenuSauvegarde.GetTaille())
		if (!strcmp(ContenuSauvegarde,""))
		//END PORTAGE LINUX
			*curSauvegarde << ContenuSauvegarde;
		else
		{
			cTexte NomFichierCommandes;
			NomFichierCommandes << vNomRepEnv;
			NomFichierCommandes.Repertoire();
			NomFichierCommandes << "JoursCirculation\\JC" << TXT(Code) << JCEXTENSION;
			cSauvegarde* curSauvegarde2 = new cSauvegarde(NomFichierCommandes);
			cDate DateDebut;
			DateDebut.setDate(1,1,vPremiereAnnee);
			cDate DateFin;
			DateFin.setDate(31,12,vDerniereAnnee);
			for (; DateDebut <= DateFin; DateDebut++)
				if (JCAAjouter->Circule(DateDebut))
				{
					*curSauvegarde2 << "+";
					DateDebut.toFormatInterne(*curSauvegarde2);
					*curSauvegarde2 << FINL;
				}
			curSauvegarde2->SauteObjet();
			delete curSauvegarde2;
#ifdef ClasseAdmin
	// ATTENTION La desactivation de cette partie dans la version non Admin
	// implique la non conservation des infos d'importation en cas de

	// sauvegarde � l'aide de cette fonction !

	// R�gler cette question avant utilisation de la fonction addJC
	// sur la version SEMVAT
			if (JCAAjouter->ImportSource())
			{
				*curSauvegarde << "I" << TXT2(JCAAjouter->ImportSource()->Code(), 6);
				JCAAjouter->ImportChaineCle(*curSauvegarde);
				*curSauvegarde << FINL;
			}
#endif

			*curSauvegarde << "+>JoursCirculation\\JC" << TXT(Code) << FINL;
		}

		// Fermeture du fichier
		delete curSauvegarde;
	}*/

	// Rendre la main
#ifdef UNIX
	pthread_mutex_unlock( &mutex_environnement );
#endif
	 
	// Sortie
	return Code;
}



void cEnvironnement::JCSupprimerInutiles(bool Supprimer)
{
/*	// Allocation
	bool* JCUtile = (bool*) calloc(vNombreJC, sizeof(bool));

	// D�termination des JC utiles dans les horaires
	for (cLigne* curLigne = PremiereLigne(); curLigne != NULL; curLigne = curLigne->Suivant())
		for (tNumeroService iNumeroService = 0; iNumeroService != curLigne->NombreServices(); iNumeroService++)
			JCUtile[curLigne->getCirculation(iNumeroService).Index()] = true;

	// D�termination des JC utiles dans les d�finitions de JC
	ChargeFichierJC(false, JCUtile);

	// Affichage des JC inutiles
	ofstream FichierListe;
	FichierListe.open(NOMFICHIERJCINUTILES.Texte());
	if (!FichierListe.is_open())
	{
		cout << "*** ERREUR Impossible d'ouvrir le fichier " << NOMFICHIERJCINUTILES << "\n";
		return;
	}

	tIndex iNumeroJC;
	for (iNumeroJC = 0; iNumeroJC < vNombreJC; iNumeroJC++)
	{
		if (!JCUtile[iNumeroJC] && GetJC(iNumeroJC) != NULL)
		{
			FichierListe << "JC Inutile - " << iNumeroJC << FINL;
		}
	}
	FichierListe.close();
	
	// Suppression des JC
	if (Supprimer)
	{
		for (iNumeroJC = 0; iNumeroJC < vNombreJC; iNumeroJC++)
			if (!JCUtile[iNumeroJC] && GetJC(iNumeroJC) != NULL)
				supprimerJC(getJC(iNumeroJC));
	}
*/
}




/*!	\brief Compte le nombre de lignes dont le code correspond au masque fourni
	\param MasqueCode Masque de comparaison du code de ligne
	\author Hugues Romain
	\date 2003

 - Si le masque est vide, toutes les lignes sont compt�es
 - Si le masque est non vide, toutes les lignes dont le d�but du code correspond au masque sont compt�es
 - Si le masque est non vide et compos� de plusieurs codes s�par�s par des points virgules, alors les lignes sont successivement compar�es � chaque code fourni
*/
size_t cEnvironnement::NombreLignes(const cTexte& MasqueCode) const
{
	// Variables locales
	int lNombreLignes = 0;
	int lLongueur = 0;
	int lPosition = 0;

	// Cas 1: Code fourni vide: comptage de toutes les lignes
	if (!MasqueCode.Taille())
		for (cLigne* curLigne = PremiereLigne(); curLigne != NULL; curLigne = curLigne->Suivant())
			lNombreLignes++;
	// Cas 2: Un seul code fourni
	else
	{
		while (lPosition < MasqueCode.Taille())
		{
			// Extraction de la prochaine d�sgnation de ligne (s�parateur ;)
			lLongueur = MasqueCode.RechercheOccurenceGauche(';', 1, lPosition) - lPosition;
			if (lLongueur == MasqueCode.Taille() - lPosition - 1)
				lLongueur += 1;

			// Boucle sur les lignes de l'environnement
			for (cLigne* curLigne = PremiereLigne(); curLigne != NULL; curLigne = curLigne->Suivant())
			{
				// Correspondance au code fourni si non nul, comptage syst�matique sinon
				if (!MasqueCode.Taille() || curLigne->getCode().Compare(MasqueCode, lLongueur, 0, lPosition))
					lNombreLignes++;
			}

			// Avancement de la position
			lPosition += lLongueur;
			lPosition++;
		}
	}

	// Sortie
	return(lNombreLignes);
}



/*!	\brief Date interpr�t�e en fonction d'un texte descriptif et des donn�es de l'environnement
	\param Texte Texte contenant la date ou la commande
	\return La date interpr�t�e d'apr�s le texte
	\author Hugues Romain
	\date 2005
	\warning Les param�tres ne sont pas contr�l�s
	
La date retourn�e est la suivante, selon le texte entr� :
 - date au format texte interne : date transcrite (pas de contr�le) (ex : 20070201 => 1/2/2007)
 - commande de date classique (TEMPS_MIN ('m'), TEMPS_MAX ('M'), TEMPS_ACTUEL ('A'), TEMPS_INCONNU ('?')) : la date correspondante (voir cDate::setDate())
 - texte vide : identique � TEMPS_ACTUEL
 - TEMPS_MIN_ENVIRONNEMENT ('e') : Date minimale possible dans les calendriers de l'environnement (1/1/premi�re ann�e) (voir cEnvironnement::DateMinPossible())
 - TEMPS_MAX_ENVIRONNEMENT ('E') : Date maximale possible dans les calendriers de l'environnement (31/12/derni�re ann�e) (voir cEnvironnement::DateMaxPossible())
 - TEMPS_MIN_CIRCULATIONS ('r') : Premi�re date o� circule au moins un service (voir cEnvironnement::DateMinReelle())
 - TEMPS_MAX_CIRCULATIONS ('R') : Derni�re date o� circule au moins un service (voir cEnvironnement::DateMaxReelle())

La relation suivante est v�rifi�e en fonctionnement nominal : \f$ TEMPS_{INCONNU}<=TEMPS_{MIN}<=TEMPS_{MIN ENVIRONNEMENT}<=TEMPS_{MIN CIRCULATIONS}<=TEMPS_{ACTUEL}<=TEMPS_{MAX CIRCULATIONS}<=TEMPS_{MAX ENVIRONNEMENT}<=TEMPS_{MAX} \f$.
*/
cDate cEnvironnement::dateInterpretee(const cTexte& Texte) const
{
	cDate tempDate;
	if (!Texte.Taille())
		tempDate.setDate(TEMPS_ACTUEL);
	else if (Texte.Taille() == 1)
		switch (Texte[0])
		{
		case TEMPS_MIN_ENVIRONNEMENT: tempDate = DateMinPossible(); break;
		case TEMPS_MAX_ENVIRONNEMENT: tempDate = DateMaxPossible(); break;
		case TEMPS_MIN_CIRCULATIONS: tempDate = DateMinReelle(); break;
		case TEMPS_MAX_CIRCULATIONS: tempDate = DateMaxReelle(); break;
		default: tempDate.setDate(Texte[0]);
		}
	else
		tempDate = Texte;
	return tempDate;
}

bool cEnvironnement::ControleNumerosArretCommuneDesignation(int nA, int nC, int nD, const cTexte& txtA) const
{
	return	(	GetArretLogique(nA) != NULL
			&&	getArretLogique(nA)->GetAccesPADe(nD)
			&&	getArretLogique(nA)->getCommune(nD)->Index() == nC
			&&	getArretLogique(nA)->getNom(nD).Compare(txtA)
			);
}



/*!	\brief M�thode d'enregistrement d'une ligne dans un environnement
	\author Hugues Romain
	\date 2001-2005
	\param Obj Ligne � int�grer � l'environnement
	\return true si l'op�ration a �t� effectu�e avec succ�s

Cette m�thode effectue l'initialisation des param�tres d'une ligne en fonction de l'environnement, ainsi que le cha�nage n�cessaire :
 - La modalit� de r�servation par d�faut de la ligne est la r�servation impossible
 - La modalit� de prise en charge des v�los par d�faut de la ligne est l'interdiction
 - La modalit� d'acceptation des handicap�s par d�faut est la non acceptation
 - Le calendrier de circulation de la ligne est param�tr� sur les ann�es de d�but et de fin de p�riode couverte par l'environnement
 - La ligne suivante de la ligne � rajouter est l'ancienne premier ligne de l'environnement
 - La ligne � ajouter devient la premi�re ligne de l'environnement
 
\warning En raison des �critures de valeurs par d�faut, et des besoins de conna�tre ces valeurs pour compl�ter l'objet ligne, cette m�thode doit �tre appel�e le plus t�t possible apr�s la cr�ation de la ligne. En particulier, aucun service ne doit �tre ajout� � la ligne avant le lancement de cette m�thode.
*/
bool cEnvironnement::Enregistre(cLigne* Obj)
{
	// Valeurs par d�faut selon r�gles de l'environnement
	Obj->setResa(getResa(0));
	Obj->setVelo(getVelo(0));
	Obj->setHandicape(getHandicape(0));
	Obj->setAnneesCirculation(PremiereAnnee(), DerniereAnnee());
	
	// Chainage
	Obj->setSuivant(PremiereLigne());
	vPremiereLigne = Obj;
	
	// Succ�s
	return true;
}






bool	cEnvironnement::ControleNumeroTexteCommune(int nC, const cTexte& txtC) const
{
	return	(	GetCommune(nC) != NULL
			&&	getCommune(nC)->GetNom().Compare(txtC)
			);
}




cJC* cEnvironnement::GetJC(int n) const
{
	// Test de validit� du num�ro
	if (_JC.IndexValide(n))
		return _JC[n];
	else
		return NULL;
}


/*!	\brief D�livre le premier index de calendrier de circulation libre dans l'environnement
	\return le premier index de calendrier de circulation libre dans l'environnement
	\author Hugues Romain
	\date 2001
*/ 
tIndex cEnvironnement::ProchainNumeroJC() const
{
	tIndex i;
	for (i=0; i<_JC.Taille(); i++)
		if (!GetJC(i))
			break;
	return(i);
}



/*!	\brief Date maximale possible dans un calendrier de l'environnement
	\return La date maximale possible dans un calendrier de l'environnement
	\author Hugues Romain
	\date 2001-2005
	
La date maximale possible correspond � la date cod�e par le dernier bit valide des tableaux de donn�es pr�sents dans les calendriers de circulation (voir classe cJourCirculation).

Il s'agit donc du 31 d�cembre de la derni�re ann�e d�clar�e dans le fichier principal des calendriers de circulation (commande "Service")
*/
cDate cEnvironnement::DateMaxPossible() const
{
	cDate tempDate;
	tempDate.setDate(TEMPS_MAX, TEMPS_MAX, vDerniereAnnee);
	return(tempDate);
}


/*!	\brief Date minimale possible dans un calendrier de l'environnement
	\return La date minimale possible dans un calendrier de l'environnement
	\author Hugues Romain
	\date 2001-2005
	
La date minimale possible correspond � la date cod�e par le premier bit valide des tableaux de donn�es pr�sents dans les calendriers de circulation (voir classe cJourCirculation).

Il s'agit donc du 1 janvier de la premi�re ann�e d�clar�e dans le fichier principal des calendriers de circulation (commande "Service")
*/
cDate cEnvironnement::DateMinPossible() const
{
	cDate tempDate;
	tempDate.setDate(TEMPS_MIN, TEMPS_MIN, vPremiereAnnee);
	return(tempDate);
}


cMateriel* cEnvironnement::GetMateriel(tIndex n) const
{
	return vMateriel.at(n);
}




/*!	\brief M�thode d'enregistrement d'un materiel dans l'environnement
	\param newVal Materiel a ajouter
	\return l'index du materiel (INCONNU si le materiel n'a pu �tre ajout�e (contr�le de l'index)
	\author Hugues Romain
	\date 2001-2005
*/
tIndex		
cEnvironnement::Enregistre(cMateriel* materiel, tIndex index) {
	//! \todo throw an exception on duplicate element ([index] != NULL)
	if (index >= vMateriel.size()) {
		// resize the vector
		vMateriel.resize(index+1, NULL);
	}
	vMateriel[index] = materiel;
	return index;
}



/*!	\brief M�thode d'enregistrement d'une modalit� de r�servation dans l'environnement
	\param newVal Modalit� de r�servation � ajouter
	\return l'index de la modalit� de r�servation (INCONNU si la modalit� de r�servation n'a pu �tre ajout�e (contr�le de l'index)
	\author Hugues Romain
	\date 2001-2005
*/
tIndex cEnvironnement::Enregistre(cModaliteReservation* newVal, tIndex index)
{
	//! \todo throw an exception on duplicate element ([index] != NULL)
	if (index >= vResa.size()) {
		// resize the vector
		vResa.resize(index+1, NULL);
	}
	vResa[index] = newVal;
	return index;
}



/*!	\brief M�thode d'enregistrement d'un objet tarification dans l'environnement
	\param newVal La tarification � ajouter � l'environnement
	\param newNum L'index de la tarification dans l'environnement
	\return L'index de la tarification dans l'environnement (INCONNU si la tarification n'a pu �tre enregistr�e)
	\author Hugues Romain
	\date 2001-2005
*/
tIndex cEnvironnement::Enregistre(cTarif *newVal, tIndex index)
{
	//! \todo throw an exception on duplicate element ([index] != NULL)
	if (index >= vTarif.size()) {
		// resize the vector
		vTarif.resize(index+1, NULL);
	}
	vTarif[index] = newVal;
	return index;	
}




tIndex cEnvironnement::Enregistre(cHandicape *newVal, tIndex index)
{
	//! \todo throw an exception on duplicate element ([index] != NULL)
	if (index >= vHandicape.size()) {
		// resize the vector
		vHandicape.resize(index+1, NULL);
	}
	vHandicape[index] = newVal;
	return index;	
}


tIndex cEnvironnement::Enregistre(cVelo *newVal, tIndex index)
{
	//! \todo throw an exception on duplicate element ([index] != NULL)
	if (index >= vVelo.size()) {
		// resize the vector
		vVelo.resize(index+1, NULL);
	}
	vVelo[index] = newVal;
	return index;	
}


tIndex cEnvironnement::Enregistre(cReseau *newVal, tIndex index)
{
	//! \todo throw an exception on duplicate element ([index] != NULL)
	if (index >= vReseau.size()) {
		// resize the vector
		vReseau.resize(index+1, NULL);
	}
	vReseau[index] = newVal;
	return index;	
}



bool        
cEnvironnement::ChargeFichiersRoutes () {
 
  cFichierXML fichierXML ("");
  fichierXML.chargeDonneesRoutes (vNomRepertoireCarto, *this);

  return true;
}



void cEnvironnement::addTown(const cCommune* town)
{
	_towns.add(town->getName(), town);
}

cCommune* cEnvironnement::getTown(size_t id)
{
	return _towns[id];
}

LogicalPlace* cEnvironnement::getLogicalPlace(size_t id)
{
	return _logicalPlaces[id];
}

void cEnvironnement::addLogicalPlace(LogicalPlace* logicalPlace)
{
	_logicalPlaces[logicalPlace->getId()] = logicalPlace;
}


/** Recherche de commune par nom, en correspondance exacte.
	@param name Nom de la commune à chercher
	@return Commune trouvée NULL sinon
*/
cCommune* cEnvironnement::getTown(const std::string& name)	const
{
	for (size_t i=0; i < _towns.size(); ++i)
		if (_towns[i]->getName() == name)
			return _towns[i];
	return NULL;
}