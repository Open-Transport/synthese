#include "MapRequestHandler.h"

#include <iostream>



namespace synthese
{
namespace carto
{


const std::string MapRequestHandler::FUNCTION_CODE ("map");




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
    stream << "Hello from map request handler !" << std::endl;
}








}
}
