#include "Environment.h"


namespace synthese
{
namespace env
{


Environment::Environment (const int& id)
    : Registrable<int,Environment> (id)
{
}



Environment::~Environment ()
{
	// Delete all objects registered in this environment.
    _axes.clear ();
    _cities.clear ();
    _documents.clear ();
    _lines.clear ();
    _lineStops.clear ();
    _logicalStops.clear ();
    _physicalStops.clear ();
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






}
}

