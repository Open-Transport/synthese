/** @file cCalculItineraire.h
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

#include "04_time/HourPeriod.h"
#include "RoutePlanningNode.h"

#include <boost/logic/tribool.hpp>


#ifdef UNIX
#include <pthread.h>
extern pthread_mutex_t mutex_calcul;
#endif


/** Espace de calcul pour thread et m�thodes de calcul
 \author Hugues Romain
 \date 2001
 @ingroup m33
 
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
        typedef map<const cArretPhysique*, synthese::time::DateTime> BestTimeMap;

        /** Tableaux des meilleures solutions vers chaque arrêt physique */
        typedef map<cArretPhysique*, cElementTrajet*> BestSolutionMap;

    private:
        //! \name Variables de travail recherche d'itin�raire
        //@{
        BestTimeMap _bestTimes;   //!< Meilleur temps de trajet trouv� depuis/vers chaque quai de chaque arr�t
        int vDureeServiceContinuPrecedent;   //!< Dur�e du trajet dans le service continu trouv� pr�c�demment
        synthese::time::DateTime vDernierDepartServiceContinuPrecedent; //!< Moment de fin de l'amplitude de validit� du service continu trouv� pr�c�demment
        int vIterationsDep;       //!< Compteur d'it�rations pour les calculs de meilleur d�part
        int vIterationsArr;       //!< Compteur d'it�rations pour les calculs de meilleure arriv�e
        cLog _LogTrace;        //!< Fichier log de tracage des recursions
        cTexte _CheminLog;        //!< Chemin des log de tracage des recursions
        synthese::time::DateTime _startTime;        //!< Time cursor in the timetable during the computing
        synthese::time::DateTime _absoluteBestTime;      //!< Best time in goal
        //@}

        //! \name R�sultats
        //@{
        cTrajets vSolution;   //!< Solution retenue
        //@}

        //! \name Parametres de calcul
        //@{
        const cEnvironnement* const vEnvironnement;  //!< Environnement de calcul
        const RoutePlanningNode _origin;   //!< Lieu de départ
        const RoutePlanningNode _destination;  //!< Lieu d'arrivée
        synthese::time::DateTime vMomentDebut;  //!< Moment de d�but du calcul (premier d�part) @todo rendre const
        synthese::time::DateTime vMomentFin;   //!< Moment de fin du calcul (dernier d�part) @todo rendre const
        const synthese::time::DateTime _MomentCalcul;  //!< Moment de lancement du calcul (pour filtrage r�sa et d�parts pass�s)
        const boost::logic::tribool vBesoinVelo;  //!< Filtre v�lo
        const boost::logic::tribool vBesoinHandicape; //!< Filtre PMR
        const boost::logic::tribool vBesoinTaxiBus;  //!< Filtre TAD
        const int vCodeTarif;   //!< Filtre tarification
        const bool _BaseTempsReel;  //!< Base utilis�e pour les calculs (v�rifier l'activation)
        const RoutePlanningNode::DistanceInMeters _maxApproachDistance; //!< Maximal approach distance
        const RoutePlanningNode::SpeedInKmh _approachSpeed; //!< Approach speed
        //@}

        //! \name Calculateurs de contr�le pour la recherche d'itin�raires
        //@{
        bool ControleLigne( const cLigne*, const cTrajet& ) const;
        bool ControleTrajetRechercheArrivee( const cElementTrajet& ) const;
        bool ControleTrajetRechercheDepart( const cElementTrajet& ) const;
        bool DestinationUtilePourArriverTot( const LogicalPlace*, const synthese::time::DateTime&
                                             , const cDistanceCarree& __DistanceCarreeBut ) const;
        bool ProvenanceUtilePourPartirTard( const LogicalPlace*, const synthese::time::DateTime&
                                            , const cDistanceCarree& __DistanceCarreeBut ) const;
        //@}

        //! \name Accesseurs variables temporaires de calcul d'itin�raire
        //@{
        const synthese::time::DateTime& getBestTime( const cArretPhysique*, bool isArrival ) const;
        const synthese::time::DateTime& absoluteBestTime( bool isArrival ) const;
        //@}

        //! \name Modificateurs et gestionnaires des variables temporaires de la recherche d'itin�raire
        //@{
        void SetET( const LogicalPlace* curPA, cElementTrajet* newET, int NumVoie = 0 );
        void setBestTime( const RoutePlanningNode&, const synthese::time::DateTime&, bool isArrival, bool forOptimizing );
        void setBestTime( const cArretPhysique*, const synthese::time::DateTime&, bool isArrival, bool forOptimizing, bool withRecursion = true );
        void resetIntermediatesVariables();
        //@}

        //! \name Calculateurs pour la recherche d'itin�raires utilis�es dans la r�cursion (sym�triques)
        //@{
        bool EvalueGareLigneArriveeCandidate( const cGareLigne* __GareLigneArr, const synthese::time::DateTime& __MomentDepart
                                              , const cGareLigne* __GareLigneDep, int __IndexService
                                              , BestSolutionMap& __SuiteElementsTrajets, const cTrajet& __TrajetEffectue
                                              , bool _OptimisationAFaire, const int& __AmplitudeServiceContinu, cLog& );
        bool EvalueGareLigneDepartCandidate( const cGareLigne* __GareLigneDep, const synthese::time::DateTime& __MomentArrivee
                                             , const cGareLigne* __GareLigneArr, int __IndexService, BestSolutionMap& __SuiteElementsTrajets
                                             , const cTrajet& __TrajetEffectue, bool __OptimisationAFaire
                                             , const int& __AmplitudeServiceContinu );
        BestSolutionMap ListeDestinations ( const cTrajet& TrajetEffectue, bool MomentDepartStrict, bool OptimisationAFaire );
        BestSolutionMap ListeProvenances ( const cTrajet& TrajetEffectue, bool MomentArriveeStrict, bool OptimisationAFaire );
        bool MeilleureArrivee( cTrajet& __Resultat, cTrajet& __TrajetEffectue, bool MomentDepartStrict
                               , bool OptimisationAFaire );
        bool MeilleurDepart( cTrajet& __Resultat, cTrajet& __TrajetEffectue, bool MomentArriveeStrict
                             , bool OptimisationAFaire );
        bool HoraireDepartArrivee( cTrajet& __Resultat );
        //  bool    HoraireArriveeDepart(cTrajet& __Resultat);
        //@}

    public:
        //! \name Accesseurs
        //@{
        const cTrajets& getSolution() const;
        //@}

        //! \name Calculateurs
        //@{
        bool FicheHoraire();
        //@}

        //! \name Constructeur et destructeur
        //@{
        cCalculateur( const cEnvironnement* const environnement, const LogicalPlace* const __LieuOrigine, const LogicalPlace* const __LieuDestination
                      , const synthese::time::Date& MomentDepartMin, const synthese::time::HourPeriod* const, const boost::logic::tribool besoinVelo
                      , const boost::logic::tribool besoinHandicape, const boost::logic::tribool besoinTaxiBus, const int codeTarif
                      , const bool __SolutionsPassees, const RoutePlanningNode::DistanceInMeters maxApproachDistance
                      , const RoutePlanningNode::SpeedInKmh approachSpeed
                    );
        ~cCalculateur();
        //@}
};

#endif
