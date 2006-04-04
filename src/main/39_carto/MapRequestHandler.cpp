#include "MapRequestHandler.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <assert.h>
#include <stdlib.h>


#include "01_util/XmlParser.h"
#include "01_util/Conversion.h"

#include "70_server/Server.h"
#include "70_server/Request.h"

#include "15_env/Environment.h"
#include "16_env_ls_xml/EnvironmentLS.h"

#include "39_carto/Map.h"
#include "39_carto/PostscriptCanvas.h"
#include "40_carto_ls_xml/MapLS.h"

#include "01_util/Log.h"
#include <boost/filesystem/operations.hpp>



using synthese::env::Environment;
using synthese::carto::Map;
using synthese::util::Conversion;
using synthese::util::Log;



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

const std::string MapRequestHandler::ENVIRONMENT_PARAMETER ("env");
const std::string MapRequestHandler::MAP_PARAMETER ("map");



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
    XMLNode envNode = XMLNode::parseString (request.getParameter (ENVIRONMENT_PARAMETER).c_str (),
					    synthese::envlsxml::EnvironmentLS::ENVIRONMENT_TAG.c_str ());

    Environment* env = synthese::envlsxml::EnvironmentLS::Load (envNode);

    XMLNode mapNode = XMLNode::parseString (request.getParameter (MAP_PARAMETER).c_str (),
					    synthese::cartolsxml::MapLS::MAP_TAG.c_str ());
    Map* map = synthese::cartolsxml::MapLS::Load (mapNode, *env);

    // Create a temporary file name based on system time
    const boost::filesystem::path& tempDir = synthese::server::Server::GetInstance ()->getTempDir ();

    // The request adress is taken as unique id for map tmp file.
    const boost::filesystem::path tempPsFile (tempDir / ("map_" + Conversion::ToString ((unsigned long) &request) + ".ps"));
    const boost::filesystem::path tempPngFile (tempDir / ("map_" + Conversion::ToString ((unsigned long) &request) + ".png"));

    // Create the postscript canvas for output
    std::ofstream of (tempPsFile.string ().c_str ());
    synthese::carto::PostscriptCanvas canvas (of);
    map->dump (canvas);
    of.close ();

    // Convert the ps file to png with ghostscript
    std::stringstream gscmd;
    gscmd << GHOSTSCRIPT_BIN << " -q -dSAFER -dBATCH -dNOPAUSE -sDEVICE=png16m -dTextAlphaBits=4 -dGraphicsAlphaBits=4 -g" 
	  << map->getWidth () << "x" << map->getHeight () 
	  << " -sOutputFile=" << tempPngFile.string () << " " << tempPsFile.string ();
    
    Log::GetInstance ().debug (gscmd.str ());

    int ret = system (gscmd.str ().c_str ());
    
    if (ret != 0)
    {
	throw synthese::util::Exception ("Error executing GhostScript (gs executable in path ?)");
    }
    // boost::filesystem::remove (tempPsFile);

    // Now get size of the generated PNG file...
    long size = boost::filesystem::file_size (tempPngFile);
    
    // ...and send the content of the file through the socket.
    std::ifstream ifpng (tempPngFile.string ().c_str ());
    char ch;
    long nbChars = 0;

    stream << size << ":";
    for (int i=0; i<size; ++i)
    {
	ifpng.get(ch);
	stream << ch;
	++nbChars;
    }
    ifpng.close ();
    
    stream << std::flush;

    // std::cout << "Sent << " << nbChars << std::endl;
    // assert (nbChars == size);

    // boost::filesystem::remove (tempPngFile);

    delete map;
    delete env;
}








}
}
