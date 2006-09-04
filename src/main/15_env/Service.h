#ifndef SYNTHESE_ENV_SERVICE_H
#define SYNTHESE_ENV_SERVICE_H

#include "BikeComplyer.h"
#include "HandicappedComplyer.h"
#include "PedestrianComplyer.h"

#include <string>

#include "01_util/UId.h"

#include "04_time/Schedule.h"
#include "15_env/Calendar.h"




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
at certain days and hours.

The days when the service is provided are stored in a Calendar object.
Even if a Service intrinsically corresponds to a sequence of 
(arrival schedule - departure schedule) couples, these schedules are not
stored in Service objects but per Edge (Line/Road). However, this is how 
Service objects are persisted.

It is completely independent from the "vehicle" : this ability 
can be provided by an external entity (bus, train...), 
but also self-provided by the traveller himself 
(walking, cycling...).

 @ingroup m15
*/
class Service : 
    public BikeComplyer,
    public HandicappedComplyer,
    public PedestrianComplyer
{
private:
    
    int _serviceNumber;
    Calendar _calendar;  //!< Which days is this service available ?
    const Path* _path;

    synthese::time::Schedule _departureSchedule; //!< Service departure schedule (from the origin).
    
public:

    Service (int serviceNumber,
	     const Path* path,
	     const synthese::time::Schedule& departureSchedule);
    ~Service ();

    
    //! @name Getters/Setters
    //@{
    const Path* getPath () const;

    int getServiceNumber () const;
    void setServiceNumber (int serviceNumber);

    Calendar& getCalendar (); // MJ constness pb


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
	@return false in default implementation.
    */
    virtual bool isReservationPossible ( const synthese::time::DateTime& departureMoment, 
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
