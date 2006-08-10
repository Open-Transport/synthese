#ifndef SYNTHESE_SERVER_SERVER_H
#define SYNTHESE_SERVER_SERVER_H

#include "module.h"


#include <iostream>
#include <string>

#include "RequestDispatcher.h"
#include "ServerConfig.h"

#include "15_env/Environment.h"
#include "11_interfaces/Interface.h"


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
    boost::filesystem::path _dbFile;
    ServerConfig _config;

    synthese::env::Environment::Registry _environments;
    synthese::interfaces::Interface::Registry _interfaces;

 protected:


 public:

    Server (const boost::filesystem::path& dbFile);

    ~Server ();


    //! @name Getters/Setters
    //@{

    synthese::env::Environment::Registry& getEnvironments ();
    const synthese::env::Environment::Registry& getEnvironments () const;
    
    synthese::interfaces::Interface::Registry& getInterfaces ();
    const synthese::interfaces::Interface::Registry& getInterfaces () const;

    RequestDispatcher& getRequestDispatcher ();
    ServerConfig& getConfig ();

    static Server* GetInstance ();
    static void SetInstance (Server* instance);
    
    //@}


    //! @name Query methods
    //@{
    //@}


    //! @name Update methods
    //@{
    void initialize ();

    void run ();


    //@}

 protected:


};


}
}



#endif
