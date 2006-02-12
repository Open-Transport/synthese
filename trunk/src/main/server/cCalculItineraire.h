/**	@file cCalculItineraire.h
	\brief En-t�te Classe CalculItineraire
*/

#ifndef SYNTHESE_cCalculateur_H
#define SYNTHESE_cCalculateur_H

class cCalculateur;
class cAccesReseau;

#include "cTrajets.h"
#include "cTrajets.h"
#include "cLog.h"
#include "Temps.h"
#include "cEnvironnement.h"
#include "cPeriodeJournee.h"

#ifdef UNIX
#include <pthread.h>
extern pthread_mutex_t mutex_calcul;
#endif

class LogicalPlace;

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
	//!	\name Occupation de l'espace
	//@{
	bool					_Libre;							//!< Indicateur d'espace libre ou occup� par un thread
	//@}
	
	//! \name Variables de travail recherche d'itin�raire
	//@{
	cMoment*				vMeilleurTemps;							//!< Meilleur temps de trajet trouv� vers chaque arr�t
	cElementTrajet**		vET;									//!< Solution trouv�e depuis/vers chaque arr�t
	cMoment**				vMeilleurTempsArretPhysique;			//!< Meilleur temps de trajet trouv� depuis/vers chaque quai de chaque arr�t
	cElementTrajet***		vETArretPhysique;						//!< Solution trouv�e depuis/vers chaque quai de chaque arr�t
	cMoment					vArriveeMax;							//!< Moment d'arriv�e maximal
	cMoment					vDepartMin;								//!< Moment de d�part minimal
	cDureeEnMinutes			vDureeServiceContinuPrecedent;			//!< Dur�e du trajet dans le service continu trouv� pr�c�demment
	cMoment					vDernierDepartServiceContinuPrecedent;	//!< Moment de fin de l'amplitude de validit� du service continu trouv� pr�c�demment
	int						vIterationsDep;							//!< Compteur d'it�rations pour les calculs de meilleur d�part
	int						vIterationsArr;							//!< Compteur d'it�rations pour les calculs de meilleure arriv�e
	cLog					_LogTrace;								//!< Fichier log de tracage des recursions
	cTexte					_CheminLog;								//!< Chemin des log de tracage des recursions
	//@}

	//! \name R�sultats
	//@{	
	cTrajets				vSolution;			//!< Solution retenue
	//@}

	//! \name Parametres de calcul
	//@{
	const cEnvironnement*	vEnvironnement;		//!< Environnement de calcul
	LogicalPlace*			_Origin;			//!< Lieu de départ
	LogicalPlace*			_Destination;		//!< Lieu d'arrivée
	cMoment					vMomentDebut;		//!< Moment de d�but du calcul (premier d�part)
	cMoment					vMomentFin;			//!< Moment de fin du calcul (dernier d�part)
	cMoment					_MomentCalcul;		//!< Moment de lancement du calcul (pour filtrage r�sa et d�parts pass�s)
	tBool3					vBesoinVelo;		//!< Filtre v�lo
	tBool3					vBesoinHandicape;	//!< Filtre PMR
	tBool3					vBesoinTaxiBus;		//!< Filtre TAD
	tNumeroTarif			vCodeTarif;			//!< Filtre tarification
	bool					_BaseTempsReel;		//!< Base utilis�e pour les calculs (v�rifier l'activation)
	//@}

	//! \name Calculateurs de contr�le pour la recherche d'itin�raires
	//@{
	bool				ControleLigne(const cLigne*, const cTrajet&)						const;
	bool				ControleTrajetRechercheArrivee(cElementTrajet&)						const;
	bool				ControleTrajetRechercheDepart(cElementTrajet&)						const;
	bool				DestinationUtilePourArriverTot(const cArretLogique*, const cMoment&
							, cDistanceCarree& __DistanceCarreeBut) 						const;
	bool				ProvenanceUtilePourPartirTard(const cArretLogique*, const cMoment&
							, cDistanceCarree& __DistanceCarreeBut) 						const;
	//@}
	
	//!	\name Accesseurs variables temporaires de calcul d'itin�raire
	//@{
	const cMoment&		GetMeilleurDepart(const cArretLogique* curPA, tNumeroVoie NumeroVoie=0)		const;
	const cMoment&		GetMeilleureArrivee(const cArretLogique* curPA, tNumeroVoie NumeroVoie=0)	const;
	cElementTrajet*		GetET(const cArretLogique* curPA, tNumeroVoie NumVoie=0);
	//@}
	
	//!	\name Modificateurs et gestionnaires des variables temporaires de la recherche d'itin�raire
	//@{
	void				SetET(const cArretLogique* curPA, cElementTrajet* newET, tNumeroVoie NumVoie=0);
	void				SetMeilleureArrivee(const cArretLogique* curPA, const cMoment& NewMoment, tNumeroVoie NumVoie=0);
	void				SetMeilleureArrivee(const cAccesPADe* curAccesPADe, const cMoment& NewMoment, tNumeroVoie NumVoie=0);
	void				SetMeilleurDepart(const cArretLogique* curPA, const cMoment& NewMoment, tNumeroVoie NumVoie=0);
	void				SetMeilleurDepart(const cAccesPADe* curAccesPADe, const cMoment& NewMoment, tNumeroVoie NumVoie=0);
	void				ResetMeilleuresArrivees();
	void				ResetMeilleursDeparts();
	//@}
	
	//!	\name Calculateurs pour la recherche d'itin�raires utilis�es dans la r�cursion (sym�triques)
	//@{
	bool				EvalueGareLigneArriveeCandidate(const cGareLigne* __GareLigneArr, const cMoment& __MomentDepart
							, const cGareLigne* __GareLigneDep, tIndex __IndexService
							, cTrajet& __SuiteElementsTrajets, const cTrajet& __TrajetEffectue
							, bool _OptimisationAFaire, const cDureeEnMinutes& __AmplitudeServiceContinu, cLog&);
	bool				EvalueGareLigneDepartCandidate(const cGareLigne* __GareLigneDep, const cMoment& __MomentArrivee
							, const cGareLigne* __GareLigneArr, tIndex __IndexService, cTrajet& __SuiteElementsTrajets
							, const cTrajet& __TrajetEffectue, bool __OptimisationAFaire
							, const cDureeEnMinutes& __AmplitudeServiceContinu);
	cElementTrajet**	ListeDestinations (const cTrajet& TrajetEffectue, bool MomentDepartStrict, bool OptimisationAFaire);
	cElementTrajet**	ListeProvenances  (const cTrajet& TrajetEffectue, bool MomentArriveeStrict, bool OptimisationAFaire);
	bool				MeilleureArrivee(cTrajet& __Resultat, cTrajet& __TrajetEffectue, bool MomentDepartStrict
							, bool OptimisationAFaire);
	bool				MeilleurDepart(cTrajet& __Resultat, cTrajet& __TrajetEffectue, bool MomentArriveeStrict
							, bool OptimisationAFaire);
	bool				HoraireDepartArrivee(cTrajet& __Resultat);
