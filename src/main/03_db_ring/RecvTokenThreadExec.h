#ifndef SYNTHESE_DBRING_RECVTOKENTHREADEXEC_H
#define SYNTHESE_DBRING_RECVTOKENTHREADEXEC_H

#include "03_db_ring/Token.h"
#include "01_util/concurrent/SynchronizedQueue.h"
#include "01_util/threads/ThreadExec.h"
#include "00_tcp/TcpService.h"



namespace synthese
{


namespace dbring
{



/** Recv token thread execution.

@ingroup m02
*/
class RecvTokenThreadExec : public synthese::util::ThreadExec
{
 private:

    const int _port;
    tcp::TcpService* _tcpService;
    util::SynchronizedQueue<TokenSPtr>& _tokenQueue;

 public:

    RecvTokenThreadExec (int port, util::SynchronizedQueue<TokenSPtr>& tokenQueue);


    void initialize ();

    /** Execution body.
     */
    void loop ();

    void finalize ();

};


}
}



#endif

