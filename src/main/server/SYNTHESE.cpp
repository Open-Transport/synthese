/*!	\file SYNTHESE.cpp
	\brief Implï¿½mentation classe principale SYNTHESE
*/

#include "SYNTHESE.h"
#include "LogicalPlace.h"
#include "cCommune.h"
#include "cInterface.h"
#include "cDescriptionPassage.h"
#include "cTableauAffichage.h"
#include "cEnvironnement.h"
#include "cCalculItineraire.h"
#include "TimeTables.h"

using namespace std;

#ifdef UNIX
pthread_mutex_t mutex_associateur = PTHREAD_MUTEX_INITIALIZER;
#endif

SYNTHESE::SYNTHESE()
{
	_NiveauLOG = LogInfo;
  	ChargeMessagesStandard();
}

SYNTHESE::~SYNTHESE()
{
}


/*!	\brief Enregistrement d'un site client dans le tableau des pointeurs
	\param __Objet L'objet ï¿½ enregistrer
	\return L'index de l'objet (INCONNU si ï¿½chec)
*/
bool SYNTHESE::Enregistre(cSite* __Objet)
{
	SitesMap::const_iterator iter = _Site.find(__Objet->getClef());

	if (iter != _Site.end())
		delete iter->second;

	_Site[__Objet->getClef()] = __Objet;

	return iter != _Site.end();
}

bool SYNTHESE::Enregistre(cTableauAffichage* __Objet)
{
	DeparturesTablesMap::const_iterator iter = _TableauxAffichage.find(__Objet->getClef());

	if (iter != _TableauxAffichage.end())
		delete iter->second;

	_TableauxAffichage[__Objet->getClef()] = __Objet;

	return iter != _TableauxAffichage.end();
}

bool SYNTHESE::Enregistre(TimeTables* __Objet)
{
	TimeTablesMap::const_iterator iter = _timeTables.find(__Objet->getClef());

	if (iter != _timeTables.end())
		delete iter->second;

	_timeTables[__Objet->getClef()] = __Objet;

	return iter != _timeTables.end();
}




/*!	\brief Messages de log standardisï¿½s
	\todo Amï¿½liorer la description des causes de panne et faire des rï¿½fï¿½rences aux fonctions qui gï¿½nï¿½rent les erreurs
	
Le chargement des libellï¿½s et nivbeaux de messages de log se fait lors de la premiï¿½re crï¿½ation d'objet cLog.

Nomenclature des messages :
<table>
*/
void SYNTHESE::ChargeMessagesStandard()
{
	if (!_MessageStandard.Taille())
	{
		/*! <tr><th colspan="4">Application SYNTHESE</th></tr>
		<tr><td>APP-001</td><td>Normal</td><td>Indique un dï¿½marrage de l'application.</td><td></td></tr>" */
		_CodesMessageStandard.SetElement("APP-001", MESSAGE_APP_START);
		_MessageStandard.SetElement("Démarrage de l'application. Ouverture des fichiers de log standard", MESSAGE_APP_START);
		_NiveauMessageStandard.SetElement(LogInfo, MESSAGE_APP_START);

		/*! <tr><th colspan="4">Sites</th></tr>
		<tr><td>SIT-001</td><td>Erreur fatale</td><td>L'allocation des sites n'a pas fonctionnï¿½.</td><td>Augmenter l'espace mï¿½moire disponible</td></tr>" */
		_CodesMessageStandard.SetElement("SIT-001", MESSAGE_SITES_PBALLOC);
		_MessageStandard.SetElement("Erreur d'allocation fichier des sites", MESSAGE_SITES_PBALLOC);
		_NiveauMessageStandard.SetElement(LogError, MESSAGE_SITES_PBALLOC);

		/*! <tr><td>SIT-002</td><td>Erreur fatale</td><td>Le fichier sites.per n'a pas ï¿½tï¿½ trouvï¿½ dans le rï¿½pertoire de l'environnement.</td><td>Vï¿½rifier que le fichier existe et que son nom est bien orthographiï¿½</td></tr>" */
		_CodesMessageStandard.SetElement("SIT-002", MESSAGE_SITES_PBOUVRIR);
		_MessageStandard.SetElement("Fichier sites.per non trouv2", MESSAGE_SITES_PBOUVRIR);
		_NiveauMessageStandard.SetElement(LogError, MESSAGE_SITES_PBOUVRIR);

		/*! <tr><td>SIT-003</td><td>Element ignorï¿½</td><td>Un site n'a pu ï¿½tre enregistrï¿½ dans l'application ete n sera donc pas disponible.</td><td></td></tr>" */
		_CodesMessageStandard.SetElement("SIT-003", MESSAGE_SITES_PBENREG);
		_MessageStandard.SetElement("Problème d'enregistrement du site", MESSAGE_SITES_PBENREG);
		_NiveauMessageStandard.SetElement(LogWarning, MESSAGE_SITES_PBENREG);
		
		/*! <tr><td>SIT-004</td><td>Element ignorï¿½</td><td>Un site pointe sur un environnement inexistant et ne sera donc pas disponible.</td><td>Vï¿½rifier la valeur du champ environnement dans la description du site</td></tr>" */
		_CodesMessageStandard.SetElement("SIT-004", MESSAGE_SITES_PBENV);
		_MessageStandard.SetElement("Site : Environnement incorrect déclaré", MESSAGE_SITES_PBENV);
		_NiveauMessageStandard.SetElement(LogWarning, MESSAGE_SITES_PBENV);

		/*! <tr><td>SIT-005</td><td>Element ignorï¿½</td><td>Un site pointe sur une interface inexistante et ne sera donc pas disponible.</td><td>Vï¿½rifier la valeur du champ interface dans la description du site</td></tr>" */
		_CodesMessageStandard.SetElement("SIT-005", MESSAGE_SITES_PBINT);
		_MessageStandard.SetElement("Site : Environnement incorrect déclaré", MESSAGE_SITES_PBINT);
		_NiveauMessageStandard.SetElement(LogWarning, MESSAGE_SITES_PBINT);
		

		/*! <tr><th colspan="4">Points d'arrï¿½t</th></tr>
		<tr><td>GAR-001</td><td>Elï¿½ment ignorï¿½</td><td>La dï¿½signation de l'arrï¿½t n'est pas correctement entrï¿½e</td><td>Vï¿½rifier la conformitï¿½ de la dï¿½signation</tr>" */
		_CodesMessageStandard.SetElement("GAR-001", MESSAGE_GARE_DESIGNATION_INCORRECTE);
		_MessageStandard.SetElement("Désignation incorrecte", MESSAGE_GARE_DESIGNATION_INCORRECTE);
		_NiveauMessageStandard.SetElement(LogError, MESSAGE_GARE_DESIGNATION_INCORRECTE);
		
		
		/*! <tr><th colspan="4">Calculateur</th></tr>
		<tr><td>CAL-001</td><td>Calcul annulï¿½</td><td>Le nombre maximal de calculs simultanï¿½s par a ï¿½tï¿½ dï¿½passï¿½.</td><td>Si cela se produit trop souvent, augmenter le nombre de serveurs</td></tr>" */
		_CodesMessageStandard.SetElement("CAL-001", MESSAGE_CALC_SATURE);
		_MessageStandard.SetElement("Nombre de calculs simultanés dépassé", MESSAGE_CALC_SATURE);
		_NiveauMessageStandard.SetElement(LogError, MESSAGE_CALC_SATURE);

	}
	/*! </table> */
}




