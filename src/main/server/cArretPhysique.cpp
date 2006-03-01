/*! \file cArretPhysique.cpp
\brief Impl�mentation classe quai
*/

#include "cArretPhysique.h"
#include "LogicalPlace.h"
#include "map/Address.h"



/*! \brief Constructeur
 \version 2
 \author Hugues Romain
 \date 2001-2005
*/
cArretPhysique::cArretPhysique( LogicalPlace* const logicalPlace, size_t rank )
        : NetworkAccessPoint( logicalPlace, rank )
{
}

cArretPhysique::~cArretPhysique()
{}



/** Liste des adresses routières permettant l'accès au lieu logique de l'arrêt physique.
 @param forDeparture : true indique que les adresses sont des point de départ, le temps de correspondance sera donc le temps pour se rendre à ces adresses depuis le point d'arrêt physique. false : inversement.
*/
NetworkAccessPoint::AddressList cArretPhysique::getAddresses( bool forDeparture ) const
{
    NetworkAccessPoint::AddressList result;
    LogicalPlace::AddressesMap addresslist = getLogicalPlace() ->getAddresses();

    for ( LogicalPlace::AddressesMap::iterator element = addresslist.begin(); element != addresslist.end(); element++ )
        result.push_back( AddressWithAccessDuration(
                              element->second
                              , forDeparture
                              ? getLogicalPlace() ->AttenteCorrespondance( ( int ) getRankInLogicalPlace(), ( int ) element->first )
                              : getLogicalPlace() ->AttenteCorrespondance( ( int ) element->first, ( int ) getRankInLogicalPlace() )
                          ) );

    return result;
}


/** Adds a Line-Stop to the physical stop.
 
 @todo Sort of the line stops by angle
*/
void cArretPhysique::addDepartureLineStop( cGareLigne* const curGareLigne )
{
    _departureLineStopVector.insert( curGareLigne );
}


/** Adds a Line-Stop to the physical stop.
 
 @todo Sort of the line stops by angle
*/
void cArretPhysique::addArrivalLineStop( cGareLigne* const curGareLigne )
{
    _arrivalLineStopVector.insert( curGareLigne );
}

