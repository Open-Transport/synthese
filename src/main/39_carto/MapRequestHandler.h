#ifndef SYNTHESE_CARTO_MAPREQUESTHANDLER_H
#define SYNTHESE_CARTO_MAPREQUESTHANDLER_H



#include "module.h"

#include "70_server/RequestHandler.h"

#include <boost/filesystem/path.hpp>

#include <iostream>
#include <string>



namespace synthese
{

namespace server
{
    class Request;
}



namespace carto
{


/** Map request handler.

Generates a map given all the information to be drawn.
The response sent through the socket has the following format

|{PNGFILESIZE}|:|{PNG FILE DATA}

@ingroup m70
*/
class MapRequestHandler : public synthese::server::RequestHandler
{
 public:

    static const std::string GHOSTSCRIPT_BIN;

    static const std::string FUNCTION_CODE;
    static const std::string ENVIRONMENT_PARAMETER;
    static const std::string MAP_PARAMETER;

 private:

 protected:

 public:

    MapRequestHandler ();

    MapRequestHandler (const MapRequestHandler& ref);

    ~MapRequestHandler ();

    //! @name Getters/Setters
    //@{
    //@}


    //! @name Query methods
    //@{
    void handleRequest (const synthese::server::Request& request,
			std::ostream& stream) const;
    //@}



    //! @name Update methods
    //@{

    //@}



};


}
}



#endif
