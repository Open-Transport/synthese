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

const City::Registry&
Environment::getCities () const
{
    return _cities;
}



Document::Registry&
Environment::getDocuments ()
{
    return _documents;
}


const Document::Registry&
Environment::getDocuments () const
{
    return _documents;
}



Line::Registry&
Environment::getLines ()
{
    return _lines;
}


const Line::Registry&
Environment::getLines () const
{
    return _lines;
}



LogicalStop::Registry&
Environment::getLogicalStops ()
{
    return _logicalStops;
}



const LogicalStop::Registry&
Environment::getLogicalStops () const
{
    return _logicalStops;
}





}
}

