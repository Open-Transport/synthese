#include "ReservationRule.h"
#include "Service.h"

#include "04_time/Schedule.h"



namespace synthese
{
namespace env
{


ReservationRule::ReservationRule ( const uid& id,
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
				   const std::string& webSiteUrl )
    : synthese::util::Registrable<uid,ReservationRule> (id)
    , _type (type)
    , _online (online)
    , _originIsReference (originIsReference)
    , _minDelayMinutes (minDelayMinutes)
    , _minDelayDays (minDelayDays)
    , _maxDelayDays (maxDelayDays)
    , _hourDeadLine (hourDeadLine)
    , _phoneExchangeNumber (phoneExchangeNumber)
    , _phoneExchangeOpeningHours (phoneExchangeOpeningHours)
    , _description (description)
    , _webSiteUrl (webSiteUrl)
{

}


ReservationRule::~ReservationRule()
{
}




const ReservationRule::ReservationType& 
ReservationRule::getType () const
{
    return _type;
}



void
ReservationRule::setType (const ReservationRule::ReservationType& type)
{
    _type = type;
}



const synthese::time::Hour& 
ReservationRule::getHourDeadLine () const
{
    return _hourDeadLine;
}



void
ReservationRule::setHourDeadLine (const synthese::time::Hour& hourDeadLine)
{
    _hourDeadLine = hourDeadLine;
}




const std::string& 
ReservationRule::getPhoneExchangeOpeningHours () const
{
    return _phoneExchangeOpeningHours;
}



void
ReservationRule::setPhoneExchangeOpeningHours (const std::string& phoneExchangeOpeningHours)
{
    _phoneExchangeOpeningHours = phoneExchangeOpeningHours;
}




const std::string& 
ReservationRule::getWebSiteUrl () const
{
    return _webSiteUrl;
}


void
ReservationRule::setWebSiteUrl (const std::string& webSiteUrl)
{
    _webSiteUrl = webSiteUrl;
}




void
ReservationRule::setPhoneExchangeNumber (const std::string& phoneExchangeNumber)
{
    _phoneExchangeNumber = phoneExchangeNumber;
}




const std::string& 
ReservationRule::getPhoneExchangeNumber () const
{
    return _phoneExchangeNumber;
}




synthese::time::DateTime 
ReservationRule::getReservationDeadLine (const Service* service, 
					 const synthese::time::DateTime& departureTime) const
{
    synthese::time::DateTime referenceTime = departureTime;

    if ( _originIsReference )
    {
        // Departure hour at origin is superior to departure hour => day before
        if ( service->getDepartureSchedule () > departureTime.getHour () )
            referenceTime--;
        referenceTime = service->getDepartureSchedule ();
    }

    synthese::time::DateTime minutesMoment = referenceTime;
    synthese::time::DateTime daysMoment = referenceTime;

    if ( _minDelayMinutes ) minutesMoment -= _minDelayMinutes;

    if ( _minDelayDays )
    {
        daysMoment.subDaysDuration( _minDelayDays );
        daysMoment.updateHour( synthese::time::TIME_MAX );
    }

    if ( _hourDeadLine < daysMoment.getHour () )
        daysMoment.setHour( _hourDeadLine );

    if ( minutesMoment < daysMoment )
        return minutesMoment;
    else
        return daysMoment;
    
}



synthese::time::DateTime 
ReservationRule::getReservationStartTime (const synthese::time::DateTime& reservationTime) const
{
    synthese::time::DateTime reservationStartTime = reservationTime;
    
    if ( _maxDelayDays )
    {
        reservationStartTime.subDaysDuration( _maxDelayDays );
        reservationStartTime.updateHour( synthese::time::TIME_MIN );
    }

    return reservationStartTime;
}





bool 
ReservationRule::isRunPossible ( const Service* service, 
				 const synthese::time::DateTime& reservationTime, 
				 const synthese::time::DateTime& departureTime ) const
{
    return _type == RESERVATION_TYPE_IMPOSSIBLE
	|| _type == RESERVATION_TYPE_OPTIONNAL
	|| isReservationPossible ( service, reservationTime, departureTime );

}




bool 
ReservationRule::isReservationPossible ( const Service* service, 
					 const synthese::time::DateTime& reservationTime, 
					 const synthese::time::DateTime& departureTime ) const
{
    return reservationTime <= getReservationDeadLine( service, departureTime )
	&& reservationTime >= getReservationStartTime (reservationTime);
}




const std::string& 
ReservationRule::getDescription () const
{
    return _description;
}




void 
ReservationRule::setDescription (const std::string& description)
{
    _description = description;
}



void 
ReservationRule::setMinDelayMinutes (int minDelayMinutes)
{
    _minDelayMinutes = minDelayMinutes;
}



void ReservationRule::setMinDelayDays (int minDelayDays)
{
    _minDelayDays = minDelayDays;
}





void ReservationRule::setMaxDelayDays (int maxDelayDays)
{
    _maxDelayDays = maxDelayDays;
}




void 
ReservationRule::setOnline (bool online)
{
    _online = online;
}
 


void 
ReservationRule::setOriginIsReference (bool originIsReference)
{
    _originIsReference = originIsReference;
}










}
}
