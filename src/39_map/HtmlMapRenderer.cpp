
/** HtmlMapRenderer class implementation.
	@file HtmlMapRenderer.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "HtmlMapRenderer.h"
#include "JpegRenderer.h"

#include "Geometry.h"
#include "DrawableLine.h"
#include "DrawablePhysicalStop.h"

#include "Map.h"

#include "Log.h"
#include "Conversion.h"
#include "Line.h"
#include "LineStop.h"
#include "Point2D.h"

#include "MapModule.h"

#include <cmath>
#include <algorithm>
#include <fstream>
#include <boost/algorithm/string/replace.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace geometry;
	using namespace util;
	using namespace map;
	using namespace pt;
	using namespace graph;
		
	namespace util
	{
		template<>
		const string FactorableTemplate<Renderer, HtmlMapRenderer>::FACTORY_KEY("html");
	}

	namespace map
	{
		
		
		HtmlMapRenderer::HtmlMapRenderer()
		: FactorableTemplate<Renderer,HtmlMapRenderer>()
		{
		
		}
		
		
		
		
		HtmlMapRenderer::~HtmlMapRenderer()
		{
		}
		
		
		
		
		std::string
		HtmlMapRenderer::render(const boost::filesystem::path& tempDir, 
					const std::string& filenamePrefix,
					const Registry<Line>& lines,
					synthese::map::Map& map,
					const synthese::map::RenderingConfig& config)
		{
		
			JpegRenderer jpegRenderer;
			jpegRenderer.render(tempDir, filenamePrefix, lines, map, config);
		
			std::string jpegFilename = filenamePrefix + ".jpg";
		
			std::string resultFilename (filenamePrefix + ".html");
			const boost::filesystem::path htmlFile (tempDir / resultFilename);
		
			std::ofstream _output (htmlFile.string ().c_str ());
			
			_config = config;
			_urlPattern = map.getUrlPattern ();
			
			/// @todo Remove server dependancy here
			_mapImgFilename = MapModule::GetParameter (MapModule::PARAM_HTTP_TEMP_URL) + "/" + jpegFilename;
			
			_output << "<html><body>";
			_output << "<map name='mapid'>" << std::endl;
		
			// Dump first physical stops cos the order of area elements in <map>
			// specifies a kind of 'layer' priority for mouse capture.
			renderPhysicalStops (_output, map);
			renderLines (_output, lines, map);
			_output << "</map>" << std::endl;
			_output << "<img src='" << _mapImgFilename << "' usemap='#mapid'/>" << std::endl;
			_output << "</body></html>" << std::endl;
		
			_output.close();
			return resultFilename;
		}
		
		
		
		
		
		
				void HtmlMapRenderer::renderLines(
					std::ostream& _output,
					const Registry<Line>& lines,
					Map& map
				){
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
					const std::vector<Point2D>& shiftedPoints = dbl->getShiftedPoints ();
		
					// Shift them again on right and left of half-width to get the enveloppe.
					const std::vector<Point2D> points1 =
					dbl->calculateAbsoluteShiftedPoints (shiftedPoints, (_config.getBorderWidth () / 2));
					
					std::string href (_urlPattern);
					boost::replace_all (href, "$id", Conversion::ToString (dbl->getLineId ()));
		
					_output << "<area href='" << href << "' shape='poly' coords='";
					for (int i=0; i<points1.size (); ++i)
					{
						_output << (int) points1[i].getX () << "," << (int) (map.getHeight () - points1[i].getY ()) << ",";
					}
		
					std::vector<Point2D> points2 = 
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
				shared_ptr<const Line> line = lines.get (dbl->getLineId ());
				const vector<Edge*>& lineStops =  line->getEdges();
				const vector<Point2D>& shiftedPoints = dbl->getShiftedPoints ();
		
				// Shift them again on right and left of half-width to get the enveloppe.
				std::vector<Point2D> points1 =
					dbl->calculateAbsoluteShiftedPoints (shiftedPoints, (_config.getBorderWidth () / 2));
					
				std::vector<Point2D> points2 = 
					dbl->calculateAbsoluteShiftedPoints (shiftedPoints, - (_config.getBorderWidth () / 2));
		
				// TODO : to be reviewed when via points will be added
				for (int i=0; i<lineStops.size (); ++i) 
				{
					const LineStop* ls = dynamic_cast<const LineStop*> (lineStops.at(i));
					std::string href (_urlPattern);
		
					// HACK : there is no interpoint concept in synthese. so refres gives interpoint id
					// in metric offset...
					boost::replace_all (href, "$id", Conversion::ToString(ls->getMetricOffset()));
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