/*!	\brief Enregistrement d'un environnement dans le tableau des pointeurs
	\param __Objet L'objet ï¿½ enregistrer
	\param __Index L'index de l'objet
	\return L'index de l'objet (INCONNU si ï¿½chec)
	\warning Pas de controle si l'index est dï¿½jï¿½ pris
*/
bool SYNTHESE::Enregistre(cEnvironnement* __Objet)
{
	EnvironmentsMap::const_iterator iter = _Environnement.find(__Objet->Index());

	if (iter != _Environnement.end())
		delete iter->second;

	_Environnement[__Objet->Index()] = __Objet;

	return iter != _Environnement.end();
}



/*!	\brief Enregistrement d'une interface dans le tableau des pointeurs
	\param __Objet L'objet ï¿½ enregistrer
	\param __Index L'index de l'objet
	\return L'index de l'objet (INCONNU si ï¿½chec)
	\warning Pas de controle si l'index est dï¿½jï¿½ pris
	\author Hugues Romain
	\date 2005
*/
bool SYNTHESE::Enregistre(cInterface* __Objet)
{
	InterfacesMap::const_iterator iter = _Interface.find(__Objet->Index());

	if (iter != _Interface.end())
		delete iter->second;

	_Interface[__Objet->Index()] = __Objet;

	return iter != _Interface.end();
}



/*!	\brief Chargement des donnï¿½es depuis les fichiers
	\param __CheminRepertoire Chemin d'accï¿½s complet au rï¿½pertoire contenant les fichiers de base (sans / ï¿½ la fin)
	\param __NombreCalculateursParEnvironnement Nombre d'espaces de calcul ï¿½ allouer pour chaque environnement
	\return true si le chargement a ï¿½tï¿½ effectuï¿½ avec succï¿½s
	\author Hugues Romain
	\date 2005

Le chargement est interrompu au premier refus de fichier, car ces fichiers de donnï¿½es sont fondamentaux, et toute erreur critique sur ceux ci rendrait le service inutilisable.
*/
bool SYNTHESE::Charge(const cTexte& __CheminRepertoire)
{
	// Prï¿½paration du nom des fichiers
	cTexte __CheminFichiers(__CheminRepertoire);
	__CheminFichiers << "/";
	
	// Fichier des environnements
	cFichierEnvironnements __FichierEnvironnements(__CheminFichiers, __CheminFichiers);
	if (!__FichierEnvironnements.Charge())
	{
// 		Erreur("Erreur de chargement du fichier des environnements", "", __CheminFichiers, "");
		return false;
	}
	
	// Fichier des interfaces
	cFichierInterfaces __FichierInterfaces(__CheminFichiers, __CheminFichiers);
	if (!__FichierInterfaces.Charge())
	{
// 		Erreur("Erreur du chargement du fichier des interfaces", "", __CheminFichiers, "");
		return false;
	}
	
	// Fichier des sites
	cFichierSites __FichierSites(__CheminFichiers, __CheminFichiers);
	if (!__FichierSites.Charge())
	{
// 		Erreur("Erreur du chargement du fichier des sites", "", __CheminFichiers, "");
		return false;
	}

	// Fichier des tableaux d'affichage
	cFichierTbDep __FichierTbDep(__CheminFichiers, __CheminFichiers);
	if (!__FichierTbDep.Charge())
	{
		return false;
	}
		
	// Sortie OK
	return true;
}



/*!	\brief Accesseur site d'aprï¿½s sa clï¿½
	\param __Cle Clï¿½ du site ï¿½ trouver
	\return Pointeur vers le site trouvï¿½ s'il est accessible
	\author Hugues Romain
	\date 2005
*/
cSite* SYNTHESE::GetSite(const string& __Cle) const
{
	SitesMap::const_iterator iter = _Site.find(__Cle);
	return (iter != _Site.end() && iter->second->valide()) ? iter->second : NULL;
}



/*!	\brief Gï¿½nï¿½ration de la fiche horaire
	\param pCtxt Flux de sortie sur lequel ï¿½crire les rï¿½sultats
	\param NumeroGareOrigine Index du point d'arrï¿½t d'origine demandï¿½
	\param NumeroGareDestination Index du point d'arrï¿½t de destination demandï¿½
	\param __DateDepart Date du calcul
	\param codePeriode Pï¿½riode de la journï¿½e sur laquelle doit s'effectuer le calcul
	\param velo Etat du filtre transport de vï¿½lo
	\param handicape Etat du filtre pris en charge handicapï¿½s
	\param taxibus Etat du filtre taxibus
	\param tarif Filtre tarification : index de la tarification sï¿½lectionnï¿½e (INCONNU pour pas de filtrage)
	\param NumeroDesignationOrigine Index de la dï¿½signation du lieu d'origine demandï¿½ (au sein du point d'arrï¿½t)
	\param NumeroDesignationDestination Index de la dï¿½signation du lieu de destination demandï¿½ (au sein du point d'arrï¿½t)
	\param __CleSite Clï¿½ du site client
	\param vThreadId Identifiant du thread concernï¿½
	\return true si une sortie a ï¿½tï¿½ effectuï¿½e, false sinon
	\author Hugues Romain
	\date 2001
*/
bool SYNTHESE::FicheHoraire(ostream &pCtxt, ostream& pCerr, const cSite* __Site
						, tIndex NumeroGareOrigine, tIndex NumeroGareDestination, const cDate& __DateDepart
						, tIndex __IndexPeriode, tBool3 velo, tBool3 handicape,tBool3 taxibus, tIndex tarif
						, long vThreadId)
{
	// Test des entrï¿½es
	if(	__Site->getEnvironnement()->getLogicalPlace(NumeroGareOrigine)
	&&	__Site->getEnvironnement()->getLogicalPlace(NumeroGareDestination)
	&&	__Site->getEnvironnement()->ControleDate(__DateDepart)
	&&	__Site->getEnvironnement()->getTarif(tarif)
	){
		// Message d'alerte
		cTexte alerte = cTexte("");

		// Nouveau calculateur
		cCalculateur __Calculateur(__Site->getEnvironnement()
				, __Site->getEnvironnement()->getLogicalPlace(NumeroGareOrigine)
				, __Site->getEnvironnement()->getLogicalPlace(NumeroGareDestination)
				, __DateDepart
				, __Site->getInterface()->GetPeriode(__IndexPeriode)
				, velo
				, handicape
				, taxibus
				, tarif
				, __Site->getSolutionsPassees()
				, 0, 0
				);
		
		// Espace saturï¿½
//		if (!__Calculateur)
//		{
//			FichierLogAcces().Ecrit(MESSAGE_CALC_SATURE, __Site->getIdentifiant());
//			return false;
//		}
		
		// Definition du lien identifiant de thread -> espace calculateur UTILE ?
		_ThreadCalculateur[vThreadId] = &__Calculateur;
		
		//calculateur->getEnvironnement()->FichierLOG() << "start fiche horaire";

		// Calcul fiche horaire
		if (!__Calculateur.FicheHoraire() && (__IndexPeriode != cInterface::ALL_DAY_PERIOD))
		{
			// pas de resultat avec le filtre periode, on relance sur toute la journee (pérvoir alerte)

			FicheHoraire(pCtxt, pCerr, __Site
					, NumeroGareOrigine, NumeroGareDestination, __DateDepart
					, cInterface::ALL_DAY_PERIOD
					, velo, handicape,taxibus, tarif
					, vThreadId);
		}
		else
		{
/*			//Entete XML
			pCtxt << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
			//pCtxt << "<!DOCTYPE fichehoraire SYSTEM \"/bl/fichehoraire.dtd\">";
			pCtxt << "\n<fichehoraire>";

			//Description de la fiche horaire
			pCtxt << "\n<description>";
			pCtxt << "\n<depart>";
			cMoment pNull;
			calculateur->getArretLogique(NumeroGareOrigine)->toXML(pCtxt, pNull, pNull, false);
			pCtxt << "\n</depart>";
			pCtxt << "\n<arrivee>";
			calculateur->getArretLogique(NumeroGareDestination)->toXML(pCtxt, pNull, pNull, false);
			pCtxt << "\n</arrivee>";
			pCtxt << "\n<date jour=\""<< TXT(MomentDepart.Jour()) << "\" mois=\""<< TXT(MomentDepart.Mois()) << "\" annee=\""<< MomentDepart.Annee() << "\"/>";
			pCtxt << "\n</description>";


			cElementTrajet* curTrajet;
			cElementTrajet* curElementTrajet;
			//defilement sur les trajets (colonnes)
			for (curTrajet = calculateur->getSolution(); curTrajet!=NULL; curTrajet = curTrajet->getTrajetSuivant())
			{
				pCtxt << "\n<trajet>";

				//defilement sur les elements du trajet (lignes)
				for (curElementTrajet = curTrajet; curElementTrajet !=NULL; curElementTrajet = curElementTrajet->Suivant())
				{
					curElementTrajet->toXML(pCtxt, MomentDepart , curTrajet->AmplitudeServiceContinu());
				}

				pCtxt << "\n</trajet>";
			}

			pCtxt << "\n</fichehoraire>";
		}
	*/	

			cInterface_Objet_Connu_ListeParametres __Parametres;
			__Parametres << __Site->getEnvironnement()->getLogicalPlace(NumeroGareOrigine)->getTown()->getName();	//0
			__Parametres << __Site->getEnvironnement()->getLogicalPlace(NumeroGareOrigine)->getTown()->getId();	//1
			__Parametres << __Site->getEnvironnement()->getLogicalPlace(NumeroGareOrigine)->getName();	//2
			__Parametres << NumeroGareOrigine;	//3
			__Parametres << __Site->getEnvironnement()->getLogicalPlace(NumeroGareDestination)->getTown()->getName();	//4
			__Parametres << __Site->getEnvironnement()->getLogicalPlace(NumeroGareDestination)->getTown()->getId();	//5
			__Parametres << __Site->getEnvironnement()->getLogicalPlace(NumeroGareDestination)->getName();	//6
			__Parametres << NumeroGareDestination;	//7
			__Parametres << "";	//8 A VIRER
			__Parametres << ""; //9 A VIRER
			__Parametres << __IndexPeriode;//10
			__Parametres << velo;//11
			__Parametres << handicape;//12
			__Parametres << taxibus;//13
			__Parametres << tarif;//14
			cTexteCodageInterne __txtDateDepart;
			__txtDateDepart << __DateDepart;
			__Parametres << __txtDateDepart;	//15
			__Site->Affiche(pCtxt, INTERFACEFicheHoraire, __Parametres, (const void*) &__Calculateur.getSolution());

			// Remise ï¿½ disposition de l'espace de calcul
			_ThreadCalculateur.erase(vThreadId);
		}
		return true;
	}
	else
		return false;
}



