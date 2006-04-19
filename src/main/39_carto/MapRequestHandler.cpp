#include "MapRequestHandler.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <assert.h>
#include <stdlib.h>

#include <boost/filesystem/operations.hpp>
#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "01_util/XmlParser.h"
#include "01_util/Conversion.h"

#include "70_server/Server.h"
#include "70_server/Request.h"
#include "70_server/RequestException.h"

#include "15_env/Environment.h"
#include "16_env_ls_xml/EnvironmentLS.h"

#include "39_carto/Map.h"
#include "39_carto/RenderingConfig.h"
#include "39_carto/PostscriptRenderer.h"
#include "39_carto/HtmlMapRenderer.h"
#include "40_carto_ls_xml/MapLS.h"

#include "01_util/Log.h"


using synthese::env::Environment;
using synthese::carto::Map;
using synthese::util::Conversion;
using synthese::util::Log;
using synthese::server::Server;
using namespace boost::posix_time;


namespace synthese
{
namespace carto
{

#ifdef WIN32
const std::string MapRequestHandler::GHOSTSCRIPT_BIN ("gswin32");
#else
const std::string MapRequestHandler::GHOSTSCRIPT_BIN ("gs");
#endif

const std::string MapRequestHandler::FUNCTION_CODE ("map");

const std::string MapRequestHandler::MODE_PARAMETER ("mode");
const std::string MapRequestHandler::OUTPUT_PARAMETER ("output");
const std::string MapRequestHandler::ENVIRONMENT_PARAMETER ("env");
const std::string MapRequestHandler::MAP_PARAMETER ("map");

const int MapRequestHandler::REQUEST_MODE_SOCKET (1);
const int MapRequestHandler::REQUEST_MODE_HTTP (2);

const std::string MapRequestHandler::REQUEST_OUTPUT_PS ("ps");
const std::string MapRequestHandler::REQUEST_OUTPUT_PNG ("png");
const std::string MapRequestHandler::REQUEST_OUTPUT_HTML ("html");


MapRequestHandler::MapRequestHandler ()
    : synthese::server::RequestHandler (FUNCTION_CODE)
{
}



MapRequestHandler::MapRequestHandler (const MapRequestHandler& ref)
    : synthese::server::RequestHandler (ref)
{

}



MapRequestHandler::~MapRequestHandler ()
{

}



void 
MapRequestHandler::handleRequest (const synthese::server::Request& request,
				  std::ostream& stream) const
{
    int mode = Conversion::ToInt (request.getParameter (MODE_PARAMETER));
    if ((mode != REQUEST_MODE_SOCKET) && (mode != REQUEST_MODE_HTTP))
    {
        throw synthese::server::RequestException ("Invalid map request mode " + Conversion::ToString (mode));
    }

    std::string output = request.getParameter (OUTPUT_PARAMETER);
    if ((output != REQUEST_OUTPUT_PS) && 
	(output != REQUEST_OUTPUT_PNG) &&
	(output != REQUEST_OUTPUT_HTML))
    {
        throw synthese::server::RequestException ("Invalid map output type " + output);
    }
    
    XMLNode envNode = XMLNode::parseString (request.getParameter (ENVIRONMENT_PARAMETER).c_str (),
					    synthese::envlsxml::EnvironmentLS::ENVIRONMENT_TAG.c_str ());

    Environment* env = synthese::envlsxml::EnvironmentLS::Load (envNode);

    XMLNode mapNode = XMLNode::parseString (request.getParameter (MAP_PARAMETER).c_str (),
					    synthese::cartolsxml::MapLS::MAP_TAG.c_str ());

    Map* map = synthese::cartolsxml::MapLS::Load (mapNode, *env);

    // Create a temporary file name based on system time
    const boost::filesystem::path& tempDir = (mode == REQUEST_MODE_SOCKET) 
            ? Server::GetInstance ()->getTempDir ()
            : Server::GetInstance ()->getHttpTempDir ();

    // Generate an id for the map file based on current time
    ptime timems = boost::date_time::microsec_clock<ptime>::local_time ();

    std::string resultFilename = "map_" + to_iso_string (timems) + ".ps";
    const boost::filesystem::path psFile (tempDir / resultFilename);

    // Create the postscript canvas for output
    std::ofstream of (psFile.string ().c_str ());

    // ---- Render postscript file ----
    RenderingConfig conf;
    synthese::carto::PostscriptRenderer psRenderer (conf, of);
    psRenderer.render (*map);

    of.close ();

    if ((output == REQUEST_OUTPUT_PNG) || (output == REQUEST_OUTPUT_HTML))
    {
	// ---- Render PNG file ----
	resultFilename = "map_" + to_iso_string (timems) + ".png";
	const boost::filesystem::path pngFile (tempDir / resultFilename);

	// Convert the ps file to png with ghostscript
	std::stringstream gscmd;
	gscmd << GHOSTSCRIPT_BIN << " -q -dSAFER -dBATCH -dNOPAUSE -sDEVICE=png16m -dTextAlphaBits=4 -dGraphicsAlphaBits=4 -g" 
	      << map->getWidth () << "x" << map->getHeight () 
	      << " -sOutputFile=" << pngFile.string () << " " << psFile.string ();
	
	Log::GetInstance ().debug (gscmd.str ());
	
	int ret = system (gscmd.str ().c_str ());
	
	if (ret != 0)
	{
	    throw synthese::util::Exception ("Error executing GhostScript (gs executable in path ?)");
	}
	
	boost::filesystem::remove (psFile);

	if (output == REQUEST_OUTPUT_HTML)
	{
	    std::string pngFilename (resultFilename);
	    resultFilename = "map_" + to_iso_string (timems) + ".html";
	    const boost::filesystem::path htmlFile (tempDir / resultFilename);

	    std::ofstream ofhtml (htmlFile.string ().c_str ());
	    synthese::carto::HtmlMapRenderer hmRenderer (conf, map->getUrlPattern (), pngFilename, ofhtml);
	    hmRenderer.render (*map);
	    ofhtml.close ();

	} 
	else 
	{
	}

    }



    if (mode == REQUEST_MODE_SOCKET) 
    {

	Log::GetInstance ().warn ("!!! Removed implementation for socket mode !!!");

	/* USELESS ! if one wants to pass args that exceed the limit, he just does not
	   through the CGI interface but the return mechanism will still be an HTTP URL!! 

	   // Now get size of the generated PNG file...
	   long size = boost::filesystem::file_size (pngFile);

	std::ifstream ifpng (tempPngFile.string ().c_str (), std::ifstream::binary);
	char * buffer;
	buffer = new char [size];
	ifpng.read (buffer, size);
	ifpng.close();

        // Send the content of the file through the socket.
        stream << size << ":";
        stream.write (buffer, size);
        stream << std::flush;

        // Remove the PNG file
        boost::filesystem::remove (tempPngFile);

	delete[] buffer;
	Log::GetInstance ().debug ("Sent PNG result (" + Conversion::ToString (size) + " bytes)");

	*/
    }
    else if (mode == REQUEST_MODE_HTTP)
    {
	std::string resultURL = Server::GetInstance ()->getHttpTempUrl () + "/" + resultFilename;

        // Send the URL to the the generated local PNG file.
        stream << resultURL << std::endl;

	Log::GetInstance ().debug ("Sent result url " + resultURL);
    }



    delete map;
    delete env;
}








}
}
