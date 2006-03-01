/*! \file cGareLigne.cpp
\brief Impl�mentation Classe GareLigne
*/

#include "cGareLigne.h"
#include "cLigne.h"
#include "cTrain.h"
#include "cArretPhysique.h"

#include "04_time/Schedule.h"
#include "04_time/DateTime.h"


/** Constructeur.
 \param newLigne Ligne � laquelle appartient l'objet
 \param newPH Positionnement lin�aire : Point hectom�trique de l'arr�t sur la ligne
 \param newArretPhysique Num�ro de l'arr�t physique desservi par la ligne au sein de l'arr�t logique
 \param newType Type de desserte : autorisation de d�part et/ou d'arriv�e � l'arr�t
 \param newPA Arr�t logique desservi par la ligne
 \author Hugues Romain
 \date 2001
*/
cGareLigne::cGareLigne( cLigne* newLigne, tDistanceM newPM, tTypeGareLigneDA newType, cArretPhysique* const physicalStop, bool newHorairesSaisis )
        : _physicalStop( physicalStop )
        , vLigne( newLigne )
        , vPM( newPM )
        , vHorairesSaisis( newHorairesSaisis )
{
    vDepartCorrespondancePrecedent = NULL;
    vDepartPrecedent = NULL;
    vArriveeCorrespondanceSuivante = NULL;
    vArriveeSuivante = NULL;
    vSuivant = NULL;
    _Precedent = NULL;

    // Ligne
    setTypeDA( newType );     // Type

    AlloueHoraires();
}


/*! \brief Destructeur
*/
cGareLigne::~cGareLigne()
{
    delete[] vHoraireArriveeDernier;
    delete[] vHoraireArriveePremier;
    delete[] vHoraireArriveePremierReel;
    delete[] vHoraireDepartDernier;
    delete[] vHoraireDepartPremier;
    delete[] vHoraireDepartPremierReel;
}


/*!  \brief Remplit les horaires � partir du tampon
 \param Tampon Texte � interpr�ter
 \param Position Premier caract�re o� doit commencer l'analyse
 \param LargeurColonne Nombre de carac�res entre chaque horaire, donn�es comprises
 \param DepartDifferentPassage Indique si la fonction est appell�e une seconde fois pour d�crire les horaires de d�part apr�s avoir d�crit ceux d'arriv�e uniquement
 \version 3.0
 \author Hugues Romain
 \date 2003
 
 L'emploi de DepartDifferentPassage emp�che l'�crasement des horaires d'arriv�e dans le cas o� les horaires de d�part sont fournis sur une autre ligne de texte.
*/
void cGareLigne::setHoraires( const cTexte& Tampon, int Position, int LargeurColonne, bool DepartDifferentPassage )
{
    for ( size_t iService = 0; iService < vLigne->getServices().size(); iService++ )
    {
        vHoraireDepartPremier[ iService ] = std::string (Tampon.Extrait( Position ).Texte ());
        vHoraireDepartPremierReel[ iService ] = vHoraireDepartPremier[ iService ];
        vHoraireDepartDernier[ iService ] = vHoraireDepartPremier[ iService ];
        vHoraireDepartDernier[ iService ] += vLigne->getTrain( iService ) ->EtalementCadence();
        if ( !DepartDifferentPassage )
        {
            vHoraireArriveePremier[ iService ] = vHoraireDepartPremier[ iService ];
            // HR ESSAI PBFREQ2
            vHoraireArriveePremier[ iService ] += vLigne->getTrain( iService ) ->Attente();
            vHoraireArriveePremierReel[ iService ] = vHoraireArriveePremier[ iService ];
            // HR ESSAI PBFREQ2
            vHoraireArriveePremierReel[ iService ] += vLigne->getTrain( iService ) ->Attente();
            vHoraireArriveeDernier[ iService ] = vHoraireDepartDernier[ iService ];
            // HR ESSAI PBFREQ2
            vHoraireArriveeDernier[ iService ] += vLigne->getTrain( iService ) ->Attente();
        }

        // Ecriture de l'horaire de d�part des circulations si au d�part de la ligne
        if ( Ligne() ->getLineStops().front() == this )
            Ligne() ->getTrain( iService ) ->setHoraireDepart( &vHoraireDepartPremier[ iService ] );

        Position += LargeurColonne;
    }

    EcritIndexArrivee();
    EcritIndexDepart();
}



/*! \brief Contr�le la pertinence des horaires entr�s
 \param GareLigneAvecHorairesPrecedente Pr�c�dent arr�t de la ligne pourvu d'horaires
 \return true si aucun probl�me n'a �t� d�tect�, false sinon
 \author Hugues Romain
 \version 1.1
 \date 2001-2005
*/
bool cGareLigne::controleHoraire( const cGareLigne* GareLigneAvecHorairesPrecedente ) const
{
    // Test de chronologie verticale
    if ( GareLigneAvecHorairesPrecedente != NULL )
    {
        for ( size_t iNumeroService = 0; iNumeroService < vLigne->getServices().size(); iNumeroService++ )
            if ( vHoraireDepartPremier[ iNumeroService ] < GareLigneAvecHorairesPrecedente->vHoraireDepartPremier[ iNumeroService ] )
                return false;
    }

    // Test de chronologie horizontale
    for ( size_t iNumeroService = 1; iNumeroService < vLigne->getServices().size(); iNumeroService++ )
        if ( vHoraireDepartPremier[ iNumeroService ] < vHoraireDepartPremier[ iNumeroService - 1 ] )
            return false;

    // Test d'existence des heurs
    for ( size_t iNumeroService = 1; iNumeroService < vLigne->getServices().size(); iNumeroService++ )
        if ( !vHoraireArriveePremier[ iNumeroService ].isValid () || !vHoraireDepartPremier[ iNumeroService ].isValid () )
            return false;

    // Succ�s
    return true;
}


