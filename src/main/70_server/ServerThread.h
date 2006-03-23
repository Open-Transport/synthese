#ifndef SYNTHESE_SERVER_THREAD_H
#define SYNTHESE_SERVER_THREAD_H

#include "module.h"



namespace synthese
{

namespace tcp
{
    class TcpService;
}

namespace server
{



/** Server thread execution body.

A server thread is waiting for an incoming tcp connection.
As soon a client has connected, another thread is ready
to accept another client connection.


@ingroup m70
*/
class ServerThread
{
 private:
    synthese::tcp::TcpService* _tcpService;

 public:

    ServerThread (synthese::tcp::TcpService* tcpService);

    /** Execution body
     */
    void operator()();

};


}
}



#endif
