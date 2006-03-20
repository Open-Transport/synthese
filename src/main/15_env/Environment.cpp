#include "Environment.h"


namespace synthese
{
namespace env
{


Environment::Environment ()
{
}



Environment::~Environment ()
{

}


City::Registry&
Environment::getCities ()
{
    return _cities;
}



Document::Registry&
Environment::getDocuments ()
{
    return _documents;
}



Line::Registry&
Environment::getLines ()
{
    return _lines;
}







}
}

