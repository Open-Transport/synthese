/** Implémentation classe calculateur d'itinéraires.
	@file cCalculItineraire.cpp
	@author Hugues Romain
	@date 2000-2006
*/
 
#include "cCalculItineraire.h"
#include "cTrajet.h"
#include "cAccesPADe.h"
#include "cArretLogique.h"
#include "SYNTHESE.h"
#include "cAccesReseau.h"

#ifdef UNIX
#include <sys/stat.h>
#include <sys/types.h>
#endif

#ifdef WIN32
#include <direct.h>
#endif

extern SYNTHESE Synthese;
#ifdef UNIX
extern pthread_mutex_t CalculMutex;
#endif

/** Modificateur Environnement de travail.
	\author Hugues Romain
	\date 2000-2005
	\param __Environnement Environnement sur lequel s'effectueront les calculs
	\return true si l'op�ration a �t� effectu�e avec succ�s
*/
bool cCalculateur::setEnvironnement(const cEnvironnement* __Environnement)
{
	// Enregistreur d'erreurs
	bool __OK = true;
	
	vEnvironnement = __Environnement;
	

	__OK = __OK && (vMeilleurTemps = new cMoment[vEnvironnement->NombrePointsArret()]);
	__OK = __OK && (vET = (cElementTrajet**) malloc ((vEnvironnement->NombrePointsArret()) * sizeof(cElementTrajet*)));

	__OK = __OK && (vMeilleurTempsArretPhysique = (cMoment**) malloc((vEnvironnement->NombrePointsArret()) * sizeof(cMoment*)));
	__OK = __OK && (vETArretPhysique = (cElementTrajet***) malloc((vEnvironnement->NombrePointsArret()) * sizeof(cElementTrajet**)));

	for (tIndex iNumeroArretLogique=0; iNumeroArretLogique < vEnvironnement->NombrePointsArret(); iNumeroArretLogique++)
	{
		if	(	vEnvironnement->getArretLogique(iNumeroArretLogique) != NULL
			&&	vEnvironnement->getArretLogique(iNumeroArretLogique)->CorrespondanceAutorisee()
			)
		{
			__OK = __OK && (vMeilleurTempsArretPhysique[iNumeroArretLogique] = new cMoment[vEnvironnement->getArretLogique(iNumeroArretLogique)->NombreArretPhysiques()+1]);
			__OK = __OK && (vETArretPhysique[iNumeroArretLogique] = (cElementTrajet**) malloc((vEnvironnement->getArretLogique(iNumeroArretLogique)->NombreArretPhysiques()+1) * sizeof(cElementTrajet*)));
		}
		else
		{
			vMeilleurTempsArretPhysique[iNumeroArretLogique] = NULL;
			vETArretPhysique[iNumeroArretLogique] = NULL;
		}
	}

	if (__OK)
	{
		Libere();
		return true;
	}
	return false;
}

/*!	\brief Constructeur
	\author Hugues Romain
	\date 2000-2005
	
Le constructeur initialise les tableaux � NULL en attendant qu'ils re�oivent leurs donn�es, et indique que l'espace de calcul n'est pas disponible pour un calcul (car non encore allou�)
*/
cCalculateur::cCalculateur()
{
	_Libre = false;
	vMeilleurTemps = NULL;
	vET = NULL;
	vMeilleurTempsArretPhysique = NULL;
	vETArretPhysique = NULL;
	_BaseTempsReel = false;
}

/*!	\brief Destructeur
*/
cCalculateur::~cCalculateur()
{
	tIndex iNumeroArretLogique=0;
	for (;iNumeroArretLogique < vEnvironnement->NombrePointsArret(); iNumeroArretLogique++)
	{
		if (	vEnvironnement->getArretLogique(iNumeroArretLogique) != NULL
		&&	vEnvironnement->getArretLogique(iNumeroArretLogique)->CorrespondanceAutorisee())
		{
			delete[] vMeilleurTempsArretPhysique[iNumeroArretLogique];
			free(vETArretPhysique[iNumeroArretLogique]);
		}
	}
	free(vMeilleurTempsArretPhysique);
	free(vETArretPhysique);
	free(vET);
	delete[] vMeilleurTemps;

	//vEnvironnement->FichierLOG() << "Destruction Calculateur\n";
}



/*!	\brief Calcul du meilleur trajet � moment de d�part connu en effectuant la d�termination de la meilleure arriv�e puis en optimisant l'heure de d�part
	\retval __Resultat Variable o� �crire le r�sultat du calcul
	\return true si un calcul a pu �tre men� � bien quel qu'en soit le r�sultat, false sinon
	\author Hugues Romain
	\date 2001-2005
*/
bool cCalculateur::HoraireDepartArrivee(cTrajet& __Resultat)
{
	// Variables locales
	cTrajet		__TrajetEffectue;
	
	// Mises � z�ro
	vDepartMin = vMomentDebut;
	vArriveeMax = vMomentFin;
	__Resultat.Vide();

	// Meilleures arriv�es
	ResetMeilleuresArrivees();
	SetMeilleureArrivee(vPADeDestination, vArriveeMax);
	SetMeilleureArrivee(vPADeOrigine, vMomentDebut);
	
	// Calcul de la meilleure arriv�e possible
	_LogTrace.Ecrit(LogDebug, "Recherche de la meilleure arriv�e", "");
	if (!MeilleureArrivee(__Resultat, __TrajetEffectue, false, false))
		return false;

	// Si un trajet a �t� trouv�, tentative d'optimisation en retardant au maximum l'heure de d�part
	if (__Resultat.Taille())
	{
		_LogTrace.Ecrit(LogDebug, "Recherche du meilleur d�part", "");
		
		// Meilleurs d�parts
		ResetMeilleursDeparts();
		for (cElementTrajet* TempET = __Resultat.getPremierElement(); TempET!=NULL; TempET = TempET->getSuivant())
			SetMeilleurDepart(TempET->getGareDepart(), TempET->MomentDepart(), TempET->VoieDepart());
		SetMeilleurDepart(vPADeDestination, __Resultat.getMomentArrivee());
		
		// Bornes du calcul
		vDepartMin = __Resultat.getMomentDepart();
		vArriveeMax = __Resultat.getMomentArrivee();

		return MeilleurDepart(__Resultat, __TrajetEffectue, true, true);
	}
	return true;
}

/*
cElementTrajet* cEnvironnement::HoraireArriveeDepart(cArretLogique* GareOrigine, cArretLogique* GareDestination, cMoment& MomentArrivee, cMoment& DepartMin, DureeEnMinutes DureeMax, TSituation Situation, bool EcritLOG)
{
	cElementTrajet* TempET = new cElementTrajet;
	cElementTrajet* Resultat;
	cMoment MomentMin;

//	IterationsArr = 0;
//	IterationsDep = 0;
	ResetMeilleuresArrivees();
	ResetMeilleursDeparts();

	TempET->GareDepart = GareDestination;
	TempET->MomentDepart = MomentArrivee;
	GareDestination->SetMeilleurDepart(MomentArrivee);
	GareOrigine->SetMeilleureArrivee(DepartMin);

	Resultat = MeilleurDepart(GareOrigine, DepartMin, TempET->MomentDepart, TempET, false, false, false, DureeMax, Situation, EcritLOG);

#ifdef DOS
	cout << "-----------DESCENTE---------\n";
	AfficheTrajet(Resultat);
#endif

	if (Resultat != NULL)
	{
		MomentMin = Resultat->Dernier->MomentArrivee;
		//		MomentMin.Heure += 1;
		TempET->GareArrivee = GareOrigine;
		TempET->MomentArrivee = Resultat->MomentDepart;
		Resultat->deleteTrajet();

		Resultat = MeilleureArrivee(GareDestination, MomentMin, TempET->MomentArrivee, TempET, false, true, true, DureeMax, Situation, EcritLOG);
		delete TempET;
	}
	return(Resultat);
	*/
/*	return(NULL);
}*/

/*!	\brief Calcul de la fiche horaire en tenant compte des param�tres d�j� fournis dans la classe
	\return true si la recherche d'itin�raires a �t� effectu�e avec succ�s, false sinon.
	\author Hugues Romain
	\date 2000-2005

Le r�sultat du calcul (null ou non) se trouve dans la variable vSolution.

Lors de chaque calcul de solution, la variable vMomentDepartMin avance tandis que vMomentArriveeMax reste bloqu�e en fin de plage.

Si une ou plusieurs solutions ont �t� trouv�es, cette m�thode effectue un ordonnancement des lignes d'affichage avant de terminer.

Cette m�thode effectue les op�rations suivantes :<ul>
*/
bool cCalculateur::FicheHoraire()
{
	cTrajet __Trajet;
	bool	__AllocationOK = true;
	vSolution.Vide();
	vDureeServiceContinuPrecedent = 0;
	vIterationsArr = 0;
	vIterationsDep = 0;
	
	// Gestion des logs de debug
	if (Synthese.getNiveauLog() <= LogDebug && Synthese.getCheminLog().Taille())
	{
		cTexte __Chemin;
		cTexteCodageInterne __MomentInterne;
		cMoment __Maintenant;
		__Maintenant.setMoment();
		__MomentInterne << __Maintenant;
		
		_CheminLog << Synthese.getCheminLog() << "/" << __MomentInterne;
#ifdef UNIX
		mkdir(_CheminLog.Texte(), 0770);
#endif
#ifdef WIN32
		mkdir(_CheminLog.Texte());
#endif
		
		__Chemin << _CheminLog << "/" << LOG_FICHIER_TRACE;
		_LogTrace.Ouvrir(__Chemin);
		
		__Chemin.Vide();
		__Chemin << _CheminLog << "/" << LOG_CHEMIN_DESTINATIONS;
#ifdef UNIX
		mkdir(__Chemin.Texte(), 0770);
#endif
#ifdef WIN32
		mkdir(__Chemin.Texte()); 
#endif
		
		__Chemin.Vide();
		__Chemin << _CheminLog << "/" << LOG_CHEMIN_PROVENANCES;
#ifdef UNIX
		mkdir(__Chemin.Texte(), 0770);
#endif
#ifdef WIN32
		mkdir(__Chemin.Texte()); 
#endif
	}
	

	/*!	<li>Boucle tant que l'on se trouve sur la plage horaire restreinte � l'utile</li><ul>	*/
	while(true)
	{
		//!	<li>Calcul du prochain trajet</li>
		_LogTrace.Ecrit(LogDebug, "Nouveau calcul de trajet", "");
		if (!HoraireDepartArrivee(__Trajet))
		{
			__AllocationOK = false;
			break;
		}

		//!	<li>Si rien trouv� et si service pr�c�dent continu alors reprise a l'issue de la p�riode de continuit�</li>
		if (!__Trajet.Taille() && vSolution.Taille() && vSolution.getDernier().getAmplitudeServiceContinu().Valeur())
		{
			vMomentDebut = vDernierDepartServiceContinuPrecedent;
			vMomentDebut += cDureeEnMinutes(1);
			vDureeServiceContinuPrecedent = 0;
			if (!HoraireDepartArrivee(__Trajet))
			{
				__AllocationOK = false;
				break;
			}
		}

		/*!	<li>Si toujours rien trouv� fin du calcul, sortie de la boucle</li>	*/
		if (!__Trajet.Taille())
			break;

		/*!	<li>En cas de rupture du service continu pr�c�dent, correction de de son amplitude</li>	*/
		if (vSolution.Taille() && vSolution.getDernier().getAmplitudeServiceContinu().Valeur() && __Trajet.getMomentDepart() <= vDernierDepartServiceContinuPrecedent)
		{
			cDureeEnMinutes Duree;
			Duree = __Trajet.getMomentArrivee() - vSolution.getDernier().getMomentArrivee();
			Duree--;
			vSolution.getDernier().setAmplitudeServiceContinu(Duree);
		}
		
		/*!	<li>En cas de nouveau service continu, enregistrement de valeur pour le calcul de la prochaine solution</li>	*/
		if (__Trajet.getAmplitudeServiceContinu().Valeur() > 1)
		{
			vDureeServiceContinuPrecedent = __Trajet.getMomentArrivee() - __Trajet.getMomentDepart();
			vDernierDepartServiceContinuPrecedent = __Trajet.getMomentDepart();
			vDernierDepartServiceContinuPrecedent += __Trajet.getAmplitudeServiceContinu();
		}
		else
			vDureeServiceContinuPrecedent = 0;
		
		/*!	<li>Avancement direct de l'heure de d�but de recherche au prochain d�part	(vraiment meilleur qu'un ++ ?)</li> */
		vMomentDebut = __Trajet.getArretLogiqueDepart()->MomentDepartSuivant(__Trajet.getMomentDepart(), vMomentFin, _MomentCalcul);
		
		/*!	<li>Enregistrement de la solution dans le tableau des r�sultats</li>	*/
		vSolution.SetElement(__Trajet, INCONNU, false);
		
		// Sortie 2 A REVOIR ON PEUT ECONOMISER LA DERNIERE RECHERCHE SI CA MARCHAIT
		if (vSolution.getDernier().getMomentArrivee() == vMomentFin)
			break;

	}	/*! </ul>	*/

	// Traitement pr�alable de la liste de trajets si besoin
	vSolution.Finalise();
	
	_LogTrace.Fermer();
	
	return __AllocationOK;
}	/*! </ul> */

