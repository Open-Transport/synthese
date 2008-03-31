#ifndef SYNTHESE_SERVER_SERVERTHREADEXEC_H
#define SYNTHESE_SERVER_SERVERTHREADEXEC_H

#include "01_util/threads/ThreadExec.h"



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


@ingroup m18
*/
class ServerThreadExec : public synthese::util::ThreadExec
{
 private:
    synthese::tcp::TcpService* _tcpService;

 public:

    ServerThreadExec (synthese::tcp::TcpService* tcpService);


    /** Execution body.
     */
    void loop ();

};


}
}



#endif

