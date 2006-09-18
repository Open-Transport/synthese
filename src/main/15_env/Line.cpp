#include "Line.h"

#include "Service.h"
#include "LineStop.h"
#include "PhysicalStop.h"


namespace synthese
{
namespace env
{





Line::Line (const uid& id,
	    const std::string& name, 
	    const Axis* axis)
    : synthese::util::Registrable<uid,Line> (id)
    , Path ()
    , _name (name)
    , _axis (axis)
    , _network (0)
    , _rollingStockId (-1)
    , _isWalkingLine (false)
    , _useInDepartureBoards (true)
    , _useInTimetables (true)
    , _useInRoutePlanning (true)
    , _color (0, 0, 0)
{

}





Line::~Line ()
{
}



const std::string& 
Line::getName () const
{
    return _name;
}



void 
Line::setName (const std::string& name)
{
    _name = name;
}



bool 
Line::getUseInDepartureBoards () const
{
    return _useInDepartureBoards;
}



void 
Line::setUseInDepartureBoards (bool useInDepartureBoards)
{
    _useInDepartureBoards = useInDepartureBoards;
}




bool 
Line::getUseInTimetables () const
{
    return _useInTimetables;
}



void 
Line::setUseInTimetables (bool useInTimetables)
{
    _useInTimetables = useInTimetables;
}




bool 
Line::getUseInRoutePlanning () const
{
    return _useInRoutePlanning;
}




void 
Line::setUseInRoutePlanning (bool useInRoutePlanning)
{
    _useInRoutePlanning = useInRoutePlanning;
}





const Axis* 
Line::getAxis () const
{
    return _axis;
}






const std::string& 
Line::getDirection () const
{
    return _direction;
}



void 
Line::setDirection (const std::string& direction)
{
    _direction = direction;
}




const std::string& 
Line::getImage () const
{
    return _image;
}



void 
Line::setImage (const std::string& image)
{
    _image = image;
}




const std::string& 
Line::getShortName () const
{
    return _shortName;
}



void 
Line::setShortName (const std::string& shortName)
{
    _shortName = shortName;
}



	
const std::string& 
Line::getLongName () const
{
    return _longName;
}



void 
Line::setLongName (const std::string& longName)
{
    _longName = longName;
}




const std::string& 
Line::getTimetableName () const
{
    return _timetableName;
}



void 
Line::setTimetableName (const std::string& timetableName)
{
    _timetableName = timetableName;
}





const TransportNetwork* 
Line::getNetwork () const
{
    return _network;
}



void 
Line::setNetwork (TransportNetwork* network)
{
    _network = network;
}



    
const std::string& 
Line::getStyle () const
{
    return _style;
}



void 
Line::setStyle (const std::string& style)
{
    _style = style;
}



	


const uid&
Line::getRollingStockId () const
{
    return _rollingStockId;
}



void 
Line::setRollingStockId (const uid& rollingStockId)
{
    _rollingStockId = rollingStockId;
}

















void
Line::setWalkingLine (bool isWalkingLine)
{
    _isWalkingLine = isWalkingLine;
}



bool 
Line::getWalkingLine () const
{
    return _isWalkingLine;
}








const synthese::util::RGBColor& 
Line::getColor () const
{
    return _color;
}



void 
Line::setColor (const synthese::util::RGBColor& color)
{
    _color = color;
}



bool 
Line::isRoad () const
{
    return false;
}



bool 
Line::isLine () const
{
    return true;
}




const uid& 
Line::getId () const
{
    return synthese::util::Registrable<uid,Line>::getId ();
}




}
}
