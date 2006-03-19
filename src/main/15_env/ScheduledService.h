#ifndef SYNTHESE_ENV_SCHEDULEDSERVICE_H
#define SYNTHESE_ENV_SCHEDULEDSERVICE_H

#include "Service.h"
#include <string>



namespace synthese
{
namespace env
{



/** Scheduled service.


 @ingroup m15
*/
class ScheduledService : public Service
{
private:

public:

    ScheduledService (const std::string& serviceNumber,
		      const Path* path,
		      Calendar* calendar,
		      const synthese::time::Schedule* departureSchedule);

    ~ScheduledService ();

    
    //! @name Getters/Setters
    //@{
    //@}

    //! @name Query methods
    //@{
    bool isContinuous () const;
    //@}

};



}
}


#endif
