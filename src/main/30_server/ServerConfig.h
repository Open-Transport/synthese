#ifndef SYNTHESE_SERVER_SERVERCONFIG_H
#define SYNTHESE_SERVER_SERVERCONFIG_H

#include "module.h"

#include "01_util/Log.h"

#include <boost/filesystem/path.hpp>
#include <string>



namespace synthese
{
namespace server
{



/** 
@ingroup m70
*/
class ServerConfig
{
 private:
    
    int _port;
    int _nbThreads;
    boost::filesystem::path _dataDir;
    boost::filesystem::path _tempDir;
    boost::filesystem::path _httpTempDir;
    std::string _httpTempUrl;
    

 protected:


 public:

    ServerConfig ();
    ~ServerConfig ();


    //! @name Getters/Setters
    //@{
    int getPort () const;
    void setPort (int port);

    synthese::util::Log::Level getLogLevel () const;
    void setLogLevel (const synthese::util::Log::Level& level) const;

    int getNbThreads () const;
    void setNbThreads (int nbThreads);

    const boost::filesystem::path& getDataDir () const;
    void setDataDir (const boost::filesystem::path& path);

    const boost::filesystem::path& getTempDir () const;
    void setTempDir (const boost::filesystem::path& path);

    const boost::filesystem::path& getHttpTempDir () const;
    void setHttpTempDir (const boost::filesystem::path& path);

    const std::string& getHttpTempUrl () const;
    void setHttpTempUrl (const std::string& httpTempUrl);
    //@}


    //! @name Query methods
    //@{
    
    //@}


    //! @name Update methods
    //@{


    //@}



};


}
}



#endif