/*! \brief Calcule les horaires aux arr�ts o� les donn�es n'ont pas �t� fournies par le fichier de donn�es
 \version 2.0
 \author Hugues Romain
 \date 2001-2003
 \param Precedent Arr�t pr�c�dent disposant des donn�es horaires
 \param Suivant Arr�t suivant disposant des donn�es horaires
 \param Position Rang de la gareligne � l'int�rieur du segment, utilis� pour l'interpolation au cas o� les PH ne sont pas document�s
 \param Nombre Nombre total de GareLigne � l'int�rieur du segment, utilis� pour l'interpolation au cas o� les PH ne sont pas document�s
 
  La fonction calcule les heures de d�part et d'arriv�e de chaque GareLigne en interpolant � partir des GareLigne Precedent et Suivant.
*/
void cGareLigne::ChaineAvecHoraireSuivant( const cGareLigne& AHPrecedent, const cGareLigne& AHSuivant, size_t Position, size_t Nombre, int NumeroServiceATraiter )
{
    // Coefficient pour interpolation
    float CoefficientPosition;
    if ( AHPrecedent.vPM == AHSuivant.vPM )
        CoefficientPosition = ( ( float ) Position ) / Nombre;
    else
        CoefficientPosition = ( ( ( float ) vPM - AHPrecedent.vPM ) ) / ( AHSuivant.vPM - AHPrecedent.vPM );

    // Ecriture des arr�ts
    float DureeAAjouter;

    for ( size_t iService = NumeroServiceATraiter == -1 ? 0 : NumeroServiceATraiter;
            NumeroServiceATraiter != -1 && iService == NumeroServiceATraiter || NumeroServiceATraiter == -1 && iService < vLigne->getServices().size();
            ++iService )
    {
        // Calcul de l'horaire
        // MODIF HR PBCOLDBLE SOL 2  DureeAAjouter = CoefficientPosition * (AHSuivant.vHoraireArriveePremier[iService] - AHPrecedent.vHoraireDepartPremier[iService]);
        DureeAAjouter = CoefficientPosition * ( AHSuivant.vHoraireDepartPremier[ iService ] - AHPrecedent.vHoraireDepartPremier[ iService ] );

        vHoraireDepartPremier[ iService ] = AHPrecedent.vHoraireDepartPremier[ iService ];
        vHoraireDepartPremier[ iService ] += int( ( int ) floor( DureeAAjouter ) );
        vHoraireDepartPremierReel[ iService ] = vHoraireDepartPremier[ iService ];
        vHoraireDepartDernier[ iService ] = AHPrecedent.vHoraireDepartDernier[ iService ];
        vHoraireDepartDernier[ iService ] += int( ( int ) floor( DureeAAjouter ) );

        // MODIF HR PBCOLDBLE SOL 2
        DureeAAjouter = CoefficientPosition * ( AHSuivant.vHoraireArriveePremier[ iService ] - AHPrecedent.vHoraireArriveePremier[ iService ] );

        vHoraireArriveePremier[ iService ] = AHPrecedent.vHoraireArriveePremier[ iService ];
        vHoraireArriveePremier[ iService ] += int( ( int ) ceil( DureeAAjouter ) );
        vHoraireArriveePremierReel[ iService ] = vHoraireArriveePremier[ iService ];
        vHoraireArriveeDernier[ iService ] = AHPrecedent.vHoraireArriveeDernier[ iService ];
        vHoraireArriveeDernier[ iService ] += int( ( int ) ceil( DureeAAjouter ) );
    }

    EcritIndexArrivee();
    EcritIndexDepart();
}




/*! \brief Ecriture des index de d�part de la gare ligne, � partir des horaires des arr�ts.
 \version 2.1
 \author Hugues Romain
 \date 2001-2004
 
Ecrit tous les index d'un coup (contrairement � la version 1)
*/
void cGareLigne::EcritIndexDepart()
{
    int iNumeroHeure;

    int DerniereHeure = 25; // MODIF HR
    size_t NumeroServicePassantMinuit = 0; // MODIF HR

    // RAZ
    for ( iNumeroHeure = 0; iNumeroHeure < synthese::time::HOURS_PER_DAY; iNumeroHeure++ )
        vIndexDepart[ iNumeroHeure ] = -1;

    // Ecriture service par service
    for ( size_t iNumeroService = 0; iNumeroService < vLigne->getServices().size(); iNumeroService++ )
    {
        if ( vHoraireDepartDernier[ iNumeroService ].getHours () < DerniereHeure )
            NumeroServicePassantMinuit = iNumeroService;
        if ( vHoraireDepartDernier[ iNumeroService ].getHours () >= vHoraireDepartPremier[ iNumeroService ].getHours () )
        {
            for ( iNumeroHeure = 0; iNumeroHeure <= vHoraireDepartDernier[ iNumeroService ].getHours (); iNumeroHeure++ )
                if ( vIndexDepart[ iNumeroHeure ] == -1 || vIndexDepart[ iNumeroHeure ] < NumeroServicePassantMinuit )
                    vIndexDepart[ iNumeroHeure ] = iNumeroService;
        }
        else
        {
            for ( iNumeroHeure = 0; iNumeroHeure < synthese::time::HOURS_PER_DAY; iNumeroHeure++ )
                if ( vIndexDepart[ iNumeroHeure ] == -1 )
                    vIndexDepart[ iNumeroHeure ] = iNumeroService;
        }
        DerniereHeure = vHoraireDepartDernier[ iNumeroService ].getHours ();

    }

    // Ecriture du temps r�el
    for ( iNumeroHeure = 0; iNumeroHeure < 24; iNumeroHeure++ )
        vIndexDepartReel[ iNumeroHeure ] = vIndexDepart[ iNumeroHeure ];

}