/*!	\brief Liste de communes correspondant ï¿½ la recherche
 	\param pCtxt le flux de sortie
 	\param depart true si la liste doit ï¿½tre une commune de dï¿½part, false d'il s'agit d'une commune d'arrivï¿½e
 	\param Entree le mot clï¿½ saisi par l'utilisateur
 	\return true si la fonction a ï¿½tï¿½ effectuï¿½e avec succï¿½s, false sinon
	\author Hugues Romain
	\date 2000-2005

Cette mï¿½thode ne sollicite pas l'usage d'un espace de calcul.
\todo Calculer la liste des communes avant l'affichage et passer le rï¿½sultat ï¿½ la methode d'interface. Pour cela rï¿½cupï¿½rer le parametre nombre d'arrets dans l'interface (ce qui permettra de virer dï¿½finitivement les .ini et compagnie)
 */
bool SYNTHESE::ListeCommunes(ostream &pCtxt, ostream& pCerr, const cSite* __Site
						, bool depart, const cTexte& Entree) const
{
	cInterface_Objet_Connu_ListeParametres __Parametres;
	__Parametres << Entree;

	if (depart)
		__Site->Affiche(pCtxt, INTERFACEListeCommunesDepart, __Parametres);
	else
		__Site->Affiche(pCtxt, INTERFACEListeCommunesArrivee, __Parametres);

	return true;
}

/*!	\brief Gï¿½nï¿½ration de la page d'accueil
 	\param pCtxt le flux de sortie
 	\param __Site Le site
	\return true si une sortie a ï¿½tï¿½ effectuï¿½e, false sinon
	\author Hugues Romain
	\date 2001-2005
	
Cette fonction ne sollicite pas l'usage d'espace de calcul.
 */
bool SYNTHESE::Accueil(ostream &pCtxt, ostream& pCerr, const cSite* __Site) const
{
	// Construction des paramï¿½tres d'interface
	cInterface_Objet_Connu_ListeParametres __Parametres;
	__Parametres << __Site->getClef();
	
	// Lancement de l'affichage
	__Site->Affiche(pCtxt, INTERFACEPageAccueil, __Parametres);
	
	// Sortie
	return true;
}



