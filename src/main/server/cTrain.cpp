/*! \file cTrain.cpp
\brief Implémentation classe cTrain
\author Hugues Romain
\date 2000-2003
*/

#include "cTrain.h"
#include "cLigne.h"
#include "cJourCirculation.h"
#include "cModaliteReservation.h"


/*! Constructeur.
*/
cTrain::cTrain( cLigne* const line )
        : vLigne( line )
{
    vEtalementCadence = 0;
    vAttente = 0;
    vEstCadence = false;
}


/*! \brief Destructeur
*/
cTrain::~cTrain()
{}

/*! \brief Vérification de critères d'utilisation du service par rapport à ses modalités de réservation
 \param __MomentDepart Heure du départ souhaité
 \param __MomentCalcul Moment du calcul pris comme référence pour le calcul du délai de réservation
 \return true si le service peut être réservé : 
  - soit la ligne n'a pas de modalité de réservation
  - soit la modalité de réservation accepte les conditions
*/
bool cTrain::ReservationOK( const synthese::time::DateTime& __MomentDepart, const synthese::time::DateTime& __MomentCalcul ) const
{
    return this->getLigne() ->GetResa() == NULL
           ? true
           : this->getLigne() ->GetResa() ->circulationPossible( this, __MomentCalcul, __MomentDepart );
}



void cTrain::setNumero( const cTexte& newVal )
{
    vNumero.Vide();
    vNumero << newVal;
}

void cTrain::setServiceContinu()
{
    vEstCadence = true;
}

cJC* cTrain::setJC( cJC* newVal )
{
    vCirculation = newVal;
    return ( newVal );
}

cJC* cTrain::getJC() const
{
    return ( vCirculation );
}

bool cTrain::EstCadence() const
{
    return ( vEstCadence );
}

const int& cTrain::EtalementCadence() const
{
    return ( vEtalementCadence );
}

const int& cTrain::Attente() const
{
    return ( vAttente );
}

void cTrain::setAttente( const int& newVal )
{
    vAttente = newVal;
}

void cTrain::setAmplitudeServiceContinu( const int& newVal )
{
    //#ifndef OS_LINUX
    //_ASSERTE(newVal >= 0 && newVal <= MinutesParJour);
    //#endif
    vEtalementCadence = newVal;
}

const cTexte& cTrain::getNumero() const
{
    return ( vNumero );
}

cLigne* cTrain::getLigne() const
{
    return ( vLigne );
}


/*! \brief Horaire de départ de la circulation
 \author Hugues Romain
 \date 2005
 \return Horaire de départ de la circulation (premier départ pour un service continu)
*/
const synthese::time::Schedule* cTrain::getHoraireDepartPremier() const
{
    return _HoraireDepart;
}

void cTrain::setHoraireDepart( synthese::time::Schedule* Ptr )
{
    _HoraireDepart = Ptr;
}


/*! \brief Calcul de la circulation du train le jour donné
 \param __DateDepart Date de départ du voyageur
 \param __JPlus Nombre de jours calendaires séparant la date de départ du voyageur de la date de départ de l'origine du train (référence du calendrier de circulation)
*/
bool cTrain::Circule( const synthese::time::Date& __DateDepart, int __JPlus ) const
{
    if ( __JPlus > 0 )
    {
        synthese::time::Date __DateDepartOrigine;
        __DateDepartOrigine = __DateDepart;
        __DateDepartOrigine -= __JPlus;
        return vCirculation->Circule( __DateDepartOrigine );
    }
    else
        return vCirculation->Circule( __DateDepart );
}
