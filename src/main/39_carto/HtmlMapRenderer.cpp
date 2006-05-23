#include "HtmlMapRenderer.h"

#include "Geometry.h"
#include "DrawableLine.h"
#include "DrawablePhysicalStop.h"

#include "Map.h"

#include "01_util/Log.h"
#include "01_util/Conversion.h"
#include "15_env/Environment.h"
#include "15_env/Line.h"
#include "15_env/LineStop.h"


#include <cmath>
#include <algorithm>

#include <boost/algorithm/string/replace.hpp>

using synthese::util::RGBColor;
using synthese::util::Log;
using synthese::util::Conversion;

using synthese::env::Point;
using synthese::env::Environment;
using synthese::env::Line;
using synthese::env::LineStop;




namespace synthese
{

namespace carto
{




HtmlMapRenderer::HtmlMapRenderer(const RenderingConfig& config, 
				 const Environment& environment,
				 const std::string& urlPattern,
				 const std::string& mapImgFilename,
				 std::ostream& output)
    : Renderer (config)
	, _environment (environment)
    , _urlPattern (urlPattern)
    , _mapImgFilename (mapImgFilename)
    , _output (output)
{

}




HtmlMapRenderer::~HtmlMapRenderer()
{
}




void 
HtmlMapRenderer::render (Map& map)
{
    _output << "<html><body>";
    _output << "<map name='mapid'>" << std::endl;

    // Dump first physical stops cos the order of area elements in <map>
    // specifies a kind of 'layer' priority for mouse capture.
    renderPhysicalStops (map);
    renderLines (map);
    _output << "</map>" << std::endl;
    _output << "<img src='" << _mapImgFilename << "' usemap='#mapid'/>" << std::endl;
    _output << "</body></html>" << std::endl;


}






void 
HtmlMapRenderer::renderLines (Map& map)
{
    const std::set<DrawableLine*>& selectedLines = map.getSelectedLines ();
    
	for (std::set<DrawableLine*>::const_iterator it = selectedLines.begin ();
		it != selectedLines.end () ; ++it) 
	{
		const DrawableLine* dbl = *it;
		dbl->prepare (map, _config.getSpacing ());
	}    

	// if (selectedLines.size () > 1) 
	{
		// Differentiation on lines

	    
		for (std::set<DrawableLine*>::const_iterator it = selectedLines.begin ();
			it != selectedLines.end () ; ++it) 
		{
			const DrawableLine* dbl = *it;
			const std::vector<Point>& shiftedPoints = dbl->getShiftedPoints ();

			// Shift them again on right and left of half-width to get the enveloppe.
			const std::vector<synthese::env::Point> points1 =
			dbl->calculateAbsoluteShiftedPoints (shiftedPoints, (_config.getBorderWidth () / 2));
			
			std::string href (_urlPattern);
			boost::replace_all (href, "$id", dbl->getLineId ());

			_output << "<area href='" << href << "' shape='poly' coords='";
			for (int i=0; i<points1.size (); ++i)
			{
				_output << (int) points1[i].getX () << "," << (int) (map.getHeight () - points1[i].getY ()) << ",";
			}

			std::vector<synthese::env::Point> points2 = 
				dbl->calculateAbsoluteShiftedPoints (shiftedPoints, - (_config.getBorderWidth () / 2));

			std::reverse (points2.begin (), points2.end ());

			for (int i=0; i<points2.size (); ++i)
			{
				_output << (int) points2[i].getX () << "," << (int) (map.getHeight () - points2[i].getY ());
				if (i != points2.size ()-1) _output << ",";
			}
			_output << "'/>" << std::endl;

		}    
	} 
	/* else if (selectedLines.size () == 1) 
	{
		// Differentiation on line stops
		const DrawableLine* dbl = *(selectedLines.begin ());
		const Line* line = _environment.getLines().get (dbl->getLineId ());
		const std::vector<LineStop*>& lineStops =  line->getLineStops();

		// TODO

	} */



}





void 
HtmlMapRenderer::renderPhysicalStops (Map& map)
{
    const std::set<DrawablePhysicalStop*>& selectedPhysicalStops = 
	map.getSelectedPhysicalStops ();

    for (std::set<DrawablePhysicalStop*>::const_iterator it = selectedPhysicalStops.begin ();
         it != selectedPhysicalStops.end () ; ++it) 
    {
	const DrawablePhysicalStop* dps = *it;

	std::string href (_urlPattern);
	boost::replace_all (href, "$id", Conversion::ToString (dps->getPhysicalStopId ()));

	_output << "<area href='" << href << "' shape='circle' coords='";
	_output << dps->getPoint ().getX ()  << "," 
		<< (int) (map.getHeight () - dps->getPoint ().getY()) << "," 
		<< _config.getLineWidth () << "'/>" << std::endl;
    }
}







}
}
