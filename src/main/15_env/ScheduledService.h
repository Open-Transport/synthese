#ifndef SYNTHESE_ENV_SCHEDULEDSERVICE_H
#define SYNTHESE_ENV_SCHEDULEDSERVICE_H

#include "Service.h"
#include <string>

#include "01_util/Registrable.h"
#include "ReservationRuleComplyer.h"


namespace synthese
{
namespace env
{



/** Scheduled service.


 @ingroup m15
*/
class ScheduledService : 
    public synthese::util::Registrable<uid,ScheduledService>, 
    public ReservationRuleComplyer,
    public Service
{
private:


public:

    ScheduledService (const uid& id,
		      int serviceNumber,
		      const Path* path,
		      const synthese::time::Schedule& departureSchedule);

    ~ScheduledService ();

    
    //! @name Getters/Setters
    //@{
    //@}

    //! @name Query methods
    //@{
    bool isContinuous () const;

    /** Is this service reservable ?
	@param departureMoment Desired departure moment
	@param calculationMoment Calculation moment taken as reference 
	for reservation delay calculation
	@return true if service can be reserved, false otherwise.
	
	A service can be reserved if :
	- the path does not have any reservation rule
	- the reservation rule accepts condition
    */
    bool isReservationPossible ( const synthese::time::DateTime& departureMoment, 
				 const synthese::time::DateTime& calculationMoment ) const;
    //@}

};



}
}


#endif