/*

// ListeDeparts 1.0 - Fabrication de la liste des d�parts d'un point d'arr�t
//____________________________________________________________________________
//
// curPA: Point d'arr�t � �tudier
//
// MomentDebut et MomentFin: indique la plage temporelle sur laquelle les
// d�parts doivent etre donn�s.
//
// NombreReponses: facultatif, indique le nombre de d�parts � donner. L'absence
// de param�tre signifie que la fonction rendra tous les d�parts de la plage.
// (valeur -1)
//
// ERR PROBABLE A RESOUDRE: Il est possible que le passage � JPLUS sup�rieur
// au sein d'une meme ligne ne soit pas g�r�. Rajouter alors un controle de
// changement de jplus et mettre curdp sur premierdp en cas de changement. A
// tester avec de telles lignes.
//____________________________________________________________________________
cDescriptionPassage* cEnvironnement::ListeDeparts(cArretLogique* curPA, cMoment MomentDebut, cMoment& MomentFin, signed int NombreReponses = -1)
{
	// Variables
	cGareLigne* curGLD;
	tNumeroService iNumeroService;
	cDescriptionPassage* PremierDP = new cDescriptionPassage(MomentDebut); // Sentinelle
	cDescriptionPassage* curDP; // Curseur;
	cDescriptionPassage* newDP;
	cMoment tempMoment;
	cHoraire tempHoraire;
	DureeEnMinutes tempAttente;
	int nombreDP = 0;

	while (MomentDebut.Date <=MomentFin.Date)
	{

		// Parcours sur toutes les lignes au d�part et sur tous les services

		for (curGLD=curPA->PremiereGareLigneDep; curGLD!= NULL; curGLD=curGLD->PADepartSuivant)
			if (curGLD->Ligne->AAfficherSurTableauDeparts)
			{
				curDP = PremierDP;
				for (iNumeroService=0; iNumeroService<curGLD->Ligne->NombreServices; iNumeroService++)
//					if (curGLD->Circule(iNumeroService, Depart, MomentDebut.Date))
					{
						tempMoment = MomentDebut;
						if ((curGLD->HeureDepartDA(iNumeroService, tempMoment, tempHoraire, tempAttente)) && (tempMoment < MomentFin))
						{
							// Recherche de l'emplacement
							for (; ((curDP->Suivant != PremierDP) && (curDP->Suivant->Moment < tempMoment)); curDP = curDP->Suivant)
							{ }

							// Si d�passement du nombre de DP voulus

							if (nombreDP != NombreReponses)
							{
								newDP = new cDescriptionPassage();
								nombreDP++;
							}
							else
							{
								newDP = PremierDP->Precedent;
								if (curDP->Suivant != newDP)
								{
									PremierDP->Precedent = newDP->Precedent;
									PremierDP->Precedent->Suivant = PremierDP;
								}
							}
							if (nombreDP == NombreReponses)
								MomentFin = PremierDP->Precedent->Moment;

							newDP->Remplit(tempMoment, curGLD, iNumeroService, tempHoraire, tempAttente);
							if (curDP->Suivant != newDP)
							{
								newDP->Suivant = curDP->Suivant;
								newDP->Precedent = curDP;
								curDP->Suivant = newDP;
								newDP->Suivant->Precedent = newDP;

						}

					}
			}

		MomentDebut.Date++;
		MomentDebut.Heure.setMinimum();
	}
	PremierDP->Precedent->Suivant = NULL;
	curDP = PremierDP->Suivant;
	delete PremierDP;
	return(curDP);
}

// � Hugues Romain 2001

// ____________________________________________________________________________
*/

/*!	\brief Evaluation de l'opportunit� d'une gare ligne d'arriv�e de figurer dans la liste des destinations directes propos�es, pour donner lieu � une nouvelle r�cursivit� de MeilleureArrivee
	\param __GareLigneArr Gareligne d'arriv�e � �tudier
	\param __MomentDepart Date/Heure du d�part sur la ligne ayant conduit � l'arriv�e fournie
	\param __GareLigneDep Gareligne de d�part ayant conduit � l'arriv�e fournie
	\param __IndexService Index du service utilis� au sein de la ligne
	\param __SuiteElementsTrajets Objet trajet servant � indexer tous les �l�ments stock�s
	\param __TrajetEffectue Trajet effectu� avant les �l�ments test�s
	\param __OptimisationAFaire Indique si le but de la recherche est la d�finition de la meilleurs solution � heuer d'arriv�e connue (true) ou bien une simple recherche d'heure de meilleur arriv�e (false)
	\param __AmplitudeServiceContinu Amplitude du service continu en cours
	\return false si le parcours de la ligne doit �tre interrompu car n'ayant aucune chance de produire d'�l�ment de trajet utile
	\author Hugues Romain
	\date 2005
	
	
*/
inline bool cCalculateur::EvalueGareLigneArriveeCandidate(const cGareLigne* __GareLigneArr, const cMoment& __MomentDepart, const cGareLigne* __GareLigneDep, tIndex __IndexService, cTrajet& __SuiteElementsTrajets, const cTrajet& __TrajetEffectue, bool __OptimisationAFaire, const cDureeEnMinutes& __AmplitudeServiceContinu, cLog& __LogTrace)
{
	if (!__GareLigneArr)
		return true;
		
	// Heure d'arriv�e
	cMoment	__MomentArrivee = __MomentDepart;
	__GareLigneArr->CalculeArrivee(*__GareLigneDep, __IndexService, __MomentDepart, __MomentArrivee);
							
	// Utilit� de la ligne et du point d'arret
	cDistanceCarree __DistanceCarreeBut;
	if (!DestinationUtilePourArriverTot(__GareLigneArr->ArretLogique(), __MomentArrivee, __DistanceCarreeBut))
		return false;
							
	// Ruptures de services continus
	if (vDureeServiceContinuPrecedent.Valeur())
	{
		if (__TrajetEffectue.Taille())
		{
			if (__TrajetEffectue.getMomentDepart() <= vDernierDepartServiceContinuPrecedent 
			&& __MomentArrivee - __TrajetEffectue.getMomentDepart() >= vDureeServiceContinuPrecedent)
				return false;
		}
		else
		{
			if (__MomentDepart < vDernierDepartServiceContinuPrecedent && __MomentArrivee - __MomentDepart >= vDureeServiceContinuPrecedent)
				return false;
		}
	}

	// Ecriture de l'ET si n�cessaire
	if ((__MomentArrivee < GetMeilleureArrivee(__GareLigneArr->ArretLogique(), __GareLigneArr->ArretPhysique()))
	|| (__OptimisationAFaire && __MomentArrivee == GetMeilleureArrivee(__GareLigneArr->ArretLogique(), __GareLigneArr->ArretPhysique())))
	{
		cElementTrajet* __ElementTrajet;
		// On fait mieux, donc ET doit etre gard�.
		if (vPADeDestination->inclue(__GareLigneArr->ArretLogique()))
			__ElementTrajet = GetET(__GareLigneArr->ArretLogique());
		else
			__ElementTrajet = GetET(__GareLigneArr->ArretLogique(), __GareLigneArr->ArretPhysique());

		bool __ETCree = false;
		if (__ElementTrajet == NULL)
		{
			__ElementTrajet = new cElementTrajet;								// Allocation
			__SuiteElementsTrajets.LieEnPremier(__ElementTrajet);			__ElementTrajet->setArretLogiqueArrivee(__GareLigneArr->ArretLogique());	// Ecriture gare d'arriv�e
			SetET(__GareLigneArr->ArretLogique(), __ElementTrajet, __GareLigneArr->ArretPhysique());	// Stockage dans la gare pour reutilisation eventuelle
			__ElementTrajet->setArretPhysiqueArrivee(__GareLigneArr->ArretPhysique());
			__ElementTrajet->setDistanceCarreeObjectif(__DistanceCarreeBut);
			__ETCree = true;
		}
		__ElementTrajet->setArretLogiqueDepart(__GareLigneDep->ArretLogique());		// Ecriture gare de d�part
		__ElementTrajet->setAmplitudeServiceContinu(__AmplitudeServiceContinu);
		__ElementTrajet->setArretPhysiqueDepart(__GareLigneDep->ArretPhysique());
		__ElementTrajet->setLigne(__GareLigneDep->Ligne());						// Ecriture ligne
		__ElementTrajet->setMomentArrivee(__MomentArrivee);						// Ecriture Moment d'arriv�e
		__ElementTrajet->setMomentDepart(__MomentDepart);						// Ecriture Moment de d�part
		__ElementTrajet->setService(__IndexService);							//Ecriture du num�ro de service
		
		// Gestion de logs
		if (Synthese.getNiveauLog() <= LogDebug)
		{
			cTexte __Message;
			if (__ETCree)
				__Message << "***CREATION***";
			if (__MomentArrivee <= __MomentDepart)
			{
				// Placer un breakpoint ici pour g�rer ce type d'erreur
				__Message << "***ERREUR CHRONOLOGIE***";
			}
			__LogTrace.Ecrit(LogDebug, __ElementTrajet, __Message, "");
		}
		
		if (vPADeDestination->inclue(__GareLigneArr->ArretLogique()))
		{
			SetMeilleureArrivee(__GareLigneArr->ArretLogique(), __MomentArrivee);	// Enregistrement meilleure arriv�e

			if (__OptimisationAFaire)
				vArriveeMax = __MomentArrivee;
			else
				vArriveeMax = vPADeDestination->momentArriveePrecedente(__MomentArrivee, vMomentDebut);	// Ecriture MomentMax le cas �ch�ant
		}
		else
			SetMeilleureArrivee(__GareLigneArr->ArretLogique(), __MomentArrivee, __GareLigneArr->ArretPhysique());	// Enregistrement meilleure arriv�e
	}
	
	// Retour arr�ter le parcours de la ligne si la destination a �t� atteinte
	return !vPADeDestination->inclue(__GareLigneArr->ArretLogique());
}