/*! \brief Ecriture des index d'arriv�e de la gare ligne, � partir des horaires des arr�ts.
 \author Hugues Romain
 \date 2001-2004
 \version 2.1
 
Ecrit tous les index d'un coup (contrairement � la version 1)
*/
void cGareLigne::EcritIndexArrivee()
{
    int iNumeroHeure;

    // RAZ
    for ( iNumeroHeure = 0; iNumeroHeure < synthese::time::HOURS_PER_DAY; iNumeroHeure++ )
        vIndexArrivee[ iNumeroHeure ] = -1;

    int DerniereHeure = 25;
    size_t NumeroServicePassantMinuit = vLigne->getServices().size();

    for ( size_t iNumeroService = vLigne->getServices().size() - 1; iNumeroService >= 0; iNumeroService-- )
    {
        if ( vHoraireArriveePremier[ iNumeroService ].getHours () > DerniereHeure )
            NumeroServicePassantMinuit = iNumeroService;
        if ( vHoraireArriveeDernier[ iNumeroService ].getHours () >= vHoraireArriveePremier[ iNumeroService ].getHours () )
        {
            for ( iNumeroHeure = vHoraireArriveePremier[ iNumeroService ].getHours (); iNumeroHeure < synthese::time::HOURS_PER_DAY; iNumeroHeure++ )
                if ( vIndexArrivee[ iNumeroHeure ] == -1 || vIndexArrivee[ iNumeroHeure ] > NumeroServicePassantMinuit )
                    vIndexArrivee[ iNumeroHeure ] = iNumeroService;
        }
        else
        {
            for ( iNumeroHeure = 0; iNumeroHeure < synthese::time::HOURS_PER_DAY; iNumeroHeure++ )
                if ( vIndexArrivee[ iNumeroHeure ] == -1 )
                    vIndexArrivee[ iNumeroHeure ] = iNumeroService;
        }
        DerniereHeure = vHoraireArriveePremier[ iNumeroService ].getHours ();
    }

    // Ecriture du temps r�el
    for ( iNumeroHeure = 0; iNumeroHeure < synthese::time::HOURS_PER_DAY; iNumeroHeure++ )
        vIndexArriveeReel[ iNumeroHeure ] = vIndexArrivee[ iNumeroHeure ];
}



/*! \brief Fournit le prochain d�part de la ligne � l'arr�t (m�thode 1)
 \version 2.0
 \param MomentDepartMax Heure de d�part � ne pas d�passer
 \param MomentDepart  Heure de pr�sence sur le lieu de d�part
 \param NumProchainMin  Index o� commencer la recherche de prochain service
 \param __MomentCalcul Moment du calcul (pour v�rification du d�lai de r�servation
 \return Indice du service utilis� au sein de la ligne
 INCONNU (-1) = Aucun service trouv�
 \retval MomentDepart  Moment pr�cis du d�part. Inutilisable si INCONNU retourn�
 \date 2003-2005
 \author Hugues Romain
*/
int cGareLigne::Prochain(
    synthese::time::DateTime &MomentDepart
    , const synthese::time::DateTime& MomentDepartMax
    , const synthese::time::DateTime& __MomentCalcul
    , int NumProchainMin
) const
{
    int NumProchain;

    // Recherche de l'horaire
    // On se positionne par rapport � un tableau d'index par heure
    NumProchain = vIndexDepart[ MomentDepart.getHours () ];
    if ( NumProchain == INCONNU )
        NumProchain = vLigne->getServices().size();

    if ( NumProchainMin > NumProchain )
        NumProchain = NumProchainMin;

    while ( MomentDepart <= MomentDepartMax )  // boucle sur les dates
    {
        // Exploration des horaires si la ligne circule
        if ( vLigne->PeutCirculer( MomentDepart.getDate() ) )
        {
            while ( NumProchain < vLigne->getServices().size() )  // boucle sur les services
            {
                // TEST 12
                // Cas JPlus != en service continu
                if ( vLigne->getTrain( NumProchain ) ->EstCadence() && vHoraireDepartPremier[ NumProchain ].getDaysSinceDeparture () != vHoraireDepartDernier[ NumProchain ].getDaysSinceDeparture () )
                {
                    // Si service apres momentdepart alors modification
                    if ( MomentDepart > vHoraireDepartDernier[ NumProchain ] && MomentDepart < vHoraireDepartPremier[ NumProchain ] )
                        MomentDepart = vHoraireDepartPremier[ NumProchain ];

                    // Possibilit� de sortie
                    if ( MomentDepart > MomentDepartMax )
                        return INCONNU;

                    // on a presque valid� le service, il faut v�rifier la possiblite eventuelle de r�servation
                    if ( vLigne->getTrain( NumProchain ) ->ReservationOK( MomentDepart, __MomentCalcul ) )
                    {
                        // Si le service circule retour du service
                        if ( MomentDepart < vHoraireDepartDernier[ NumProchain ] )
                        {
                            if ( vLigne->getTrain( NumProchain ) ->Circule( MomentDepart.getDate(), vHoraireDepartDernier[ NumProchain ].getDaysSinceDeparture () ) )
                                return NumProchain;
                        }
                        else
                            if ( vLigne->getTrain( NumProchain ) ->Circule( MomentDepart.getDate(), vHoraireDepartPremier[ NumProchain ].getDaysSinceDeparture () ) )
                                return NumProchain;
                    }
                } // Fin TEST 12
                else //cas classique
                {
                    // Si service trop tot, ne peut pas etre pris
                    if ( MomentDepart <= vHoraireDepartDernier[ NumProchain ] )
                    {
                        // Si service apres moment depart alors modification, moment d�part = premier depart du service
                        if ( MomentDepart < vHoraireDepartPremier[ NumProchain ] )
                            MomentDepart = vHoraireDepartPremier[ NumProchain ];

                        // Possibilit� de sortie si le d�part trouv� est sup�rieur au dernier d�part permis
                        if ( MomentDepart > MomentDepartMax )
                            return INCONNU;

                        // Si le service circule retour du service
                        if ( vLigne->getTrain( NumProchain ) ->Circule( MomentDepart.getDate(), vHoraireDepartPremier[ NumProchain ].getDaysSinceDeparture () ) )
                        {
                            // on a presque valid� le service, il faut v�rifier la possiblite eventuelle de r�servation
                            // si il est trop tard pour r�server, on abandonne ce service
                            if ( vLigne->getTrain( NumProchain ) ->ReservationOK( MomentDepart, __MomentCalcul ) )
                            {
                                return NumProchain;
                            }
                        }
                    }
                }

                NumProchain++;

            } //end while

        } //end if

        MomentDepart++;
        MomentDepart.updateHour ( 0, 0 );

        // MODIF HR FIN JOUR MANQUANT  NumProchain = vIndexDepart[MomentDepart.getHours ()];
        NumProchain = vIndexDepart[ 0 ];
    }

    return INCONNU;
}


