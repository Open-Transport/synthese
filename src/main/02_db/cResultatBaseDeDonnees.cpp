
#include "cResultatBaseDeDonnees.h"


bool cResultatBaseDeDonnees::AddEnregistrement()
{
    tResultatBaseDeDonnees_Enregistrement __Enregistrement;
    _EnregistrementCourant = _Donnees.SetElement( __Enregistrement );
    return _EnregistrementCourant != INCONNU;
}

bool cResultatBaseDeDonnees::SetValeur( int __Position, cResultatBaseDeDonnees_Cellule* __Cellule )
{
    return _Donnees.getElement( _EnregistrementCourant ).SetElement( __Cellule, __Position ) != INCONNU;
}

void cResultatBaseDeDonnees::Vide()
{
    _Donnees.Vide();
    _EnregistrementCourant = INCONNU;
    _PositionChamps.clear();
}

bool cResultatBaseDeDonnees::SetPositionChamp( char* __NomChamp, int __Position )
{
    _PositionChamps[ __NomChamp ] = __Position;
    return true;
}

int cResultatBaseDeDonnees::getNombre( const std::string& __Champ, int __Index )
{
    // Index
    if ( __Index == INCONNU )
        __Index = _EnregistrementCourant;
    if ( !_Donnees.IndexValide( __Index ) )
        return INCONNU;

    // R�sultat
    return _Donnees[ __Index ][ _PositionChamps[ __Champ.Texte() ] ] ->getNombre();
}



synthese::time::DateTime cResultatBaseDeDonnees::getMoment( const std::string& __Champ, int __Index )
{
    // Index
    if ( __Index == INCONNU )
        __Index = _EnregistrementCourant;
    if ( !_Donnees.IndexValide( __Index ) )
        return synthese::time::DateTime();

    // R�sultat
    return _Donnees[ __Index ][ _PositionChamps[ __Champ.Texte() ] ] ->getMoment();
}



std::string cResultatBaseDeDonnees::getTexte( const std::string& __Champ, int __Index )
{
    // Index
    if ( __Index == INCONNU )
        __Index = _EnregistrementCourant;
    if ( !_Donnees.IndexValide( __Index ) )
        return std::string();

    // R�sultat
    return _Donnees[ __Index ][ _PositionChamps[ __Champ.Texte() ] ] ->getTexte();
}



tBool3 cResultatBaseDeDonnees::getBool( const std::string& __Champ, int __Index )
{
    // Index
    if ( __Index == INCONNU )
        __Index = _EnregistrementCourant;
    if ( !_Donnees.IndexValide( __Index ) )
        return Indifferent;

    // R�sultat
    return _Donnees[ __Index ][ _PositionChamps[ __Champ.Texte() ] ] ->getBool();
}


bool cResultatBaseDeDonnees::GoPremier()
{
    if ( _Donnees.size () )
    {
        _EnregistrementCourant = 0;
        return true;
    } // if (_Donnees.size ())
    else
    {
        _EnregistrementCourant = INCONNU;
        return false;
    }
}

bool cResultatBaseDeDonnees::GoSuivant()
{
    if ( _EnregistrementCourant + 1 < _Donnees.size () )
    {
        _EnregistrementCourant++;
        return true;
    }
    else
    {
        _EnregistrementCourant = INCONNU;
        return false;
    }
}

const tPositionChamps& cResultatBaseDeDonnees::getPositionsChamps() const
{
    return _PositionChamps;
}