/*!	\brief Evaluation de l'opportunit� d'une gare ligne de d�part de figurer dans la liste des provenances directes propos�es, pour donner lieu � une nouvelle r�cursivit� de MeilleurDepart
	\param __GareLigneDep Gareligne de d�part � �tudier
	\param __MomentArrivee Date/Heure de l'arriv�e sur la ligne ayant conduit au d�part fourni
	\param __GareLigneArr Gareligne d'arriv�e ayant conduit au d�part fourni
	\param __IndexService Index du service utilis� au sein de la ligne
	\param __SuiteElementsTrajets Objet trajet servant � indexer tous les �l�ments stock�s
	\param __TrajetEffectue Trajet effectu� apr�s les �l�ments test�s
	\param __OptimisationAFaire Indique si le but de la recherche est la d�finition de la meilleurs solution � heure de d�part connu (true) ou bien une simple recherche d'heure de meilleur d�part (false)
	\param __AmplitudeServiceContinu Amplitude du service continu en cours
	\return false si le parcours de la ligne doit �tre interrompu car n'ayant aucune chance de produire d'�l�ment de trajet utile
	\author Hugues Romain
	\date 2005
	
	Cette fonction teste la compatibilit� avec la modalit�s de r�servation de la ligne, s'agissant de la premi�re d�couverte de l'heure de d�part
*/
inline bool cCalculateur::EvalueGareLigneDepartCandidate(const cGareLigne* __GareLigneDep, const cMoment& __MomentArrivee, const cGareLigne* __GareLigneArr, tIndex __IndexService, cTrajet& __SuiteElementsTrajets, const cTrajet& __TrajetEffectue, bool __OptimisationAFaire, const cDureeEnMinutes& __AmplitudeServiceContinu)
{
	if (!__GareLigneDep)
		return true;
		
	// Heure de d�part
	cMoment	__MomentDepart = __MomentArrivee;
	__GareLigneDep->CalculeDepart(*__GareLigneArr, __IndexService, __MomentArrivee, __MomentDepart);

	// Test modalit� de r�servation
	if (!__GareLigneDep->Ligne()->getTrain(__IndexService)->ReservationOK(__MomentDepart, _MomentCalcul))
		return false;

	// Utilit� de la ligne et du point d'arret
	cDistanceCarree __DistanceCarreeBut;
	if (!ProvenanceUtilePourPartirTard(__GareLigneDep->ArretLogique(), __MomentDepart, __DistanceCarreeBut))
		return false;
	
	// Controle de non d�passement de Dur�emax
	if (vDureeServiceContinuPrecedent.Valeur())
	{
		if (__MomentDepart <= vDernierDepartServiceContinuPrecedent)
			if (__TrajetEffectue.Taille())
			{
				if ((__TrajetEffectue.getMomentArrivee() - __MomentDepart) >= vDureeServiceContinuPrecedent)
					return false;
			}
			else
				if ((__MomentArrivee - __MomentDepart) >= vDureeServiceContinuPrecedent)
					return false;
	}

	// Ecriture de l'ET si n�cessaire
	if ((__MomentDepart > GetMeilleurDepart(__GareLigneDep->ArretLogique(), __GareLigneDep->ArretPhysique()))
	|| (__OptimisationAFaire && __MomentDepart == GetMeilleurDepart(__GareLigneDep->ArretLogique(), __GareLigneDep->ArretPhysique())))
	{
		cElementTrajet* __ElementTrajet;
		// On fait mieux, donc ET doit etre gard�.
		if (vPADeOrigine->inclue(__GareLigneDep->ArretLogique()))
			__ElementTrajet = GetET(__GareLigneDep->ArretLogique());
		else
			__ElementTrajet = GetET(__GareLigneDep->ArretLogique(), __GareLigneDep->ArretPhysique());

		if (__ElementTrajet == NULL)
		{
			__ElementTrajet = new cElementTrajet;								// Allocation
			__SuiteElementsTrajets.LieEnPremier(__ElementTrajet);			__ElementTrajet->setArretLogiqueDepart(__GareLigneDep->ArretLogique());	// Ecriture gare d'arriv�e
			SetET(__GareLigneDep->ArretLogique(), __ElementTrajet, __GareLigneDep->ArretPhysique());	// Stockage dans la gare pour reutilisation eventuelle
			__ElementTrajet->setArretPhysiqueDepart(__GareLigneDep->ArretPhysique());
			__ElementTrajet->setDistanceCarreeObjectif(__DistanceCarreeBut);
		}
		__ElementTrajet->setArretLogiqueArrivee(__GareLigneArr->ArretLogique());		// Ecriture gare de d�part
		__ElementTrajet->setAmplitudeServiceContinu(__AmplitudeServiceContinu);
		__ElementTrajet->setArretPhysiqueArrivee(__GareLigneArr->ArretPhysique());
		__ElementTrajet->setLigne(__GareLigneArr->Ligne());						// Ecriture ligne
		__ElementTrajet->setMomentDepart(__MomentDepart);						// Ecriture Moment de d�part
		__ElementTrajet->setMomentArrivee(__MomentArrivee);						// Ecriture Moment d'arriv�e
		__ElementTrajet->setService(__IndexService);							//Ecriture du num�ro de service

		if (vPADeOrigine->inclue(__GareLigneDep->ArretLogique()))
		{
			SetMeilleurDepart(__GareLigneDep->ArretLogique(), __MomentDepart);	// Enregistrement meilleure arriv�e

			if (__OptimisationAFaire)
				vDepartMin = __MomentDepart;
			else
				vDepartMin = vPADeOrigine->momentDepartSuivant(__MomentDepart, vMomentFin, _MomentCalcul);	// Ecriture MomentMax le cas �ch�ant
		}
		else
			SetMeilleurDepart(__GareLigneDep->ArretLogique(), __MomentDepart, __GareLigneDep->ArretPhysique());	// Enregistrement meilleure arriv�e
	}
	
	// Retour arr�ter le parcours de la ligne si la destination a �t� atteinte
	return !vPADeOrigine->inclue(__GareLigneDep->ArretLogique());
}



