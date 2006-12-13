/*! \file cHandicape.cpp
\brief Impl�mentation classe modalit� de prise en charge des handicap�s
*/



#include "cHandicape.h"



/*! \brief Constructeur
 \author Hugues Romain
 \date 2005
 
Les objets modalit� de prise en charge des handicap�s sont initialis�s par les valeurs par d�faut suivantes :
 - Prise en charge gratuite
 - Pas de r�servation
 - Une place par v�hicule
*/
cHandicape::cHandicape( const size_t& id )
        : _id( id )
{
    setPrix( 0 );
    setResa( NULL );
    setContenance( 1 );
}

cHandicape::~cHandicape()
{}

void cHandicape::setTypeHandicape( tBool3 __Valeur )
{
    _Valeur = __Valeur;
}

tBool3 cHandicape::getTypeHandicape() const
{
    return ( _Valeur );
}

void cHandicape::setResa( cModaliteReservation* __Resa )
{
    _Resa = __Resa;
}

void cHandicape::setContenance( tContenance __Contenance )
{
    _Contenance = __Contenance;
}

tContenance cHandicape::getContenance() const
{
    return _Contenance;
}

void cHandicape::setDoc( const std::string& __Doc )
{
    _Doc = __Doc;
}


void cHandicape::setPrix( tPrix __Prix )
{
    _Prix = __Prix;
}

const size_t& cHandicape::getId() const
{
    return _id;
}

