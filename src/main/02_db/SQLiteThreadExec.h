#ifndef SYNTHESE_DB_SQLITETHREADEXEC_H
#define SYNTHESE_DB_SQLITETHREADEXEC_H

#include "01_util/ThreadExec.h"



namespace synthese
{

namespace tcp
{
    class TcpService;
}



namespace db
{



/** SQLite server thread execution body.


@ingroup m02
*/
class SQLiteThreadExec : public synthese::util::ThreadExec
{
 private:
    synthese::tcp::TcpService* _tcpService;

 public:

    SQLiteThreadExec (synthese::tcp::TcpService* tcpService);


    /** Execution body.
     */
    void loop ();

};


}
}



#endif

