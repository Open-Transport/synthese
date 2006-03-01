/*--------------------------------------*
|                                      |
|  APDOS - SYNTHESE v0.6               |
|  © Hugues Romain 2000/2002           |
|  cPhoto.cpp                          |
|  Implementation Classes Photo        |
|                                      |
*--------------------------------------*/

#include "cPhoto.h"

/*! \brief Constructeur
 \author Hugues Romain
 \date 2002
*/
cPhoto::cPhoto( const size_t& id )
        : cDocument( id )
{}



/*! \brief Constructeur de lien vers autre photo
 
Effectue la copie des chaines de caracètres fournies
 
 \author Hugues Romain
 \date 2002
*/
bool cMapPhoto::SetDonnees( const cTexte& newCoords, int newLienPhoto, const cTexteHTML& newAlt )
{
    vCoords = newCoords;
    vLienPhoto = newLienPhoto;
    vAlt = newAlt;
    vURL.Vide();
    return true;
}



/*! \brief Constructeur de lien vers URL
 
Effectue la copie des chaînes de caractères fournies
 
 \author Hugues Romain
 \date 2002
*/
bool cMapPhoto::SetDonnees( const cTexte& newCoords, const cTexte& newURL, const cTexteHTML& newAlt )
{
    vCoords = newCoords;
    vLienPhoto = 0;
    vAlt = newAlt;
    vURL = newURL;
    return true;
}


/*! \brief Ajout d'une zone cliquable liant une autre photo
 \warning Pas de controle d'erreur
 \author Hugues Romain
 \date 2002
*/
bool cPhoto::addMapPhoto( const cTexte& newCoords, int newLienPhoto, const cTexteHTML& newAlt )
{
    // Recherche du premier index disponible
    int NumeroMap = _Map.Active();
    if ( NumeroMap == INCONNU )
        return false;

    return _Map.getElement( NumeroMap ).SetDonnees( newCoords, newLienPhoto, newAlt );
}



/*! \brief Ajout d'une zone cliquable liant une URL
 \warning Pas de controle d'erreur
 \author Hugues Romain
 \date 2002
*/
bool cPhoto::addMapPhoto( const cTexte& newCoords, const cTexte& newURL, const cTexteHTML& newAlt )
{
    // Recherche du premier index disponible
    int NumeroMap = _Map.Active();
    if ( NumeroMap == INCONNU )
        return false;

    return _Map.getElement( NumeroMap ).SetDonnees( newCoords, newURL, newAlt );
}

int cPhoto::NombreMaps() const
{
    return _Map.Taille();
}





// cPhoto 1.0 - Map
// ____________________________________________________________________________
//
//
// ____________________________________________________________________________
const cMapPhoto& cPhoto::Map( int i ) const
{
    return _Map[ i ];
}
// © Hugues Romain 2003
// ____________________________________________________________________________





const cTexteHTML& cMapPhoto::Alt() const
{
    return vAlt;
}

const cTexte& cMapPhoto::Coords() const
{
    return ( vCoords );
}

const int cMapPhoto::LienPhoto() const
{
    return ( vLienPhoto );
}

const cTexte& cMapPhoto::URL() const
{
    return ( vURL );
}
