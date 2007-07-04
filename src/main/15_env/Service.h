
/** service class header.
    @file service.h
 
    This file belongs to the SYNTHESE project (public transportation specialized software)
    Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
 
    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_ENV_SERVICE_H
#define SYNTHESE_ENV_SERVICE_H

#include "15_env/Calendar.h"
#include "15_env/Complyer.h"
#include "15_env/ServicePointer.h"

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

/** Service abstract base class.
    TRIDENT = VehicleJourney

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
class Service : public Complyer
{
private:

    int				_serviceNumber;
    Calendar		_calendar;  //!< Which days is this service available ?
    Path*			_path;
//			time::Schedule	_arrivalSchedule;

public:
    //! \name Update methods
    //@{

    //@}

    Service (int serviceNumber,
             Path* path
            );
    Service();
    ~Service ();


    //! @name Getters
    //@{
    const Path*			getPath () const;
    Path*			getPath ();
    int				getServiceNumber () const;
    Calendar&		getCalendar (); // MJ constness pb

    /** Returns the departure schedule for this service.

    @return If this service is continuous, the first departure schedule
    (first course) is returned. Otherwise, it is the "normal" 
    departure schedule.
    */
    virtual const time::Schedule& getDepartureSchedule () const = 0;

    /** Latest schedule of the service : the last arrival at the last vertex.
    @return The latest schedule of the service
    */
    virtual const time::Schedule& getLastArrivalSchedule() const = 0;
    //@}

    //! @name Setters
    //@{
    void setPath(Path* path);
    void setServiceNumber (int serviceNumber);
    //@}



    //! @name Query methods
    //@{
    virtual bool isContinuous () const = 0;

    /** Accessor to the key of the service, provided by the implementation subclass.
    @return id of the service in the database
    */
    virtual uid		getId()	const = 0;


    /** Is this service reservable ?
    @param departureMoment Desired departure moment
    @param calculationMoment Calculation moment taken as reference 
    for reservation delay calculation
    @return false in default implementation.
    */
    virtual bool isReservationPossible ( const synthese::time::DateTime& departureMoment,
                                         const synthese::time::DateTime& calculationMoment ) const;


    /** Is this service providen a given day ?
    @param originDate Departure date of the service from its origin (warning: do not test the customer departure date which can be one or more days later; use getOriginDateTime to compute the origin date)
    @return true if the service runs at the specified date according to its Calendar
    */
    bool isProvided(const time::Date& originDate) const;

    virtual const time::Schedule& getDepartureBeginScheduleToIndex(const Edge* edge) const = 0;
    virtual const time::Schedule& getDepartureEndScheduleToIndex(const Edge* edge) const = 0;
    virtual const time::Schedule& getArrivalBeginScheduleToIndex(const Edge* edge) const = 0;
    virtual const time::Schedule& getArrivalEndScheduleToIndex(const Edge* edge) const = 0;

    /** Generation of the next departure of a service according to a schedule and a presence date time, in the day of the presence time only, according to the compliances.
    @param presenceDateTime
    @param departureTime
    @return A full ServicePointer to the service. If the service cannot be used at the specified date/time, then the ServicePointer points to a NULL service.
    @author Hugues Romain
    @date 2007
    @warning The service index is unknown in the generated ServicePointer.					
    */
    virtual ServicePointer getFromPresenceTime(
        ServicePointer::DeterminationMethod method
        , const Edge* edge
        , const time::DateTime& presenceDateTime
        , const time::DateTime& computingTime
    ) const = 0;

    virtual time::DateTime getLeaveTime(
        const ServicePointer& servicePointer
        , const Edge* edge
    ) const = 0;

    /** Date of the departure from the origin of the service.
    @param departureDate Date of use of the service at the scheduled point
    @param departureTime Known schedule of departure in the service journey
    @return time::DateTime Date of the departure from the origin of the service.
    @author Hugues Romain
    @date 2007

    */
    time::DateTime getOriginDateTime(const time::Date& departureDate, const time::Schedule& departureTime) const;
    //@}
};
}
}

#endif