/*! \version 2.0
	\brief Fabrication de la liste des destinations directes d'un point d'arr�t
	\author Hugues Romain 
	\date 2001
	\return un tableau rempli par des �l�ments de trajet dont chacun propose la solution la plus rapide pour rallier chacune des destinations possibles depuis la gare de fin de TrajetEffectue. Ce tableau se termine par NULL. Les �l�ments sont tri�s dans l'ordre d�croissant d'int�r�t, d�finis par la fonction de tri CompareUtiliteETPourMeilleureArrivee() donnant, en r�sum�
		- Les �l�ments permettant d'arriver au but
		- Les �l�ments situ�s � moins de 2 km carr�s du but
		- Les autres �l�ments tri�s selon l'ensemble des crit�res

Au cours de l'�x�cution de la fonction, les �l�ments sont chain�s par le pointeur Suivant pour pouvoir etre repris ensuite. Sa fonction originelle est restitu�e en fin de fonction (mise � NULL)

*/
inline cElementTrajet** cCalculateur::ListeDestinations(const cTrajet& TrajetEffectue, bool MomentDepartStrict, bool OptimisationAFaire)
{
	// D�clarations
	cElementTrajet**	TableauRetour;

	const cAccesPADe*	GareOrigine;
	const cGareLigne*	CurrentGLD;
	const cGareLigne*	CurrentGLA;
	tNumeroService		NumArret;
	cMoment				MomentDepartInitial;
	cMoment				MomentDepart;
	cMoment				MomentArrivee;
	cTrajet				__SuiteElementsTrajets;
	cDureeEnMinutes		DureeEnMinutesCorrespondance;
	cDureeEnMinutes		AmplitudeServiceContinu;
	cDistanceCarree		D;
	cLog				__LogTrace;
	
	if (Synthese.getNiveauLog() <= LogDebug && _CheminLog.Taille())
	{
		cTexte __Chemin;
		__Chemin << _CheminLog;
		__Chemin << "/" << LOG_CHEMIN_DESTINATIONS << "/" << vIterationsArr << LOG_EXTENSION;
		__LogTrace.Ouvrir(__Chemin);
	}

	MomentDepartInitial = TrajetEffectue.Taille() ? TrajetEffectue.getMomentArrivee() : vDepartMin;

	// Balayage des lignes
	for (	GareOrigine = TrajetEffectue.Taille() ? TrajetEffectue.getArretLogiqueArrivee()->getAccesPADe() : vPADeOrigine;
			GareOrigine != NULL; 
			GareOrigine = GareOrigine->getSuivant()	)
	{
		for (CurrentGLD = GareOrigine->getPremiereGareLigneDep(); CurrentGLD != NULL; )
		{
			// La ligne est-elle utilisable ? (axe deja pris dans le trajet effectu�)
			if (ControleLigne(CurrentGLD->Ligne(), TrajetEffectue))
			{
				// Moment de d�part
				MomentDepart = MomentDepartInitial;
				if (TrajetEffectue.Taille())
					DureeEnMinutesCorrespondance = GareOrigine->getArretLogique()->AttenteCorrespondance(TrajetEffectue.getIndexArretPhysiqueArrivee(), CurrentGLD->ArretPhysique());

				if (DureeEnMinutesCorrespondance.Valeur() != 99)
				{
					//Rajout de la dur�e de correspondance
					MomentDepart += DureeEnMinutesCorrespondance;

					// La ligne courante permet elle d'atteindre la destination directement ?
					CurrentGLA = CurrentGLD->getLiaisonDirecteVers(vPADeDestination);

					// Tentative de cr�ation de nouvelle solution
					NumArret = CurrentGLD->Prochain(
						MomentDepart
						, (CurrentGLA && OptimisationAFaire) 
							? vMeilleurTemps[CurrentGLA->ArretLogique()->Index()]
							: vArriveeMax
						, AmplitudeServiceContinu
						, INCONNU
						, _MomentCalcul
					);

					if (NumArret != INCONNU && (!MomentDepartStrict || MomentDepart == MomentDepartInitial))
					{
						// Evaluation de la gareligne rendant vers la destination si trouv�e
						EvalueGareLigneArriveeCandidate(CurrentGLA, MomentDepart, CurrentGLD, NumArret, __SuiteElementsTrajets, TrajetEffectue, OptimisationAFaire, AmplitudeServiceContinu, __LogTrace);
						
						for (CurrentGLA = CurrentGLD->getArriveeCorrespondanceSuivante();
							CurrentGLA != NULL;
							CurrentGLA = CurrentGLA->getArriveeCorrespondanceSuivante())
						{
							if (!EvalueGareLigneArriveeCandidate(CurrentGLA, MomentDepart, CurrentGLD, NumArret, __SuiteElementsTrajets, TrajetEffectue, OptimisationAFaire, AmplitudeServiceContinu, __LogTrace))
								break;
						}//end boucle currentGLA
					}//end num arret !=-1
				}//end duree correspondance
				CurrentGLD = CurrentGLD->PADepartSuivant();
			}//end controle axe
			else
				CurrentGLD = CurrentGLD->PADepartAxeSuivant();
		}//end currentGLD
	}//end gare origine

	// Allocation du tableau de r�sultats
	TableauRetour  = (cElementTrajet**) malloc ((__SuiteElementsTrajets.Taille()+1)*sizeof(cElementTrajet*));
		
	// Stockage en supprimant les arriv�es trop tardives r�siduelles (parues avant la mise
	// � jour de MomentMax
	tIndex __NombreET=0;
	while(__SuiteElementsTrajets.PremierElement())
	{
		SetET(__SuiteElementsTrajets.PremierElement()->getGareArrivee(), NULL, __SuiteElementsTrajets.PremierElement()->VoieArrivee());
	
		// COnservation et finition de l'ET
		if	(vPADeDestination->inclue(__SuiteElementsTrajets.PremierElement()->getGareArrivee())
			|| DestinationUtilePourArriverTot (
				__SuiteElementsTrajets.PremierElement()->getGareArrivee(), 
				__SuiteElementsTrajets.PremierElement()->MomentArrivee(), 
				__SuiteElementsTrajets.getPremierElement()->getDistanceCarreeObjectif()
		)	){
			TableauRetour[__NombreET] = __SuiteElementsTrajets.getPremierElement();
			__SuiteElementsTrajets.DeliePremier();
			TableauRetour[__NombreET]->CalculeDureeEnMinutesRoulee();                        // DureeEnMinutes roulee
			TableauRetour[__NombreET]->setSuivant(NULL);
			__NombreET++;
		}
		else
		{ // Destruction de l'ET
			__SuiteElementsTrajets.SupprimePremier();
		}
	}
	TableauRetour[__NombreET]=NULL;

	// Tri des ET
	qsort(TableauRetour, __NombreET, sizeof(cElementTrajet*), CompareUtiliteETPourMeilleureArrivee);

	return TableauRetour;
}



/*! \version 2.0
	\brief Fabrication de la liste des provenances directes d'un point d'arr�t
	\author Hugues Romain 
	\date 2001-2005
	\return un tableau rempli par des �l�ments de trajet dont chacun propose la solution la plus rapide pour rallier chacune des provennaces possibles vers la gare de d�but de TrajetEffectue. Ce tableau se termine par NULL. Les �l�ments sont tri�s dans l'ordre d�croissant d'int�r�t, d�finis par la fonction de tri CompareUtiliteETPourMeilleureArrivee() donnant, en r�sum�
		- Les �l�ments permettant de partir du but
		- Les �l�ments situ�s � moins de 2 km carr�s du but
		- Les autres �l�ments tri�s selon l'ensemble des crit�res

Au cours de l'�x�cution de la fonction, les �l�ments sont chain�s par le pointeur Suivant pour pouvoir etre repris ensuite. Sa fonction originelle est restitu�e en fin de fonction (mise � NULL)

*/
inline cElementTrajet** cCalculateur::ListeProvenances(const cTrajet& TrajetEffectue, bool MomentArriveeStrict, bool OptimisationAFaire)
{
	// D�clarations
	cElementTrajet**	TableauRetour;

	const cAccesPADe*	GareDestination;
	const cGareLigne*	CurrentGLA;
	const cGareLigne*	CurrentGLD;
	tNumeroService		NumArret;
	cMoment				MomentArriveeInitial;
	cMoment				MomentArrivee;
	cMoment				MomentDepart;
	cTrajet				__SuiteElementsTrajets;
	cDureeEnMinutes		DureeEnMinutesCorrespondance;
	cDureeEnMinutes		AmplitudeServiceContinu;
	cDistanceCarree		D;

	MomentArriveeInitial = TrajetEffectue.Taille() ? TrajetEffectue.getMomentDepart() : vArriveeMax;

	// Balayage des lignes
	for (	GareDestination = TrajetEffectue.Taille() ? TrajetEffectue.getArretLogiqueDepart()->getAccesPADe() : vPADeDestination;
			GareDestination != NULL; 
			GareDestination = GareDestination->getSuivant()	)
	{
		for (CurrentGLA = GareDestination->getPremiereGareLigneArr(); CurrentGLA != NULL; )
		{
			// La ligne est-elle utilisable ? (axe deja pris dans le trajet effectu�)
			if (ControleLigne(CurrentGLA->Ligne(), TrajetEffectue))
			{
				// Moment de d�part
				MomentArrivee = MomentArriveeInitial;
				if (TrajetEffectue.Taille())
					DureeEnMinutesCorrespondance = GareDestination->getArretLogique()->AttenteCorrespondance(CurrentGLA->ArretPhysique(), TrajetEffectue.getIndexArretPhysiqueDepart());

				if (DureeEnMinutesCorrespondance.Valeur() != 99)
				{
					//Rajout de la dur�e de correspondance
					MomentArrivee -= DureeEnMinutesCorrespondance;

					// La ligne courante permet elle d'atteindre la destination directement ?
					CurrentGLD = CurrentGLA->getLiaisonDirecteDepuis(vPADeOrigine);

					// Tentative de cr�ation de nouvelle solution
					NumArret = CurrentGLA->Precedent(
						MomentArrivee,
						(CurrentGLD && OptimisationAFaire) ? vMeilleurTemps[CurrentGLD->ArretLogique()->Index()] : vDepartMin,
						AmplitudeServiceContinu
						);

					if (NumArret != INCONNU && (!MomentArriveeStrict || MomentArrivee == MomentArriveeInitial))
					{
						// Evaluation de la gareligne rendant vers la destination si trouv�e
						EvalueGareLigneDepartCandidate(CurrentGLD, MomentArrivee, CurrentGLA, NumArret, __SuiteElementsTrajets, TrajetEffectue, OptimisationAFaire, AmplitudeServiceContinu);
			
						for (CurrentGLD = CurrentGLA->getDepartCorrespondancePrecedent();
							CurrentGLD != NULL;
							CurrentGLD = CurrentGLD->getDepartCorrespondancePrecedent())
						{
							if (!EvalueGareLigneDepartCandidate(CurrentGLD, MomentArrivee, CurrentGLA, NumArret, __SuiteElementsTrajets, TrajetEffectue, OptimisationAFaire, AmplitudeServiceContinu))
								break;
						}//end boucle currentGLD
					}//end num arret !=-1
				}//end duree correspondance
				CurrentGLA = CurrentGLA->PAArriveeSuivante();
			}//end controle axe
			else
				CurrentGLA = CurrentGLA->PAArriveeAxeSuivante();
		}//end currentGLA
	}//end gare origine

	// Allocation du tableau de r�sultats
	TableauRetour  = (cElementTrajet**) malloc ((__SuiteElementsTrajets.Taille()+1)*sizeof(cElementTrajet*));
		
	// Stockage en supprimant les arriv�es trop tardives r�siduelles (parues avant la mise
	// � jour de MomentMax
	tIndex __NombreET=0;
	while(__SuiteElementsTrajets.PremierElement())
	{
		SetET(__SuiteElementsTrajets.PremierElement()->getGareDepart(), NULL, __SuiteElementsTrajets.PremierElement()->VoieDepart());
	
		// COnservation et finition de l'ET
		if	(vPADeOrigine->inclue(__SuiteElementsTrajets.PremierElement()->getGareDepart())
			|| ProvenanceUtilePourPartirTard(
				__SuiteElementsTrajets.PremierElement()->getGareDepart(), 
				__SuiteElementsTrajets.PremierElement()->MomentDepart(), 
				__SuiteElementsTrajets.getPremierElement()->getDistanceCarreeObjectif()	)	)
		{
			TableauRetour[__NombreET] = __SuiteElementsTrajets.getPremierElement();
			__SuiteElementsTrajets.DeliePremier();
			TableauRetour[__NombreET]->CalculeDureeEnMinutesRoulee();                        // DureeEnMinutes roulee
			TableauRetour[__NombreET]->setSuivant(NULL);
			__NombreET++;
		}
		else
		{ // Destruction de l'ET
			__SuiteElementsTrajets.SupprimePremier();
		}
	}
	TableauRetour[__NombreET]=NULL;

	// Tri des ET
	qsort(TableauRetour, __NombreET, sizeof(cElementTrajet*), CompareUtiliteETPourMeilleurDepart);

	return TableauRetour;
}



