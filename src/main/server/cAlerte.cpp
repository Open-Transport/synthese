/** Alert message class implementation.
@file cAlerte.cpp
*/

#include "cAlerte.h"

using namespace std;

const cAlerte::Level cAlerte::LEVEL_WARNING = 99;
const cAlerte::Level cAlerte::LEVEL_INFO = 50;

/*! \brief Constructeur
 \author Hugues Romain
 \date 2005
 
Les éléments alerte sont initialisés avec les valeurs suivantes :
 - période d'applicabilité permanente
 - niveau d'alerte 1 (ATTENTION)
*/
cAlerte::cAlerte()
{
    _Debut.updateDateTime( synthese::time::TIME_MIN );
    _Fin.updateDateTime( synthese::time::TIME_MAX );
    _Niveau = LEVEL_WARNING;
}


/*! \brief Test d'applicabilité du message d'alerte
 \param __Debut Début de la plage temporelle d'application
 \param __Fin Fin de la plage temporelle d'application
 \return true si le message et non vide et est valide pour tout ou partie de la plage temporelle d'application fournie, false sinon
 \author Hugues Romain
 \date 2005
*/
bool cAlerte::showMessage( const synthese::time::DateTime& __Debut, const synthese::time::DateTime& __Fin ) const
{
    //pas de message
    if ( !_Message.size() )
        return false;

    //test de la validite du message
    if ( ( __Fin < _Debut ) || ( __Debut > _Fin ) )
        return false;

    return true;
}


const string& cAlerte::getMessage() const
{
    return ( _Message );
}

void cAlerte::setMessage( const string& __Message )
{
    _Message = __Message;
}

void cAlerte::setMomentDebut( const synthese::time::DateTime& __Debut )
{
    _Debut = __Debut;
}

void cAlerte::setMomentFin( const synthese::time::DateTime& __Fin )
{
    _Fin = __Fin;
}



/*! \brief Accesseur Niveau de l'alerte
 \return Le niveau de l'alerte
*/
const cAlerte::Level& cAlerte::Niveau() const
{
    return _Niveau;
}

void cAlerte::setNiveau( const Level& __Niveau )
{
    _Niveau = __Niveau;
}