/*! \brief Fournit le prochain d�part de la ligne � l'arr�t (m�thode 2)
 \param MomentDepart Heure de pr�sence sur le lieu de d�part
 \param MomentDepartMax Heure de d�part � ne pas d�passer
 \param AmplitudeServiceContinu 
 \param NumProchainMin Index o� commencer la recherche de prochain service
 \param __MomentCalcul Moment du calcul pour validation d�lai de r�servation
 \return Indice du service utilis� au sein de la ligne. -1 = Aucun service trouv�. 
 \retval MomentDepart Moment pr�cis du d�part. Inutilisable si -1 retourn�.
 \retval AmplitudeServiceContinu Amplitude du service continu 0 = service � horaires
 \version 2.0
 \author Hugues Romain
 \date 2003
*/
int cGareLigne::Prochain(
    synthese::time::DateTime& MomentDepart
    , const synthese::time::DateTime& MomentDepartMax
    , int& AmplitudeServiceContinu
    , int NumProchainMin
    , const synthese::time::DateTime& __MomentCalcul
) const
{

    int NumProchain = Prochain( MomentDepart, MomentDepartMax, __MomentCalcul, NumProchainMin );

    // TEST 5
    if ( NumProchain != INCONNU && vLigne->getTrain( NumProchain ) ->EstCadence() )
    {
        if ( MomentDepart > vHoraireDepartDernier[ NumProchain ] )
            AmplitudeServiceContinu = 1440 - ( MomentDepart.getHour() - vHoraireDepartDernier[ NumProchain ].getHour() );
        else
            AmplitudeServiceContinu = vHoraireDepartDernier[ NumProchain ].getHour() - MomentDepart.getHour();
    } // FIN TEST 5
    else
        AmplitudeServiceContinu = 0;
    return NumProchain;
}


