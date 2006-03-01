/** Implémentation classe calculateur d'itinéraires.
@file cCalculItineraire.cpp
@author Hugues Romain
@date 2000-2006
*/

#include "cCalculItineraire.h"
#include "cTrajet.h"
#include "LogicalPlace.h"
#include "SYNTHESE.h"
#include "RoutePlanningNode.h"
#include "cGareLigne.h"
#include "cTrain.h"
#include "cVelo.h"
#include "cHandicape.h"
#include "cTarif.h"
#include "cElementTrajet.h"
#include "cModaliteReservation.h"

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

/** Constructeur.
 
 
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
cCalculateur::cCalculateur( const cEnvironnement* const environnement, const LogicalPlace* const __LieuOrigine, const LogicalPlace* const __LieuDestination
                            , const synthese::time::Date& __DateDepart
                            , const cPeriodeJournee* const __PeriodeJournee
                            , const tBool3 besoinVelo, const tBool3 besoinHandicape
                            , const tBool3 besoinTaxiBus, const int codeTarif
                            , const bool __SolutionsPassees
                            , const RoutePlanningNode::DistanceInMeters maxApproachDistance
                            , const RoutePlanningNode::SpeedInKmh approachSpeed
                          )
        : vEnvironnement( environnement )
        , _maxApproachDistance( maxApproachDistance )
        , _approachSpeed( approachSpeed )
        , _origin( __LieuOrigine, true, _maxApproachDistance, _approachSpeed )
        , _destination( __LieuDestination, false, _maxApproachDistance, _approachSpeed )
        , _MomentCalcul()
        , _BaseTempsReel( __DateDepart == _MomentCalcul.getDate() )
        , vBesoinVelo( besoinVelo )
        , vBesoinHandicape( besoinHandicape )
        , vBesoinTaxiBus( besoinTaxiBus )
        , vCodeTarif( codeTarif )
{

    vMomentDebut = __DateDepart;
    vMomentFin = vMomentDebut;


    // Application de la plage horaire
    __PeriodeJournee->AppliquePeriode( vMomentDebut, vMomentFin, _MomentCalcul, __SolutionsPassees );

    vMomentFin += int( cDistanceCarree( *__LieuDestination, *__LieuOrigine ).Distance() );


    // Optimisations (int�ret � v�rifier)
    // vMomentDebut = vPADeOrigine->momentDepartSuivant(vMomentDebut, vMomentFin, _MomentCalcul);
    // vMomentFin = vPADeDestination->momentArriveePrecedente(vMomentFin, vMomentDebut);
    // vMomentFin += int(1);

}





/*! \brief Destructeur
*/
cCalculateur::~cCalculateur()
{}



/** Calcul du meilleur trajet � moment de d�part connu en effectuant la d�termination de la meilleure arriv�e puis en optimisant l'heure de d�part.
 \retval __Resultat Variable o� �crire le r�sultat du calcul
 \return true si un calcul a pu �tre men� � bien quel qu'en soit le r�sultat, false sinon
 \author Hugues Romain
 \date 2001-2005
*/
bool cCalculateur::HoraireDepartArrivee( cTrajet& __Resultat )
{
    // Variables locales
    cTrajet __TrajetEffectue;

    // Mises � z�ro

    _startTime = vMomentDebut;
    __Resultat.Vide();

    // Meilleures arriv�es
    resetIntermediatesVariables();
    setBestTime( _destination, vMomentFin, true, false );
    setBestTime( _origin, vMomentDebut, true, false );

    // Calcul de la meilleure arriv�e possible
    _LogTrace.Ecrit( LogDebug, "Recherche de la meilleure arriv�e", "" );
    if ( !MeilleureArrivee( __Resultat, __TrajetEffectue, false, false ) )
        return false;

    // Si un trajet a �t� trouv�, tentative d'optimisation en retardant au maximum l'heure de d�part
    if ( __Resultat.Taille() )
    {
        _LogTrace.Ecrit( LogDebug, "Recherche du meilleur d�part", "" );

        // Meilleurs d�parts
        resetIntermediatesVariables();
        for ( cElementTrajet * TempET = __Resultat.getPremierElement(); TempET != 0; TempET = TempET->getSuivant() )
            setBestTime( ( cArretPhysique* ) TempET->getOrigin(), TempET->MomentDepart(), false, true );
        setBestTime( _destination, __Resultat.getMomentArrivee(), false, true );

        // Bornes du calcul
        _startTime = __Resultat.getMomentArrivee();

        return MeilleurDepart( __Resultat, __TrajetEffectue, true, true );
    }
    return true;
}

/*
cElementTrajet* cEnvironnement::HoraireArriveeDepart(LogicalPlace* GareOrigine, LogicalPlace* GareDestination, synthese::time::DateTime& MomentArrivee, synthese::time::DateTime& DepartMin, DureeEnMinutes DureeMax, TSituation Situation, bool EcritLOG)
{
 cElementTrajet* TempET = new cElementTrajet;
 cElementTrajet* Resultat;
 synthese::time::DateTime MomentMin;
 
// IterationsArr = 0;
// IterationsDep = 0;
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
 
 if (Resultat != 0)
 {
  MomentMin = Resultat->Dernier->MomentArrivee;
  //  MomentMin.Heure += 1;
  TempET->GareArrivee = GareOrigine;
  TempET->MomentArrivee = Resultat->MomentDepart;
  Resultat->deleteTrajet();
 
  Resultat = MeilleureArrivee(GareDestination, MomentMin, TempET->MomentArrivee, TempET, false, true, true, DureeMax, Situation, EcritLOG);
  delete TempET;
 }
 return(Resultat);
 */ 
/* return(0);
}*/

