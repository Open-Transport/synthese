#ifndef SYNTHESE_DB_DBCOMMANDTHREADEXEC_H
#define SYNTHESE_DB_DBCOMMANDTHREADEXEC_H

#include "01_util/threads/ThreadExec.h"



namespace synthese
{

namespace tcp
{
    class TcpService;
}



namespace dbring
{



/** SQLite server thread execution body.


@ingroup m10
*/
class DbCommandThreadExec : public synthese::util::ThreadExec
{
 private:
    synthese::tcp::TcpService* _tcpService;

 public:

    DbCommandThreadExec (synthese::tcp::TcpService* tcpService);


    /** Execution body.
     */
    void loop ();

};


}
}



#endif

