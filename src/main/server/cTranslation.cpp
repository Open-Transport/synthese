
#include "cTranslation.h"


// fr
// en
// ...
const std::string cTranslation::DEFAULT_LANGUAGE_CODE = "fr";



cTranslation::cTranslation ()
{}


cTranslation::cTranslation ( const cTranslation& ref )
        : _translations ( ref._translations )
{
}



cTranslation::~cTranslation ()
{}

void
cTranslation::addTranslation ( const std::string& languageCode, const std::string& translation )
{
    _translations.insert ( std::pair<std::string, std::string> ( languageCode, translation ) );
}



const std::string&
cTranslation::getTranslation ( const std::string& code ) const
{
    std::map<std::string, std::string>::const_iterator mes = _translations.find ( code );
    if ( mes == _translations.end () )
        throw std::string ( "No such language code " + code );
    return mes->second;
}


const std::string&
cTranslation::getDefaultTranslation () const
{
    return getTranslation( DEFAULT_LANGUAGE_CODE );
}


cTranslation&
cTranslation::operator=( const cTranslation& rhs )
{
    _translations = rhs._translations;
    return ( *this );
}