/*! \brief Calcul de la fiche horaire en tenant compte des param�tres d�j� fournis dans la classe
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
    bool __AllocationOK = true;
    vSolution.Vide();
    vDureeServiceContinuPrecedent = 0;
    vIterationsArr = 0;
    vIterationsDep = 0;

    // Gestion des logs de debug
    if ( Synthese.getNiveauLog() <= LogDebug && Synthese.getCheminLog().Taille() )
    {
        cTexte __Chemin;
        cTexteCodageInterne __MomentInterne;
        synthese::time::DateTime __Maintenant;
        __Maintenant.updateDateTime();
        // __MomentInterne << __Maintenant; // MJ review this

        _CheminLog << Synthese.getCheminLog() << "/" << __MomentInterne;
#ifdef UNIX

        mkdir( _CheminLog.Texte(), 0770 );
#endif
#ifdef WIN32

        mkdir( _CheminLog.Texte() );
#endif

        __Chemin << _CheminLog << "/" << LOG_FICHIER_TRACE;
        _LogTrace.Ouvrir( __Chemin );

        __Chemin.Vide();
        __Chemin << _CheminLog << "/" << LOG_CHEMIN_DESTINATIONS;
#ifdef UNIX

        mkdir( __Chemin.Texte(), 0770 );
#endif
#ifdef WIN32

        mkdir( __Chemin.Texte() );
#endif

        __Chemin.Vide();
        __Chemin << _CheminLog << "/" << LOG_CHEMIN_PROVENANCES;
#ifdef UNIX

        mkdir( __Chemin.Texte(), 0770 );
#endif
#ifdef WIN32

        mkdir( __Chemin.Texte() );
#endif

    }


    /*! <li>Boucle tant que l'on se trouve sur la plage horaire restreinte � l'utile</li><ul> */
    while ( true )
    {
        //! <li>Calcul du prochain trajet</li>
        _LogTrace.Ecrit( LogDebug, "Nouveau calcul de trajet", "" );
        if ( !HoraireDepartArrivee( __Trajet ) )
        {
            __AllocationOK = false;
            break;
        }

        //! <li>Si rien trouv� et si service pr�c�dent continu alors reprise a l'issue de la p�riode de continuit�</li>
        if ( !__Trajet.Taille() && vSolution.Taille() && vSolution.getDernier().getAmplitudeServiceContinu() )
        {
            vMomentDebut = vDernierDepartServiceContinuPrecedent;
            vMomentDebut += int( 1 );
            vDureeServiceContinuPrecedent = 0;
            if ( !HoraireDepartArrivee( __Trajet ) )
            {
                __AllocationOK = false;
                break;
            }
        }

        /*! <li>Si toujours rien trouv� fin du calcul, sortie de la boucle</li> */
        if ( !__Trajet.Taille() )
            break;

        /*! <li>En cas de rupture du service continu pr�c�dent, correction de de son amplitude</li> */
        if ( vSolution.Taille() && vSolution.getDernier().getAmplitudeServiceContinu() && __Trajet.getMomentDepart() <= vDernierDepartServiceContinuPrecedent )
        {
            int Duree;
            Duree = __Trajet.getMomentArrivee() - vSolution.getDernier().getMomentArrivee();
            Duree--;
            vSolution.getDernier().setAmplitudeServiceContinu( Duree );
        }

        /*! <li>En cas de nouveau service continu, enregistrement de valeur pour le calcul de la prochaine solution</li> */
        if ( __Trajet.getAmplitudeServiceContinu() > 1 )
        {
            vDureeServiceContinuPrecedent = __Trajet.getMomentArrivee() - __Trajet.getMomentDepart();
            vDernierDepartServiceContinuPrecedent = __Trajet.getMomentDepart();
            vDernierDepartServiceContinuPrecedent += __Trajet.getAmplitudeServiceContinu();
        }
        else
            vDureeServiceContinuPrecedent = 0;

        /*! <li>Avancement direct de l'heure de d�but de recherche au prochain d�part (vraiment meilleur qu'un ++ ?)</li> */
        //vMomentDebut = __Trajet.getOrigin()->MomentDepartSuivant(__Trajet.getMomentDepart(), vMomentFin, _MomentCalcul);
        vMomentDebut++;

        /*! <li>Enregistrement de la solution dans le tableau des r�sultats</li> */
        vSolution.SetElement( __Trajet, INCONNU, false );

        // Sortie 2 A REVOIR ON PEUT ECONOMISER LA DERNIERE RECHERCHE SI CA MARCHAIT
        if ( vSolution.getDernier().getMomentArrivee() == vMomentFin )
            break;

    } /*! </ul> */

    // Traitement pr�alable de la liste de trajets si besoin
    vSolution.Finalise();

    _LogTrace.Fermer();

    return __AllocationOK;
} /*! </ul> */