//BEGIN Une documentation de grande taille est dï¿½crite pour cette entitï¿½ en fin de fichier.     
//END   Prendre garde ï¿½ mettre ï¿½ jour le lien dans ce fichier en cas de changement de prototype.
bool SYNTHESE::ValidFH(ostream &pCtxt, ostream& pCerr, const cSite* __Site
					, const cTexte& txtCD, tIndex nCD, tIndex nAD, tIndex  nDD, 
						const cTexte& txtCA, tIndex nCA, tIndex nAA, tIndex nDA, 
						const cTexte& txtAD, const cTexte& txtAA, 
						const cDate& __DateDepart, tIndex codePeriode,
						tBool3 velo, tBool3 handicape, tBool3 taxibus, tIndex tarif) const
{
    /* patch de test
       aucune intégration
       pour intégration au model synthese, voir version full c++ en cours
       ceci ne fonctionne qu'avec un fichier de réseau de neurones généré avec csv2net
       avec la ligne 115 definie comme suit: final = city+':'+stop
       */
/*    cAssocResult resPD, resPA;
    cAssocResult::iterator it;
    vector<string> output;
    cTexte newtxtCD,newtxtAD,newtxtCA,newtxtAA;
#ifdef UNIX
    pthread_mutex_lock( &mutex_associateur );
#endif
    // utilisation de l'associateur pour le départ
    _Associator->Try(txtCD.Texte(),txtAD.Texte());
    resPD = _Associator->MatchCity(1); // liste des communes
    it = resPD.begin();
    cout << "commune " << it->id << " ";
    resPD = _Associator->MatchPoint(10); // liste des arrets
    // vérification d'ambiguité sur les arrets
    it = resPD.begin();
    _Associator->tokenize(it->id, output, ":");
    cout << "depart " << output[0] << "-" << output[1] << endl;
    newtxtCD = output[0];
    newtxtAD = output[1];
 */   /*
    if((it->score == 100) && (it->delta >= 10)) // && resPT.size() < 10 ??
        cout << "best: " << it->id << endl;
    else for(; it!=resPD.end(); ++it)
        cout << it->id << " " << it->score << " " << it->delta << endl;
        */ /*
    //resPL = _Associator->MatchPlace(10); // liste des adresses
    // vérification d'ambiguité sur les arrets

    // utilisation de l'associateur pour l'arrivée
    _Associator->Try(txtCA.Texte(),txtAA.Texte());
    resPA = _Associator->MatchCity(1); // liste des communes
    it = resPA.begin();
    cout << "commune " << it->id << " ";
    resPA = _Associator->MatchPoint(10); // liste des arrets
    it = resPA.begin();
    _Associator->tokenize(it->id, output, ":");
    cout << "arrivee " << output[0] << "-" << output[1] << endl;
    newtxtCA = output[0];
    newtxtAA = output[1];
*/    /*
    if((it->score == 100) && (it->delta >= 10)) // && resPT.size() < 10 ??
        cout << "best: " << it->id << endl;
    else for(; it!=resPA.end(); ++it)
        cout << it->id << " " << it->score << " " << it->delta << endl;
        */
#ifdef UNIX
    pthread_mutex_unlock( &mutex_associateur );
#endif

        // si pas ambiguité arret mais ambiguité lieu, prendre arret
        // si embiguité arret mais pas ambiguité lieu, demander arret à carto
        // si ambiguite arret et lieu, présenter liste
        // si pas ambiguité arret et pas ambiguité lieu, présenter couple

        // chopper topographie
        // route=getroad(idroute)
        //getcommune(pade)->index()


	// Test des entrï¿½es
	if(	(nCD == INCONNU || __Site->getEnvironnement()->ControleNumeroTexteCommune(nCD, txtCD))
	&&	(nAD == INCONNU || __Site->getEnvironnement()->ControleNumerosArretCommuneDesignation(nAD, nCD, txtAD))
	&&	(nCA == INCONNU || __Site->getEnvironnement()->ControleNumeroTexteCommune(nCA, txtCA))
	&&	(nAA == INCONNU || __Site->getEnvironnement()->ControleNumerosArretCommuneDesignation(nAA, nCA, txtAA))
	&& 	__DateDepart.OK()
	&&	__Site->getEnvironnement()->getTarif(tarif))
	{
		// Déclarations
		vector<cCommune*>	tbCommune;
		vector<LogicalPlace*>	tbPADe;

		// Traitement des entrées
		if (nAD == INCONNU)
		{
			if (nCD == INCONNU)
			{
				tbCommune = __Site->getEnvironnement()->searchTown(string(txtCD.Texte()), 2);
				if (tbCommune.size() == 1)
					nCD = tbCommune[0]->getId();
			}

			if (nCD != INCONNU)
			{
				if (txtAD.Taille() == 0)
				{
					nAD = __Site->getEnvironnement()->getTown(nCD)->getMainLogicalPlace()->getId();
				}
				else
				{
					tbPADe = __Site->getEnvironnement()->getTown(nCD)->searchLogicalPlaces(string(txtAD.Texte()), 2);
					if (tbPADe.size() == 1)
					{
						nAD = tbPADe[0]->getId();
					}
				}
			}
		}

		if (nAA == INCONNU)
		{
			if (nCA == INCONNU)
			{
				tbCommune = __Site->getEnvironnement()->searchTown(string(txtCA.Texte()), 2);
				if (tbCommune.size() == 1)
					nCA = tbCommune[0]->getId();
			}
			if (nCA != INCONNU)
			{
				if (txtAA.Taille() == 0)
				{
					nAA = __Site->getEnvironnement()->getTown(nCA)->getMainLogicalPlace()->getId();
				}
				else
				{
					tbPADe = __Site->getEnvironnement()->getTown(nCA)->searchLogicalPlaces(string(txtAA.Texte()), 2);
					if (tbPADe.size() == 1)
					{
						nAA = tbPADe[0]->getId();
					}
				}
			}
		}

		// Action
		if (nAA != INCONNU && nAD != INCONNU)
		{
			// Message d'attente et lancement de la fiche
			cInterface_Objet_Connu_ListeParametres __Parametres;
			__Parametres << nAD;	//0
			__Parametres << nAA;	//1
			cTexteCodageInterne __txtDate;
			__txtDate << __DateDepart;	
			__Parametres << __txtDate;	//2
			__Parametres << "";	//3
			__Parametres << "";	//4
			__Parametres << codePeriode;	//5
			__Parametres << velo;	//6
			__Parametres << handicape;	//7
			__Parametres << taxibus;	//8
			__Parametres << tarif;	//9
			__Site->Affiche(pCtxt, INTERFACEAttente, __Parametres);
		}
		else
		{
			cInterface_Objet_Connu_ListeParametres __Parametres;
			__Parametres << __Site->getClef();	//0

			__Parametres << ((nCD != INCONNU) ? __Site->getEnvironnement()->getTown(nCD)->getName() : txtCD);	//1
			__Parametres << nCD;	//2
			__Parametres << ((nAD != INCONNU && nDD != INCONNU) ? __Site->getEnvironnement()->getLogicalPlace(nAD)->getName() : txtAD);	//3
			__Parametres << nAD;	//4
			__Parametres << ((nCA != INCONNU) ? __Site->getEnvironnement()->getTown(nCA)->getName() : txtCA);	//5
			__Parametres << nCA;	//6
			__Parametres << ((nAA != INCONNU && nDA != INCONNU) ? __Site->getEnvironnement()->getLogicalPlace(nAA)->getName() : txtAA);	//7
			__Parametres << nAA; //8
			__Parametres << ""; //9
			__Parametres << ""; //10
			__Parametres << __DateDepart.Jour(); //11
			__Parametres << __DateDepart.Mois(); //12
			__Parametres << __DateDepart.Annee(); //13
			__Parametres << codePeriode; //14

			//Message d'alerte
			cTexte alerte = cTexte("");
/*			if (nCD==-1)
\todo REFAIRE HRO				alerte << MESSAGE_ERREUR_COMMUNE_DEPART;
			else if (nCA==-1)
				alerte << MESSAGE_ERREUR_COMMUNE_ARRIVEE;
			else if (nAD==-1)
				alerte << MESSAGE_ERREUR_ARRET_DEPART;
			else if (nAA==-1)
				alerte << MESSAGE_ERREUR_ARRET_ARRIVEE;
*/
			__Parametres << alerte; //15
			__Parametres << velo; //16
			__Parametres << handicape; //17
			__Parametres << taxibus; //18
			__Parametres << tarif; //19
			__Parametres << cTexteCodageInterne() << __DateDepart; //20
			__Site->Affiche(pCtxt, INTERFACEErreurArretsFicheHoraire, __Parametres);
		}

			
		return true;
	}
	else
		return false;
}



/*! \brief Gï¿½nï¿½ration du formulaire de rï¿½servation
 	\param pCtxt le flux de sortie
	\param  tCodeLigne le code de la ligne ï¿½ rï¿½server
	\param  iNumeroService le numero du service ï¿½ rï¿½server
	\param  iNumeroPADepart le numï¿½ro du point d'arret de dï¿½part
	\param  iNumeroPAArrivee le numï¿½ro du point d'arret d'arrivï¿½e
	\param  tDateDepart date de dï¿½part pour la rï¿½servation 
	\param  tClefSite la clef texte du site
	\return 0 si une sortie a ï¿½tï¿½ effectuï¿½e, 1 sinon
	\todo Voir pourquoi ReservationEnLigne renvoie faux
 */
