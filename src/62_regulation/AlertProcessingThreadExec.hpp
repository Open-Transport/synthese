#ifndef SYNTHESE_SERVER_ALERTPROCESSINGTHREADEXEC_H
#define SYNTHESE_SERVER_ALERTPROCESSINGTHREADEXEC_H

#include "01_util/threads/ThreadExec.h"

#include <vector>
#include <boost/shared_ptr.hpp>
#include "AlertProcessor.hpp"


namespace synthese
{

namespace regulation
{

/** Regulation thread execution body.
    This thread is aimed at executing each alert processor

@ingroup m15
*/
class AlertProcessingThreadExec : public synthese::util::ThreadExec
{
 private:

    boost::mutex _processingMutex;
    std::vector<boost::shared_ptr<AlertProcessor> > _alertProcessors;

public:

    AlertProcessingThreadExec();

    /** Execution body.
     */
    void loop();

 private:


};


}
}



#endif