/*!	\brief Retourne un trajet permettant d'arriver le plus tot possible � un point destination
	\param __Resultat R�f�rence vers la variable o� �crire le r�sultat, peut contenir un r�sultat candidat calcul� par ailleurs
	\retval __Resultat R�sultat du calcul
	\param __TrajetEffectue Contient le d�but de parcours d�fini par les appels pr�c�dents dans la r�cursivit�
	\param MomentDepartStrict Indique si l'heure de d�part trouv�e doit �tre strictement �gale � l'heure de fin du trajet pr�c�dent
	\param OptimisationAFaire Indique si il s'agit de trouver seulement un moyen d'arriver le plus tot possible, ou bien de trouver le moyen le meilleur
	\return true si l'op�ration a �t� men�e avec succ�s (avec un r�sultat vide �ventuel), false sinon

Cette m�thode rend TrajetEffectue intact malgr� les manipulations temporaires qui sont effectu�es sur l'objet.
*/
bool cCalculateur::MeilleureArrivee(cTrajet& __Resultat, cTrajet& __TrajetEffectue, bool MomentDepartStrict, bool OptimisationAFaire)
{
	// Variables locales
	cElementTrajet**		__ElementsTrajetsDirects;	// Liste des �l�ments de trajet direct
	cTrajet					__Candidat;					// Trajet Candidat
	
	// Sortie si trop de correspondances
	if (__TrajetEffectue.Taille() > NMAXPROFONDEUR)
		return true;
	
	// logs
	vIterationsArr++;
	_LogTrace.Ecrit(LogDebug, __TrajetEffectue, vIterationsArr, "", "");
	
	// Obtention de la liste des trajets directs possibles au d�part de l'arr�t
	__ElementsTrajetsDirects = ListeDestinations(__TrajetEffectue, MomentDepartStrict, OptimisationAFaire);
	
	// Etude de chaque possibilit� en effectuant une correspondance par les �l�ments de trajet direct propos�s
	for (int __i=0; __ElementsTrajetsDirects[__i] != NULL; __i++)
	{
		// A ce stade, le Candidat est toujours vide.
		// Cas �l�ment se rendant � destination le candidat est l'�l�ment
		if (vPADeDestination->inclue(__ElementsTrajetsDirects[__i]->getGareArrivee()))
		{
			__Candidat.LieEnPremier(__ElementsTrajetsDirects[__i]);
			_LogTrace.Ecrit(LogDebug, __Candidat, vIterationsArr, "***RESULTAT***", "");
		}
		else	// Sinon r�cursion
		{
			// R��valuation du candidat
			if (ControleTrajetRechercheArrivee(*__ElementsTrajetsDirects[__i]))
			{
				// Ajout de l'�l�ment de trajet �tuid� au trajet effectu�
				__TrajetEffectue.LieEnDernier(__ElementsTrajetsDirects[__i]);

				// R�cursion		
				if (!MeilleureArrivee(__Candidat, __TrajetEffectue, false, OptimisationAFaire))
				{
					free(__ElementsTrajetsDirects);
					return false;	// Cas erreur technique (allocation)
				}
				
				// Suppression du lien vers l'�l�ment de trajet �tudi� sur le trajet effectu�
				__TrajetEffectue.DelieDernier();
				
				// Si r�cursion a produit un r�sultat : exploitation
				if (__Candidat.Taille())
				{
					// Insertion de l'�l�ment �tudi� en t�te du candidat pour produire un r�sultat potentiel
					__Candidat.LieEnPremier(__ElementsTrajetsDirects[__i]);
				}
				else
					delete __ElementsTrajetsDirects[__i];
			}
			else
				delete __ElementsTrajetsDirects[__i];
		}
		__Candidat.Finalise();

		// Si un candidat est cr�� : �lection
		if (__Candidat.Taille())
		{
			if (!__Resultat.Taille()
				|| __Candidat.getMomentArrivee() < __Resultat.getMomentArrivee()
				|| (__Candidat.getMomentArrivee() == __Resultat.getMomentArrivee() && __Candidat > __Resultat)
			){
				// Remplacement du r�sultat par le candidat (le r�sultat pr�c�dent est d�truit et le candidat est vid�)
				__Resultat = __Candidat;
			}
			else
			{
				// Destruction du candidat
				__Candidat.Vide();
			}
		}
	}

	// Nettoyage du tableau interm�diaire
	free(__ElementsTrajetsDirects);
	return true;
}



/*!	\brief Retourne un trajet permettant de partir le plus tard possible d'un point provenance
	\param __Resultat R�f�rence vers la variable o� �crire le r�sultat, peut contenir un r�sultat candidat calcul� par ailleurs
	\retval __Resultat R�sultat du calcul
	\param __TrajetEffectue Contient la fin de parcours d�finie par les appels pr�c�dents dans la r�cursivit�
	\param MomentDepartStrict Indique si l'heure de d�part trouv�e doit �tre strictement �gale � l'heure de fin du trajet pr�c�dent
	\param OptimisationAFaire Indique si il s'agit de trouver seulement un moyen de partir le plus tard possible, ou bien de trouver le moyen le meilleur
	\return true si l'op�ration a �t� men�e avec succ�s (avec un r�sultat vide �ventuel), false sinon

Cette m�thode rend TrajetEffectue intact malgr� les manipulations temporaires qui sont effectu�es sur l'objet.
*/
bool cCalculateur::MeilleurDepart(cTrajet& __Resultat, cTrajet& __TrajetEffectue, bool MomentArriveeStricte, bool OptimisationAFaire)
{
	// Variables locales
	cElementTrajet**		__ElementsTrajetsDirects;	// Liste des �l�ments de trajet direct
	cTrajet					__Candidat;					// Trajet Candidat
	
	// Sortie si trop de correspondances
	if (__TrajetEffectue.Taille() > NMAXPROFONDEUR)
		return true;
	
	// logs
	vIterationsDep++;
	_LogTrace.Ecrit(LogDebug, __TrajetEffectue, vIterationsDep, "", "");

	// Obtention de la liste des trajets directs possibles au d�part de l'arr�t
	__ElementsTrajetsDirects = ListeProvenances(__TrajetEffectue, MomentArriveeStricte, OptimisationAFaire);
	
	// Etude de chaque possibilit� en effectuant une correspondance par les �l�ments de trajet direct propos�s
	for (int __i=0; __ElementsTrajetsDirects[__i] != NULL; __i++)
	{
		// A ce stade, le Candidat est toujours vide.
		// Cas �l�ment se rendant � destination le candidat est l'�l�ment
		if (vPADeOrigine->inclue(__ElementsTrajetsDirects[__i]->getGareDepart()))
		{
			__Candidat.LieEnPremier(__ElementsTrajetsDirects[__i]);
		}
		else	// Sinon r�cursion
		{
			// R��valuation du candidat
			if (ControleTrajetRechercheDepart(*__ElementsTrajetsDirects[__i]))
			{
				// Ajout de l'�l�ment de trajet �tuid� au trajet effectu�
				__TrajetEffectue.LieEnPremier(__ElementsTrajetsDirects[__i]);

				// R�cursion		
				if (!MeilleurDepart(__Candidat, __TrajetEffectue, false, OptimisationAFaire))
				{
					free(__ElementsTrajetsDirects);
					return false;	// Cas erreur technique (allocation)
				}
				
				// Suppression du lien vers l'�l�ment de trajet �tudi� sur le trajet effectu�
				__TrajetEffectue.DeliePremier();
				
				// Si r�cursion a produit un r�sultat : exploitation
				if (__Candidat.Taille())
				{
					// Insertion de l'�l�ment �tudi� en t�te du candidat pour produire un r�sultat potentiel
					__Candidat.LieEnDernier(__ElementsTrajetsDirects[__i]);
				}
				else	// Sinon destruction du maillon
					delete __ElementsTrajetsDirects[__i];
			}
			else
				delete __ElementsTrajetsDirects[__i];
		}
		// Proc�dure de finalisation de l'objet trajet candidat, le rendant apte � une comparaison
		__Candidat.Finalise();

		// Si un candidat est cr�� : �lection
		if (__Candidat.Taille())
		{
			if (!__Resultat.Taille()
				|| __Candidat.getMomentDepart() > __Resultat.getMomentDepart()
				|| (__Candidat.getMomentDepart() == __Resultat.getMomentDepart() && __Candidat > __Resultat)
			){
				// Remplacement du r�sultat par le candidat (le r�sultat pr�c�dent est d�truit et le candidat est vid�)
				__Resultat = __Candidat;
			}
			else
			{
				// Destruction du candidat
				__Candidat.Vide();
			}
		}
	}

	// Nettoyage du tableau interm�diaire
	free(__ElementsTrajetsDirects);
	return true;
}