bool SYNTHESE::FormulaireReservation(ostream &pCtxt, ostream& pCerr, const cSite* __Site
					, const cTexte& tCodeLigne, tIndex iNumeroService
					, tIndex iNumeroPADepart, tIndex iNumeroPAArrivee, const cDate& __DateDepart) const
{
	cLigne* curLigne;
	
	if(	(curLigne = __Site->getEnvironnement()->GetLigne(string(tCodeLigne.Texte())))
	&&	__Site->getEnvironnement()->getLogicalPlace(iNumeroPADepart)
	&&	__Site->getEnvironnement()->getLogicalPlace(iNumeroPAArrivee)
	&&	curLigne->GetResa()
//	&& 	curLigne->GetResa()->ReservationEnLigne()
	&&	__DateDepart.OK()
	)
	{
		// Preparation des parametres pour la construction de l'interface
		cInterface_Objet_Connu_ListeParametres __Parametres;
		__Parametres << __Site->getClef();						//0
		__Parametres << curLigne->getCode(); 					//1 code ligne
		__Parametres << iNumeroService;						//2 numero service 
		__Parametres << iNumeroPADepart;	 					//3
		__Parametres << iNumeroPAArrivee;	 					//4
		__Parametres << cTexteCodageInterne() << __DateDepart;	//5
		
		// Creation de l'affichage				
		__Site->Affiche(pCtxt, INTERFACEFormResa, __Parametres);
		
		return true;
	}
	else
		return false;
}



/*!	\brief Enregistrement d'une rï¿½servation
	\param pCtxt le flux de sortie
	\param tClefSite la clef texte du site
	\param CodeLigne le code de la ligne ï¿½ rï¿½server
	\param NumeroService Index du service ï¿½ rï¿½server dans la ligne
	\param iNumeroPADepart Index du point d'arrï¿½t de dï¿½part
	\param iNumeroPAArrivee Index du point d'arrï¿½t d'arrivï¿½e
	\param tDateDepart Moment du dï¿½part (format texte interne)
	\param tNom Nom du client
	\param tPrenom Prï¿½nom du client
	\param tAdresse Adresse du client
	\param tEmail E-mail du client
	\param tTelephone Numï¿½ro de tï¿½lï¿½phone du client
	\param tNumAbonne Numï¿½ro de carte d'abonnement du client
	\param tAdressePAArrivee Adresse d'arrivï¿½e
	\param tAdressePADepart Adresse de dï¿½part
	\param iNombrePlaces Nombre de places ï¿½ rï¿½server
	\return 0 si une sortie a ï¿½tï¿½ effectuï¿½e, 1 sinon
	\author Hugues Romain
	\date 2005
 */
bool SYNTHESE::ValidationReservation(ostream &pCtxt, ostream& pCerr, const cSite* __Site
					, const cTexte& CodeLigne, tIndex NumeroService, tIndex iNumeroPADepart, tIndex iNumeroPAArrivee
					, const cDate& tDateDepart, const cTexte& tNom
					, const cTexte& tPrenom, const cTexte& tAdresse, const cTexte& tEmail
					, const cTexte& tTelephone, const cTexte& tNumAbonne
					, const cTexte& tAdressePADepart, const cTexte& tAdressePAArrivee
					, int iNombrePlaces) const
{
/*	// Variables locales
	const cLigne*	curLigne;
	const LogicalPlace*	curPADepart;
	const LogicalPlace*	curPAArrivee;
	const cTrain*	curService;
	cMoment MomentDepart;
	MomentDepart = tDateDepart;
	cDatabaseManager DatabaseManager;
	const cSite* __Site;
	
	// Contrï¿½le des paramï¿½tres et enregistrement des objets
	if(	(__Site = GetSite(__CleSite))
		&&  	(curLigne = __Site->getEnvironnement()->GetLigne(CodeLigne))
		&&	(curPADepart = __Site->getEnvironnement()->GetArretLogique(iNumeroPADepart))
		&&	(curPAArrivee = __Site->getEnvironnement()->GetArretLogique(iNumeroPAArrivee))
		&&	curLigne->GetResa()
		&& 	curLigne->GetResa()->ReservationEnLigne()
		&&	MomentDepart.OK()
		&&	(curService = curLigne->GetTrain(NumeroService))
		&&	DatabaseManager.connect()
	)
	{
		// Enregistrement de l'objet modalitï¿½ de rï¿½servation en ligne
		cResaEnLigne* curResaEnLigne = (cResaEnLigne*) curLigne->GetResa();
		
		// Lancement de la procï¿½dure de rï¿½servation
		bool Succes = curResaEnLigne->Reserver(	curService
										, curPADepart, curPAArrivee, MomentDepart, tNom, tNomBrut
										, tPrenom, tAdresse, tEmail
										, tTelephone, tNumAbonne
										, tAdressePAArrivee, tAdressePADepart
										, (size_t) iNombrePlaces, &DatabaseManager);
		
		// Preparation des parametres pour la construction de l'interface
		cTexte message;
		if (Succes)
			message = "Rï¿½servation effectuï¿½e";
		else
			message = "Echec : rï¿½servation non effectuï¿½e";
		
		// Construction des paramï¿½tres d'interface
		//! \todo Attention changer le passage de texte : passer la valeur du rï¿½sultat et le message est ï¿½ donner en parametre par l'objet appelant
		cObjetInterfaceParametresStatiques __Parametres;
		__Parametres << __CleSite; 	//0
		__Parametres << message;											//1 message rï¿½ponse ï¿½cran

		// Lancement de l'affichage	
		__Site->Affiche(pCtxt, INTERFACEValidResa, __Parametres);
		
		// Sortie
		return true;
	}
	else
*/		return false;
}

/*!	\brief Annulation de rï¿½servation
	\param pCtxt le flux de sortie
	\param CodeReservation Code de la rï¿½servation ï¿½ annuler
	\param Nom Nom du client dï¿½tenteur de la rï¿½servation
	\param tClefSite la clef texte du site
	\author Hugues Romain
	\date 2005
	\return 0 si une sortie a ï¿½tï¿½ effectuï¿½e, 1 sinon
	\todo intï¿½grer les messages d'erreur ou de validation dans l'interface (ils ont rien compris......)
*/
bool SYNTHESE::AnnulationReservation(ostream &pCtxt, ostream& pCerr, const cSite* __Site, const cTexte& CodeReservation, const cTexte& Nom) const
{
/*	// Gestionnaire de base de donnï¿½es
	cDatabaseManager DatabaseManager;
	const cSite* __Site;
	
	// Contrï¿½le des paramï¿½tres et enregistrement des objets
	if(	(__Site = GetSite(__CleSite))
		&&	DatabaseManager.connect()
	){
		// Rï¿½sultat
		bool Succes = false;
		
		// Prï¿½paration de la requï¿½te de controle des coordonnï¿½es de la rï¿½servation
		cTexte Requete;
		Requete	<< "SELECT " << TABLE_RESERVATION_MOD_RESA
				<< " FROM " << TABLE_RESERVATION
				<< " WHERE LOWER(" << TABLE_RESERVATION_NOM << ")=LOWER('" << cTexteSQL(Nom) << "')"
					<< " AND " << TABLE_RESERVATION_NUMERO << "='" << cTexteSQL(CodeReservation) << "'"
				<< " LIMIT 1;";
		
		// Lancement de la requï¿½te
		if (DatabaseManager.execute(Requete) && DatabaseManager.getResultats()->size())
		{
			Row row;
			const Result * res = DatabaseManager.getResultats();
		  	Result::iterator i = res->begin();
		  	row = *i; 
		  
		  	// Rï¿½cupï¿½ration de la modalitï¿½ de rï¿½servation d'origine
		  	cResaEnLigne* curResa = (cResaEnLigne*) __Site->getEnvironnement()->getResa((int) row[0]);
			
			// Lancement de l'annulation
			Succes = curResa->Annuler(CodeReservation, &DatabaseManager);
		}
		
		// Preparation des parametres pour la construction de l'interface
		cTexte message;
		if (Succes)
			message = "Annulation effectuï¿½e";
		else
			message = "Echec de l'annulation";
			
		// Construction des paramï¿½tres d'interface
		//!	\todo Idem rï¿½sa supprimer les messages en dur dans le code
		cObjetInterfaceParametresStatiques __Parametres;
		__Parametres << __CleSite; 	//0
		__Parametres << message;											//1 message rï¿½ponse ï¿½cran
	
		// Lancement de l'affichage
		__Site->Affiche(pCtxt, INTERFACEValidResa, __Parametres);
		
		// Sortie
		return true;
	}
	else
*/		return false;
}



