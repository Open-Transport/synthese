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

namespace env
{
    class Environment;
}



namespace carto
{

    class Map;
    class RenderingConfig;


/** Map request handler.

Generates a map given all the information to be drawn.

Two modes are available for requesting :

* Mode 1 : "Socket mode"
  Request format : mode=1&output=png&env={XML_ENV}&map={XML_MAP}
       where XML_ENV is the XML definition of the local
       environment used for map generation; 
       where XML_MAP is the XML definition of the map 
       drawing parameters.
  Response type : PNG file sent through socket
  Response format : |{PNGFILESIZE}|:|{PNG FILE DATA}
  
* Mode 2 : "HTTP mode"
  Request format : mode=2&output=ps&env={XML_ENV}&map={XML_MAP}
       where XML_ENV is the XML definition of the local
       environment used for map generation; 
       where XML_MAP is the XML definition of the map 
       drawing parameters.
  Response type : JPEG file generated locally on server side.
  Response format : |{JPEG_URL}|
       where PNG_URL is the url of the generated PNG file.

The different output types are :
  - ps  : PostScript
  - jpeg : JPEG bitmap
  - html : JPEG + HTML map (only available in http mode)
  - mapinfo : MID/MIF files returned in a zip
  
@ingroup m39
*/
class MapRequestHandler : public synthese::server::RequestHandler
{
 public:

    static const std::string GHOSTSCRIPT_BIN;

    static const std::string FUNCTION_CODE;

    static const std::string MODE_PARAMETER;
    static const std::string OUTPUT_PARAMETER;
    static const std::string ENVIRONMENT_PARAMETER;
    static const std::string MAP_PARAMETER;

    static const int REQUEST_MODE_SOCKET;
    static const int REQUEST_MODE_HTTP;

    static const std::string REQUEST_OUTPUT_PS;
    static const std::string REQUEST_OUTPUT_JPEG;
    static const std::string REQUEST_OUTPUT_HTML;
    static const std::string REQUEST_OUTPUT_MAPINFO;

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

 private:

    std::string renderPsFile (const boost::filesystem::path& tempDir, 
			      const std::string filenamePrefix,
			      Map& map,
			      const RenderingConfig& config) const;
    
    std::string renderJpegFile (const boost::filesystem::path& tempDir, 
			       const std::string filenamePrefix,
			       Map& map,
			       const RenderingConfig& config) const;
    
    std::string renderHtmlFile (const boost::filesystem::path& tempDir, 
				const std::string filenamePrefix,
				const synthese::env::Environment& environment,
				Map& map,
				const RenderingConfig& config) const;
    
    std::string renderMapinfoFile (const boost::filesystem::path& tempDir, 
				   const std::string filenamePrefix,
				   Map& map,
				   const RenderingConfig& config) const;
    

};


}
}



#endif
