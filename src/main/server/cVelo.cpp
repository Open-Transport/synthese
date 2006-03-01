#include "cVelo.h"

using namespace std;

cVelo::cVelo( const size_t& id )
        : _id( id )
{}

cVelo::~cVelo()
{}

void cVelo::setTypeVelo( tBool3 newVal )
{
    vTypeVelo = newVal;
}

tBool3 cVelo::TypeVelo() const
{
    return ( vTypeVelo );
}

void cVelo::setResa( cModaliteReservation* newVal )
{
    vResa = newVal;
}


void cVelo::setContenance( tContenance newVal )
{
    vContenance = newVal;
}

tContenance cVelo::Contenance() const
{
    return ( vContenance );
}


void cVelo::setDoc( const string &newVal )
{
    vDoc = newVal;
}


void cVelo::setPrix( tPrix newVal )
{
    vPrix = newVal;
}

const size_t& cVelo::getId() const
{
    return _id;
}
