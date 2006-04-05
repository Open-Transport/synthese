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


void 
DrawablePhysicalStop::preDraw (Map& map, PostscriptCanvas& canvas) const
{

}



void 
DrawablePhysicalStop::draw (Map& map, PostscriptCanvas& canvas) const
{
	Point cp = map.toOutputFrame (_point);
	canvas.moveto (cp.getX (), cp.getY ());
    canvas.sticker (_name, synthese::util::RGBColor ("yellow"), 10, 10);
}


void 
DrawablePhysicalStop::postDraw (Map& map, PostscriptCanvas& canvas) const
{

}


}
}