/*! \brief Evaluation de l'opportunit� d'une gare ligne d'arriv�e de figurer dans la liste des destinations directes propos�es, pour donner lieu � une nouvelle r�cursivit� de MeilleureArrivee
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
inline bool cCalculateur::EvalueGareLigneArriveeCandidate( const cGareLigne* __GareLigneArr, const synthese::time::DateTime& __MomentDepart, const cGareLigne* __GareLigneDep, int __IndexService, BestSolutionMap& __SuiteElementsTrajets, const cTrajet& __TrajetEffectue, bool __OptimisationAFaire, const int& __AmplitudeServiceContinu, cLog& __LogTrace )
{
    if ( !__GareLigneArr )
        return true;

    // Heure d'arriv�e
    synthese::time::DateTime __MomentArrivee = __MomentDepart;
    __GareLigneArr->CalculeArrivee( *__GareLigneDep, __IndexService, __MomentDepart, __MomentArrivee );

    // Utilit� de la ligne et du point d'arret
    cDistanceCarree __DistanceCarreeBut;
    // if (!DestinationUtilePourArriverTot(__GareLigneArr->ArretLogique(), __MomentArrivee, __DistanceCarreeBut))
    //  return false;

    // Ruptures de services continus
    if ( vDureeServiceContinuPrecedent )
    {
        if ( __TrajetEffectue.Taille() )
        {
            if ( __TrajetEffectue.getMomentDepart() <= vDernierDepartServiceContinuPrecedent
                    && __MomentArrivee - __TrajetEffectue.getMomentDepart() >= vDureeServiceContinuPrecedent )
                return false;
        }
        else
        {
            if ( __MomentDepart < vDernierDepartServiceContinuPrecedent && __MomentArrivee - __MomentDepart >= vDureeServiceContinuPrecedent )
                return false;
        }
    }

    // Ecriture de l'ET si n�cessaire
    if ( ( __MomentArrivee < getBestTime( __GareLigneArr->ArretPhysique(), true ) )
            || ( __OptimisationAFaire && __MomentArrivee == getBestTime( __GareLigneArr->ArretPhysique(), true ) ) )
    {
        cElementTrajet * __ElementTrajet = new cElementTrajet(
                                               __GareLigneDep->ArretPhysique()
                                               , __GareLigneArr->ArretPhysique()
                                               , __MomentDepart
                                               , __MomentArrivee
                                               , __IndexService
                                               , __GareLigneDep->Ligne()
                                               , eTrajetAvecVoyageurs
                                               , __AmplitudeServiceContinu
                                               , __DistanceCarreeBut
                                           );





        // Arrêt physique déjà trouvé
        // On prend la nouvelle solution si elle est strictement mieux (@todo A VERIFIER)
        if ( __SuiteElementsTrajets.find( __GareLigneArr->ArretPhysique() ) == __SuiteElementsTrajets.end()
                || CompareUtiliteETPourMeilleureArrivee( &__ElementTrajet, &__SuiteElementsTrajets[ __GareLigneArr->ArretPhysique() ] ) )
        {
            delete __SuiteElementsTrajets[ __GareLigneArr->ArretPhysique() ];
            __SuiteElementsTrajets[ __GareLigneArr->ArretPhysique() ] = __ElementTrajet;
        }
        else
            delete __ElementTrajet;


        // Gestion de logs
        if ( Synthese.getNiveauLog() <= LogDebug )
        {
            cTexte __Message;
            //   if (__ETCree)
            //    __Message << "***CREATION***";
            if ( __MomentArrivee <= __MomentDepart )
            {
                // Placer un breakpoint ici pour g�rer ce type d'erreur
                __Message << "***ERREUR CHRONOLOGIE***";
            }
            __LogTrace.Ecrit( LogDebug, __ElementTrajet, __Message, "" );
        }

        setBestTime( __GareLigneArr->ArretPhysique(), __MomentArrivee, true, __OptimisationAFaire ); // Enregistrement meilleure arriv�e
        /*  if (_destination->includes(__GareLigneArr->ArretPhysique()))
          {
           setBestTime(__GareLigneArr->ArretPhysique(), __MomentArrivee); // Enregistrement meilleure arriv�e
         
           if (__OptimisationAFaire)
            vArriveeMax = __MomentArrivee;
           else
            vArriveeMax = vPADeDestination->momentArriveePrecedente(__MomentArrivee, vMomentDebut); // Ecriture MomentMax le cas �ch�ant
          }
          else
           SetMeilleureArrivee(__GareLigneArr->ArretLogique(), __MomentArrivee, __GareLigneArr->ArretPhysique()); // Enregistrement meilleure arriv�e
        */ 
    }

    // Retour arr�ter le parcours de la ligne si la destination a �t� atteinte
    return !_destination.includes( __GareLigneArr->ArretPhysique() );
}



/*! \brief Evaluation de l'opportunit� d'une gare ligne de d�part de figurer dans la liste des provenances directes propos�es, pour donner lieu � une nouvelle r�cursivit� de MeilleurDepart
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
inline bool cCalculateur::EvalueGareLigneDepartCandidate( const cGareLigne* __GareLigneDep, const synthese::time::DateTime& __MomentArrivee, const cGareLigne* __GareLigneArr, int __IndexService, BestSolutionMap& __SuiteElementsTrajets, const cTrajet& __TrajetEffectue, bool __OptimisationAFaire, const int& __AmplitudeServiceContinu )
{
    if ( !__GareLigneDep )
        return true;

    // Heure de d�part
    synthese::time::DateTime __MomentDepart = __MomentArrivee;
    __GareLigneDep->CalculeDepart( *__GareLigneArr, __IndexService, __MomentArrivee, __MomentDepart );

    // Test modalit� de r�servation
    if ( !__GareLigneDep->Ligne() ->getTrain( __IndexService ) ->ReservationOK( __MomentDepart, _MomentCalcul ) )
        return false;

    // Utilit� de la ligne et du point d'arret
    cDistanceCarree __DistanceCarreeBut;
    if ( !ProvenanceUtilePourPartirTard( __GareLigneDep->ArretPhysique() ->getLogicalPlace(), __MomentDepart, __DistanceCarreeBut ) )
        return false;

    // Controle de non d�passement de Dur�emax
    if ( vDureeServiceContinuPrecedent )
    {
        if ( __MomentDepart <= vDernierDepartServiceContinuPrecedent )
            if ( __TrajetEffectue.Taille() )
            {
                if ( ( __TrajetEffectue.getMomentArrivee() - __MomentDepart ) >= vDureeServiceContinuPrecedent )
                    return false;
            }
            else
                if ( ( __MomentArrivee - __MomentDepart ) >= vDureeServiceContinuPrecedent )
                    return false;
    }

    // Ecriture de l'ET si n�cessaire
    if ( __MomentDepart > getBestTime( __GareLigneDep->ArretPhysique(), false )
            || __OptimisationAFaire && __MomentDepart == getBestTime( __GareLigneDep->ArretPhysique(), false ) )
    {
        cElementTrajet * __ElementTrajet = new cElementTrajet(
                                               __GareLigneDep->ArretPhysique()
                                               , __GareLigneArr->ArretPhysique()
                                               , __MomentDepart
                                               , __MomentArrivee
                                               , __IndexService
                                               , __GareLigneDep->Ligne()
                                               , eTrajetAvecVoyageurs
                                               , __AmplitudeServiceContinu
                                               , __DistanceCarreeBut
                                           );

        // Arrêt physique déjà trouvé
        // On prend la nouvelle solution si elle est strictement mieux (@todo A VERIFIER)
        if ( __SuiteElementsTrajets.find( __GareLigneDep->ArretPhysique() ) == __SuiteElementsTrajets.end()
                || CompareUtiliteETPourMeilleurDepart( &__ElementTrajet, &__SuiteElementsTrajets[ __GareLigneDep->ArretPhysique() ] ) )
        {
            delete __SuiteElementsTrajets[ __GareLigneDep->ArretPhysique() ];
            __SuiteElementsTrajets[ __GareLigneDep->ArretPhysique() ] = __ElementTrajet;
        }
        else
            delete __ElementTrajet;



        setBestTime( __GareLigneDep->ArretPhysique(), __MomentDepart, false, __OptimisationAFaire ); // Enregistrement meilleure arriv�e
        /*  if (vPADeOrigine->inclue(__GareLigneDep->ArretLogique()))
          {
          
        setBestTime(__GareLigneDep->ArretPhysique(), __MomentDepart, false); // Enregistrement meilleure arriv�e
          
           if (__OptimisationAFaire)
            vDepartMin = __MomentDepart;
           else
            vDepartMin = vPADeOrigine->momentDepartSuivant(__MomentDepart, vMomentFin, _MomentCalcul); // Ecriture MomentMax le cas �ch�ant
          }
          else
           SetMeilleurDepart(__GareLigneDep->ArretLogique(), __MomentDepart, __GareLigneDep->ArretPhysique()); // Enregistrement meilleure arriv�e
        */ 
    }

    // Retour arr�ter le parcours de la ligne si la destination a �t� atteinte
    return !_origin.includes( __GareLigneDep->ArretPhysique() );
}



