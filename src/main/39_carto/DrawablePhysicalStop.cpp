#include "DrawablePhysicalStop.h"

#include "15_env/PhysicalStop.h"
#include "01_util/RGBColor.h"

#include "Map.h"
#include "PostscriptCanvas.h"

using synthese::env::PhysicalStop;
using synthese::env::Point;


namespace synthese
{
namespace carto
{


DrawablePhysicalStop::DrawablePhysicalStop (const synthese::env::PhysicalStop* physicalStop)
: _physicalStopId (physicalStop->getId ())
, _name (physicalStop->getName ())
, _point (*physicalStop)
{

}


DrawablePhysicalStop::~DrawablePhysicalStop ()
{

}



int 
DrawablePhysicalStop::getPhysicalStopId () const
{
    return _physicalStopId;
}


const std::string& 
DrawablePhysicalStop::getName () const
{
	return _name;
}


const synthese::env::Point& 
DrawablePhysicalStop::getPoint () const
{
	return _point;
}



void 
DrawablePhysicalStop::prepare (Map& map)
{
    _point = map.toOutputFrame (_point);
}





}
}

