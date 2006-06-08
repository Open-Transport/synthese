#include "MapRequestHandler.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <assert.h>
#include <stdlib.h>

#include <boost/filesystem/operations.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/iostreams/filtering_stream.hpp>

#include "01_util/PlainCharFilter.h"
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
#include "39_carto/MapInfoRenderer.h"

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
const std::string MapRequestHandler::REQUEST_OUTPUT_JPEG ("jpeg");
const std::string MapRequestHandler::REQUEST_OUTPUT_HTML ("html");
const std::string MapRequestHandler::REQUEST_OUTPUT_MAPINFO ("mapinfo");


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
	(output != REQUEST_OUTPUT_JPEG) &&
	(output != REQUEST_OUTPUT_HTML) &&
	(output != REQUEST_OUTPUT_MAPINFO))
    {
        throw synthese::server::RequestException ("Invalid map output type " + output);
    }

	std::string envXml (request.getParameter (ENVIRONMENT_PARAMETER));
	// std::cerr << "envxml " << envXml.size () << std::endl;
    XMLNode envNode = XMLNode::parseString (envXml.c_str (),
					    synthese::envlsxml::EnvironmentLS::ENVIRONMENT_TAG.c_str ());

    Environment* env = synthese::envlsxml::EnvironmentLS::Load (envNode);

    XMLNode mapNode = XMLNode::parseString (request.getParameter (MAP_PARAMETER).c_str (),
					    synthese::cartolsxml::MapLS::MAP_TAG.c_str ());

    Map* map = synthese::cartolsxml::MapLS::Load (mapNode, *env);
    // Prepare the map (once for all renderings!)
    map->prepare ();

    // Create a temporary file name based on system time
    const boost::filesystem::path& tempDir = (mode == REQUEST_MODE_SOCKET) 
            ? Server::GetInstance ()->getTempDir ()
            : Server::GetInstance ()->getHttpTempDir ();

    // Generate an id for the map file based on current time
    ptime timems = boost::date_time::microsec_clock<ptime>::local_time ();

    std::string filePrefix = "map_" + to_iso_string (timems);

    std::string resultFilename = "";
    RenderingConfig conf;

    if (output == REQUEST_OUTPUT_PS) 
    {
	resultFilename = renderPsFile (tempDir, filePrefix, *map, conf);
    } 
    else if (output == REQUEST_OUTPUT_JPEG) 
    {
	resultFilename = renderJpegFile (tempDir, filePrefix, *map, conf);
    }
    else if (output == REQUEST_OUTPUT_HTML) 
    {
	resultFilename = renderHtmlFile (tempDir, filePrefix, *env, *map, conf);
    }
    else if (output == REQUEST_OUTPUT_MAPINFO) 
    {
	resultFilename = renderMapinfoFile (tempDir, filePrefix, *map, conf);
    }



    if (mode == REQUEST_MODE_SOCKET) 
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
    else if (mode == REQUEST_MODE_HTTP)
    {
	std::string resultURL = Server::GetInstance ()->getHttpTempUrl () + "/" + resultFilename;

        // Send the URL to the the generated local JPEG file.
        stream << resultURL << std::endl;

	Log::GetInstance ().debug ("Sent result url " + resultURL);
    }

    delete map;
    delete env;
}







std::string 
MapRequestHandler::renderPsFile (const boost::filesystem::path& tempDir, 
				 const std::string filenamePrefix,
				 Map& map,
				 const RenderingConfig& conf) const
{
    std::string resultFilename = filenamePrefix + ".ps";
    const boost::filesystem::path psFile (tempDir / resultFilename);

    // Create the postscript canvas for output
	std::ofstream of (psFile.string ().c_str ());

	// Filter accents (temporary workaround til having found how to
	// render accents properly in ghostscript.

	// boost::iostreams::filtering_ostream fof;
	// fof.push (synthese::util::PlainCharFilter());
	// fof.push (of);

    // ---- Render postscript file ----
    synthese::carto::PostscriptRenderer psRenderer (conf, of);
    psRenderer.render (map);

    of.close ();

    return resultFilename;
}





std::string 
MapRequestHandler::renderJpegFile (const boost::filesystem::path& tempDir, 
				  const std::string filenamePrefix,
				  Map& map,
				  const RenderingConfig& conf) const
{
    renderPsFile (tempDir, filenamePrefix, map, conf);

    std::string psFilename = filenamePrefix + ".ps";
    const boost::filesystem::path psFile (tempDir / psFilename);

    std::string resultFilename (filenamePrefix + ".jpg");
    const boost::filesystem::path jpegFile (tempDir / resultFilename);
    
    // Convert the ps file to jpeg with ghostscript
    std::stringstream gscmd;
    gscmd << GHOSTSCRIPT_BIN << " -q -dSAFER -dBATCH -dNOPAUSE -sDEVICE=jpeg -dJPEGQ=50 -dTextAlphaBits=4 -dGraphicsAlphaBits=4 -g" 
	  << map.getWidth () << "x" << map.getHeight () 
	  << " -sOutputFile=" << jpegFile.string () << " " << psFile.string ();
    
    Log::GetInstance ().debug (gscmd.str ());
    
    int ret = system (gscmd.str ().c_str ());
    
    boost::filesystem::remove (psFile);

    if (ret != 0)
    {
	throw synthese::util::Exception ("Error executing GhostScript (gs executable in path ?)");
    }

    return resultFilename;
}





    
std::string 
MapRequestHandler::renderHtmlFile (const boost::filesystem::path& tempDir, 
				   const std::string filenamePrefix,
				   const Environment& environment,
				   Map& map,
				   const RenderingConfig& conf) const
{
    renderJpegFile (tempDir, filenamePrefix, map, conf);

    std::string jpegFilename = filenamePrefix + ".jpg";

    std::string resultFilename (filenamePrefix + ".html");
    const boost::filesystem::path htmlFile (tempDir / resultFilename);
    
    std::ofstream ofhtml (htmlFile.string ().c_str ());
    synthese::carto::HtmlMapRenderer hmRenderer (conf, environment, map.getUrlPattern (), 
		Server::GetInstance ()->getHttpTempUrl () + "/" + jpegFilename, ofhtml);
    hmRenderer.render (map);
    ofhtml.close ();
    
    return resultFilename;
}





    
std::string 
MapRequestHandler::renderMapinfoFile (const boost::filesystem::path& tempDir, 
				      const std::string filenamePrefix,
				      Map& map,
				      const RenderingConfig& conf) const
{
    std::string resultFilename (filenamePrefix + ".zip");
    boost::filesystem::path zipOutput (tempDir / resultFilename);

    synthese::carto::MapInfoRenderer mapinfoRenderer (conf, zipOutput);
    mapinfoRenderer.render (map);

    return resultFilename;
}



    




}
}
