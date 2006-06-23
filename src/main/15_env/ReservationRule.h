#ifndef SYNTHESE_ENV_RESERVATIONRULE_H
#define SYNTHESE_ENV_RESERVATIONRULE_H



#include "01_util/Registrable.h"


#include "04_time/Hour.h"
#include "04_time/DateTime.h"


namespace synthese
{
namespace env
{

class Service;



/** Reservation rule.
 @ingroup m15
*/
class ReservationRule : public synthese::util::Registrable<int,ReservationRule>
{
public:

    /** Type of reservation rule */
    typedef enum
    {
	RESERVATION_TYPE_IMPOSSIBLE = 'N', //!< Reservation is not possible, services should be taken without any announcement
        RESERVATION_TYPE_OPTIONNAL = 'F',     //!< Reservation is possible but not compulsory, services should be taken with announcement for much security
	RESERVATION_TYPE_COMPULSORY = 'O',     //!< Reservation is compulsory, passengers without reservation are not accepted
	RESERVATION_TYPE_AT_LEAST_ONE_REQUIRED = 'C' //!< At least on reservation is compulsory, the service do not go if no reservations are done, but will go if at least one is done, and will bi opened for passengers without reservation
    } ReservationType;


private:

    ReservationType _type; //!< Reservation type
    bool _online;  //!< Reservation via SYNTHESE active

    bool _originIsReference; //!< Whether reference departure time is the line run departure time at its origin (true) or client departure time (false)

    int _minDelayMinutes;  //!< Minimum delay in minutes between reservation and reference moment
    int _minDelayDays;   //!< Minimum delay in days between reservation and reference moment
    int _maxDelayDays;  //!< Maxium number of days between reservation and departure.

    synthese::time::Hour _hourDeadLine; //!< Latest reservation hour the last day open for reservation

    std::string _phoneExchangeNumber;  //!< Phone number for reservation
    std::string _phoneExchangeOpeningHours;  //!< Opening hours for phone exchange
    std::string _description;   //!< Additional info about phone exchange or reservation mode
    std::string _webSiteUrl;    //!< URL of a website allowing online reservation

public:

    ReservationRule ( const int& id,
		      const ReservationType& type,
		      bool online,
		      bool originIsReference,
		      int minDelayMinutes,
		      int minDelayDays,
		      int maxDelayDays,
		      synthese::time::Hour hourDeadLine,
		      const std::string& phoneExchangeNumber,
		      const std::string& phoneExchangeOpeningHours,
		      const std::string& description,
		      const std::string& webSiteUrl );


    virtual ~ReservationRule();

    //! @name Getters/Setters
    //@{
    const ReservationType& getType () const;
    const synthese::time::Hour& getHourDeadLine () const;

    const std::string& getPhoneExchangeOpeningHours () const;
    const std::string& getWebSiteUrl () const;

    const std::string& getPhoneExchangeNumber () const;
    //@}


    //! @name Query methods
    //@{


    /** Reference function for reservation dead line calculation.
 
    It is done according to the following steps:
    - Choice of reference time (client departure or line run departure at origin)
    - Calculation 1 : x minutes before reference time :
    - Decrease of _minDelayMinutes before reference

    - Calculation 2 : x days before reference time :
    - Decrease of _minDelayDays before reference
    - Sets hour to _hourDeadLine

    - The smallest date time is chosen.

    If no explicit rule defines the reservation dead line, 
    the actual reservation time is returned.
    */
    synthese::time::DateTime getReservationDeadLine (
	const Service*, 
	const synthese::time::DateTime& departureTime ) const;



    /** Reference function for calculation of start reservation date time.
	@param reservationTime Time when booking is done.
	@return The minimum date time to make a reservation.
	
	If no explicit rule defines this minimum time, the actual reservation time is returned.
    */
    synthese::time::DateTime getReservationStartTime ( 
	const synthese::time::DateTime& reservationTime) const;


    
    /** Indicates whether or not a path can be taken at a given date, 
	taking into account reservation delay rules.
	@param run Line run to test.
	@param reservationTime Time of booking, if required.
	@param departureTime Desired departure time.
	@return true if the line run can be taken, false otherwise.

	This methods checks the following conditions :
	- if reservation is not compulsory, the run can be taken.
	- if reservation is compulsory, reservation time must precede reservation 
	dead line and be after reservation opening time.
    */
    bool isRunPossible ( const Service* run, 
			 const synthese::time::DateTime& reservationTime, 
			 const synthese::time::DateTime& departureTime ) const;
    


    /** Indicates whether or not a reservation is possible for a given line run,
	at a certain date, taking into account delay rules.
	@param run Line run asked for booking.
	@param reservationTime Time of booking.
	@param departureTime Desired departure time.
	@return true if the reservation is possible, false otherwise.
 
	This methods checks the following conditions :
	- reservation time must precede reservation dead line
	- reservation time must be later than reservation start time.
    */
    bool isReservationPossible ( const Service* run, 
				 const synthese::time::DateTime& reservationTime, 
				 const synthese::time::DateTime& departureTime ) const;

    //@}

};




}
}

#endif
