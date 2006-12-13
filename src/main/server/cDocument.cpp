/** Implémentation classe document.
@file cDocument.cpp
*/

#include "cDocument.h"

using namespace std;

cDocument::cDocument( const size_t& id )
        : _id( id )
{ }

cDocument::~cDocument()
{}


/*! \brief Modificateur de la description générale de la photo
 \param newDescriptionGenerale Nouvelle description générale
 \author Hugues Romain
 \date 2002
*/
bool cDocument::setDescriptionGenerale( const string& __DescriptionGenerale )
{
    _DescriptionGenerale = __DescriptionGenerale;
    return true;
}



/*! \brief Modificateur de la description locale de la photo
 \param newDescriptionLocale Nouvelle description locale
 \author Hugues Romain
 \date 2002
*/
bool cDocument::setDescriptionLocale( const string& __DescriptionLocale )
{
    _DescriptionLocale = __DescriptionLocale;
    return true;
}



/*! \brief Modificateur du nom du fichier contenant la photo
 \param newNomFichier Nouveau nom du fichier
 \author Hugues Romain
 \date 2002
*/
bool cDocument::SetURL( const string& __URL )
{
    _URL = __URL;
    return true;
}



/*! \brief DescriptionLocale
 \author Hugues Romain
 \date 2003-2005
*/
const string& cDocument::DescriptionLocale() const
{
    return _DescriptionLocale;
}



const string& cDocument::URL() const
{
    return _URL;
}



// cPhoto 1.0 - Code
// ____________________________________________________________________________
//
//
// ____________________________________________________________________________
const size_t& cDocument::getId() const
{
    return _id;
}
// © Hugues Romain 2002
// ____________________________________________________________________________

