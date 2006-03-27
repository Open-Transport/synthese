#include "Environment.h"


namespace synthese
{
namespace env
{

Environment::Registry Environment::_environments;


Environment::Environment (const int& id)
    : Registrable<int,Environment> (id)
{
}



Environment::~Environment ()
{

}



Environment::Registry& 
Environment::GetEnvironments ()
{
    return _environments;
}



Axis::Registry& 
Environment::getAxes ()
{
    return _axes;
}



const Axis::Registry& 
Environment::getAxes () const
{
    return _axes;
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


PhysicalStop::Registry&
Environment::getPhysicalStops ()
{
    return _physicalStops;
}



const PhysicalStop::Registry&
Environment::getPhysicalStops () const
{
    return _physicalStops;
}


LineStop::Registry& 
Environment::getLineStops () 
{
    return _lineStops;
}


const LineStop::Registry& 
Environment::getLineStops () const
{
    return _lineStops;
}







}
}

