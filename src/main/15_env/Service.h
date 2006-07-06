#ifndef SYNTHESE_ENV_SERVICE_H
#define SYNTHESE_ENV_SERVICE_H

#include "Regulated.h"

#include <string>

#include "01_util/UId.h"

#include "04_time/Schedule.h"




namespace synthese
{

namespace time
{
    class Date;
    class DateTime;
}
 
namespace env
{

    class Path;
    class Calendar;


/** Service abstract base class.

A service represents the ability to follow a path
at a certain moment.

It is completely independent from the "vehicle" : this ability 
can be provided by an external entity (bus, train...), 
but also self-provided by the traveller himself 
(walking, cycling...).

 @ingroup m15
*/
class Service : 
     public Regulated
{
private:
    
    const std::string _serviceNumber;
    const Path* _path; //!< Path this service allows to follow.
    
    Calendar* _calendar;  //!< Which days is this service available ?

    synthese::time::Schedule _departureSchedule; //!< Service departure schedule.
    
public:

    Service (const std::string& serviceNumber,
	     const Path* path,
	     Calendar* calendar,
	     const synthese::time::Schedule& departureSchedule);
    ~Service ();

    
    //! @name Getters/Setters
    //@{
    const std::string& getServiceNumber () const;
    const Path* getPath () const;
    Calendar* getCalendar (); // MJ constness pb

    /** Returns the departure schedule for this service.

	@return If this service is continuous, the first departure schedule
	(first course) is returned. Otherwise, it is the "normal" 
	departure schedule.
    */
    const synthese::time::Schedule& getDepartureSchedule () const;
    void setDepartureSchedule (const synthese::time::Schedule& departureSchedule);
    //@}


    //! @name Query methods
    //@{
    virtual bool isContinuous () const = 0;


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


    /** Is this service providen a given day ?
	@param departureDate Departure date of client
	@param jplus Number of days between client departure date 
	and departure date of the path service origin.
    */
    bool isProvided ( const synthese::time::Date& departureDate,
		      int jplus ) const;

    //@}

};





}
}


#endif
