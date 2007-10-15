#ifndef SYNTHESE_DBRING_APPLYUPDATETHREADEXEC_H
#define SYNTHESE_DBRING_APPLYUPDATETHREADEXEC_H


#include "01_util/threads/ThreadExec.h"

#include <boost/thread/recursive_mutex.hpp>
#include <boost/shared_ptr.hpp>


#include <iostream>



namespace synthese
{


namespace dbring
{



/** This thread is executed only on master authority only.
    It is aimed at applying chronologically SQL updates held by update records,
    according to their timestamps.
    
    The loop consist in the following steps :
    1) Check for PENDING update records timestamped before last ACKNOWLEDGED update record.

    * if policy is Immediate, the update record is executed immediately
    * if policy is Postpone, the update record is delayed and enqueued back
    * if policy is Abort, the update record is marked failed

    2) Take the next PENDING update record after last ACKNOWLEDGED timestamp, open a transation,
       flag this update record as acknowledged, execute its SQL data and commit transaction.
       If this commit fails, the update record is marked as FAILED.
      

@ingroup m03
*/
class ApplyUpdateThreadExec : public synthese::util::ThreadExec
{

    typedef enum { IMMEDIATE, POSTPONE, ABORT } LatePendingPolicy;

 private:

    LatePendingPolicy _latePendingPolicy;
//    boost::shared_ptr<boost::recursive_mutex> _applyMutex; 

 public:
    
    ApplyUpdateThreadExec (const LatePendingPolicy& latePendingPolicy = POSTPONE);

    /** Execution body.
     */
    void loop ();

    void finalize ();

  private:
    
    void processLatePendingRecords ();


};


}
}



#endif