/*!	\brief Calcul de la liste des points d'arr�t desquels existent une liaison directe vers l'arr�t de d�part du trajet fourni en param�tre
	\param TrajetEffectue Trajet d�j� calcul� � partir du point d'arr�t en cours vers la destination
	\param EnCorrespondance true si une correspondance est effectu�e au point d'arr�t en cours (la dur�e de correspondance inter-quais sera retir�e au moment de d�part pour obtenir l'arriv�e maximale au point d'arr�t en cours). false s'il s'agit de trouver les provenances directes vers la destination.
	\param MomentArriveeStrict true si les solutions n'arrivant pas exactement � l'heure fournie doivent �tre filtr�es. Param�tre utile si l'heure d'arriv�e exacte a d�j� �t� calcul�e par ailleurs, et si cette fonction n'est utilis�e que dans le cadre d'une optimisation de la solution trouv�e
	\param OptimisationAFaire true si le calcul est lanc� dans le cadre d'une optimisation d'une solution d�j� trouv�e (seuls les solutions permettant de gagner du temps strictement seront fournies)
	\return Liste d'�l�ments de trajet dont chacun propose la solution la plus rapide pour rallier chacune des provenances possibles vers la gare de d�but de TrajetEffectue.
	Les �l�ments retourn�s sont stock�s dans un tableau qu'il conviendra de d�truire ensuite.
	Si la destination n'est pas atteinte, alors l'element 0 est a NULL.
	Le tableau se termine par un pointeur NULL.
	
	Cette fonction est inline car appel�e en un seul point du code (aucune diff�rence de poids du programme une fois compil�), et appel�e tr�s fr�quemment � l'ex�cution (perte de temps de calcul si un appel de fonction devait s'op�rer � chaque fois).
*/
/*inline cElementTrajet** cCalculateur::ListeProvenances(const cTrajet& TrajetEffectue, bool EnCorrespondance, bool MomentArriveeStrict, bool OptimisationAFaire, TSituation Situation)
{

	// D�clarations
//	cMoment			MomentMin = MomentMinAbsolu;
//	cArretLogique*			GareDestination = TrajetEffectue->getGareDepart();
	const cAccesPADe*	GareDestination;
	const cGareLigne*	CurrentGLD;
	const cGareLigne*	CurrentGLA;
	int					NumArret;
	cMoment				MomentArriveeInitial;
	cMoment				MomentDepart;
	cMoment				MomentArrivee;
	cElementTrajet*		CurrentET;
	cElementTrajet*		__PremierET = NULL;
	tIndex				__NombreET = 0;
	cElementTrajet**	TableauRetour;
	bool				DesserteAAffiner;
	const cGareLigne*	LigneDesservantProvenance;
	char				VoieDepart;
	char				VoieArrivee;
	cDureeEnMinutes		DureeEnMinutesCorrespondance;
	cDureeEnMinutes		AmplitudeServiceContinu;
	cDistanceCarree		D;

	if (TrajetEffectue.Taille())
	{
		GareDestination = TrajetEffectue.getArretLogiqueDepart()->getAccesPADe();
		MomentArriveeInitial = TrajetEffectue.getMomentDepart();
	}
	else
	{
		GareDestination = vPADeDestination;
		MomentArriveeInitial = vArriveeMax;
	}

	//_ASSERTE (vMeilleurTemps[vPADeOrigine->getArretLogique()->Index()] <= vDepartMin);
	
//	vDepartMin = vMeilleurTemps[vPAOrigine->Index()];

	// Remplissage des ET
	for (; GareDestination != NULL; GareDestination = GareDestination->getSuivant())
		for (CurrentGLA = GareDestination->getPremiereGareLigneArr(); CurrentGLA != NULL; )
		{
			if (ControleLigne(CurrentGLA->Ligne(), TrajetEffectue))
			{
				// Enregistrement des param�tres principaux dans des variables interm�diaires
				VoieArrivee = CurrentGLA->ArretPhysique();

				MomentArrivee = MomentArriveeInitial;
				if (EnCorrespondance)
					DureeEnMinutesCorrespondance = GareDestination->getArretLogique()->AttenteCorrespondance(VoieArrivee, TrajetEffectue.getIndexArretPhysiqueDepart());
				if (DureeEnMinutesCorrespondance.Valeur() != 99)
				{
					MomentArrivee -= DureeEnMinutesCorrespondance;
					
					// Desserte de la provenance
//					LigneDesservantProvenance = vPADeOrigine->DessertAuDepart(CurrentGLA->Ligne());

					if (LigneDesservantProvenance && OptimisationAFaire)
						// Tentative d'am�lioration de la solution en cours
						NumArret = CurrentGLA->Precedent(MomentArrivee, vMeilleurTemps[LigneDesservantProvenance->ArretLogique()->Index()], AmplitudeServiceContinu);
					else
						// Tentative de cr�ation de nouvelle solution
						NumArret = CurrentGLA->Precedent(MomentArrivee, vDepartMin, AmplitudeServiceContinu);

					if (NumArret != INCONNU && (!MomentArriveeStrict || MomentArrivee == MomentArriveeInitial))
					{
						// Balayage des destinations selon le cas
						DesserteAAffiner = false;
// 						if (!vPADeOrigine->correspondanceAutorisee() && LigneDesservantProvenance)
// 							DesserteAAffiner = true;
// 						if (DesserteAAffiner)
// 							CurrentGLD = CurrentGLA->getDepartPrecedent();
// 						else
// 							CurrentGLD = CurrentGLA->getDepartCorrespondancePrecedent();

						while (CurrentGLD != NULL)
						{
							if (CurrentGLD->ArretLogique()->CorrespondanceAutorisee() || vPADeOrigine->inclue(CurrentGLD->ArretLogique()))
							{
								// Enregistrement des param�tres principaux dans des variables interm�diaires
								VoieDepart = CurrentGLD->ArretPhysique();

								// Horaire d�part
								MomentDepart = MomentArrivee;
								CurrentGLD->CalculeDepart(*CurrentGLA, NumArret, MomentArrivee, MomentDepart);

								// Utilit� de la ligne et du point d'arr�t
								D=0;
// 								if (!vPADeOrigine->inclue(CurrentGLD->ArretLogique()) && !ProvenanceUtilePourPartirTard(CurrentGLD->ArretLogique(), MomentDepart, LigneDesservantProvenance, D))
// 									break;
								
								//  SET FE.2.3.4.001 : rajout filtre resa : la ligne et le point d'arret ne sont pas OK	
								if (!CurrentGLD->Ligne()->verificationReservation(MomentDepart, true))
									break;
								
								// Controle de non d�passement de Dur�emax
								if (vDureeServiceContinuPrecedent.Valeur())
								{
									if (MomentDepart <= vDernierDepartServiceContinuPrecedent)
										if (TrajetEffectue.Taille())
										{
											if ((TrajetEffectue.getMomentArrivee() - MomentDepart) >= vDureeServiceContinuPrecedent)
												break;
										}
										else
											if ((MomentArrivee - MomentDepart) >= vDureeServiceContinuPrecedent)
												break;
								}

								if ((MomentDepart > GetMeilleurDepart(CurrentGLD->ArretLogique(), VoieDepart))
								|| (OptimisationAFaire && MomentDepart == GetMeilleurDepart(CurrentGLD->ArretLogique(), VoieDepart)))
								{
									if (vPADeOrigine->inclue(CurrentGLD->ArretLogique()))
										CurrentET = GetET(CurrentGLD->ArretLogique());
									else
										CurrentET = GetET(CurrentGLD->ArretLogique(), VoieDepart);

									if (CurrentET == NULL)
									{
										CurrentET = new cElementTrajet;
										CurrentET->setSuivant(__PremierET);	// Chainage provisoire
										SetET(CurrentGLD->ArretLogique(), CurrentET, VoieDepart);
										CurrentET->setArretLogiqueDepart(CurrentGLD->ArretLogique());
										CurrentET->setDistanceCarreeObjectif(D);
										__PremierET = CurrentET;
										__NombreET++;
									}

									CurrentET->setAmplitudeServiceContinu(AmplitudeServiceContinu);
									CurrentET->setArretLogiqueArrivee(CurrentGLA->ArretLogique());
									CurrentET->setArretPhysiqueDepart(VoieDepart);
									CurrentET->setArretPhysiqueArrivee(VoieArrivee);
									CurrentET->setLigne(CurrentGLA->Ligne());
									CurrentET->setMomentArrivee(MomentArrivee);
									CurrentET->setMomentDepart(MomentDepart);
									CurrentET->CalculeDureeEnMinutesRoulee();
									CurrentET->setService(NumArret);
									
									if (vPADeOrigine->inclue(CurrentGLD->ArretLogique()))
									{
										SetMeilleurDepart(CurrentGLD->ArretLogique(), MomentDepart);
										if (OptimisationAFaire) // MODIF
											vDepartMin = MomentDepart; // MODIF
										else // MODIF
											//vDepartMin = vPADeOrigine->momentDepartSuivant(CurrentET->MomentDepart(), TrajetEffectue->MomentArriveeFinale());
											vDepartMin = vPADeOrigine->momentDepartSuivant(CurrentET->MomentDepart(), vMomentFin);
									}
									else
										SetMeilleurDepart(CurrentGLD->ArretLogique(), MomentDepart, VoieDepart);
								}
							}
							if (DesserteAAffiner)
								CurrentGLD = CurrentGLD->getDepartPrecedent();
							else
								CurrentGLD = CurrentGLD->getDepartCorrespondancePrecedent();
						}
					}
				}
				CurrentGLA = CurrentGLA->PAArriveeSuivante();
			}
			else
				CurrentGLA = CurrentGLA->PAArriveeAxeSuivante();
		}

	// Stockage des pointeurs et des distances dans le tableau
	TableauRetour	= (cElementTrajet**) malloc((__NombreET+1)*sizeof(cElementTrajet*));
	
	__NombreET = 0;
	for (CurrentET = __PremierET; CurrentET!=NULL; CurrentET = __PremierET)
	{
		SetET(CurrentET->getGareDepart(), NULL, CurrentET->VoieDepart());
		__PremierET = CurrentET->getSuivant();

		if (ProvenanceUtilePourPartirTard(CurrentET->getGareDepart(), CurrentET->MomentDepart(),  CurrentET->getDistanceCarreeObjectif()))
		{
			CurrentET->CalculeDureeEnMinutesRoulee();                        // DureeEnMinutes roulee
			TableauRetour[__NombreET] = CurrentET;
			TableauRetour[__NombreET]->setSuivant(NULL);
			__NombreET++;
		}
		else
		{ // Destruction de l'ET
			delete CurrentET;
		}
	}
	TableauRetour[__NombreET] = NULL;

	qsort(TableauRetour, __NombreET, sizeof(cElementTrajet*), CompareUtiliteETPourMeilleurDepart);

	return TableauRetour;
}
*/