/*!	\brief Liste de pos d'arrï¿½t correspondant ï¿½ la recherche
	\param pCtxt le flux de sortie
 	\param depart choix de l'objet d'affichage true si liste d'arrï¿½ts pour un dï¿½part false si arrivï¿½e
 	\param Commune le nom de la commune de recherche
 	\param NumeroCommune le numï¿½ro de la commune de recherche si dï¿½jï¿½ connu
 	\param __Arret le mot clï¿½ saisi par l'utilisateur 

Liste des arrï¿½ts de la commune souhaitï¿½e. Si aucune commune sï¿½lectionnï¿½e liste des communes en premier.
*/
bool SYNTHESE::ListeArrets(ostream &pCtxt, ostream& pCerr, const cSite* __Site
				, bool depart, const cTexte& Commune, tIndex NumeroCommune, const cTexte& __Arret) const
{
	cTexte Arret;
	Arret = __Arret;
	
	// 1: Recherche de la commune entrï¿½e.
	cCommune* curCommune = __Site->getEnvironnement()->getTown(NumeroCommune);
    
	if (curCommune == NULL)
	{
		vector<cCommune*> tbCommune = __Site->getEnvironnement()->searchTown(string(Commune.Texte()), 2);
		if (tbCommune.size() != 1)
		{
			pCtxt << "<script>alert(\"Aucune commune " << (depart ? "de départ" : "d'arrivée") << " ne correspond. Veuillez la modifier.\")</script>";
			ListeCommunes(pCtxt, pCerr, __Site, depart, Commune);
		}
		else
			curCommune = tbCommune[0];
	}

	if (curCommune != NULL)
	{
		if (Arret.Taille()==0)
			Arret.Vide();

		cInterface_Objet_Connu_ListeParametres __Parametres;
		__Parametres << Arret;
		__Parametres << curCommune->getId();
		__Site->Affiche(pCtxt, depart ? INTERFACEListeArretsDepart : INTERFACEListeArretsArrivee, __Parametres);
	}
	
	return true;
}



bool SYNTHESE::TableauDepartsGare(ostream& pCtxt, ostream& pCerr, const cTableauAffichage* __TbDep
									, const cMoment& __MomentDepart) const
{
	if (__MomentDepart.OK())
	{
		cDescriptionPassage* __DP = __TbDep->Calcule(__MomentDepart);

/*		cHeure __HeureLimite;
		__HeureLimite.setHeure(2, 0);	//! \todo Mettre en parametre
		if (__MomentFin.getHeure() >= __HeureLimite)
			__MomentFin.addDureeEnJours(1);
		__MomentFin.setHeure(__HeureLimite);
*/					
		// Calcul du tableau des dï¿½parts
//		cDescriptionPassage* __Departs = __Site->getEnvironnement()->ListeDeparts(__ArretLogique, __MomentDepart, __MomentFin, _NombreReponses);
		
		// Preparation des parametres pour la construction de l'interface
		cInterface_Objet_Connu_ListeParametres __Parametres;

		__Parametres << __TbDep->getTitre();

		__Parametres << __TbDep->getNumeroPanneau();
		
		// Creation de l'affichage				
		__TbDep->Affiche(pCtxt, INTERFACETbDepGare, __Parametres, __DP);
		
		return true;
	}
	else
		return false;
}


