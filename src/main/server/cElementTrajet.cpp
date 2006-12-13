
#include "cElementTrajet.h"
#include "LogicalPlace.h"


/*! \brief Constructeur
 \author Hugues Romain
 \date 2001-2005
*/
cElementTrajet::cElementTrajet( const NetworkAccessPoint* const origin,
                                const NetworkAccessPoint* const destination,
                                const synthese::time::DateTime& departureDate,
                                const synthese::time::DateTime& arrivalDate,
                                const size_t& serviceNumber,
                                const cLigne* const line,
                                const tTypeElementTrajet type,
                                int amplitude,
                                const cDistanceCarree& squaredDistanceFromGoal

                              ) : _origin( origin )
        , _destination( destination )
        , vMomentDepart( departureDate )
        , vMomentArrivee( arrivalDate )
        , vLigne( line )
        , vType( type )
        , vDureeEnMinutesRoulee( vMomentArrivee - vMomentDepart )
        , vAmplitudeServiceContinu( amplitude )
        , vDistanceCarreeObjectif( squaredDistanceFromGoal )
        , vNumeroService( serviceNumber )
{
    vSuivant = NULL;
}



cElementTrajet::~cElementTrajet()
{}


const cDistanceCarree& cElementTrajet::getDistanceCarreeObjectif() const
{
    return vDistanceCarreeObjectif;
}


/*! \brief Optimisation 3 - Fonction d'imitation d'un ET � un autre moment
 \warning Ne peut �tre utilis�e sur un service continu (renvoie NULL)
 \author Hugues Romain
 \date 2002
*/ 
/*cElementTrajet* cElementTrajet::Imite(const synthese::time::DateTime& MomentDepart, const synthese::time::DateTime& ArriveeMax) const
{
 int tempArretPhysique=0;
 synthese::time::DateTime tempMomentDepart=MomentDepart;
 cElementTrajet* newET;
 cElementTrajet* firstET=NULL;
 
 if (vAmplitudeServiceContinu)
  return(NULL);
 
 for (const cElementTrajet* curET=this; curET!=NULL; curET=curET->vSuivant)
 {
  if((newET = curET->vGareDepart->ProchainDirect(curET->vGareArrivee,tempMomentDepart, ArriveeMax, tempArretPhysique)))
  {
   tempArretPhysique = newET->vVoieArrivee;
   tempMomentDepart = newET->vMomentArrivee;
   if (firstET == NULL)
    firstET = newET;
   else
    *firstET += newET;
  }
  else
   return(NULL);
 }
 return(firstET);
}*/



const cAxe* cElementTrajet::Axe() const
{
    if ( vLigne == NULL )
        return ( NULL );
    else
        return ( vLigne->Axe() );
}

/*! \brief Optimisation 2 - Fonction de comparaison pour qsort dans ListeProvenances
 \param ET1 Premier trajet � comparer
 \param ET2 Second trajet � comparer
 \return true si ET1 est jug� plus int�ressant que ET2, false sinon
 \author Hugues Romain
 \date 2002-2005
*/
int CompareUtiliteETPourMeilleurDepart( const void* ET1, const void* ET2 )
{
    const cElementTrajet * oET1 = * ( const cElementTrajet** ) ET1;
    const cElementTrajet* oET2 = * ( const cElementTrajet** ) ET2;

    //! <li> depart du lieu souhaite</li>
    if ( oET1->getDistanceCarreeObjectif() == 0 )
        return true;
    if ( oET2->getDistanceCarreeObjectif() == 0 )
        return false;

    //! <li> diff�rence < 2 km</li>
    if ( oET1->getDistanceCarreeObjectif() == oET2->getDistanceCarreeObjectif() )
        return ( false );

    //! <li>diff�rence de niveau</li>
    if ( oET1->getOrigin() ->getLogicalPlace() ->NiveauCorrespondance( oET1->getDistanceCarreeObjectif() ) != oET2->getOrigin() ->getLogicalPlace() ->NiveauCorrespondance( oET2->getDistanceCarreeObjectif() ) )
        return ( oET2->getOrigin() ->getLogicalPlace() ->NiveauCorrespondance( oET2->getDistanceCarreeObjectif() ) - oET1->getOrigin() ->getLogicalPlace() ->NiveauCorrespondance( oET1->getDistanceCarreeObjectif() ) );

    //! <li>Comparaison directe des distances carr�es</li>
    return ( oET2->getDistanceCarreeObjectif() <= oET1->getDistanceCarreeObjectif() );
}

/*! \brief Optimisation 2 - Fonction de comparaison pour qsort dans ListeDestinations
 \param ET1 Premier trajet � comparer
 \param ET2 Second trajet � comparer
 \return true si ET1 est jug� plus int�ressant que ET2, false sinon
 \author Hugues Romain
 \date 2002-2005
*/
int CompareUtiliteETPourMeilleureArrivee( const void* ET1, const void* ET2 )
{
    const cElementTrajet * oET1 = * ( const cElementTrajet** ) ET1;
    const cElementTrajet* oET2 = * ( const cElementTrajet** ) ET2;

    //! <li>Arrivee au lieu souhaite</li>
    if ( oET1->getDistanceCarreeObjectif() == 0 )
        return true;
    if ( oET2->getDistanceCarreeObjectif() == 0 )
        return false;

    //! <li>diff�rence < 2 km</li>
    if ( oET1->getDistanceCarreeObjectif() == oET2->getDistanceCarreeObjectif() )
        return false;

    //! <li>diff�rence de niveau</li>
    if ( oET1->getDestination() ->getLogicalPlace() ->NiveauCorrespondance( oET1->getDistanceCarreeObjectif() ) != oET2->getDestination() ->getLogicalPlace() ->NiveauCorrespondance( oET2->getDistanceCarreeObjectif() ) )
        return ( oET2->getDestination() ->getLogicalPlace() ->NiveauCorrespondance( oET2->getDistanceCarreeObjectif() ) - oET1->getDestination() ->getLogicalPlace() ->NiveauCorrespondance( oET1->getDistanceCarreeObjectif() ) );

    //! <li>Comparaison directe des distances vers l'objectif</li>
    return ( oET2->getDistanceCarreeObjectif() <= oET1->getDistanceCarreeObjectif() );
}




const cTrain* cElementTrajet::getService() const
{
    return vLigne->getTrain( vNumeroService );
}

tDistanceHM cElementTrajet::DistanceParcourue() const
{
    return ( vDistanceParcourue );
}

cElementTrajet* cElementTrajet::getSuivant() const
{
    return vSuivant;
}


const cLigne* cElementTrajet::getLigne() const
{
    return vLigne;
}

const int& cElementTrajet::AmplitudeServiceContinu() const
{
    return ( vAmplitudeServiceContinu );
}

void cElementTrajet::setAmplitudeServiceContinu( const int& newVal )
{
    vAmplitudeServiceContinu = newVal;
}

void cElementTrajet::setSuivant( cElementTrajet *newVal )
{
    if ( newVal != NULL )
        newVal->_Precedent = this;
    vSuivant = newVal;
}


const int& cElementTrajet::DureeEnMinutesRoulee() const
{
    return ( vDureeEnMinutesRoulee );
}

const synthese::time::DateTime& cElementTrajet::MomentDepart() const
{
    return ( vMomentDepart );
}

const synthese::time::DateTime& cElementTrajet::MomentArrivee() const
{
    return ( vMomentArrivee );
}



tTypeElementTrajet cElementTrajet::Type() const
{
    return ( vType );
}




const cElementTrajet* cElementTrajet::Precedent() const
{
    return _Precedent;
}