// 	bool				HoraireArriveeDepart(cTrajet& __Resultat);
	//@}
	
	//!	@name Calculateurs d'initialisation
	//@{
		void			_buildTransportNetworkAccessForDeparture(const cLieuLogique* __LieuOrigine, bool __AvecApprocheAPied=true);
		void			_buildTransportNetworkAccessForArrival(const cLieuLogique* __LieuDestination, bool __AvecApprocheAPied=true);
	//@}
	
public:
	//!	\name Accesseurs
	//@{
	const cTrajets&		getSolution()			const;
	//@}
	
	//!	\name Modificateurs
	//@{
	bool 				InitialiseFicheHoraireJournee(const cLieuLogique* __LieuOrigine, const cLieuLogique* __LieuDestination
								, const cDate& MomentDepartMin, const cPeriodeJournee*, tBool3 besoinVelo
								, tBool3 besoinHandicape, tBool3 besoinTaxiBus, tNumeroTarif codeTarif
								, bool __SolutionsPassees);
	void 				Libere();
	cCalculateur*		Prend();
	bool				setEnvironnement(const cEnvironnement*);
	//@}
	
	//!	\name Calculateurs
	//@{
	bool 				FicheHoraire();
	//@}

	//!	\name Constructeur et destructeur
	//@{
	cCalculateur();
	~cCalculateur();
	//@}
};

/** @} */

#endif