/*! \brief Exï¿½cution d'une requï¿½te
    \param pCtxt le flux de sortie
    \param pCerr le flux d'erreur
    \param __Requete Requï¿½te ï¿½ exï¿½cuter
    \return true si la requï¿½te a ï¿½tï¿½ effectuï¿½e avec succï¿½s, false sinon
    \author Christophe Romain
    \author Hugues Romain
    \date 2001-2005

Cette mï¿½thode exï¿½cute la requï¿½te fournie en paramï¿½tre (voir cTexteRequeteSYNTHESE pour la documentation du format des requï¿½tes)

Le rï¿½sultat de la requï¿½te est envoyï¿½ sur le flux de sortie, et les rapports d'erreurs sont envoyï¿½s au flux d'erreur.

Cette mï¿½thode effectue les contrï¿½les communs ï¿½ tous les types de requï¿½tes :
 - Controle de la prï¿½sence du nom de la fonction ï¿½ appeler
 - Contrï¿½le de la prï¿½sence et de la validitï¿½ du site client ou du panneau de tï¿½lï¿½affichage client

L'identifiant du thread concernï¿½ doit ï¿½tre renseignï¿½.

La valeur de retour indique si la requï¿½te a donnï¿½ lieu ï¿½ l'ï¿½xï¿½cution d'une fonction, quel que soit le rï¿½sultat produit par la fonction. Les causes d'ï¿½chec possibles sont les suivantes :
 - Nom de fonction non fourni ou incorrect
 - Parametres fournis ï¿½ la fonction invalides
 - Site invalide
*/
bool SYNTHESE::ExecuteRequete(ostream &pCtxt, ostream &pCerr, cTexteRequeteSYNTHESE& __Requete, long vThreadId)
{
    const cTableauAffichage*	__TableauAffichage;
	const cSite*				__Site;
	
	// rï¿½cupï¿½ration du paramï¿½tre fonction obligatoire
	cTexte __Fonction = __Requete.getTexte(REQUETE_COMMANDE_FONCTION);
	if(!__Fonction.Taille())
		return false;

	// Mode tï¿½lï¿½affichage
	if (__TableauAffichage = GetTbDep(string(__Requete.getTexte(REQUETE_COMMANDE_CODE_TABLEAUDEPART).Texte())))
	{
		if (__Fonction == FONCTION_TABLEAU_DEPART_GARE)
			return TableauDepartsGare(pCtxt, pCerr, __TableauAffichage
				, __Requete.getMoment(REQUETE_COMMANDE_DATE)
			);

	}
	// Mode site web
	else if (__Site = GetSite(string(__Requete.getTexte(REQUETE_COMMANDE_SITE).Texte())))
	{
		if (__Fonction == FONCTION_ACCUEIL)
			return Accueil(pCtxt, pCerr, __Site
					);

		if (__Fonction == FONCTION_ANNULATION_RESA)
			return AnnulationReservation(pCtxt, pCerr, __Site
					, __Requete.getTexte(REQUETE_COMMANDE_CODE_RESA)
					, __Requete.getTexte(REQUETE_COMMANDE_CLIENT_NOM)
					);
	        
		if (__Fonction == FONCTION_VALIDATION_RESA)
			return ValidationReservation(pCtxt, pCerr, __Site
					, __Requete.getTexte(REQUETE_COMMANDE_CODE_LIGNE)
					, __Requete.getInt(REQUETE_COMMANDE_NUMERO_SERVICE)
					, __Requete.getInt(REQUETE_COMMANDE_NUMERO_POINT_ARRET_DEPART)
					, __Requete.getInt(REQUETE_COMMANDE_NUMERO_POINT_ARRET_ARRIVEE)
					, __Requete.getDate(REQUETE_COMMANDE_DATE)
					, __Requete.getTexte(REQUETE_COMMANDE_CLIENT_NOM)
					, __Requete.getTexte(REQUETE_COMMANDE_CLIENT_PRENOM)
					, __Requete.getTexte(REQUETE_COMMANDE_CLIENT_ADRESSE)
					, __Requete.getTexte(REQUETE_COMMANDE_CLIENT_EMAIL)
					, __Requete.getTexte(REQUETE_COMMANDE_CLIENT_TELEPHONE)
					, __Requete.getTexte(REQUETE_COMMANDE_CLIENT_NUMERO_ABONNE)
					, __Requete.getTexte(REQUETE_COMMANDE_ADRESSE_ARRIVEE)
					, __Requete.getTexte(REQUETE_COMMANDE_ADRESSE_DEPART)
					, __Requete.getInt(REQUETE_COMMANDE_NOMBRE_PLACES)
					);
		 	
		if (__Fonction == FONCTION_FORMULAIRE_RESA)
			return FormulaireReservation(pCtxt, pCerr, __Site
		 			, __Requete.getTexte(REQUETE_COMMANDE_CODE_LIGNE)
					, __Requete.getInt(REQUETE_COMMANDE_NUMERO_SERVICE)
					, __Requete.getInt(REQUETE_COMMANDE_NUMERO_POINT_ARRET_DEPART)
					, __Requete.getInt(REQUETE_COMMANDE_NUMERO_POINT_ARRET_ARRIVEE)
					, __Requete.getDate(REQUETE_COMMANDE_DATE)
					);
		
		if (__Fonction == FONCTION_FICHE_HORAIRE)
			return FicheHoraire(pCtxt, pCerr, __Site
		 			, __Requete.getInt(REQUETE_COMMANDE_NUMERO_POINT_ARRET_DEPART)
					, __Requete.getInt(REQUETE_COMMANDE_NUMERO_POINT_ARRET_ARRIVEE)
					, __Requete.getDate(REQUETE_COMMANDE_DATE)
					, __Requete.getInt(REQUETE_COMMANDE_PERIODE)
					, __Requete.getBool3(REQUETE_COMMANDE_VELO)
					, __Requete.getBool3(REQUETE_COMMANDE_HANDICAPE)
					, __Requete.getBool3(REQUETE_COMMANDE_TAXIBUS)
					, __Requete.getInt(REQUETE_COMMANDE_TARIF)
					, vThreadId
						);
		
	//	if (__Fonction == FICHE_ARRET)
	//		return FicheArret(pCtxt, pCerr, __Site
	//				, __Requete.getInt(NUMERO_POINT_ARRET)
	//				);
	        
		
	//	if (__Fonction == TABLEAU_DEPART_GARE)
	//		return TableauDepartsGare(pCtxt, pCerr, __Site
	//				, __Requete.getInt(NUMERO_POINT_ARRET)
	//				, __Requete.getInt(NOMBRE_PROPOSITIONS)	// A mettre dans l'interface
	//				);
					
	//	if (__Fonction == MINI_TABLEAU_DEPART)
	//		return MiniTbDepGare(pCtxt, pCerr, __Site
	//				, __Requete.getInt(NUMERO_POINT_ARRET)
	//				, __Requete.getInt(NOMBRE_PROPOSITIONS)	// A mettre dans l'interface
	//				);
		
        /* cela ne fonctionne plus comme ça, donc ces fonctions deviennent inutile
        note: j'ai aussi change interfaces/6/formulaireentree.elementinterface.per
		if (__Fonction == FONCTION_LISTE_COMMUNE)
			return ListeCommunes(pCtxt, pCerr, __Site
					, __Requete.getInt(REQUETE_COMMANDE_SENS) != 0
					, __Requete.getTexte(REQUETE_COMMANDE_RECHERCHE)
					);
		
		if (__Fonction == FONCTION_LISTE_POINT_ARRET)
			return ListeArrets(pCtxt, pCerr, __Site
					, __Requete.getInt(REQUETE_COMMANDE_SENS) != 0
					, __Requete.getTexte(REQUETE_COMMANDE_COMMUNE)
					, __Requete.getInt(REQUETE_COMMANDE_NUMERO_COMMUNE)
					, __Requete.getTexte(REQUETE_COMMANDE_RECHERCHE)
					);
			*/

		if (__Fonction == FONCTION_VALID_FICHE_HORAIRE)
			return ValidFH(pCtxt, pCerr, __Site
					, __Requete.getTexte(REQUETE_COMMANDE_COMMUNE_DEPART)
					, __Requete.getInt(REQUETE_COMMANDE_NUMERO_COMMUNE_DEPART)
					, __Requete.getInt(REQUETE_COMMANDE_NUMERO_POINT_ARRET_DEPART)
					, __Requete.getInt(REQUETE_COMMANDE_NUMERO_DESIGNATION_DEPART)
					, __Requete.getTexte(REQUETE_COMMANDE_COMMUNE_ARRIVEE)
					, __Requete.getInt(REQUETE_COMMANDE_NUMERO_COMMUNE_ARRIVEE)
					, __Requete.getInt(REQUETE_COMMANDE_NUMERO_POINT_ARRET_ARRIVEE)
					, __Requete.getInt(REQUETE_COMMANDE_NUMERO_DESIGNATION_ARRIVEE)
					, __Requete.getTexte(REQUETE_COMMANDE_POINT_ARRET_DEPART)
					, __Requete.getTexte(REQUETE_COMMANDE_POINT_ARRET_ARRIVEE)
					, __Requete.getDate(REQUETE_COMMANDE_DATE)
					, __Requete.getInt(REQUETE_COMMANDE_PERIODE)
					, __Requete.getBool3(REQUETE_COMMANDE_VELO)
					, __Requete.getBool3(REQUETE_COMMANDE_HANDICAPE)
					, __Requete.getBool3(REQUETE_COMMANDE_TAXIBUS)
					, __Requete.getInt(REQUETE_COMMANDE_TARIF)
					);
	}

    return false;
}

/*! \fn bool SYNTHESE::TermineCalculateur(int vThreadId)
    \brief Assure la terminaison forcï¿½e d'un calculateur
    \param vThreadId Identifiant du thread concernï¿½
    \return true si l'opï¿½ration a ï¿½tï¿½ effectuï¿½e avec succï¿½s
    \author Christophe Romain
    \date 2005
	@todo UTILITE ?
  */
