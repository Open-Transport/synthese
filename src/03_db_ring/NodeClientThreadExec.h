#ifndef SYNTHESE_DBRING_NODECLIENTTHREADEXEC_H
#define SYNTHESE_DBRING_NODECLIENTTHREADEXEC_H

#include "03_db_ring/TransmissionStatusMap.h"
#include "03_db_ring/NodeInfo.h"

#include "01_util/threads/ThreadExec.h"

#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>



namespace synthese
{


namespace tcp
{
    class TcpClientSocket;
}



namespace dbring
{



/** Node client thread execution.

@ingroup m101
*/
class NodeClientThreadExec : public synthese::util::ThreadExec
{
 private:

    NodeInfo _serverNodeInfo;
    NodeInfo _clientNodeInfo;
    NodeInfoMap _clientNodeInfoMap;
    TransmissionStatusMap& _clientStatusMap;

 public:
    
    NodeClientThreadExec (
	NodeInfo serverNodeInfo,
	NodeInfo clientNodeInfo,
	NodeInfoMap clientNodeInfoMap,
	TransmissionStatusMap& clientStatusMap);

    ~NodeClientThreadExec ();


    void initialize ();

    /** Execution body.
     */
    void loop ();

    void finalize ();

};


}
}



#endif

