#include "Journey.h"
#include "JourneyLeg.h"

#include "04_time/DateTime.h"

#include "15_env/Alarm.h"
#include "15_env/ReservationRuleComplyer.h"
#include "15_env/ReservationRule.h"
#include "15_env/Path.h"
#include "15_env/Service.h"
#include "15_env/ContinuousService.h"
#include "15_env/ConnectionPlace.h"
#include "15_env/Vertex.h"

using synthese::env::Alarm;
using synthese::env::ReservationRuleComplyer;
using synthese::env::ReservationRule;
using synthese::env::Service;
using synthese::env::ContinuousService;
using synthese::env::ConnectionPlace;
using synthese::env::Vertex;
using synthese::env::Path;



namespace synthese
{
namespace routeplanner
{

Journey::Journey ()
{
}
    
Journey::~Journey ()
{
}



int 
Journey::getJourneyLegCount () const
{
    return (int) _journeyLegs.size ();
}




const JourneyLeg* 
Journey::getJourneyLeg (int index) const
{
    return _journeyLegs.at (index);
}




const 
JourneyLeg* Journey::getFirstJourneyLeg () const
{
    return getJourneyLeg (0);
}



const JourneyLeg* 
Journey::getLastJourneyLeg () const
{
    return getJourneyLeg (getJourneyLegCount () - 1);
}



const synthese::env::Vertex* 
Journey::getOrigin() const
{
    return getFirstJourneyLeg ()->getOrigin ();
}



const synthese::env::Vertex* 
Journey::getDestination() const
{
    return getLastJourneyLeg ()->getDestination ();
}



const synthese::time::DateTime& 
Journey::getDepartureTime () const
{
    return getFirstJourneyLeg ()->getDepartureTime ();
}



const synthese::time::DateTime& 
Journey::getArrivalTime () const
{
    return getLastJourneyLeg ()->getArrivalTime ();
}




void 
Journey::clear ()
{
    _journeyLegs.clear ();
}



void 
Journey::prepend (const JourneyLeg* leg)
{
    _journeyLegs.push_front (leg);
}



void 
Journey::append (const JourneyLeg* leg)
{
    _journeyLegs.push_back (leg);
}





int 
Journey::getMaxAlarmLevel () const
{
    synthese::time::DateTime alarmStart, alarmStop, now;
    int maxAlarmLevel = 0;
    
    for (int i=0; i<getJourneyLegCount (); ++i)
    {
	const JourneyLeg* leg = getJourneyLeg (i);
	bool legIsConnection = (i < getJourneyLegCount ()-1);

        // -- Alarm on origin --
        // Alarm start = first departure
        // Alarm stop = last departure
        alarmStart = leg->getDepartureTime ();
        alarmStop = alarmStart;
	if (leg->getService ()->isContinuous ()) 
	    alarmStop += ((const ContinuousService*) leg->getService ())->getRange ();

        if ( leg->getOrigin ()->getConnectionPlace ()->hasApplicableAlarm (alarmStart, alarmStop)
	     && maxAlarmLevel < leg->getOrigin()->getConnectionPlace ()->getAlarm ()->getLevel () )
            maxAlarmLevel = leg->getOrigin()->getConnectionPlace ()->getAlarm ()->getLevel ();
	
	const ReservationRuleComplyer* rr = dynamic_cast<const ReservationRuleComplyer*> (leg->getService ());

        // -- Mandatory reservation --
        now.updateDateTime ();
	if ( (rr != 0) &&
	     (rr->getReservationRule ()->getType () == ReservationRule::RESERVATION_TYPE_COMPULSORY) &&
	     (leg->getService ()->isReservationPossible ( now, leg->getDepartureTime ())) &&
	     (maxAlarmLevel < Alarm::ALARM_LEVEL_WARNING) )
	{
            maxAlarmLevel = Alarm::ALARM_LEVEL_WARNING;
	}

        // -- Possible reservation --
        now.updateDateTime();
	if ( (rr != 0) &&
	     (rr->getReservationRule ()->getType () == ReservationRule::RESERVATION_TYPE_OPTIONAL) &&
	     (leg->getService ()->isReservationPossible ( now, leg->getDepartureTime ())) &&
	     (maxAlarmLevel < Alarm::ALARM_LEVEL_INFO) )
	{
            maxAlarmLevel = Alarm::ALARM_LEVEL_INFO;
	}


        // -- Service alarme --
        // Alarm start = first departure
        // Alarm stop = last arrival
        alarmStart = leg->getDepartureTime ();
        alarmStop = leg->getArrivalTime ();
	if (leg->getService ()->isContinuous ()) 
	    alarmStop += ((const ContinuousService*) leg->getService ())->getRange ();

        if ( (leg->getService ()->getPath ()->hasApplicableAlarm (alarmStart, alarmStop)) &&
	     (maxAlarmLevel < leg->getService ()->getPath ()->getAlarm ()->getLevel ()) )
	{
            maxAlarmLevel = leg->getService ()->getPath ()->getAlarm ()->getLevel ();
	}
	
        // -- Alarm on arrival --
        // Alarm start = first arrival
        // Alarm stop = last arrival if connection, last arrival otherwise
        alarmStart = leg->getArrivalTime ();
        alarmStop = alarmStart;
        if (legIsConnection)
	{
            alarmStop = getJourneyLeg (i+1)->getDepartureTime ();
	}

	if (leg->getService ()->isContinuous ()) 
	    alarmStop += ((const ContinuousService*) leg->getService ())->getRange ();

        if ( (leg->getDestination ()->getConnectionPlace ()->hasApplicableAlarm (alarmStart, alarmStop)) &&
             (maxAlarmLevel < leg->getDestination()->getConnectionPlace ()->getAlarm ()->getLevel ()) )
	{
	    maxAlarmLevel = leg->getDestination()->getConnectionPlace ()->getAlarm ()->getLevel ();
	}
    }

    return maxAlarmLevel;
}



int
Journey::getDuration () const
{
    if (getDepartureTime ().getHour ().isUnknown () ||
	getArrivalTime ().getHour ().isUnknown ()) return synthese::time::UNKNOWN_VALUE;
    
    return getArrivalTime () - getDepartureTime ();
}



int 
Journey::getContinuousServiceRange () const
{
    int continuousServiceRange = synthese::time::UNKNOWN_VALUE;
    for (std::deque<const JourneyLeg*>::const_iterator it =  _journeyLegs.begin ();
	 it != _journeyLegs.end (); ++it)
    {
	const JourneyLeg* leg = *it;
	if ( (continuousServiceRange == synthese::time::UNKNOWN_VALUE) ||
	     (leg->getContinuousServiceRange () < continuousServiceRange) )
	{
	    continuousServiceRange = leg->getContinuousServiceRange ();
	}
	if (continuousServiceRange == 0) break;
    }
    return continuousServiceRange;
}



}
}
