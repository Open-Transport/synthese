
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <assert.h>
#include <stdlib.h>

#include <boost/filesystem/operations.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/iostreams/filtering_stream.hpp>


#include "01_util/XmlParser.h"
#include "01_util/Log.h"
#include "01_util/PlainCharFilter.h"
#include "01_util/Conversion.h"

#include "15_env/Environment.h"
#include "16_env_ls_xml/EnvironmentLS.h"

#include "30_server/Server.h"
#include "30_server/RequestException.h"

#include "40_carto_ls_xml/MapLS.h"
#include "39_carto/Map.h"
#include "39_carto/RenderingConfig.h"
#include "39_carto/Renderer.h"
#include "39_carto/PostscriptRenderer.h"
#include "39_carto/HtmlMapRenderer.h"
#include "39_carto/MapInfoRenderer.h"
#include "39_carto/MapRequestHandler.h"


using synthese::carto::Map;
using synthese::carto::RenderingConfig;

using synthese::cartolsxml::MapLS;


using synthese::env::Environment;
using synthese::carto::Map;

using synthese::util::Conversion;
using synthese::util::Log;
using namespace boost::posix_time;


namespace synthese
{
	using namespace interfaces;
	using namespace server;

	namespace carto
	{

		const std::string MapRequestHandler::MODE_PARAMETER ("mode");
		const std::string MapRequestHandler::OUTPUT_PARAMETER ("output");
		const std::string MapRequestHandler::ENVIRONMENT_PARAMETER ("env");
		const std::string MapRequestHandler::MAP_PARAMETER ("map");

		const int MapRequestHandler::REQUEST_MODE_SOCKET (1);
		const int MapRequestHandler::REQUEST_MODE_HTTP (2);



		void MapRequestHandler::setFromParametersMap( const ParametersMap& map )
		{
			ParametersMap::const_iterator it;

			// Mode
			it = map.find(MODE_PARAMETER);
			if (it == map.end())
				throw RequestException("Mode not specified");
			_mode = Conversion::ToInt (it->second);
			if ((_mode != REQUEST_MODE_SOCKET) && (_mode != REQUEST_MODE_HTTP))
			{
				throw RequestException ("Invalid map request mode " + Conversion::ToString (_mode));
			}

			// Output
			it = map.find(OUTPUT_PARAMETER);
			if (it == map.end())
				throw RequestException("Output not specified");
			_output = it->second;
			if (!Factory<Renderer>::contains(_output))
			{
				throw RequestException ("Invalid map output type " + _output);
			}

			// Env XML
			/// @todo Ne peut on pas se passer de temporary environment ?
			it = map.find(ENVIRONMENT_PARAMETER);
			if (it == map.end())
				throw RequestException("temporary environment not specified");
			// std::cerr << "envxml " << envXml.size () << std::endl;
			/// @todo Throw an exception if xml parsing fails
			_envNode = XMLNode::parseString (it->second.c_str (), synthese::envlsxml::EnvironmentLS::ENVIRONMENT_TAG.c_str ());

			// Map XML
			it = map.find(ENVIRONMENT_PARAMETER);
			if (it == map.end())
				throw RequestException("Map to draw not specified");
			/// @todo Throw an exception if xml parsing fails
			_mapNode = XMLNode::parseString (it->second.c_str (), synthese::cartolsxml::MapLS::MAP_TAG.c_str ());

		}

		/** @todo To be implemented if needed.
		*/
		Request::ParametersMap MapRequestHandler::getParametersMap() const
		{
			ParametersMap map;
			return map;
		}

		void MapRequestHandler::run( std::ostream& stream ) const /*= 0*/
		{
			XMLNode node = _envNode;
			Environment* env = synthese::envlsxml::EnvironmentLS::Load (node);
			node = _mapNode;
			Map* map = synthese::cartolsxml::MapLS::Load (node, *env);

			// Prepare the map (once for all renderings!)
			map->prepare ();

			// Create a temporary file name based on system time
			const boost::filesystem::path& tempDir = (_mode == REQUEST_MODE_SOCKET) 
				? Server::GetInstance ()->getConfig ().getTempDir ()
				: Server::GetInstance ()->getConfig ().getHttpTempDir ();


			RenderingConfig conf;

			// Choose the renderer
			Renderer* renderer = Factory<Renderer>::create(_output);

			// Generate an id for the map file based on current time
			ptime timems (boost::date_time::microsec_clock<ptime>::local_time ());
			std::string filePrefix = "map_" + to_iso_string (timems);


			std::string resultFilename = renderer->render(tempDir, filePrefix, env, *map, conf);


			// Broadcast of the result
			if (_mode == REQUEST_MODE_SOCKET) 
			{

				Log::GetInstance ().warn ("!!! Removed implementation for socket mode !!!");

				/* USELESS ! if one wants to pass args that exceed the limit, he just does not
				through the CGI interface but the return mechanism will still be an HTTP URL!! 

				// Now get size of the generated JPEG file...
				long size = boost::filesystem::file_size (jpegFile);

				std::ifstream ifjpeg (tempJpegFile.string ().c_str (), std::ifstream::binary);
				char * buffer;
				buffer = new char [size];
				ifjpeg.read (buffer, size);
				ifjpeg.close();

				// Send the content of the file through the socket.
				stream << size << ":";
				stream.write (buffer, size);
				stream << std::flush;

				// Remove the JPEG file
				boost::filesystem::remove (tempJpegFile);

				delete[] buffer;
				Log::GetInstance ().debug ("Sent JPEG result (" + Conversion::ToString (size) + " bytes)");

				*/
			}
			else if (_mode == REQUEST_MODE_HTTP)
			{
				std::string resultURL = Server::GetInstance ()->getConfig ().getHttpTempUrl () + "/" + resultFilename;

				// Send the URL to the the generated local JPEG file.
				stream << resultURL << std::endl;

				Log::GetInstance ().debug ("Sent result url " + resultURL);
			}

			delete map;
			delete env;

		}
	}
}
