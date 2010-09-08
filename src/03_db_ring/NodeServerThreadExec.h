#ifndef SYNTHESE_DBRING_NODESERVERTHREADEXEC_H
#define SYNTHESE_DBRING_NODESERVERTHREADEXEC_H

#include "03_db_ring/TransmissionStatusMap.h"
#include "03_db_ring/NodeInfo.h"

#include "01_util/threads/ThreadExec.h"

#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>


#include <iostream>



namespace synthese
{

namespace tcp
{
    class TcpServerSocket;
}


namespace dbring
{



/** Node server thread execution.

@ingroup m101
*/
class NodeServerThreadExec : public synthese::util::ThreadExec
{
 private:

    NodeInfo _clientNodeInfo;
    int _port;
    tcp::TcpServerSocket* _serverSocket;
    NodeInfoMap _nodeInfoMap;
    TransmissionStatusMap& _serverStatusMap;

 public:
    
    NodeServerThreadExec (NodeInfo clientNodeInfo,
			  int port,
			  tcp::TcpServerSocket* serverSocket, 
			  const NodeInfoMap& nodeInfoMap,
			  TransmissionStatusMap& serverStatusMap);
    

    /** Execution body.
     */
    void loop ();

    void finalize ();

};


}
}



#endif

