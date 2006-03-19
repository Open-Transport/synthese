#include "Point.h"



cPoint::cPoint()
{
    setX( INCONNU );
    setY( INCONNU );
}


cPoint::~cPoint()
{}




long int cPoint::XKMM() const
{
    if ( unknownLocation() )
        return INCONNU;
    else
        return _XKM * 1000 + _XM;
}

long int cPoint::YKMM() const
{
    if ( unknownLocation() )
        return INCONNU;
    else
        return _YKM * 1000 + _YM;
}

bool cPoint::unknownLocation() const
{
    return _XKM == INCONNU || _YKM == INCONNU;
}



/*! \brief Modificateur de la longitude
 \param newlong int Nouvelle longitude arrondie au mètre près (INCONNU pour coordonnées inconnues)
 
 Ce modificateur assure la séparation de la longitude en deux valeurs : la valeur en kilomètres, et le reste en mètres
*/
void cPoint::setX( const long int __XKMM )
{
    if ( __XKMM == INCONNU )
        _XKM = INCONNU;
    else
    {
        _XKM = ( short int ) ( __XKMM / 1000 );
        _XM = ( short int ) ( __XKMM % 1000 );
    }
}



/*! \brief Modificateur de la latitude
 \param newlong int Nouvelle latitude arrondie au mètre près (INCONNU pour coordonnées inconnues)
 
 Ce modificateur assure la séparation de la longitude en deux valeurs : la valeur en kilomètres, et le reste en mètres
*/
void cPoint::setY( const long int __YKMM )
{
    if ( __YKMM == INCONNU )
        _YKM = INCONNU;
    else
    {
        _YKM = ( short int ) ( __YKMM / 1000 );
        _YM = ( short int ) ( __YKMM % 1000 );
    }
}



/*! \brief Opérateur de comparaison des coordonnées de deux points
 \param AutrePoint le point à tester
 \return true si les coordonnées sont identiques au mètre près, false si l'un des deux au moins est inconnu
 \author Hugues Romain
 \date 2005
*/
int cPoint::operator== ( const cPoint& __Point ) const
{
    // Un des points est inconnu : pas d'égalité
    if ( unknownLocation() || __Point.unknownLocation() )
        return false;

    // Test d'égalité
    return __Point._XKM == _XKM && __Point._YKM == _YKM;
}