/*! \brief Recherche du d�part pr�c�dent pouvant �tre emprunt� sur la gare ligne
 \version 2.0
 \author Hugues Romain
 \date 2003
 \param MomentArriveeMin Heure d'arrivee � ne pas d�passer.
 \param MomentArrivee    Heure de pr�sence sur le lieu d'arrivee.
 \return Indice du service utilis� au sein de la ligne (INCONNU (-1) = Aucun service trouv�)
 \retval MomentArrivee Moment pr�cis de l'arriv�e. Inutilisable si INCONNU retourn�
 \warning La v�rification de l'utilisabilit� du service vis � vis d'un �ventuel d�lai de r�servation doit �tre ffectu�e lors de la d�termination de l'�arr�t de d�part, dont l'heure de passage fait foi pour le calcul du d�lai
*/
int cGareLigne::Precedent( synthese::time::DateTime &MomentArrivee, const synthese::time::DateTime& MomentArriveeMin ) const
{
    int NumPrecedent;

    // Recherche de l'horaire
    NumPrecedent = vIndexArrivee[ MomentArrivee.getHours () ];

    while ( MomentArrivee >= MomentArriveeMin )  // boucle sur les dates
    {
        // Exploration des horaires si la ligne circule
        if ( vLigne->PeutCirculer( MomentArrivee.getDate() ) )
            while ( NumPrecedent >= 0 )  // boucle sur les services
            {
                // Cas JPlus != en service continu
                if ( vLigne->getTrain( NumPrecedent ) ->EstCadence() && vHoraireArriveePremier[ NumPrecedent ].getDaysSinceDeparture () != vHoraireArriveeDernier[ NumPrecedent ].getDaysSinceDeparture () )
                {
                    // Si service apres momentdepart alors modification
                    if ( MomentArrivee > vHoraireArriveeDernier[ NumPrecedent ] && MomentArrivee < vHoraireArriveePremier[ NumPrecedent ] )
                        MomentArrivee = vHoraireArriveeDernier[ NumPrecedent ];

                    // Possibilit� de sortie
                    if ( MomentArrivee < MomentArriveeMin )
                        return INCONNU;

                    // Si le service circule retour du service
                    if ( MomentArrivee > vHoraireDepartPremier[ NumPrecedent ] )
                    {
                        if ( vLigne->getTrain( NumPrecedent ) ->Circule( MomentArrivee.getDate(), vHoraireArriveePremier[ NumPrecedent ].getDaysSinceDeparture () ) )
                            return NumPrecedent;
                    }
                    else
                        if ( vLigne->getTrain( NumPrecedent ) ->Circule( MomentArrivee.getDate(), vHoraireArriveeDernier[ NumPrecedent ].getDaysSinceDeparture () ) )
                            return NumPrecedent;
                }
                else
                {
                    // Si service trop tot, ne peut pas etre pris
                    if ( MomentArrivee >= vHoraireArriveePremier[ NumPrecedent ] )
                    {
                        // Si service apres momentdepart alors modification
                        if ( MomentArrivee > vHoraireArriveeDernier[ NumPrecedent ] )
                            MomentArrivee = vHoraireArriveeDernier[ NumPrecedent ];

                        // Possibilit� de sortie
                        if ( MomentArrivee < MomentArriveeMin )
                            return INCONNU;

                        // Si le service circule retour du service
                        if ( vLigne->getTrain( NumPrecedent ) ->Circule( MomentArrivee.getDate(), vHoraireArriveeDernier[ NumPrecedent ].getDaysSinceDeparture () ) )
                            return NumPrecedent;
                    }
                }
                NumPrecedent--;
            }

        MomentArrivee--;
        MomentArrivee.updateHour ( 23, 59 );
        // MODIF HR FIN JOURNEE NON PRESENT  NumPrecedent = vLigne->getServices().size() - 1;
        NumPrecedent = vIndexArrivee[ 23 ];
    }

    return INCONNU;
}



int cGareLigne::Precedent(
    synthese::time::DateTime & MomentArrivee
    , const synthese::time::DateTime& MomentArriveeMin
    , int& AmplitudeServiceContinu
) const
{

    int NumPrecedent = Precedent( MomentArrivee, MomentArriveeMin );

    if ( NumPrecedent != INCONNU && vLigne->getTrain( NumPrecedent ) ->EstCadence() )
    {
        if ( MomentArrivee > vHoraireArriveeDernier[ NumPrecedent ] )
            AmplitudeServiceContinu = 1440 - ( MomentArrivee.getHour() - vHoraireArriveeDernier[ NumPrecedent ].getHour () );
        // HR PBFREQ ESSAI1   AmplitudeServiceContinu = 1440 - (MomentArrivee.getHeure() - vHoraireArriveeDernier[NumPrecedent].getHeure()) + Attente(NumPrecedent);
        else
            AmplitudeServiceContinu = vHoraireArriveeDernier[ NumPrecedent ].getHour() - MomentArrivee.getHour ();
        // HR PBFREQ ESSAI1   AmplitudeServiceContinu = (vHoraireArriveeDernier[NumPrecedent].getHour () - MomentArrivee.getHour ()) + Attente(NumPrecedent);
    }
    else
        AmplitudeServiceContinu = 0;
    return NumPrecedent;
}


// Validit� solution
/*
void cGareLigne::ValiditeSolution(tNumeroService NumeroService, synthese::time::DateTime& Op)
{
 ArretDepart[NumeroService]->ValiditeSolution(Op);
}
*/ 
// HoraireDA

/*
bool cGareLigne::HeureArriveeAD(tNumeroService NumeroService, synthese::time::DateTime& MomentArrivee, synthese::time::Schedule& HoraireDepart)
{
 return(vArretArrivee[NumeroService].HeureArriveeAD(MomentArrivee, HoraireDepart));
}
 
void cGareLigne::HeureArriveeDA(tNumeroService NumeroService, synthese::time::DateTime& MomentArrivee, synthese::time::DateTime& MomentDepart, synthese::time::Schedule& HoraireDepart)
{
 vArretArrivee[NumeroService].HeureArriveeDA(MomentArrivee, MomentDepart, HoraireDepart);
}
 
void cGareLigne::HeureDepartAD(tNumeroService NumeroService, synthese::time::DateTime& MomentDepart, synthese::time::DateTime& MomentArrivee, synthese::time::Schedule& HoraireArrivee)
{
 vArretDepart[NumeroService].HeureDepartAD(MomentDepart, MomentArrivee, HoraireArrivee);
}
 
bool cGareLigne::HeureDepartDA(tNumeroService NumeroService, synthese::time::DateTime& MomentDepart, synthese::time::Schedule& HoraireDepart)
{
 return(vArretDepart[NumeroService].HeureDepartDA(MomentDepart, HoraireDepart));
}
*/

// Circule
/*
cGareLigne::Circule(tNumeroService NumeroService, tTypeGareLigneDA TypeGareLigne, synthese::time::Date& DateTest)
{
 if (TypeGareLigne == Depart)
  return(ArretDepart[NumeroService]->Circule(DateTest));
 else
  return(ArretArrivee[NumeroService]->Circule(DateTest));
}
*/

// NUL
/*bool cGareLigne::EstDepart()
{
 return(vTypeDA == Depart || vTypeDA == Passage);
}
 
bool cGareLigne::EstArrivee()
{
 return(vTypeDA == Arrivee || vTypeDA == Passage);
}
*/