/*! \version 2.0
 \brief Fabrication de la liste des destinations directes d'un point d'arr�t
 \author Hugues Romain 
 \date 2001
 \return un tableau rempli par des �l�ments de trajet dont chacun propose la solution la plus rapide pour rallier chacune des destinations possibles depuis la gare de fin de TrajetEffectue. Ce tableau se termine par 0. Les �l�ments sont tri�s dans l'ordre d�croissant d'int�r�t, d�finis par la fonction de tri CompareUtiliteETPourMeilleureArrivee() donnant, en r�sum�
  - Les �l�ments permettant d'arriver au but
  - Les �l�ments situ�s � moins de 2 km carr�s du but
  - Les autres �l�ments tri�s selon l'ensemble des crit�res
 
Au cours de l'�x�cution de la fonction, les �l�ments sont chain�s par le pointeur Suivant pour pouvoir etre repris ensuite. Sa fonction originelle est restitu�e en fin de fonction (mise � 0)
 
*/
cCalculateur::BestSolutionMap cCalculateur::ListeDestinations( const cTrajet& TrajetEffectue, bool MomentDepartStrict, bool OptimisationAFaire )
{
    // D�clarations
    RoutePlanningNode * origins = TrajetEffectue.Taille()
                                  ? new RoutePlanningNode( TrajetEffectue.getDestination(), true, _maxApproachDistance, _approachSpeed )
                                  : new RoutePlanningNode( _origin );
    int NumArret;
    synthese::time::DateTime MomentDepartInitial;
    synthese::time::DateTime MomentDepart;
    synthese::time::DateTime MomentArrivee;
    BestSolutionMap __SuiteElementsTrajets;
    int AmplitudeServiceContinu;
    cDistanceCarree D;
    cLog __LogTrace;

    // Trace log
    if ( Synthese.getNiveauLog() <= LogDebug && _CheminLog.Taille() )
    {
        cTexte __Chemin;
        __Chemin << _CheminLog;
        __Chemin << "/" << LOG_CHEMIN_DESTINATIONS << "/" << vIterationsArr << LOG_EXTENSION;
        __LogTrace.Ouvrir( __Chemin );
    }

    // Initial time
    MomentDepartInitial = TrajetEffectue.Taille() ? TrajetEffectue.getMomentArrivee() : _startTime;

    // Iteration on start physical stops
    for ( RoutePlanningNode::AccessPointsMap::const_iterator iterOrigin = origins->getAccessPoints().begin();
            iterOrigin != origins->getAccessPoints().end();
            iterOrigin++ )
    {
        const cArretPhysique* const origin = iterOrigin->first;
        const int& transferDuration = iterOrigin->second.first;

        // Iteration on line-stops
        for ( cArretPhysique::LineStopVector::const_iterator iterDepLineStop = origin->departureLineStopVector().begin();
                iterDepLineStop != origin->departureLineStopVector().end();
                ++iterDepLineStop
            )
        {
            const cGareLigne* const CurrentGLD = *iterDepLineStop;

            // La ligne est-elle utilisable ? (axe deja pris dans le trajet effectu�)
            if ( ControleLigne( CurrentGLD->Ligne(), TrajetEffectue ) )
            {
                // Moment de d�part
                MomentDepart = MomentDepartInitial;
                MomentDepart += transferDuration;

                // La ligne courante permet elle d'atteindre la destination directement ?
                const cGareLigne* CurrentGLA = 0;
                //const cGareLigne* CurrentGLA = CurrentGLD->getLiaisonDirecteVers(vPADeDestination);

                // Tentative de cr�ation de nouvelle solution
                /*    NumArret = CurrentGLD->Prochain(
                     MomentDepart
                     , (CurrentGLA && OptimisationAFaire) 
                      ? vMeilleurTemps[CurrentGLA->ArretLogique()->Index()]
                      : vArriveeMax
                     , AmplitudeServiceContinu
                     , INCONNU
                     , _MomentCalcul
                    ); TODO REPRENDRE
                */
                if ( NumArret != INCONNU && ( !MomentDepartStrict || MomentDepart == MomentDepartInitial ) )
                {
                    // Evaluation de la gareligne rendant vers la destination si trouv�e
                    EvalueGareLigneArriveeCandidate( CurrentGLA, MomentDepart, CurrentGLD, NumArret, __SuiteElementsTrajets, TrajetEffectue, OptimisationAFaire, AmplitudeServiceContinu, __LogTrace );

                    for ( CurrentGLA = CurrentGLD->getArriveeCorrespondanceSuivante();
                            CurrentGLA != 0;
                            CurrentGLA = CurrentGLA->getArriveeCorrespondanceSuivante() )
                    {
                        if ( !EvalueGareLigneArriveeCandidate( CurrentGLA, MomentDepart, CurrentGLD, NumArret, __SuiteElementsTrajets, TrajetEffectue, OptimisationAFaire, AmplitudeServiceContinu, __LogTrace ) )
                            break;
                    } //end boucle currentGLA
                } //end num arret !=-1
            } //end controle ligne
        } //end currentGLD
    } //end gare origine

    // Allocation du tableau de r�sultats
    /* TableauRetour  = (cElementTrajet**) malloc ((__SuiteElementsTrajets.Taille()+1)*sizeof(cElementTrajet*));
      
     // Stockage en supprimant les arriv�es trop tardives r�siduelles (parues avant la mise
     // � jour de MomentMax
     int __NombreET=0;
     while(__SuiteElementsTrajets.PremierElement())
     {
      SetET(__SuiteElementsTrajets.PremierElement()->getGareArrivee(), 0, __SuiteElementsTrajets.PremierElement()->VoieArrivee());
     
      // COnservation et finition de l'ET
      if (vPADeDestination->inclue(__SuiteElementsTrajets.PremierElement()->getGareArrivee())
       || DestinationUtilePourArriverTot (
        __SuiteElementsTrajets.PremierElement()->getGareArrivee(), 
        __SuiteElementsTrajets.PremierElement()->MomentArrivee(), 
        __SuiteElementsTrajets.getPremierElement()->getDistanceCarreeObjectif()
      ) ){
       TableauRetour[__NombreET] = __SuiteElementsTrajets.getPremierElement();
       __SuiteElementsTrajets.DeliePremier();
       TableauRetour[__NombreET]->CalculeDureeEnMinutesRoulee();                        // DureeEnMinutes roulee
       TableauRetour[__NombreET]->setSuivant(0);
       __NombreET++;
      }
      else
      { // Destruction de l'ET
       __SuiteElementsTrajets.SupprimePremier();
      }
     }
     TableauRetour[__NombreET]=0;
    */ 
    // Tri des ET TODO REACTIVER
    // qsort(TableauRetour, __NombreET, sizeof(cElementTrajet*), CompareUtiliteETPourMeilleureArrivee);

    return __SuiteElementsTrajets;
}



