#include "HtmlMapRenderer.h"
#include "JpegRenderer.h"

#include "Geometry.h"
#include "DrawableLine.h"
#include "DrawablePhysicalStop.h"

#include "Map.h"

#include "01_util/Log.h"
#include "01_util/Conversion.h"
#include "15_env/Environment.h"
#include "15_env/Line.h"
#include "15_env/LineStop.h"
#include "30_server/Server.h"

#include <cmath>
#include <algorithm>
#include <fstream>
#include <boost/algorithm/string/replace.hpp>

using synthese::util::RGBColor;
using synthese::util::Log;
using synthese::util::Conversion;

using synthese::env::Point;
using synthese::env::Environment;
using synthese::env::Line;
using synthese::env::LineStop;
using synthese::env::Edge;




namespace synthese
{

namespace carto
{


HtmlMapRenderer::HtmlMapRenderer()
{

}




HtmlMapRenderer::~HtmlMapRenderer()
{
}




std::string
HtmlMapRenderer::render(const boost::filesystem::path& tempDir, 
						const std::string& filenamePrefix,
						const synthese::env::Environment* environment,
						synthese::carto::Map& map,
						const synthese::carto::RenderingConfig& config)
{

	JpegRenderer jpegRenderer;
	jpegRenderer.render(tempDir, filenamePrefix, environment, map, config);

	std::string jpegFilename = filenamePrefix + ".jpg";

	std::string resultFilename (filenamePrefix + ".html");
	const boost::filesystem::path htmlFile (tempDir / resultFilename);

	std::ofstream _output (htmlFile.string ().c_str ());
	
	_config = config;
	_environment = environment;
	_urlPattern = map.getUrlPattern ();
	
	/// @todo Remove server dependancy here
	_mapImgFilename = synthese::server::Server::GetInstance ()->getConfig ().getHttpTempUrl () + "/" + jpegFilename;
	
    _output << "<html><body>";
    _output << "<map name='mapid'>" << std::endl;

    // Dump first physical stops cos the order of area elements in <map>
    // specifies a kind of 'layer' priority for mouse capture.
    renderPhysicalStops (_output, map);
    renderLines (_output, map);
    _output << "</map>" << std::endl;
    _output << "<img src='" << _mapImgFilename << "' usemap='#mapid'/>" << std::endl;
    _output << "</body></html>" << std::endl;

	_output.close();
	return resultFilename;
}






void 
HtmlMapRenderer::renderLines (std::ostream& _output, Map& map)
{
    const std::set<DrawableLine*>& selectedLines = map.getSelectedLines ();
    
	for (std::set<DrawableLine*>::const_iterator it = selectedLines.begin ();
		it != selectedLines.end () ; ++it) 
	{
		const DrawableLine* dbl = *it;
		dbl->prepare (map, _config.getSpacing ());
	}    

	if (selectedLines.size () > 1) 
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
			boost::replace_all (href, "$id", Conversion::ToString (dbl->getLineId ()));

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
	 else if (selectedLines.size () == 1) 
	{
		// Differentiation on line stops
		const DrawableLine* dbl = *(selectedLines.begin ());
		const Line* line = _environment->getLines().get (dbl->getLineId ());
		const std::vector<Edge*>& lineStops =  line->getEdges();
		const std::vector<Point>& shiftedPoints = dbl->getShiftedPoints ();

		// Shift them again on right and left of half-width to get the enveloppe.
		std::vector<synthese::env::Point> points1 =
			dbl->calculateAbsoluteShiftedPoints (shiftedPoints, (_config.getBorderWidth () / 2));
			
		std::vector<synthese::env::Point> points2 = 
			dbl->calculateAbsoluteShiftedPoints (shiftedPoints, - (_config.getBorderWidth () / 2));

		// TODO : to be reviewed when via points will be added
		for (int i=0; i<lineStops.size (); ++i) 
		{
		    const LineStop* ls = dynamic_cast<const LineStop*> (lineStops.at(i));
		    std::string href (_urlPattern);
		    boost::replace_all (href, "$id", Conversion::ToString(ls->getId ()));
		    _output << "<area href='" << href << "' shape='poly' coords='";
		    
		    _output << (int) points1[i].getX () << "," << (int) (map.getHeight () - points1[i].getY ()) << ",";
		    _output << (int) points1[i+1].getX () << "," << (int) (map.getHeight () - points1[i+1].getY ()) << ",";
		    _output << (int) points2[i+1].getX () << "," << (int) (map.getHeight () - points2[i+1].getY ()) << ",";
		    _output << (int) points2[i].getX () << "," << (int) (map.getHeight () - points2[i].getY ());
		    if (i < lineStops.size ()-1) _output << ",";
		    _output << "'/>" << std::endl;
		}
	} 



}





void 
HtmlMapRenderer::renderPhysicalStops (std::ostream& _output, Map& map)
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