int cGareLigne::MeilleurTempsParcours( const cGareLigne& autreGL ) const
{
    int curT;
    int bestT;

    for ( size_t iNumeroService = 0; iNumeroService != vLigne->getServices().size(); iNumeroService++ )
    {
        curT = autreGL.vHoraireArriveePremier[ iNumeroService ] - vHoraireDepartPremier[ iNumeroService ];
        if ( curT < 1 )
            curT = 1;
        if ( bestT == 0 || curT < bestT )
            bestT = curT;
    }
    return ( bestT );
}






/*! \brief Estimation de  la coh�rence d'une succession entre deux GAreLigne en fonction des PH et des coordonn�es des points d'arret
 \param AutreGareLigne GareLigne � comparer
 \return true si les donn�es semblent coh�rentes, false sinon
 \author Hugues Romain
 \version 1
 \date 2001
*/
bool cGareLigne::CoherenceGeographique( const cGareLigne& AutreGareLigne ) const
{
    tDistanceM DeltaPM;
    if ( PM() > AutreGareLigne.PM() )
        DeltaPM = ( vPM - AutreGareLigne.vPM ) / 1000;
    else
        DeltaPM = ( AutreGareLigne.vPM - vPM ) / 1000;
    tDistanceKM DeltaGPS = cDistanceCarree( *( ( const cPoint* ) ArretPhysique() ->getLogicalPlace() ), *( ( const cPoint* ) AutreGareLigne.ArretPhysique() ->getLogicalPlace() ) ).Distance();

    if ( DeltaPM > 10 * DeltaGPS && DeltaPM - DeltaGPS > 1 )
    {
        //  cTexte MessageErreur(TAILLETAMPON);
        //  MessageErreur << "Ligne: " << vLigne->getCode() << " PH: " << vPH << " PA1: " << ArretLogique()->Index() << " PA2: " << AutreGareLigne.ArretLogique()->Index() << " DeltaPH: " << DeltaPH << " DeltaGPS: " << DeltaGPS;
        //  Erreur(FichierLOG, "Erreur de coh�rence g�ographique (trajet trop long)", MessageErreur, "03006");
        return false;
    }
    if ( DeltaPM < DeltaGPS && DeltaGPS - DeltaPM > 1 )
    {
        //  cTexte MessageErreur(TAILLETAMPON);
        //  MessageErreur << "Ligne: " << vLigne->getCode() << " PH: " << vPH << " PA1: " << ArretLogique()->Index() << " PA2: " << AutreGareLigne.ArretLogique()->index() << " DeltaPH: " << DeltaPH << " DeltaGPS: " << DeltaGPS;
        //  Erreur(FichierLOG, "Erreur de coh�rence g�ographique (trajet trop court)", MessageErreur, "03007");
        return false;
    }

    // Succ�s
    return true;
}


/*
tTypeRefusLigne cGareLigne::UtilePourMeilleurDepart(cElementTrajet* ET, const synthese::time::DateTime& MomentMin) const
{
 if (!Circule(MomentMin, ET->MomentDepart()))
  return(RefusDate);
 
 if (!vLigne->Axe()->ETValide(ET))
  return(RefusAxe);
 
 return(Accepte);
}
*/


bool cGareLigne::Circule( const synthese::time::DateTime& MomentDebut, const synthese::time::DateTime& MomentFin ) const
{
    synthese::time::Date DateDebut;
    for ( DateDebut = MomentDebut; DateDebut <= MomentFin; DateDebut++ )
        if ( vLigne->PeutCirculer( DateDebut ) )
            return true;
    return false;
}


void cGareLigne::RealloueHoraires( size_t newService )
{
    // SET PORTAGE LINUX
    //_ASSERTE(vLigne->getServices().size() > 0);
    //END PORTAGE LINUX

    if ( vLigne->getServices().size() == 1 )
        AlloueHoraires();
    else
    {
        size_t iNumeroService;
        synthese::time::Schedule* newHoraire;

        newHoraire = new synthese::time::Schedule[ vLigne->getServices().size() ];
        for ( iNumeroService = 0; iNumeroService < vLigne->getServices().size() - 1; iNumeroService++ )
            if ( iNumeroService >= newService )
                newHoraire[ iNumeroService + 1 ] = vHoraireArriveeDernier[ iNumeroService ];
            else
                newHoraire[ iNumeroService ] = vHoraireArriveeDernier[ iNumeroService ];
        delete[] vHoraireArriveeDernier;
        vHoraireArriveeDernier = newHoraire;

        newHoraire = new synthese::time::Schedule[ vLigne->getServices().size() ];
        for ( iNumeroService = 0; iNumeroService < vLigne->getServices().size() - 1; iNumeroService++ )
            if ( iNumeroService >= newService )
                newHoraire[ iNumeroService + 1 ] = vHoraireArriveePremier[ iNumeroService ];
            else
                newHoraire[ iNumeroService ] = vHoraireArriveePremier[ iNumeroService ];
        delete[] vHoraireArriveePremier;
        vHoraireArriveePremier = newHoraire;

        newHoraire = new synthese::time::Schedule[ vLigne->getServices().size() ];
        for ( iNumeroService = 0; iNumeroService < vLigne->getServices().size() - 1; iNumeroService++ )
            if ( iNumeroService >= newService )
                newHoraire[ iNumeroService + 1 ] = vHoraireArriveePremierReel[ iNumeroService ];
            else
                newHoraire[ iNumeroService ] = vHoraireArriveePremierReel[ iNumeroService ];
        delete[] vHoraireArriveePremierReel;
        vHoraireArriveePremierReel = newHoraire;

        newHoraire = new synthese::time::Schedule[ vLigne->getServices().size() ];
        for ( iNumeroService = 0; iNumeroService < vLigne->getServices().size() - 1; iNumeroService++ )
            if ( iNumeroService >= newService )
                newHoraire[ iNumeroService + 1 ] = vHoraireDepartDernier[ iNumeroService ];
            else
                newHoraire[ iNumeroService ] = vHoraireDepartDernier[ iNumeroService ];
        delete[] vHoraireDepartDernier;
        vHoraireDepartDernier = newHoraire;

        newHoraire = new synthese::time::Schedule[ vLigne->getServices().size() ];
        for ( iNumeroService = 0; iNumeroService < vLigne->getServices().size() - 1; iNumeroService++ )
            if ( iNumeroService >= newService )
                newHoraire[ iNumeroService + 1 ] = vHoraireDepartPremier[ iNumeroService ];
            else
                newHoraire[ iNumeroService ] = vHoraireDepartPremier[ iNumeroService ];
        delete[] vHoraireDepartPremier;
        vHoraireDepartPremier = newHoraire;

        newHoraire = new synthese::time::Schedule[ vLigne->getServices().size() ];
        for ( iNumeroService = 0; iNumeroService < vLigne->getServices().size() - 1; iNumeroService++ )
            if ( iNumeroService >= newService )
                newHoraire[ iNumeroService + 1 ] = vHoraireDepartPremierReel[ iNumeroService ];
            else
                newHoraire[ iNumeroService ] = vHoraireDepartPremierReel[ iNumeroService ];
        delete[] vHoraireDepartPremierReel;
        vHoraireDepartPremierReel = newHoraire;
    }
}


