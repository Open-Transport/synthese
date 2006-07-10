#ifndef SYNTHESE_SERVER_SERVER_H
#define SYNTHESE_SERVER_SERVER_H

#include "module.h"


#include <iostream>
#include <string>

#include "RequestDispatcher.h"

#include <boost/filesystem/path.hpp>


namespace synthese
{
namespace server
{



/** Main server class.

This class holds :
- the TCP server 
- request queue (thread-safe access)
- the threading mechanism 
- the request dispatcher instance

@ingroup m70
*/
class Server
{
 private:

    static Server* _instance;

    RequestDispatcher _requestDispatcher;
    int _port;
    int _nbThreads;
    boost::filesystem::path _dataDir;
    boost::filesystem::path _tempDir;
    boost::filesystem::path _httpTempDir;
    std::string _httpTempUrl;

 protected:


 public:

    Server (int port, 
	    int nbThreads,
	    const std::string& dataDir,
	    const std::string& tempDir,
        const std::string& httpTempDir, 
        const std::string& httpTempUrl);

    ~Server ();


    //! @name Getters/Setters
    //@{
    RequestDispatcher& getRequestDispatcher ();

    int getPort () const;
    int getNbThreads () const;
    const boost::filesystem::path& getDataDir () const;
    const boost::filesystem::path& getTempDir () const;
    const boost::filesystem::path& getHttpTempDir () const;
    const std::string& getHttpTempUrl () const;

    static Server* GetInstance ();
    static void SetInstance (Server* instance);
    
    //@}


    //! @name Query methods
    //@{
    //@}


    //! @name Update methods
    //@{
    void run ();


    //@}

 protected:

    void initialize ();


};


}
}



#endif
