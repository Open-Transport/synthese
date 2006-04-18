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
: _name (physicalStop->getName ())
, _point (*physicalStop)
{

}


DrawablePhysicalStop::~DrawablePhysicalStop ()
{

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




}
}
