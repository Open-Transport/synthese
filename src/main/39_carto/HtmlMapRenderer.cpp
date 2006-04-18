#include "HtmlMapRenderer.h"

#include "Geometry.h"
#include "DrawableLine.h"
#include "DrawablePhysicalStop.h"

#include "Map.h"

#include "01_util/Log.h"
#include "01_util/Conversion.h"

#include <cmath>
#include <algorithm>

using synthese::util::RGBColor;
using synthese::util::Log;
using synthese::util::Conversion;
using synthese::env::Point;




namespace synthese
{

namespace carto
{




HtmlMapRenderer::HtmlMapRenderer(const RenderingConfig& config, std::ostream& output)
    : Renderer (config)
    , _output (output)
{

}




HtmlMapRenderer::~HtmlMapRenderer()
{
}




void 
HtmlMapRenderer::render (Map& map)
{
    map.prepare ();
	_output << "<html><body>";
    _output << "<map name='testmap'>" << std::endl;
    renderLines (map);
    _output << "</map>" << std::endl;
	_output << "<img src='testmap.png' usemap='#testmap'/></body></html>";


}






void 
HtmlMapRenderer::renderLines (Map& map)
{
    const std::set<DrawableLine*>& selectedLines = map.getSelectedLines ();
    
    for (std::set<DrawableLine*>::const_iterator it = selectedLines.begin ();
         it != selectedLines.end () ; ++it) {
		const DrawableLine* dbl = *it;
		dbl->prepare (map, _config.getSpacing ());
    }    
    
    // Draw
    for (std::set<DrawableLine*>::const_iterator it = selectedLines.begin ();
         it != selectedLines.end () ; ++it) 
	{
		const DrawableLine* dbl = *it;
		const std::vector<Point>& shiftedPoints = dbl->getShiftedPoints ();

		// Shift them again on right and left of half-width to get the enveloppe.
		const std::vector<synthese::env::Point> points1 =
		  dbl->calculateAbsoluteShiftedPoints (shiftedPoints, 4);

		_output << "<area href='#' shape='poly' coords='";
		for (int i=0; i<points1.size (); ++i)
		{
			_output << points1[i].getX () << "," << (map.getHeight () - points1[i].getY ()) << ",";
		}

		std::vector<synthese::env::Point> points2 = 
			dbl->calculateAbsoluteShiftedPoints (shiftedPoints, -4);

		std::reverse (points2.begin (), points2.end ());

		for (int i=0; i<points2.size (); ++i)
		{
			_output << points2[i].getX () << "," << (map.getHeight () - points2[i].getY ());
			if (i != points2.size ()-1) _output << ",";
		}
		_output << "'/>" << std::endl;

    }    



}










void 
HtmlMapRenderer::doDrawCurvedLine (const DrawableLine* dbl)
{
/*
    const std::vector<Point>& shiftedPoints = dbl->getShiftedPoints ();
    _canvas.newpath();
    _canvas.moveto(shiftedPoints[0].getX(), shiftedPoints[0].getY());
    
    for (unsigned int i=1; i<shiftedPoints.size (); ++i) 
    {
	double x = shiftedPoints[i].getX();
	double y = shiftedPoints[i].getY();
	double radiusShift = 0.0;
	
	if (_config.getEnableCurves () && (i < shiftedPoints.size () - 1)) 
	{
	    // Not supported in html maps.
	    // Really useful to keep it in Postscriptrendering ?? bof
	} 
	else 
	{ 
	    _canvas.lineto(x, y);	
	}
	
    }
    
    _canvas.stroke();
*/  
}













void 
HtmlMapRenderer::renderPhysicalStops (Map& map)
{
    // !! problem how to guess real size of sticker in pixel !!!!!
    // First step : physical stops not included in html map

    /*
    _canvas.setfont("Helvetica", 8);
    _canvas.setrgbcolor(0, 0, 0);

    // Draw
    for (std::set<DrawablePhysicalStop*>::const_iterator it = 
        map.getSelectedPhysicalStops ().begin ();
         it != map.getSelectedPhysicalStops ().end () ; ++it) 
    {
        const DrawablePhysicalStop* dps = *it;
	
	Point cp = map.toOutputFrame (dps->getPoint ());
	_canvas.moveto (cp.getX (), cp.getY ());
	_canvas.sticker (dps->getName (), synthese::util::RGBColor ("yellow"), 10, 10);
    }
    */

}







}
}