/*!	\brief Calcul de l'heure de d�part la plus tardive avec proposition d'une solution, permettant l'arriv�e � un point donn� depuis un autre point, � une heure donn�e
	\retval __Resultat R�sultat du calcul
	\param profondeur Nombre d'appels r�cursifs successifs (pour limiter)
	\param TrajetEffectue Trajet d�j� calcul� par les appels r�cursifs pr�c�dents
	\param EnCorrespondance Indique si l'�l�ment sera fix� en correspondance au trajet effectu� (voir profondeur)
	\param MomentArriveeStrict Indique si l'heure d'arriv�e doit �tre strictement identique � l'heure but
	\param OptimisationAFaire Indique si le calcul doit s�lectionner le meilleur trajet au sens de l'op�rateur > ou bien se contenter de fournir une solution valide
	\param Situation Indique si l'usage de la base temps r�el doit �tre forc�
	\param __Finaliste Proposition de r�sultat calcul�e par ailleurs (passage par valeur)
	\return true si le calcul a pu �tre men� avec succ�s ind�pendamment du r�sultat, false sinon
	\author Hugues Romain
	\date 2000-2005
*/
/*bool cCalculateur::MeilleurDepart(cTrajet& __Resultat, cTrajet& __TrajetEffectue, bool EnCorrespondance, bool MomentArriveeStrict, bool OptimisationAFaire, TSituation Situation)
{
	// Variables locales
	cElementTrajet**		__ElementsTrajetsDirects;	// Liste des �l�ments de trajet direct
	cTrajet					__Candidat;					// Trajet Candidat
	
	// Sortie si trop de correspondances
	if (__TrajetEffectue.Taille() > NMAXPROFONDEUR)
		return true;
	
	// Obtention de la liste des trajets directs possibles au d�part de l'arr�t
	__ElementsTrajetsDirects = ListeProvenances(__TrajetEffectue, EnCorrespondance, MomentArriveeStrict, OptimisationAFaire, Situation);
	
	// Etude de chaque possibilit� en effectuant une correspondance par les �l�ments de trajet direct propos�s
	for (int __i=0; __ElementsTrajetsDirects[__i] != NULL; __i++)
	{
		// A ce stade, le Candidat est toujours vide.
		// Cas �l�ment se rendant � destination le candidat est l'�l�ment
		if (vPADeOrigine->inclue(__ElementsTrajetsDirects[__i]->getGareDepart()))
		{
			__Candidat.LieEnPremier(__ElementsTrajetsDirects[__i]);
		}
		else	// Sinon r�cursion
		{
			// R��valuation du candidat
			if (ControleTrajetRechercheDepart(*__ElementsTrajetsDirects[__i]))
			{
				// Ajout de l'�l�ment de trajet �tuid� au trajet effectu�
				__TrajetEffectue.LieEnPremier(__ElementsTrajetsDirects[__i]);

				// R�cursion		
				if (!MeilleurDepart(__Candidat, __TrajetEffectue, true, false, OptimisationAFaire, Situation))
				{
					free(__ElementsTrajetsDirects);
					return false;	// Cas erreur technique (allocation)
				}
				
				// Suppression du lien vers l'�l�ment de trajet �tudi� sur le trajet effectu�
				__TrajetEffectue.DeliePremier();
				
				// Si r�cursion a produit un r�sultat : exploitation
				if (__Candidat.Taille())
				{
					// Insertion de l'�l�ment �tudi� en t�te du candidat pour produire un r�sultat potentiel
					__Candidat.LieEnDernier(__ElementsTrajetsDirects[__i]);
				}
			}
		}

		// Si un candidat est cr�� : �lection
		if (__Candidat > __Resultat)
		{
			// Remplacement du r�sultat par le candidat (le r�sultat pr�c�dent est d�truit et le candidat est vid�)
			__Resultat = __Candidat;
		}
		else
		{
			// Destruction du candidat
			__Candidat.Vide();
		}
	}

	// Nettoyage du tableau interm�diaire
	free(__ElementsTrajetsDirects);
	return true;
}
*/
void cCalculateur::ResetMeilleursDeparts()
{
	tNumeroVoie iVoie;
	for (tIndex i=0; i< vEnvironnement->NombrePointsArret(); i++)
	{
		vMeilleurTemps[i].setMoment(TEMPS_MIN);
		vET[i] = NULL;
		if	(	vEnvironnement->getArretLogique(i) != NULL
			&&	vEnvironnement->getArretLogique(i)->CorrespondanceAutorisee()
			)
			for (iVoie=1; iVoie<=vEnvironnement->getArretLogique(i)->NombreArretPhysiques(); iVoie++)
			{
				vMeilleurTempsArretPhysique[i][iVoie].setMoment(TEMPS_MIN);
				vETArretPhysique[i][iVoie] = NULL;
			}
	}
}

void cCalculateur::ResetMeilleuresArrivees()
{
	tNumeroVoie iVoie;
	for (tIndex i=0; i<vEnvironnement->NombrePointsArret(); i++)
	{
		vMeilleurTemps[i].setMoment(TEMPS_MAX);
		vET[i] = NULL;
		if	(	vEnvironnement->getArretLogique(i) != NULL
			&&	vEnvironnement->getArretLogique(i)->CorrespondanceAutorisee()
			)
			for (iVoie=1; iVoie<=vEnvironnement->getArretLogique(i)->NombreArretPhysiques(); iVoie++)
			{
				vMeilleurTempsArretPhysique[i][iVoie].setMoment(TEMPS_MAX);
				vETArretPhysique[i][iVoie] = NULL;
			}
	}
}

void cCalculateur::SetMeilleurDepart(const cAccesPADe* curAccesPADe, const cMoment& NewMoment, tNumeroVoie NumVoie)
{
	for (; curAccesPADe != NULL; curAccesPADe = curAccesPADe->getSuivant())
		SetMeilleurDepart(curAccesPADe->getArretLogique(), NewMoment, NumVoie);
}

void cCalculateur::SetMeilleurDepart(const cArretLogique* curPA, const cMoment& __Moment, tNumeroVoie NumVoie)
{
	cMoment __MomentDepart = __Moment;
	
	if (curPA->CorrespondanceAutorisee())
	{
		if (NumVoie)
		{
			vMeilleurTempsArretPhysique[curPA->Index()][NumVoie] = __MomentDepart;
			__MomentDepart -= curPA->PireAttente(NumVoie);
			if (__MomentDepart > vMeilleurTemps[curPA->Index()])
				vMeilleurTemps[curPA->Index()] = __MomentDepart;
		}
		else
		{
			for (tNumeroVoie iVoie=1; iVoie<= curPA->NombreArretPhysiques(); iVoie++)
				vMeilleurTempsArretPhysique[curPA->Index()][NumVoie] = __MomentDepart;
			vMeilleurTemps[curPA->Index()] = __MomentDepart;
		}
	}
	else
		vMeilleurTemps[curPA->Index()] = __MomentDepart;
}

void cCalculateur::SetMeilleureArrivee(const cAccesPADe* curAccesPADe, const cMoment& NewMoment, tNumeroVoie NumVoie)
{
	for (; curAccesPADe != NULL; curAccesPADe = curAccesPADe->getSuivant())
	{
		SetMeilleureArrivee(curAccesPADe->getArretLogique(), NewMoment, NumVoie);
	}
}

void cCalculateur::SetMeilleureArrivee(const cArretLogique* curPA, const cMoment& __Moment, tNumeroVoie NumVoie)
{
	cMoment __MomentArrivee = __Moment;
	
	if (curPA->CorrespondanceAutorisee())
	{
		if (NumVoie)
		{
			vMeilleurTempsArretPhysique[curPA->Index()][NumVoie] = __MomentArrivee;
			__MomentArrivee += curPA->PireAttente(NumVoie);
			if (__MomentArrivee < vMeilleurTemps[curPA->Index()])
				vMeilleurTemps[curPA->Index()] = __MomentArrivee;
		}
		else
		{
			for (tNumeroVoie iVoie=1; iVoie<=curPA->NombreArretPhysiques(); iVoie++)
				vMeilleurTempsArretPhysique[curPA->Index()][NumVoie] = __MomentArrivee;
			vMeilleurTemps[curPA->Index()] = __MomentArrivee;
		}
	}
	else
		vMeilleurTemps[curPA->Index()] = __MomentArrivee;
}

cElementTrajet* cCalculateur::GetET(const cArretLogique* curPA, tNumeroVoie NumeroVoie)
{
	if (curPA->CorrespondanceAutorisee() && NumeroVoie)
		return(vETArretPhysique[curPA->Index()][NumeroVoie]);
	else
		return(vET[curPA->Index()]);
}

void cCalculateur::SetET(const cArretLogique* curPA, cElementTrajet* NewET, tNumeroVoie NumVoie)
{
	if (curPA->CorrespondanceAutorisee() && NumVoie)
		vETArretPhysique[curPA->Index()][NumVoie] = NewET;
	vET[curPA->Index()] = NewET;
}



/**	Préparation d'un calcul de fiche horaire journée.
	@param __LieuOrigine Lieu d'origine du calcul
	@param __LieuDestination Lieu de destination du calcul
	\param __DateDepart Date de la fiche horaire.
	\param iPeriode P�riode de la journ�e � afficher
	\param besoinVelo Filtre prise en charge des v�los
	\param besoinHandicape Filtre prise en charge des handicap�s
	\param besoinTaxiBus Filtre lignes � la demande
	\param codeTarif Filtre tarif
	\return true si une fiche horaire peut �tre calcul�e, false sinon. La valeur false peut survenir si la date de calcul est ant�rieure au moment pr�sent et si les solutions pass�es ne doivent pas �tre affich�es
	\author Hugues Romain
	\date 2001-2006

La pr�paration du calcul de fiche horaire journ�e effectue les initialisations de tous les param�tres :
	- Prise en compte des divers paramètre transmis directement
	- Construction des accès au réseau de transport (départ et arrivée) à partir des lieux logiques d'origine et de destination

La variable vArriveeMax, qui permet de limiter le moment d'arriv�e lors du calcul est initalis�e comme suit :
	- Application de la p�riode aux moments de d�but et de fin de calcul
	- Ajout d'une minute par kilom�tre � vol d'oiseau pour permettre des solutions dont l'arriv�e d�passe la fin de p�riode, et pouvant �tre toutefois utile � fournir (ex: train de nuit)
*/
bool cCalculateur::InitialiseFicheHoraireJournee(
	const cLieuLogique* __LieuOrigine, const cLieuLogique* __LieuDestination
	, const cDate& __DateDepart
	, const cPeriodeJournee* __PeriodeJournee
	, tBool3 besoinVelo, tBool3 besoinHandicape
	, tBool3 besoinTaxiBus, tNumeroTarif codeTarif
	, bool __SolutionsPassees
){
	// Enregistrement de la date de la demande
	_MomentCalcul.setMoment();

	// Enregistrement de la plage temporelle de calcul
	if (__DateDepart < _MomentCalcul.getDate())
		return false;
	
	vMomentDebut = __DateDepart;
	vArriveeMax = vMomentDebut;
	if (__DateDepart == _MomentCalcul.getDate())
		_BaseTempsReel = true;
	else
		_BaseTempsReel = false;

	// Construction des accès au réseau départ et arrivée
	_ConstructionAccesDepart(__LieuOrigine);
	_ConstructionAccesArrivee(__LieuDestination);

	// Application de la plage horaire
	if (!__PeriodeJournee->AppliquePeriode(vMomentDebut, vArriveeMax, _MomentCalcul, __SolutionsPassees))
		return false;
	vArriveeMax += cDureeEnMinutes(cDistanceCarree(Destination->getArretLogique()->getPoint(), Origine->getArretLogique()->getPoint()).Distance());
			
	// enregistrement des filtres
	vBesoinVelo = besoinVelo;
	vBesoinHandicape = besoinHandicape;
	vBesoinTaxiBus = besoinTaxiBus;
	vCodeTarif = codeTarif;

	// Optimisations (int�ret � v�rifier)
	vMomentDebut = vPADeOrigine->momentDepartSuivant(vMomentDebut, vArriveeMax, _MomentCalcul);
	vMomentFin = vPADeDestination->momentArriveePrecedente(vArriveeMax, vMomentDebut);
	vMomentFin += cDureeEnMinutes(1);

	return true;
}



/*!	\brief M�thode lib�rant l'espace de calcul pour un autre thread
	\author Hugues Romain
	\date 2001-2005

Cette m�thode supprime les objets temporaire stockant les r�sultats, et lib�re l'espace de calcul
*/
void cCalculateur::Libere()
{
#ifdef UNIX
	pthread_mutex_lock( &CalculMutex );
#endif
	vSolution.Vide();
	_Libre = true;
#ifdef UNIX
	pthread_mutex_unlock( &CalculMutex );
#endif
}