bool SYNTHESE::TermineCalculateur(long vThreadId)
{
    if(!_ThreadCalculateur.count(vThreadId))
        return false;
    _ThreadCalculateur.erase(vThreadId);
    return true;
}

/*!	\fn bool SYNTHESE::ValidFH(ostream &pCtxt, ostream &pCerr, const cSite *__Site, const cTexte &txtCD, tIndex nCD, tIndex nAD, tIndex nDD, const cTexte &txtCA, tIndex nCA, tIndex nAA, tIndex nDA, const cTexte &txtAD, const cTexte &txtAA, const cDate &DateDepart, tIndex codePeriode, tBool3 velo, tBool3 handicape, tBool3 taxibus, tNumeroTarif tarif) const

	\brief Valide la saisie du formulaire d'entree de la recherche d'itinï¿½raire
 	\param pCtxt Le flux de sortie sur lequel ï¿½crire les rï¿½sultats
 	\param txtCD Nom de la commune de dï¿½part entrï¿½ dans le formulaire
 	\param nCD Numero de la commune de dï¿½part si dï¿½jï¿½ connu
 	\param nAD Numï¿½ro du point d'arrï¿½t de dï¿½part demandï¿½ si dï¿½jï¿½ connu
 	\param nDD Numï¿½ro de dï¿½signation du point d'arrï¿½t de dï¿½part demandï¿½ si dï¿½jï¿½ connu
 	\param txtCA Nom entrï¿½ de la commune d'arrivï¿½e
 	\param nCA Numero de la commune d'arrivï¿½e si dï¿½jï¿½ connu
 	\param nAA Numï¿½ro de PA d'arrivï¿½e si dï¿½jï¿½ connu
 	\param nDA Numï¿½ro de dï¿½signation du PA d'arrivï¿½e si dï¿½jï¿½ connu
 	\param txtAD Nom entrï¿½ de l'arret de depart
 	\param txtAA Nom entrï¿½ de l'arret d'arrivï¿½e
 	\param Date Date de recherche
 	\param PeriodeJournee Pï¿½riode de la journï¿½e
 	\param handicape Etat du filtre handicape
 	\param velo Etat du filtre velo
 	\param taxibus Etat du filtre taxibus
 	\param tarif code tarification ï¿½ filtrer
	\return true si l'opï¿½ration a ï¿½tï¿½ effectuï¿½e avec succï¿½s
	\author Hugues Romain
	\date 2000-2005

La politique d'affichage est la mï¿½me pour le dï¿½part et l'arrivï¿½e :

<table cellspacing="0" cellpadding="1" class="tableau">
 <tr>
  <th rowspan="3">Commune</th>
  <th colspan="4">Arrï¿½t</th>
 </tr>
 <tr>
  <th rowspan="2">Validï¿½</th>
  <th colspan="2">Validï¿½</th>
  <th rowspan="2">Non saisi</th>
 </tr>
 <tr>
  <th>Non ambigu</th>
  <th>Ambigu</th>
 </tr>
 <tr>
  <th>Validï¿½e</th>
  <td>\ref InterfaceObjetsStandard4 </td>
  <td>\ref InterfaceObjetsStandard4 </td>
  <td>Arrï¿½t rejetï¿½ sur \ref InterfaceObjetsStandard11 </td>
  <td>\ref InterfaceObjetsStandard4 <b>(avec arrï¿½t principal)</b></td>
 </tr>
 <tr>
  <th>Non validï¿½e non ambiguï¿½</th>
  <td>ï¿½&nbsp;Erreur de requï¿½te&nbsp;ï¿½</td>
  <td>\ref InterfaceObjetsStandard4 </td>
  <td> Arrï¿½t rejetï¿½ sur \ref InterfaceObjetsStandard11 </td>
  <td>\ref InterfaceObjetsStandard4 <b>(avec arrï¿½t principal)</b></td>
 </tr>
 <tr>
  <th>Non validï¿½e ambiguï¿½</th>
  <td colspan="4"> Commune rejetï¿½e sur \ref InterfaceObjetsStandard11 </td>
 </tr>
 <tr>
  <th>Non saisie</th>
  <td colspan="4">ï¿½&nbsp;Erreur de requï¿½te&nbsp;ï¿½</td>
 </tr>
</table>

Une commune est dite ï¿½&nbsp;validï¿½e&nbsp;ï¿½ si son numï¿½ro est
fourni (utilisation des guides).

Un point dï¿½arrï¿½t est dit ï¿½&nbsp;validï¿½ si son numï¿½ro estï¿½
fourni (utilisation des guides).

*/ 


cTableauAffichage* SYNTHESE::GetTbDep(const string& __Code) const
{
	DeparturesTablesMap::const_iterator iter = _TableauxAffichage.find(__Code);
	return (iter != _TableauxAffichage.end()) ? iter->second : NULL;
}

void SYNTHESE::SetNiveauLog(tNiveauLog __NiveauLog)
{
	_NiveauLOG = __NiveauLog;
	_FichierLogAcces.SetNiveau(_NiveauLOG);
	_FichierLogBoot.SetNiveau(_NiveauLOG);
}

void SYNTHESE::OuvrirLogs()
{
	if (_CheminLOG.Taille())
	{
		cTexte __Chemin;
		
		__Chemin << _CheminLOG << SEPARATEUR_REPERTOIRE_TXT << LOG_FICHIER_BOOT;
		_FichierLogBoot.Ouvrir(__Chemin);
		
		__Chemin.Vide();
		__Chemin << _CheminLOG << SEPARATEUR_REPERTOIRE_TXT << LOG_FICHIER_ACCES;
		_FichierLogAcces.Ouvrir(__Chemin);
		
		_FichierLogBoot.Ecrit(MESSAGE_APP_START);
	}
}


/*!	\brief Accesseur ï¿½lï¿½ment environnement avec controle de l'index
	\param __Index Index de l'environnement
	\return L'objet demandï¿½, NULL si non trouvï¿½
*/
cEnvironnement* SYNTHESE::GetEnvironnement(size_t __Index) const
{
	EnvironmentsMap::const_iterator iter = _Environnement.find(__Index);
	return (iter != _Environnement.end()) ? iter->second : NULL;
}

/*!	\brief Accesseur ï¿½lï¿½ment interface avec controle de l'index
	\param __Index Index de l'environnement
	\return L'objet demandï¿½, NULL si non trouvï¿½
*/
cInterface* SYNTHESE::GetInterface(size_t __Index) const
{
	InterfacesMap::const_iterator iter = _Interface.find(__Index);
	return (iter != _Interface.end()) ? iter->second : NULL;
}

/*
bool SYNTHESE::InitAssociateur(const cTexte& __NomAssociateur)
{
    _Associator = new cAssociator(__NomAssociateur.Texte());
    return _Associator->IsLoaded();
}
*/
/*!	\brief Accesseur niveau de log
	\return le niveau de log
*/
tNiveauLog SYNTHESE::getNiveauLog() const
{
	return _NiveauLOG;
}


void	SYNTHESE::SetCheminLog(const cTexte& __CheminLog)
{
	_CheminLOG = __CheminLog;
	OuvrirLogs();
}



/*!	\brief Accesseur chemin des log
	\return le niveau de log
*/
const cTexte& SYNTHESE::getCheminLog() const
{
	return _CheminLOG;
}

cLog& SYNTHESE::FichierLogAcces()
{
	return _FichierLogAcces;
}

cLog& SYNTHESE::FichierLogBoot()
{
	return _FichierLogBoot;
}
