
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
#include "39_map/MapRequest.h"

#include "01_util/Conversion.h"
#include "01_util/XmlToolkit.h"

#include "15_env/XmlBuilder.h"
#include "15_env/Line.h"

#include "30_server/RequestException.h"

#include "39_map/MapModule.h"
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

using namespace boost::posix_time;
using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace util::XmlToolkit;
	using namespace env;

	namespace util
	{
		template<>
		const string FactorableTemplate<Function, map::MapRequest>::FACTORY_KEY("map");
	}

	namespace map
	{


		const std::string MapRequest::OUTPUT_PARAMETER ("output");
		const std::string MapRequest::DATA_PARAMETER ("data");
		const std::string MapRequest::MAP_PARAMETER ("map");
		const string MapRequest::PARAMETER_USE_ENVIRONMENT("ue");
		

		MapRequest::MapRequest()
			: _useEnvironment(true)
		{}

		ParametersMap MapRequest::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(OUTPUT_PARAMETER, _output);
			map.insert(DATA_PARAMETER, _data);
			map.insert(MAP_PARAMETER, _query);
			map.insert(PARAMETER_USE_ENVIRONMENT, _useEnvironment);
			return map;
		}



		void MapRequest::_setFromParametersMap(const ParametersMap& map)
		{
			setUseEnvironment(map.getBool(PARAMETER_USE_ENVIRONMENT, false, true, FACTORY_KEY));

			// Output
			setOutput(map.getString(OUTPUT_PARAMETER, true, FACTORY_KEY));
			
			if (!_useEnvironment)
			{
				// XML data
				setData(map.getString(DATA_PARAMETER, false, FACTORY_KEY));
			}
			
			// Map XML
			setQuery(map.getString(MAP_PARAMETER, true, FACTORY_KEY));
		}




		void MapRequest::_run( std::ostream& stream ) const
		{
			if (!_map.get())
				return;

			// Prepare the map (once for all renderings!)
			_map->prepare ();

			// Create a temporary file name based on system time
			const filesystem::path tempDir(MapModule::GetParameter (MapModule::PARAM_HTTP_TEMP_DIR), filesystem::native);

			RenderingConfig conf;

			// Choose the renderer
			shared_ptr<Renderer> renderer(Factory<Renderer>::create(_output));

			// Generate an id for the map file based on current time
			ptime timems (boost::date_time::microsec_clock<ptime>::local_time ());
			std::string filePrefix = "map_" + to_iso_string (timems);

			std::string resultFilename = renderer->render (tempDir, filePrefix, _temporaryEnvironment.getRegistry<Line>(), *_map, conf);

			// Broadcast of the result
			std::string resultURL = MapModule::GetParameter (MapModule::PARAM_HTTP_TEMP_URL) 
			    + "/" + resultFilename;
			
			// Send the URL to the the generated local JPEG file.
			stream << resultURL;
			
			Log::GetInstance ().debug ("Sent result url " + resultURL);

		}



		MapRequest::~MapRequest()
		{
		}



		void MapRequest::setData( const std::string& value )
		{
			_data = value;

			/// @todo Throw an exception if xml parsing fails
			XMLNode dataNode = XMLNode::parseString (_data.c_str (), "data");

			// Fill in local registries

			XMLNode citiesNode = GetChildNode (dataNode, "cities", 0);
			int nbCities = GetChildNodeCount (citiesNode, "city");
			for (int i=0; i<nbCities; ++i) 
			{
				XMLNode cityNode = GetChildNode (citiesNode, "city", i);
				_temporaryEnvironment.getEditableRegistry<City>().add (env::XmlBuilder::CreateCity (cityNode));
			}

			XMLNode axesNode = GetChildNode (dataNode, "axes", 0);
			int nbAxes = GetChildNodeCount (axesNode, "axis");
			for (int i=0; i<nbAxes; ++i) 
			{
				XMLNode axisNode = GetChildNode (axesNode, "axis", i);
				_temporaryEnvironment.getEditableRegistry<Axis>().add (env::XmlBuilder::CreateAxis (axisNode));
			}

			XMLNode connectionPlacesNode = GetChildNode (dataNode, "connectionPlaces", 0);
			int nbConnectionPlaces = GetChildNodeCount (connectionPlacesNode, "connectionPlace");
			for (int i=0; i<nbConnectionPlaces; ++i) 
			{
				XMLNode connectionPlaceNode = GetChildNode (connectionPlacesNode, "connectionPlace", i);
				_temporaryEnvironment.getEditableRegistry<PublicTransportStopZoneConnectionPlace>().add (synthese::env::XmlBuilder::CreateConnectionPlace (connectionPlaceNode, _temporaryEnvironment.getEditableRegistry<City>()));
			}

			XMLNode physicalStopsNode = GetChildNode (dataNode, "physicalStops", 0);
			int nbPhysicalStops = GetChildNodeCount (physicalStopsNode, "physicalStop");
			for (int i=0; i<nbPhysicalStops; ++i) 
			{
				XMLNode physicalStopNode = GetChildNode (physicalStopsNode, "physicalStop", i);
				_temporaryEnvironment.getEditableRegistry<PhysicalStop>().add (synthese::env::XmlBuilder::CreatePhysicalStop (physicalStopNode, _temporaryEnvironment.getEditableRegistry<PublicTransportStopZoneConnectionPlace>()));
			}

			XMLNode commercialLinesNode = GetChildNode (dataNode, "commercialLines", 0);
			int nbCommercialLines = GetChildNodeCount (commercialLinesNode, "commercialLine");
			for (int i=0; i<nbCommercialLines; ++i) 
			{
				XMLNode commercialLineNode = GetChildNode (commercialLinesNode, "commercialLine", i);
				_temporaryEnvironment.getEditableRegistry<CommercialLine>().add (synthese::env::XmlBuilder::CreateCommercialLine (commercialLineNode));
			}

			XMLNode linesNode = GetChildNode (dataNode, "lines", 0);
			int nbLines = GetChildNodeCount (linesNode, "line");
			for (int i=0; i<nbLines; ++i) 
			{
				XMLNode lineNode = GetChildNode (linesNode, "line", i);
				_temporaryEnvironment.getEditableRegistry<Line>().add (synthese::env::XmlBuilder::CreateLine (lineNode, _temporaryEnvironment.getEditableRegistry<Axis>(), _temporaryEnvironment.getEditableRegistry<CommercialLine>()));
			}

			XMLNode lineStopsNode = GetChildNode (dataNode, "lineStops", 0);
			int nbLineStops = GetChildNodeCount (lineStopsNode, "lineStop");
			for (int i=0; i<nbLineStops; ++i) 
			{
				XMLNode lineStopNode = GetChildNode (lineStopsNode, "lineStop", i);
				_temporaryEnvironment.getEditableRegistry<LineStop>().add (synthese::env::XmlBuilder::CreateLineStop (lineStopNode, _temporaryEnvironment.getEditableRegistry<Line>(), _temporaryEnvironment.getEditableRegistry<PhysicalStop>()));
			}
		}



		void MapRequest::setQuery( const std::string& value )
		{
			_query = value;

			/// @todo Throw an exception if xml parsing fails
			XMLNode mapNode = XMLNode::parseString (_query.c_str (), "map");

			_map.reset(map::XmlBuilder::CreateMap (mapNode, (_useEnvironment ? Env::GetOfficialEnv() : _temporaryEnvironment).getRegistry<Line>() ));
		}



		void MapRequest::setOutput( const std::string& value )
		{
			_output = value;

			if (!Factory<Renderer>::contains(_output))
			{
				throw RequestException ("Invalid map output type " + _output);
			}
		}



		void MapRequest::setUseEnvironment( bool value )
		{
			_useEnvironment = value;
		}



		bool MapRequest::_isAuthorized(

			) const {
			return true;
		}
	}
}