void cGareLigne::CalculeArrivee( const cGareLigne &GareLigneDepart, size_t iNumeroService, const synthese::time::DateTime &MomentDepart, synthese::time::DateTime &MomentArrivee ) const
{
    // TEST 5
    if ( vLigne->getTrain( iNumeroService ) ->EstCadence() )
    {
        MomentArrivee = MomentDepart;
        // HR ESSAI PBFREQ2  MomentArrivee += vLigne->Attente(iNumeroService);
        MomentArrivee += ( vHoraireArriveePremier[ iNumeroService ] - GareLigneDepart.vHoraireDepartPremier[ iNumeroService ] );
    } // FIN TEST 5
    else
    {
        MomentArrivee = vHoraireArriveePremier[ iNumeroService ];
        MomentArrivee.addDaysDuration( vHoraireArriveePremier[ iNumeroService ].getDaysSinceDeparture () - GareLigneDepart.vHoraireDepartPremier[ iNumeroService ].getDaysSinceDeparture () );
    }
}

void cGareLigne::CalculeDepart( const cGareLigne &GareLigneArrivee, size_t iNumeroService, const synthese::time::DateTime &MomentArrivee, synthese::time::DateTime &MomentDepart ) const
{
    // TEST 5
    if ( vLigne->getTrain( iNumeroService ) ->EstCadence() )
    {
        MomentDepart = MomentArrivee;
        // HR ESSAI PBFREQ2  MomentDepart -= vLigne->Attente(iNumeroService);
        MomentDepart -= ( GareLigneArrivee.vHoraireArriveePremier[ iNumeroService ] - vHoraireDepartPremier[ iNumeroService ] );
    } // TEST 5
    else
    {
        MomentDepart = vHoraireDepartPremier[ iNumeroService ];
        MomentDepart.subDaysDuration( GareLigneArrivee.vHoraireArriveePremier[ iNumeroService ].getDaysSinceDeparture () - vHoraireDepartPremier[ iNumeroService ].getDaysSinceDeparture () );
    }
}


/*synthese::time::DateTime cGareLigne::CalculeArrivee(const cDescriptionPassage* curDP) const
{
 synthese::time::DateTime tempMoment;
 CalculeArrivee(*curDP->getGareLigne(), curDP->NumArret(), curDP->getMomentPrevu(), tempMoment);
 return(tempMoment);
}*/



void cGareLigne::AlloueHoraires()
{
    //_ASSERTE(vLigne->getServices().size() > 0);

    vHoraireArriveeDernier = new synthese::time::Schedule[ vLigne->getServices().size() ];
    vHoraireArriveePremier = new synthese::time::Schedule[ vLigne->getServices().size() ];
    vHoraireArriveePremierReel = new synthese::time::Schedule[ vLigne->getServices().size() ];
    vHoraireDepartDernier = new synthese::time::Schedule[ vLigne->getServices().size() ];
    vHoraireDepartPremier = new synthese::time::Schedule[ vLigne->getServices().size() ];
    vHoraireDepartPremierReel = new synthese::time::Schedule[ vLigne->getServices().size() ];
}


void cGareLigne::setDepartPrecedent( cGareLigne *newVal )
{
    vDepartPrecedent = newVal;
}


void cGareLigne::setDepartCorrespondancePrecedent( cGareLigne* __GL )
{
    vDepartCorrespondancePrecedent = __GL;
}



