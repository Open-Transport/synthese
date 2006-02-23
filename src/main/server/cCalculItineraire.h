/**	@file cCalculItineraire.h
	\brief En-t�te Classe CalculItineraire
*/

#ifndef SYNTHESE_cCalculateur_H
#define SYNTHESE_cCalculateur_H

class RoutePlanningNode;
class LogicalPlace;
class cEnvironnement;
class LogicalPlace;

#include "cTrajets.h"
#include "cLog.h"
#include "Temps.h"
#include "cPeriodeJournee.h"
#include "RoutePlanningNode.h"

#ifdef UNIX
#include <pthread.h>
extern pthread_mutex_t mutex_calcul;
#endif


/**	@defgroup m33 33 Recherche d'itin�raires
	@{
*/
 
/** Espace de calcul pour thread et m�thodes de calcul
	\author Hugues Romain
	\date 2001
	
Cette classe contient l'ensemble de la m�moire allou�e disponible pour contenir les variables d'�tat n�cessaires aux diff�rents calculs complexes de SYNTHESE. Cette classe ne contient cependant pas d'espace allou� pour les objets temporaires g�n�r�s lors des phases r�cursives des calculs (qui sont g�r�s en direct par les m�thodes de calcul)

Les instances de cette classe sont li�s � l'environnement sur lequel s'effectueront les calculs � suivre. L'espace de stockage est en effet dimensionn� selon les caract�ristiques de l'environnement.

Les calculs suivants sont disponibles :
 - Calcul d'itin�raire
 - Tableaux de d�parts (non encore int�gr�)
 - Fabrication de listes d'arr�t (non encore int�gr�)
Pour int�grer les nouvelles fonctionnalit�s diviser la classe (h�ritages)

Pour les op�rations se rapportant � la r�servation en ligne, se r�f�rer � la classe cResaEnLigne qui g�re elle-m�me l'ensemble de ses fonctionnalit�s.

*/
class cCalculateur
{
public:

	/** Tableaux temporaires de meilleurs temps */
	typedef map<const cArretPhysique*, cMoment> BestTimeMap;

	/** Tableaux des meilleures solutions vers chaque arrêt physique */
	typedef map<cArretPhysique*, cElementTrajet*> BestSolutionMap;

private:
	//! \name Variables de travail recherche d'itin�raire
	//@{
	BestTimeMap				_bestTimes;			//!< Meilleur temps de trajet trouv� depuis/vers chaque quai de chaque arr�t
	tDureeEnMinutes			vDureeServiceContinuPrecedent;			//!< Dur�e du trajet dans le service continu trouv� pr�c�demment
	cMoment					vDernierDepartServiceContinuPrecedent;	//!< Moment de fin de l'amplitude de validit� du service continu trouv� pr�c�demment
	int						vIterationsDep;							//!< Compteur d'it�rations pour les calculs de meilleur d�part
	int						vIterationsArr;							//!< Compteur d'it�rations pour les calculs de meilleure arriv�e
	cLog					_LogTrace;								//!< Fichier log de tracage des recursions
	cTexte					_CheminLog;								//!< Chemin des log de tracage des recursions
	cMoment					_startTime;								//!< Time cursor in the timetable during the computing
	cMoment					_absoluteBestTime;						//!< Best time in goal
	//@}

	//! \name R�sultats
	//@{	
	cTrajets				vSolution;			//!< Solution retenue
	//@}

	//! \name Parametres de calcul
	//@{
		const cEnvironnement* const 	vEnvironnement;		//!< Environnement de calcul
		const RoutePlanningNode		_origin;			//!< Lieu de départ
		const RoutePlanningNode		_destination;		//!< Lieu d'arrivée
		cMoment					vMomentDebut;		//!< Moment de d�but du calcul (premier d�part) @todo rendre const
		cMoment					vMomentFin;			//!< Moment de fin du calcul (dernier d�part) @todo rendre const
		const cMoment					_MomentCalcul;		//!< Moment de lancement du calcul (pour filtrage r�sa et d�parts pass�s)
		const tBool3					vBesoinVelo;		//!< Filtre v�lo
		const tBool3					vBesoinHandicape;	//!< Filtre PMR
		const tBool3					vBesoinTaxiBus;		//!< Filtre TAD
		const tIndex			vCodeTarif;			//!< Filtre tarification
		const bool					_BaseTempsReel;		//!< Base utilis�e pour les calculs (v�rifier l'activation)
		const RoutePlanningNode::DistanceInMeters					_maxApproachDistance;	//!< Maximal approach distance
		const RoutePlanningNode::SpeedInKmh			_approachSpeed;	//!< Approach speed
	//@}

