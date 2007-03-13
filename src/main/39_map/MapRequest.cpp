
/** MapRequest class implementation.
	@file MapRequest.cpp

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
#include "MapRequest.h"

#include "01_util/Conversion.h"
#include "01_util/XmlToolkit.h"
#include "15_env/XmlBuilder.h"

#include "30_server/ServerModule.h" // to be removed!
#include "30_server/RequestException.h"

#include "39_map/XmlBuilder.h"
#include "39_map/Map.h"
#include "39_map/RenderingConfig.h"
#include "39_map/Renderer.h"
#include "39_map/PostscriptRenderer.h"
#include "39_map/HtmlMapRenderer.h"
#include "39_map/MapInfoRenderer.h"


#include <boost/filesystem/operations.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/iostreams/filtering_stream.hpp>

using synthese::map::Map;
using synthese::map::RenderingConfig;

using synthese::util::Conversion;
using synthese::util::Log;
using namespace boost::posix_time;

using namespace std;
using namespace synthese::util::XmlToolkit;


namespace synthese
{
	using namespace util;
	using namespace server;

	namespace map
	{


		const std::string MapRequest::OUTPUT_PARAMETER ("output");
		const std::string MapRequest::DATA_PARAMETER ("data");
		const std::string MapRequest::MAP_PARAMETER ("map");

		/// @todo Parameter names declarations
		// eg const std::string AdminRequest::PARAMETER_PAGE = "rub";
		
		/// @todo build of the attributes
		MapRequest::MapRequest()
		{}

		Request::ParametersMap MapRequest::getParametersMap() const
		{
			Request::ParametersMap map;
			/// @todo Map filling
			// eg : map.insert(make_pair(PARAMETER_PAGE, _page->getFactoryKey()));
			return map;
		}



		void MapRequest::setFromParametersMap(const Request::ParametersMap& map)
		{
			Request::ParametersMap::const_iterator it;

			// Output
			it = map.find(OUTPUT_PARAMETER);
			if (it == map.end())
			    throw RequestException("Output not specified");

			_output = it->second;
			if (!Factory<Renderer>::contains(_output))
			{
			    throw RequestException ("Invalid map output type " + _output);
			}
			
			// XML data
			it = map.find (DATA_PARAMETER);
			if (it == map.end())
			    throw RequestException("No data specified");

			/// @todo Throw an exception if xml parsing fails
			XMLNode dataNode = XMLNode::parseString (it->second.c_str (), "data");

			// Fill in local registries

			XMLNode citiesNode = GetChildNode (dataNode, "cities", 0);
			int nbCities = GetChildNodeCount (citiesNode, "city");
			for (int i=0; i<nbCities; ++i) 
			{
			    XMLNode cityNode = GetChildNode (citiesNode, "city", i);
			    _cities.add (synthese::env::XmlBuilder::CreateCity (cityNode));
			}

			XMLNode axesNode = GetChildNode (dataNode, "axes", 0);
			int nbAxes = GetChildNodeCount (axesNode, "axis");
			for (int i=0; i<nbAxes; ++i) 
			{
			    XMLNode axisNode = GetChildNode (axesNode, "axis", i);
			    _axes.add (synthese::env::XmlBuilder::CreateAxis (axisNode));
			}

			XMLNode connectionPlacesNode = GetChildNode (dataNode, "connectionPlaces", 0);
			int nbConnectionPlaces = GetChildNodeCount (connectionPlacesNode, "connectionPlace");
			for (int i=0; i<nbConnectionPlaces; ++i) 
			{
			    XMLNode connectionPlaceNode = GetChildNode (connectionPlacesNode, "connectionPlace", i);
			    _connectionPlaces.add (synthese::env::XmlBuilder::CreateConnectionPlace (connectionPlaceNode, _cities));
			}
    
			XMLNode physicalStopsNode = GetChildNode (dataNode, "physicalStops", 0);
			int nbPhysicalStops = GetChildNodeCount (physicalStopsNode, "physicalStop");
			for (int i=0; i<nbPhysicalStops; ++i) 
			{
			    XMLNode physicalStopNode = GetChildNode (physicalStopsNode, "physicalStop", i);
			    _physicalStops.add (synthese::env::XmlBuilder::CreatePhysicalStop (physicalStopNode, _connectionPlaces));
			}
			
			XMLNode commercialLinesNode = GetChildNode (dataNode, "commercialLines", 0);
			int nbCommercialLines = GetChildNodeCount (commercialLinesNode, "commercialLine");
			for (int i=0; i<nbCommercialLines; ++i) 
			{
			    XMLNode commercialLineNode = GetChildNode (commercialLinesNode, "commercialLine", i);
			    _commercialLines.add (synthese::env::XmlBuilder::CreateCommercialLine (commercialLineNode));
			}
			
			XMLNode linesNode = GetChildNode (dataNode, "lines", 0);
			int nbLines = GetChildNodeCount (linesNode, "line");
			for (int i=0; i<nbLines; ++i) 
			{
			    XMLNode lineNode = GetChildNode (linesNode, "line", i);
			    _lines.add (synthese::env::XmlBuilder::CreateLine (lineNode, _axes, _commercialLines));
			}
			
			XMLNode lineStopsNode = GetChildNode (dataNode, "lineStops", 0);
			int nbLineStops = GetChildNodeCount (lineStopsNode, "lineStop");
			for (int i=0; i<nbLineStops; ++i) 
			{
			    XMLNode lineStopNode = GetChildNode (lineStopsNode, "lineStop", i);
			    _lineStops.add (synthese::env::XmlBuilder::CreateLineStop (lineStopNode, _lines, _physicalStops));
			}
			
			// Map XML
			it = map.find (MAP_PARAMETER);
			if (it == map.end())
			    throw RequestException("Map to draw not specified");

			/// @todo Throw an exception if xml parsing fails
			XMLNode mapNode = XMLNode::parseString (it->second.c_str (), "map");
			
			_map = synthese::map::XmlBuilder::CreateMap (mapNode, _lines);

		}




		void MapRequest::run( std::ostream& stream ) const
		{
			// Prepare the map (once for all renderings!)
			_map->prepare ();

			// Create a temporary file name based on system time
			const boost::filesystem::path& tempDir = ServerModule::getConfig ().getHttpTempDir ();

			RenderingConfig conf;

			// Choose the renderer
			Renderer* renderer = Factory<Renderer>::create(_output);

			// Generate an id for the map file based on current time
			ptime timems (boost::date_time::microsec_clock<ptime>::local_time ());
			std::string filePrefix = "map_" + to_iso_string (timems);

			std::string resultFilename = renderer->render (tempDir, filePrefix, _lines, *_map, conf);

			// Broadcast of the result
			std::string resultURL = ServerModule::getConfig ().getHttpTempUrl () + "/" + resultFilename;
			
			// Send the URL to the the generated local JPEG file.
			stream << resultURL << std::endl;
			
			Log::GetInstance ().debug ("Sent result url " + resultURL);

		}



		MapRequest::~MapRequest()
		{
		    delete _map;
		}
	}
}