/*! \version 2.0
 \brief Fabrication de la liste des provenances directes d'un point d'arr�t
 \author Hugues Romain 
 \date 2001-2005
 \return un tableau rempli par des �l�ments de trajet dont chacun propose la solution la plus rapide pour rallier chacune des provennaces possibles vers la gare de d�but de TrajetEffectue. Ce tableau se termine par 0. Les �l�ments sont tri�s dans l'ordre d�croissant d'int�r�t, d�finis par la fonction de tri CompareUtiliteETPourMeilleureArrivee() donnant, en r�sum�
  - Les �l�ments permettant de partir du but
  - Les �l�ments situ�s � moins de 2 km carr�s du but
  - Les autres �l�ments tri�s selon l'ensemble des crit�res
 
Au cours de l'�x�cution de la fonction, les �l�ments sont chain�s par le pointeur Suivant pour pouvoir etre repris ensuite. Sa fonction originelle est restitu�e en fin de fonction (mise � 0)
 
 @todo A INTEGRER A LISTE DESTINATIONS REVERSIBLE
*/

cCalculateur::BestSolutionMap cCalculateur::ListeProvenances( const cTrajet& TrajetEffectue, bool MomentArriveeStrict, bool OptimisationAFaire )
{
    /* // D�clarations
     cElementTrajet** TableauRetour;
     
     const cAccesPADe* GareDestination;
     const cGareLigne* CurrentGLA;
     const cGareLigne* CurrentGLD;
     tNumeroService  NumArret;
     synthese::time::DateTime    MomentArriveeInitial;
     synthese::time::DateTime    MomentArrivee;
     synthese::time::DateTime    MomentDepart;
     cTrajet    __SuiteElementsTrajets;
     int  DureeEnMinutesCorrespondance;
     int  AmplitudeServiceContinu;
     cDistanceCarree  D;
     
     MomentArriveeInitial = TrajetEffectue.Taille() ? TrajetEffectue.getMomentDepart() : vArriveeMax;
     
     // Balayage des lignes
     for ( GareDestination = TrajetEffectue.Taille() ? TrajetEffectue.getArretLogiqueDepart()->getAccesPADe() : vPADeDestination;
       GareDestination != 0; 
       GareDestination = GareDestination->getSuivant() )
     {
      for (CurrentGLA = GareDestination->getPremiereGareLigneArr(); CurrentGLA != 0; )
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
           CurrentGLD != 0;
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
     int __NombreET=0;
     while(__SuiteElementsTrajets.PremierElement())
     {
      SetET(__SuiteElementsTrajets.PremierElement()->getGareDepart(), 0, __SuiteElementsTrajets.PremierElement()->VoieDepart());
     
      // COnservation et finition de l'ET
      if (vPADeOrigine->inclue(__SuiteElementsTrajets.PremierElement()->getGareDepart())
       || ProvenanceUtilePourPartirTard(
        __SuiteElementsTrajets.PremierElement()->getGareDepart(), 
        __SuiteElementsTrajets.PremierElement()->MomentDepart(), 
        __SuiteElementsTrajets.getPremierElement()->getDistanceCarreeObjectif() ) )
      {
       TableauRetour[__NombreET] = __SuiteElementsTrajets.getPremierElement();
       __SuiteElementsTrajets.DeliePremier();
       TableauRetour[__NombreET]->CalculeDureeEnMinutesRoulee();                        // DureeEnMinutes roulee
       TableauRetour[__NombreET]->setSuivant(0);
       __NombreET++;
      }
      else
      { // Destruction de l'ET
       __SuiteElementsTrajets.SupprimePremier();
      }
     }
     TableauRetour[__NombreET]=0;
     
     // Tri des ET
     qsort(TableauRetour, __NombreET, sizeof(cElementTrajet*), CompareUtiliteETPourMeilleurDepart);
     
     return TableauRetour;
    */ return  BestSolutionMap();
}