	//! \name Calculateurs de contr�le pour la recherche d'itin�raires
	//@{
		bool				ControleLigne(const cLigne*, const cTrajet&)						const;
		bool				ControleTrajetRechercheArrivee(const cElementTrajet&)						const;
		bool				ControleTrajetRechercheDepart(const cElementTrajet&)						const;
		bool				DestinationUtilePourArriverTot(const LogicalPlace*, const cMoment&
								, const cDistanceCarree& __DistanceCarreeBut) 						const;
		bool				ProvenanceUtilePourPartirTard(const LogicalPlace*, const cMoment&
								, const cDistanceCarree& __DistanceCarreeBut) 						const;
	//@}
	
	//!	\name Accesseurs variables temporaires de calcul d'itin�raire
	//@{
		const cMoment&			getBestTime(const cArretPhysique*, bool isArrival)	const;
		const cMoment&			absoluteBestTime(bool isArrival)	const;
	//@}
	
	//!	\name Modificateurs et gestionnaires des variables temporaires de la recherche d'itin�raire
	//@{
		void				SetET(const LogicalPlace* curPA, cElementTrajet* newET, tIndex NumVoie=0);
		void				setBestTime(const RoutePlanningNode&, const cMoment&, bool isArrival, bool forOptimizing);
		void				setBestTime(const cArretPhysique*, const cMoment&, bool isArrival, bool forOptimizing, bool withRecursion=true);
		void				resetIntermediatesVariables();
	//@}
	
	//!	\name Calculateurs pour la recherche d'itin�raires utilis�es dans la r�cursion (sym�triques)
	//@{
		bool				EvalueGareLigneArriveeCandidate(const cGareLigne* __GareLigneArr, const cMoment& __MomentDepart
								, const cGareLigne* __GareLigneDep, tIndex __IndexService
								, BestSolutionMap& __SuiteElementsTrajets, const cTrajet& __TrajetEffectue
								, bool _OptimisationAFaire, const tDureeEnMinutes& __AmplitudeServiceContinu, cLog&);
		bool				EvalueGareLigneDepartCandidate(const cGareLigne* __GareLigneDep, const cMoment& __MomentArrivee
								, const cGareLigne* __GareLigneArr, tIndex __IndexService, BestSolutionMap& __SuiteElementsTrajets
								, const cTrajet& __TrajetEffectue, bool __OptimisationAFaire
								, const tDureeEnMinutes& __AmplitudeServiceContinu);
		BestSolutionMap		ListeDestinations (const cTrajet& TrajetEffectue, bool MomentDepartStrict, bool OptimisationAFaire);
		BestSolutionMap		ListeProvenances  (const cTrajet& TrajetEffectue, bool MomentArriveeStrict, bool OptimisationAFaire);
		bool				MeilleureArrivee(cTrajet& __Resultat, cTrajet& __TrajetEffectue, bool MomentDepartStrict
								, bool OptimisationAFaire);
		bool				MeilleurDepart(cTrajet& __Resultat, cTrajet& __TrajetEffectue, bool MomentArriveeStrict
								, bool OptimisationAFaire);
		bool				HoraireDepartArrivee(cTrajet& __Resultat);
	// 	bool				HoraireArriveeDepart(cTrajet& __Resultat);
	//@}
	
public:
	//!	\name Accesseurs
	//@{
		const cTrajets&		getSolution()			const;
	//@}
	
	//!	\name Calculateurs
	//@{
		bool 				FicheHoraire();
	//@}

	//!	\name Constructeur et destructeur
	//@{
		cCalculateur(const cEnvironnement* const environnement, const LogicalPlace* const __LieuOrigine, const LogicalPlace* const __LieuDestination
								, const cDate& MomentDepartMin, const cPeriodeJournee* const, const tBool3 besoinVelo
								, const tBool3 besoinHandicape, const tBool3 besoinTaxiBus, const tIndex codeTarif
								, const bool __SolutionsPassees, const RoutePlanningNode::DistanceInMeters maxApproachDistance
								, const RoutePlanningNode::SpeedInKmh approachSpeed
		);
		~cCalculateur();
	//@}
};

/** @} */

#endif