/*!	\brief Contr�le de l'acceptabilit� de la Ligne au vu des �lements en possession
	\param Ligne la ligne � contr�ler
	\param __Trajet trajet d�j� effectu� pris pour r�f�rence pour le contr�le des axes (peut �tre vide)
	\return true si la ligne peut �tre utilis�e � ce stade

Les points de contr�le sont les suivants:
 - Contr�le de l'acceptation des voyageurs � bord de la ligne
 - Test de la compatibilit� de la prise en charge des v�los avec le filtre
 - Test de la compatibilit� de la prise en charge des handicap�s avec le filtre
 - Test de la compatibilit� de la modalit� de r�servation avec le filtre
 - Test de la compatibilit� de la tarification avec le filtre
 - Contr�le des axes par rapport � un trajet effectu� si fourni
*/
bool cCalculateur::ControleLigne(const cLigne* Ligne, const cTrajet& __Trajet) const
{
	// L'axe de la ligne autorise-t-il la prise des voyageurs ?
	if (!Ligne->Axe()->Autorise())
		return false;
	
	// tests sur la prise en charge des velos
	if (vBesoinVelo == Vrai)
	{
		if (Ligne->getVelo()==NULL
		|| Ligne->getVelo()->TypeVelo() == Faux)
		return false;
	}
	
	// tests sur la prise en charge des handicap�s
	if (vBesoinHandicape == Vrai)
	{
		if (Ligne->getHandicape()==NULL
		|| Ligne->getHandicape()->getTypeHandicape() == Faux)
			return false;
	}

	// tests sur la restriction sur les taxi bus
	if (vBesoinTaxiBus == Vrai)
	{
		if (Ligne->GetResa()==NULL
		|| Ligne->GetResa()->TypeResa() != Obligatoire)
			return false;
	}

	// tests sur le choix des tarifs
	if (vCodeTarif >-1)
	{
		//tarif ligne==tarif demande + tarif 0=> gratuit
		if ((Ligne->getTarif()== NULL)
		|| ((Ligne->getTarif()->getNumeroTarif() != vCodeTarif)
		&& (Ligne->getTarif()->getNumeroTarif() != 0)))
			return false;
	}
		
	// Contr�le de l'axe vis � vis des axes d�j� emprunt�s, effectu� que si l'axe de la ligne est non libre et si un ET est fourni
	if (!Ligne->Axe()->Libre() && __Trajet.Taille())
	{
		for (const cElementTrajet* curET = __Trajet.PremierElement(); curET != NULL; curET = curET->Suivant())
			if (curET->Axe() == Ligne->Axe())
				return false;
	}
	
	// Succ�s
	return true;
}



/*! \brief Fonction d'optimisation visant � stopper la r�cursivit� de la recherche d'itin�raire � partir de l'arr�t s'il est certain que le temps disponible pour terminer le trajet n'est pas suffisant
	\param __ArretLogique Point d'arr�t depuis lequel partirait la r�cursivit�
	\param __Moment Date/Heure d'arriv�e � l'arr�t avec la ligne pr�c�dente
	\param __DistanceCarreeBut distance carr�e entre l'arr�t et l'arr�t de destination
	\return true si le calcul d'itin�raire doit �tre poursuivi r�cursivement � partir de cet arr�t, false sinon car n'ayant aucune chance d'obtenir de r�sultats
	\retval __DistanceCarreeBut distance carr�e entre l'arr�t et l'arr�t de destination
	\author Hugues Romain
	\date 2005
	
	Le test s'effectue sur des crit�res de distance et d'heure, bas�s sur le fait que les diff�rents arr�ts d'un groupement ne sont pas �loign�s de plus d'1.5 km environ.
	
	
	Etapes de la fonction :
*/
bool cCalculateur::DestinationUtilePourArriverTot(const cArretLogique* __ArretLogique, const cMoment& __Moment, cDistanceCarree& __DistanceCarreeBut) const
{
	//! <li>Calcul de la distance carr�e si non fournie</li>
	if (__DistanceCarreeBut.EstInconnu())
		__DistanceCarreeBut.setFromPoints(__ArretLogique->getPoint(), vPADeDestination->getArretLogique()->getPoint());

	//! <li>Evaluation du moment "au plus tot" o� peut d�marrer</li>
	cMoment __MomentArriveeAvantInclusCorrespondance = __Moment;
	if (!vPADeDestination->inclue(__ArretLogique))
		__MomentArriveeAvantInclusCorrespondance += __ArretLogique->AttenteMinimale();

	//! <li>Test 1 : Non d�passement du moment d'arriv�e maximal</li>
	if (__MomentArriveeAvantInclusCorrespondance > vArriveeMax)
		return false;
	
	//!	\todo Remettre ici un controle par VMAX
	
	return true;
}



/*! \brief Fonction d'optimisation visant � stopper la r�cursivit� de la recherche d'itin�raire � partir de l'arr�t s'il est certain que le temps disponible pour terminer le trajet n'est pas suffisant
	\param __ArretLogique Point d'arr�t depuis lequel partirait la r�cursivit�
	\param __Moment Date/Heure de d�part � l'arr�t avec la ligne pr�c�dente
	\param __DistanceCarreeBut distance carr�e entre l'arr�t et l'arr�t de provenance
	\return true si le calcul d'itin�raire doit �tre poursuivi r�cursivement � partir de cet arr�t, false sinon car n'ayant aucune chance d'obtenir de r�sultats
	\retval __DistanceCarreeBut distance carr�e entre l'arr�t et l'arr�t de destination
	\author Hugues Romain
	\date 2005
	
	Le test s'effectue sur des crit�res de distance et d'heure, bas�s sur le fait que les diff�rents arr�ts d'un groupement ne sont pas �loign�s de plus d'1.5 km environ.
		
	Etapes de la fonction :
*/
bool cCalculateur::ProvenanceUtilePourPartirTard(const cArretLogique* __ArretLogique, const cMoment& __Moment, cDistanceCarree& __DistanceCarreeBut) const
{
	//! <li>Calcul de la distance carr�e si non fournie</li>
	if (__DistanceCarreeBut.EstInconnu())
		__DistanceCarreeBut.setFromPoints(__ArretLogique->getPoint(), vPADeOrigine->getArretLogique()->getPoint());
	
	//! <li>Evaluation du moment "au plus tard" o� peut arriver</li>
	cMoment __MomentDepartApresInclusCorrespondance = __Moment;
	if (!vPADeOrigine->inclue(__ArretLogique))
		__MomentDepartApresInclusCorrespondance -= __ArretLogique->AttenteMinimale();
	
	//! <li>Test 1 : Non d�passement du moment de d�part minimal</li>
	if (__MomentDepartApresInclusCorrespondance < vDepartMin)
		return false;

	//!	\todo Remettre ici un controle par VMAX
		
	return true;
}


/*!	\brief Contr�le de l'int�r�t de l'�l�ment de trajet pour rejoindre la destination
	\return true si l'�l�ment de trajet est susceptible d'avoir un interet, false si non
	\author Hugues Romain
	\date 2005
*/
bool cCalculateur::ControleTrajetRechercheArrivee(cElementTrajet& __ET) const
{
	return __ET.MomentArrivee() <= GetMeilleureArrivee(__ET.getGareArrivee(), __ET.VoieArrivee()) 
		&&  DestinationUtilePourArriverTot(__ET.getGareArrivee(), __ET.MomentArrivee(), __ET.getDistanceCarreeObjectif());
}


bool cCalculateur::ControleTrajetRechercheDepart(cElementTrajet& __ET) const
{
	return __ET.MomentDepart() >= GetMeilleurDepart(__ET.getGareDepart(), __ET.VoieDepart())
		&& ProvenanceUtilePourPartirTard(__ET.getGareDepart(), __ET.MomentDepart(), __ET.getDistanceCarreeObjectif());
}

const cMoment& cCalculateur::GetMeilleurDepart(const cArretLogique* curPA, tNumeroVoie NumeroVoie) const
{
	if (curPA->CorrespondanceAutorisee() && NumeroVoie && vMeilleurTempsArretPhysique[curPA->Index()][NumeroVoie] > vMeilleurTemps[curPA->Index()])
		return(vMeilleurTempsArretPhysique[curPA->Index()][NumeroVoie]);
	else
		return(vMeilleurTemps[curPA->Index()]);
}

const cMoment& cCalculateur::GetMeilleureArrivee(const cArretLogique* curPA, tNumeroVoie NumeroVoie) const
{
	if (curPA->CorrespondanceAutorisee() && NumeroVoie && vMeilleurTempsArretPhysique[curPA->Index()][NumeroVoie] < vMeilleurTemps[curPA->Index()])
		return(vMeilleurTempsArretPhysique[curPA->Index()][NumeroVoie]);
	else
		return(vMeilleurTemps[curPA->Index()]);
}


/*!	\brief Accesseur solution du dernier calcul d'itin�raire
	\return Pointeur vers trajets trouv�s par le dernier calcul d'itin�raire
	\author Hugues Romain
	\date 2000-2005
*/
const cTrajets& cCalculateur::getSolution() const
{
	return vSolution;
}



/*!	\brief Prise en possession de l'espace de calcul
	\return Pointeur vers l'espace de calcul si disponible, NULL sinon
	\author Hugues Romain
	\date 2005
*/
cCalculateur* cCalculateur::Prend()
{
    // Prise de la possession de l'espace si disponible
    // avec gestion mutex pour �viter la modification simultan�e
#ifdef UNIX
    pthread_mutex_lock( &CalculMutex );
#endif
    if (_Libre)
    {
        _Libre = false;
#ifdef UNIX
        pthread_mutex_unlock( &CalculMutex );
#endif
        return this;
    }
#ifdef UNIX
    pthread_mutex_unlock( &CalculMutex );
#endif
	
	// Retour Echec
	return NULL;
}


/** Fabrication de la liste complète des accès au réseau de transport.
	@param __MomentDepart Moment de départ du lieu
	@param __MomentMax Moment maximal de départ du lieu
	@return Liste complète des accès au réseau de transport

	L'accès au réseau de transport, pour un départ, est construit par les opérations suivantes :
		- pour chaque adresse accessible immédiatement, fabrication de la liste des trajets possibles vers des arrêts en respectant les critères
		- un filtrage des points d'entrée inutiles est opéré
			- l'objet cAccesReseau est construit et transmis au calculateur
*/
void cCalculateur::_ConstructionAccesOrigine(const cLieuLogique* __LieuOrigine)
{
	_AccesDepart.

	for (tIndex __i = 0; __LieuOrigine->GetArretLogique(__i); __i++)

}