/*! \brief Retourne un trajet permettant d'arriver le plus tot possible � un point destination
 \param __Resultat R�f�rence vers la variable o� �crire le r�sultat, peut contenir un r�sultat candidat calcul� par ailleurs
 \retval __Resultat R�sultat du calcul
 \param __TrajetEffectue Contient le d�but de parcours d�fini par les appels pr�c�dents dans la r�cursivit�
 \param MomentDepartStrict Indique si l'heure de d�part trouv�e doit �tre strictement �gale � l'heure de fin du trajet pr�c�dent
 \param OptimisationAFaire Indique si il s'agit de trouver seulement un moyen d'arriver le plus tot possible, ou bien de trouver le moyen le meilleur
 \return true si l'op�ration a �t� men�e avec succ�s (avec un r�sultat vide �ventuel), false sinon
 
Cette m�thode rend TrajetEffectue intact malgr� les manipulations temporaires qui sont effectu�es sur l'objet.
*/
bool cCalculateur::MeilleureArrivee( cTrajet& __Resultat, cTrajet& __TrajetEffectue, bool MomentDepartStrict, bool OptimisationAFaire )
{
    // Variables locales
    cTrajet __Candidat;     // Trajet Candidat

    // Sortie si trop de correspondances
    if ( __TrajetEffectue.Taille() > NMAXPROFONDEUR )
        return true;

    // logs
    vIterationsArr++;
    _LogTrace.Ecrit( LogDebug, __TrajetEffectue, vIterationsArr, "", "" );

    // Obtention de la liste des trajets directs possibles au d�part de l'arr�t
    BestSolutionMap __ElementsTrajetsDirects = ListeDestinations( __TrajetEffectue, MomentDepartStrict, OptimisationAFaire );

    // Etude de chaque possibilit� en effectuant une correspondance par les �l�ments de trajet direct propos�s
    for ( BestSolutionMap::iterator iterSolution = __ElementsTrajetsDirects.begin();
            iterSolution != __ElementsTrajetsDirects.end();
            ++iterSolution )
    {
        // A ce stade, le Candidat est toujours vide.
        // Cas �l�ment se rendant � destination le candidat est l'�l�ment
        if ( _destination.includes( iterSolution->first ) )
        {
            __Candidat.LieEnPremier( iterSolution->second );
            _LogTrace.Ecrit( LogDebug, __Candidat, vIterationsArr, "***RESULTAT***", "" );
        }
        else // Sinon r�cursion
        {
            // R��valuation du candidat
            if ( ControleTrajetRechercheArrivee( *iterSolution->second ) )
            {
                // Ajout de l'�l�ment de trajet �tuid� au trajet effectu�
                __TrajetEffectue.LieEnDernier( iterSolution->second );

                // R�cursion
                if ( !MeilleureArrivee( __Candidat, __TrajetEffectue, false, OptimisationAFaire ) )
                {
                    return false; // Cas erreur technique (allocation)
                }

                // Suppression du lien vers l'�l�ment de trajet �tudi� sur le trajet effectu�
                __TrajetEffectue.DelieDernier();

                // Si r�cursion a produit un r�sultat : exploitation
                if ( __Candidat.Taille() )
                {
                    // Insertion de l'�l�ment �tudi� en t�te du candidat pour produire un r�sultat potentiel
                    __Candidat.LieEnPremier( iterSolution->second );
                }
                else
                    delete iterSolution->second;
            }
            else
                delete iterSolution->second;
        }
        __Candidat.Finalise();

        // Si un candidat est cr�� : �lection
        if ( __Candidat.Taille() )
        {
            if ( !__Resultat.Taille()
                    || __Candidat.getMomentArrivee() < __Resultat.getMomentArrivee()
                    || ( __Candidat.getMomentArrivee() == __Resultat.getMomentArrivee() && __Candidat > __Resultat )
               )
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
    }

    // Nettoyage du tableau interm�diaire
    return true;
}



/*! \brief Retourne un trajet permettant de partir le plus tard possible d'un point provenance
 \param __Resultat R�f�rence vers la variable o� �crire le r�sultat, peut contenir un r�sultat candidat calcul� par ailleurs
 \retval __Resultat R�sultat du calcul
 \param __TrajetEffectue Contient la fin de parcours d�finie par les appels pr�c�dents dans la r�cursivit�
 \param MomentDepartStrict Indique si l'heure de d�part trouv�e doit �tre strictement �gale � l'heure de fin du trajet pr�c�dent
 \param OptimisationAFaire Indique si il s'agit de trouver seulement un moyen de partir le plus tard possible, ou bien de trouver le moyen le meilleur
 \return true si l'op�ration a �t� men�e avec succ�s (avec un r�sultat vide �ventuel), false sinon
 
Cette m�thode rend TrajetEffectue intact malgr� les manipulations temporaires qui sont effectu�es sur l'objet.
 
@todo A REINTEGRER A MEILLEUREARRIVEE REVERSIBLE
*/
bool cCalculateur::MeilleurDepart( cTrajet& __Resultat, cTrajet& __TrajetEffectue, bool MomentArriveeStricte, bool OptimisationAFaire )
{
    /* // Variables locales
     cElementTrajet**  __ElementsTrajetsDirects; // Liste des �l�ments de trajet direct
     cTrajet     __Candidat;     // Trajet Candidat
     
     // Sortie si trop de correspondances
     if (__TrajetEffectue.Taille() > NMAXPROFONDEUR)
      return true;
     
     // logs
     vIterationsDep++;
     _LogTrace.Ecrit(LogDebug, __TrajetEffectue, vIterationsDep, "", "");
     
     // Obtention de la liste des trajets directs possibles au d�part de l'arr�t
     __ElementsTrajetsDirects = ListeProvenances(__TrajetEffectue, MomentArriveeStricte, OptimisationAFaire);
     
     // Etude de chaque possibilit� en effectuant une correspondance par les �l�ments de trajet direct propos�s
     for (int __i=0; __ElementsTrajetsDirects[__i] != 0; __i++)
     {
      // A ce stade, le Candidat est toujours vide.
      // Cas �l�ment se rendant � destination le candidat est l'�l�ment
      if (vPADeOrigine->inclue(__ElementsTrajetsDirects[__i]->getGareDepart()))
      {
       __Candidat.LieEnPremier(__ElementsTrajetsDirects[__i]);
      }
      else // Sinon r�cursion
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
         return false; // Cas erreur technique (allocation)
        }
        
        // Suppression du lien vers l'�l�ment de trajet �tudi� sur le trajet effectu�
        __TrajetEffectue.DeliePremier();
        
        // Si r�cursion a produit un r�sultat : exploitation
        if (__Candidat.Taille())
        {
         // Insertion de l'�l�ment �tudi� en t�te du candidat pour produire un r�sultat potentiel
         __Candidat.LieEnDernier(__ElementsTrajetsDirects[__i]);
        }
        else // Sinon destruction du maillon
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
    */ return  true;
}





void cCalculateur::resetIntermediatesVariables()
{
    _bestTimes.clear();
}









/*! \brief Contr�le de l'acceptabilit� de la Ligne au vu des �lements en possession
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
bool cCalculateur::ControleLigne( const cLigne* Ligne, const cTrajet& __Trajet ) const
{
    // L'axe de la ligne autorise-t-il la prise des voyageurs ?
    if ( !Ligne->Axe() ->Autorise() )
        return false;

    // tests sur la prise en charge des velos
    if ( vBesoinVelo == Vrai )
    {
        if ( Ligne->getVelo() == 0
                || Ligne->getVelo() ->TypeVelo() == Faux )
            return false;
    }

    // tests sur la prise en charge des handicap�s
    if ( vBesoinHandicape == Vrai )
    {
        if ( Ligne->getHandicape() == 0
                || Ligne->getHandicape() ->getTypeHandicape() == Faux )
            return false;
    }

    // tests sur la restriction sur les taxi bus
    if ( vBesoinTaxiBus == Vrai )
    {
        if ( Ligne->GetResa() == 0
                || Ligne->GetResa() ->TypeResa() != cModaliteReservation::RuleType_COMPULSORY )
            return false;
    }

    // tests sur le choix des tarifs
    if ( vCodeTarif > -1 )
    {
        //tarif ligne==tarif demande + tarif 0=> gratuit
        if ( ( Ligne->getTarif() == 0 )
                || ( ( Ligne->getTarif() ->getNumeroTarif() != vCodeTarif )
                     && ( Ligne->getTarif() ->getNumeroTarif() != 0 ) ) )
            return false;
    }

    // Contr�le de l'axe vis � vis des axes d�j� emprunt�s, effectu� que si l'axe de la ligne est non libre et si un ET est fourni
    if ( !Ligne->Axe() ->Libre() && __Trajet.Taille() )
    {
        for ( const cElementTrajet * curET = __Trajet.PremierElement(); curET != 0; curET = curET->getSuivant() )
            if ( curET->Axe() == Ligne->Axe() )
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
bool cCalculateur::DestinationUtilePourArriverTot( const LogicalPlace* __ArretLogique, const synthese::time::DateTime& __Moment, const cDistanceCarree& __DistanceCarreeBut ) const
{
    /* //! <li>Calcul de la distance carr�e si non fournie</li>
     if (__DistanceCarreeBut.EstInconnu())
      __DistanceCarreeBut.setFromPoints(*__ArretLogique, *_destination);
     
     //! <li>Evaluation du moment "au plus tot" o� peut d�marrer</li>
     synthese::time::DateTime __MomentArriveeAvantInclusCorrespondance = __Moment;
     if (!vPADeDestination->inclue(__ArretLogique))
      __MomentArriveeAvantInclusCorrespondance += __ArretLogique->AttenteMinimale();
     
     //! <li>Test 1 : Non d�passement du moment d'arriv�e maximal</li>
     if (__MomentArriveeAvantInclusCorrespondance > vArriveeMax)
      return false;
     
     //! \todo Remettre ici un controle par VMAX
    */ 
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
bool cCalculateur::ProvenanceUtilePourPartirTard( const LogicalPlace* __ArretLogique, const synthese::time::DateTime& __Moment, const cDistanceCarree& __DistanceCarreeBut ) const
{
    //! <li>Calcul de la distance carr�e si non fournie</li>
    /* if (__DistanceCarreeBut.EstInconnu())
      __DistanceCarreeBut.setFromPoints(__ArretLogique->getPoint(), vPADeOrigine->getArretLogique()->getPoint());
     
     //! <li>Evaluation du moment "au plus tard" o� peut arriver</li>
     synthese::time::DateTime __MomentDepartApresInclusCorrespondance = __Moment;
     if (!vPADeOrigine->inclue(__ArretLogique))
      __MomentDepartApresInclusCorrespondance -= __ArretLogique->AttenteMinimale();
     
     //! <li>Test 1 : Non d�passement du moment de d�part minimal</li>
     if (__MomentDepartApresInclusCorrespondance < vDepartMin)
      return false;
     
     //! \todo Remettre ici un controle par VMAX
      
    */ return  true;
}


/*! \brief Contr�le de l'int�r�t de l'�l�ment de trajet pour rejoindre la destination
 \return true si l'�l�ment de trajet est susceptible d'avoir un interet, false si non
 \author Hugues Romain
 \date 2005
*/
bool cCalculateur::ControleTrajetRechercheArrivee( const cElementTrajet& __ET ) const
{
    return __ET.MomentArrivee() <= getBestTime( ( cArretPhysique* ) __ET.getDestination(), true )
           && DestinationUtilePourArriverTot( __ET.getDestination() ->getLogicalPlace(), __ET.MomentArrivee(), __ET.getDistanceCarreeObjectif() );
}


bool cCalculateur::ControleTrajetRechercheDepart( const cElementTrajet& __ET ) const
{
    return __ET.MomentDepart() >= getBestTime( ( cArretPhysique* ) __ET.getOrigin(), false )
           && ProvenanceUtilePourPartirTard( __ET.getOrigin() ->getLogicalPlace(), __ET.MomentDepart(), __ET.getDistanceCarreeObjectif() );
}

/** Accesseur meilleur temps vers un arrêt
*/
const synthese::time::DateTime& cCalculateur::getBestTime( const cArretPhysique* accessPoint, bool isArrival ) const
{
    BestTimeMap::const_iterator bestTime = _bestTimes.find( accessPoint );
    return ( bestTime == _bestTimes.end() || bestTime->second > _absoluteBestTime )
           ? _absoluteBestTime
           : bestTime->second;
}

/*! \brief Accesseur solution du dernier calcul d'itin�raire
 \return Pointeur vers trajets trouv�s par le dernier calcul d'itin�raire
 \author Hugues Romain
 \date 2000-2005
*/
const cTrajets& cCalculateur::getSolution() const
{
    return vSolution;
}




/** Sets the best time for each access points of a node.
 @param node Node
 @param moment Time
 @param isArrival true id time is an arrival time, false if departure time
*/
void cCalculateur::setBestTime( const RoutePlanningNode& node, const synthese::time::DateTime& moment, bool isArrival, bool forOptimizing )
{
    // Best time for each access point reachable by the node
    for ( RoutePlanningNode::AccessPointsMap::const_iterator accessPoint = node.getAccessPoints().begin();
            accessPoint != node.getAccessPoints().end();
            ++accessPoint )
    {
        synthese::time::DateTime otherDatetime = moment;
        if ( isArrival )
            otherDatetime += accessPoint->second.first;
        else
            otherDatetime -= accessPoint->second.first;
        setBestTime( accessPoint->first, otherDatetime, isArrival, forOptimizing, false );
    }
}



/** Sets the best time for an access point.
 @param accessPoint Access Point
 @param datetime Time
 @param isArrival true if time is an arrival time, false if departure time
 @param forOptimizing true if the better solution is computed, false if only the best time is computed
 @param withRecursion true if the time of the reachables access points of the logical point should be updated
*/
void cCalculateur::setBestTime( const cArretPhysique* accessPoint, const synthese::time::DateTime& datetime, bool isArrival, bool forOptimizing, bool withRecursion )
{
    // the goal
    const RoutePlanningNode & goal = isArrival ? _destination : _origin;

    // if optimization : the research continues even if the duration is the same as the best one. If not, the research continues only if the duration is strictly lower than the best one
    int optimizationDuration = forOptimizing ? 0 : 1;

    // saving the best moment if better or if first passage
    if ( _bestTimes.find( accessPoint ) == _bestTimes.end() || _bestTimes[ accessPoint ] > datetime )
        _bestTimes[ accessPoint ] = datetime;

    // Best time on the other access points reachable by a transfer in the logical place
    if ( withRecursion )
    {
        // The access point belongs to the goal : updating other points of the goal, and updating the whole already reached access points
        if ( goal.includes( accessPoint ) )
        {
            for ( RoutePlanningNode::AccessPointsMap::const_iterator item = goal.getAccessPoints().begin();
                    item != goal.getAccessPoints().end();
                    ++item )
            {
                const synthese::time::DateTime otherDateTime = datetime;
                // otherDateTime += accessPoint->second.first; ??
                // otherDateTime -= item->second.first; ??
                //! @todo REPARER
                setBestTime( item->first, otherDateTime, isArrival, forOptimizing, false );
            }
        } // The access point allows transfers
        else if ( accessPoint->getLogicalPlace() ->CorrespondanceAutorisee() != LogicalPlace::CorrInterdite )
            for ( LogicalPlace::AccessPointsVector::const_iterator otherAccessPoint = accessPoint->getLogicalPlace() ->getNetworkAccessPoints().begin();
                    otherAccessPoint != accessPoint->getLogicalPlace() ->getNetworkAccessPoints().end();
                    ++otherAccessPoint )
            {
                const int& transferDuration = accessPoint->getLogicalPlace() ->AttenteCorrespondance(
                            ( isArrival ? accessPoint : *otherAccessPoint ) ->getRankInLogicalPlace()
                            , ( isArrival ? *otherAccessPoint : accessPoint ) ->getRankInLogicalPlace()
                        );
                if ( transferDuration != LogicalPlace::FORBIDDEN_TRANSFER_DELAY )
                {
                    const synthese::time::DateTime otherdatetime = datetime;
                    //otherdatetime += isArrival ? transferDuration + optimizationDuration : -transferDuration - optimizationDuration; ??
                    setBestTime( ( const cArretPhysique* ) * otherAccessPoint, otherdatetime, isArrival, forOptimizing, false );
                }
            }
    }

}


/** Best time valid on all access points.
 @param isArrival true if time is arival time, false if time is departure time
 
 The best time for all access points is the worse best time at the goal access points, corrected by the amplitude of approach duration of the goal
*/
const synthese::time::DateTime& cCalculateur::absoluteBestTime( bool isArrival ) const
{
    const RoutePlanningNode & goal = isArrival ? _destination : _origin;
    RoutePlanningNode::AccessPointsMap::const_iterator bestItem = goal.getAccessPoints().begin();
    for ( RoutePlanningNode::AccessPointsMap::const_iterator item = goal.getAccessPoints().begin();
            item != goal.getAccessPoints().end();
            ++item )
    {
        if ( getBestTime( item->first, isArrival ) < getBestTime( bestItem->first, isArrival ) )
            bestItem = item;
    }
    return getBestTime( bestItem->first, isArrival );
}