/*! \brief Recherche de liaison directe vers un groupe d'arr�ts en empruntant la ligne
 \param __Destination Groupe d'arr�t � rejoindre
 \return Adresse de la GareLigne permettant d'arriver au plus t�t � destination en empruntant la ligne courante, � partir de l'arr�t courant. NULL si liaison impossible selon ces conditions.
 \author Hugues Romain
 \date 2005
*/ 
/*const cGareLigne* cGareLigne::getLiaisonDirecteVers(const cAccesPADe* __Destination) const
{
 // Initialisation � la gareligne en cours
 const cGareLigne* __GareLigne = this; 
 
 while (true)
 {
  // Avancement de noeud en noeud si la destination est forc�ment un noeud (plus rapide), avncement d'arr�t en arr�t sinon
  if(__Destination->TousArretsDeCorrespondance())
   __GareLigne = __GareLigne->getArriveeCorrespondanceSuivante();
  else
   __GareLigne = __GareLigne->getArriveeSuivante();
  
  // Sortie si fin de ligne ou si arr�t reli� faisant partie de la destination
  if (__GareLigne == NULL || __Destination->inclue(__GareLigne->ArretLogique()))
   break;
 }
 
 // Sortie
 return __GareLigne;
}
*/


/*! \brief Recherche de liaison directe depuis un groupe d'arr�ts en empruntant la ligne
 \param __Provenance Groupe d'arr�t depuis lequel rejoindre
 \return Adresse de la GareLigne permettant de partir au plus tard de la provenance en empruntant la ligne courante, vers l'arr�t courant. NULL si liaison impossible selon ces conditions.
 \author Hugues Romain
 \date 2005
*/ 
/*const cGareLigne* cGareLigne::getLiaisonDirecteDepuis(const cAccesPADe* __Provenance) const
{
 // Initialisation � la gareligne en cours
 const cGareLigne* __GareLigne = this; 
 
 while (true)
 {
  // Avancement de noeud en noeud si la destination est forc�ment un noeud (plus rapide), avncement d'arr�t en arr�t sinon
  if(__Provenance->TousArretsDeCorrespondance())
   __GareLigne = __GareLigne->getDepartCorrespondancePrecedent();
  else
   __GareLigne = __GareLigne->getDepartPrecedent();
  
  // Sortie si fin de ligne ou si arr�t reli� faisant partie de la destination
  if (__GareLigne == NULL || __Provenance->inclue(__GareLigne->ArretLogique()))
   break;
 }
 
 // Sortie
 return __GareLigne;
}
*/

cLigne* cGareLigne::Ligne() const
{
    return ( vLigne );
}

void cGareLigne::setTypeDA( tTypeGareLigneDA newVal )
{
    vTypeDA = newVal;
    if ( EstDepart() )
        _physicalStop->addDepartureLineStop( this );
    if ( EstArrivee() )
        _physicalStop->addArrivalLineStop( this );
}

bool cGareLigne::EstArrivee() const
{
    return ( vTypeDA == Passage || vTypeDA == Arrivee );
}

bool cGareLigne::EstDepart() const
{
    return ( vTypeDA == Passage || vTypeDA == Depart );
}



cArretPhysique* cGareLigne::ArretPhysique() const
{
    return _physicalStop;
}

cGareLigne::tTypeGareLigneDA cGareLigne::TypeDA() const
{
    return ( vTypeDA );
}


bool cGareLigne::HorairesSaisis() const
{
    return ( vHorairesSaisis );
}


cGareLigne* cGareLigne::getArriveeSuivante() const
{
    return ( vArriveeSuivante );
}

cGareLigne* cGareLigne::getArriveeCorrespondanceSuivante() const
{
    return ( vArriveeCorrespondanceSuivante );
}

cGareLigne* cGareLigne::getDepartPrecedent() const
{
    return ( vDepartPrecedent );
}

cGareLigne* cGareLigne::getDepartCorrespondancePrecedent() const
{
    return ( vDepartCorrespondancePrecedent );
}

const synthese::time::Schedule& cGareLigne::getHoraireDepartPremier( size_t iService ) const
{
    //_ASSERTE(iService >= 0 && iService < vLigne->getServices().size());

    return ( vHoraireDepartPremier[ iService ] );
}


const synthese::time::Schedule& cGareLigne::getHoraireDepartDernier( size_t iNumeroService ) const
{
    //_ASSERTE(iNumeroService >= 0 && iNumeroService < vLigne->getServices().size());

    return ( vHoraireDepartDernier[ iNumeroService ] );
}

const synthese::time::Schedule& cGareLigne::getHoraireArriveePremier( size_t iNumeroService ) const
{
    //_ASSERTE(iNumeroService >= 0 && iNumeroService < vLigne->getServices().size());

    return ( vHoraireArriveePremier[ iNumeroService ] );
}


const synthese::time::Schedule& cGareLigne::getHoraireArriveeDernier( size_t iNumeroService ) const
{
    //_ASSERTE(iNumeroService >= 0 && iNumeroService < vLigne->getServices().size());

    return ( vHoraireArriveeDernier[ iNumeroService ] );
}

void cGareLigne::setArriveeSuivante( cGareLigne *newVal )
{
    vArriveeSuivante = newVal;
}

void cGareLigne::setArriveeCorrespondanceSuivante( cGareLigne *newVal )
{
    vArriveeCorrespondanceSuivante = newVal;
}

const synthese::time::Schedule& cGareLigne::getHoraireDepartPremierReel( size_t iNumeroService ) const
{
    return ( vHoraireDepartPremierReel[ iNumeroService ] );
}

const synthese::time::Schedule& cGareLigne::getHoraireArriveePremierReel( size_t iNumeroService ) const
{
    return ( vHoraireArriveePremierReel[ iNumeroService ] );
}

tDistanceM cGareLigne::PM() const
{
    return ( vPM );
}


