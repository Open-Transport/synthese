#ifndef SYNTHESE_DBRING_SENDTOKENTHREADEXEC_H
#define SYNTHESE_DBRING_SENDTOKENTHREADEXEC_H

#include "03_db_ring/TransmissionStatusMap.h"
#include "03_db_ring/Token.h"

#include "01_util/threads/ThreadExec.h"

#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>



namespace synthese
{


namespace dbring
{



/** Send token thread execution.

@ingroup m02
*/
class SendTokenThreadExec : public synthese::util::ThreadExec
{
 private:

    const NodeId _emitterNodeId;
    const NodeInfo _nodeInfo;
    std::stringstream _tokenBuffer;
    TransmissionStatusMap& _transmissionStatusMap;

 public:

    SendTokenThreadExec (const NodeId& emitterNodeId, 
			 const NodeInfo& nodeInfo, 
			 const TokenSPtr token, 
			 TransmissionStatusMap& transmissionStatusMap);


    /** Execution body.
     */
    void loop ();

};


}
}



#endif

