#ifndef SYNTHESE_SERVER_SERVER_H
#define SYNTHESE_SERVER_SERVER_H

#include "module.h"


#include <deque>
#include <iostream>
#include <string>

#include "Request.h"
#include "RequestDispatcher.h"

#include "00_tcp/TcpServerSocket.h"
#include "00_tcp/TcpService.h"

#include "01_util/Log.h"

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

    int _port;
    int _nbThreads;
    boost::filesystem::path _dataDir;
    boost::filesystem::path _tempDir;

 protected:


 public:

    Server (int port = 3591, 
	    int nbThreads = 10,
	    const std::string& dataDir = ".",
	    const std::string& tempDir = ".");

    ~Server ();


    //! @name Getters/Setters
    //@{
    int getPort () const;
    int getNbThreads () const;
    const boost::filesystem::path& getDataDir () const;
    const boost::filesystem::path& getTempDir () const;

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
