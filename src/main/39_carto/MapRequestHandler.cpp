#include "MapRequestHandler.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include <stdlib.h>


#include "01_util/XmlParser.h"

#include "15_env/Environment.h"
#include "16_env_ls_xml/EnvironmentLS.h"

#include "39_carto/Map.h"
#include "39_carto/PostscriptCanvas.h"
#include "40_carto_ls_xml/MapLS.h"

#include "70_server/Request.h"


using synthese::env::Environment;
using synthese::carto::Map;




namespace synthese
{
namespace carto
{


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

    // Create the postscript canvas for output
    std::ofstream of ("/home/mjambert/temp/map.ps");
    synthese::carto::PostscriptCanvas canvas (of);
    map->dump (canvas);
    of.close ();

    stream << "Hello from map request handler !" << std::endl;

    // Convert the ps file to png with ghostscript
    std::stringstream gscmd;
    gscmd << "gs -q -dSAFER -dBATCH -dNOPAUSE -sDEVICE=png16m -dGraphicsAlphaBits=4 -g" 
	  << map->getWidth () << "x" << map->getHeight () 
	  << " -sOutputFile=/home/mjambert/map.png /home/mjambert/temp/map.ps";
    
    int ret = system (gscmd.str ().c_str ());
    
    if (ret != 0)
    {
	throw synthese::util::Exception ("Error executing GhostScript (gs executable in path ?)");
    }


    delete map;
    delete env;
}








}
}
