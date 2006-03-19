#include "LineStop.h"

#include "SquareDistance.h"
#include "Line.h"
#include "Service.h"
#include "ContinuousService.h"
#include "PhysicalStop.h"



namespace synthese
{
namespace env
{


LineStop::LineStop (const Line* line,
	      int metricOffset,
	      LineStopType type,
	      const PhysicalStop* physicalStop,
	      bool scheduleInput)
    : _line (line)
    , _metricOffset (metricOffset)
    , _type (type)
    , _physicalStop (physicalStop)
    , _scheduleInput (scheduleInput)
{

}






LineStop::~LineStop()
{

}



const LineStop* 
LineStop::getPreviousDeparture () const
{
    return _previousDeparture;
}



const LineStop* 
LineStop::getFollowingArrival () const
{
    return _followingArrival;
}


const LineStop* 
LineStop::getPreviousConnectionDeparture () const
{
    return _previousConnectionDeparture;
}



const LineStop* 
LineStop::getFollowingConnectionArrival () const
{
    return _followingConnectionArrival;
}


const synthese::time::Schedule& 
LineStop::getFirstDepartureSchedule (int serviceNumber) const
{
    return _firstDepartureSchedule[serviceNumber];
}


const synthese::time::Schedule& 
LineStop::getRealFirstDepartureSchedule (int serviceNumber) const
{
    return _realFirstDepartureSchedule[serviceNumber];
}



const synthese::time::Schedule& 
LineStop::getLastDepartureSchedule (int serviceNumber) const
{
    return _lastDepartureSchedule[serviceNumber];
}



const synthese::time::Schedule& 
LineStop::getFirstArrivalSchedule (int serviceNumber) const
{
    return _firstArrivalSchedule[serviceNumber];
}



const synthese::time::Schedule& 
LineStop::getRealFirstArrivalSchedule (int serviceNumber) const
{
    return _realFirstArrivalSchedule[serviceNumber];
}



const synthese::time::Schedule& 
LineStop::getLastArrivalSchedule (int serviceNumber) const
{
    return _lastArrivalSchedule[serviceNumber];
}



bool 
LineStop::getScheduleInput () const
{
    return _scheduleInput;
}




const Line* 
LineStop::getLine () const
{
    return _line;
}



int 
LineStop::getMetricOffset () const
{
    return _metricOffset;
}



const PhysicalStop* 
LineStop::getPhysicalStop () const
{
    return _physicalStop;
}


    
LineStop::LineStopType 
LineStop::getType () const
{
    return _type;
}



void 
LineStop::setType ( const LineStopType& type )
{
    _type = type;
}



void 
LineStop::setPreviousDeparture ( const LineStop* previousDeparture)
{
    _previousDeparture = previousDeparture;
}




void 
LineStop::setPreviousConnectionDeparture( const LineStop* previousConnectionDeparture)
{
    _previousConnectionDeparture = previousConnectionDeparture;
}




void 
LineStop::setFollowingArrival ( const LineStop* followingArrival)
{
    _followingArrival = followingArrival;
}




void 
LineStop::setFollowingConnectionArrival( const LineStop* followingConnectionArrival)
{
    _followingConnectionArrival = followingConnectionArrival;
}



bool 
LineStop::isArrival () const
{
    return ( _type == LINE_STOP_PASSAGE || _type == LINE_STOP_ARRIVAL );
}



bool 
LineStop::isDeparture () const
{
    return ( _type == LINE_STOP_PASSAGE || _type == LINE_STOP_DEPARTURE );
}



bool 
LineStop::isRunning( const synthese::time::DateTime& startMoment, 
		     const synthese::time::DateTime& endMoment ) const
{
    synthese::time::Date startDate;
    for ( startDate = startMoment; startDate <= endMoment; startDate++ )
        if ( _line->isInService ( startDate ) )
            return true;
    return false;
}




void 
LineStop::calculateArrival (const LineStop& departureLineStop, 
			    int serviceNumber,
			    const synthese::time::DateTime& departureMoment, 
			    synthese::time::DateTime& arrivalMoment ) const
{
    if ( _line->getService (serviceNumber)->isContinuous () )
    {
        arrivalMoment = departureMoment;
        arrivalMoment += ( _firstArrivalSchedule[serviceNumber ] - departureLineStop._firstDepartureSchedule[ serviceNumber ] );
    } 
    else
    {
        arrivalMoment = _firstArrivalSchedule[ serviceNumber ];
        arrivalMoment.addDaysDuration( _firstArrivalSchedule[ serviceNumber ].getDaysSinceDeparture () - departureLineStop._firstDepartureSchedule[ serviceNumber ].getDaysSinceDeparture () );
    }
    
}



void 
LineStop::calculateDeparture (const LineStop& arrivalLineStop, 
			      int serviceNumber,
			      const synthese::time::DateTime& arrivalMoment, 
			      synthese::time::DateTime& departureMoment ) const
{
    if ( _line->getService( serviceNumber )->isContinuous() )
    {
        departureMoment = arrivalMoment;
        departureMoment -= ( arrivalLineStop._firstArrivalSchedule[ serviceNumber ] - _firstDepartureSchedule[ serviceNumber ] );
    } 
    else
    {
        departureMoment = _firstDepartureSchedule[ serviceNumber ];
        departureMoment.subDaysDuration( arrivalLineStop._firstArrivalSchedule[ serviceNumber ].getDaysSinceDeparture () - _firstDepartureSchedule[ serviceNumber ].getDaysSinceDeparture () );
    }
}



int 
LineStop::getBestRunTime (const LineStop& other ) const
{
    int curT;
    int bestT;

    for ( int s = 0; s != _line->getServices().size(); s++ )
    {
        curT = other._firstArrivalSchedule[ s ] - _firstDepartureSchedule[ s ];
        if ( curT < 1 )
            curT = 1;
        if ( bestT == 0 || curT < bestT )
            bestT = curT;
    }
    return ( bestT );
}



bool 
LineStop::checkSchedule (const LineStop* lineStopWithPreviousSchedule ) const
{
    // Vertical chronology check
    if ( lineStopWithPreviousSchedule != NULL )
    {
        for ( int s = 0; s < _line->getServices().size(); s++ )
            if ( _firstDepartureSchedule[ s ] < lineStopWithPreviousSchedule->_firstDepartureSchedule[ s ] )
                return false;
    }

    // Horizontal chronology check
    for ( int s = 1; s < _line->getServices().size(); s++ )
        if ( _firstDepartureSchedule[ s ] < _firstDepartureSchedule[ s - 1 ] )
            return false;

    // Check if hours exist
    for ( size_t s = 1; s < _line->getServices().size(); s++ )
        if ( !_firstArrivalSchedule[ s ].isValid () || !_firstDepartureSchedule[ s ].isValid () )
            return false;

    return true;
}




bool 
LineStop::seemsGeographicallyConsistent (const LineStop& other) const
{
    int deltaMO; // meters
    if ( getMetricOffset () > other.getMetricOffset () )
        deltaMO = ( getMetricOffset () - other.getMetricOffset () ) / 1000;
    else
        deltaMO = ( other.getMetricOffset () - getMetricOffset () ) / 1000;

/* MJ compil
    int deltaGPS = SquareDistance ( 
	*( getPhysicalStop ()->getLogicalPlace() ), 
	*( other.getPhysicalStop ()->getLogicalPlace() ) ).getDistance(); // kilometers
    if ( deltaMO > 10 * deltaGPS && deltaMO - deltaGPS > 1 )
    {
        return false;
    }
    if ( deltaMO < deltaGPS && deltaGPS - deltaMO > 1 )
    {
        return false;
    }
	*/

    return true;
    
}




int 
LineStop::getNextService (synthese::time::DateTime& departureMoment, 
			  const synthese::time::DateTime& maxDepartureMoment,
			  const synthese::time::DateTime& calculationMoment,
			  int minNextServiceNumber ) const
{
    int next;

    // Search schedule
    next = _departureIndex[ departureMoment.getHours () ];
    if ( next == UNKNOWN_VALUE )
        next = _line->getServices().size();

    if ( minNextServiceNumber > next )
        next = minNextServiceNumber;

    while ( departureMoment <= maxDepartureMoment )  // boucle sur les dates
    {
        // Look in schedule for when the line is in service
        if ( _line->isInService( departureMoment.getDate() ) )
        {
            while ( next < _line->getServices().size() )  // boucle sur les services
            {
                // Case != continuous service
                if ( _line->getService( next )->isContinuous() && _firstDepartureSchedule[ next ].getDaysSinceDeparture () != _lastDepartureSchedule[ next ].getDaysSinceDeparture () )
                {
                    // if service after departure moment then modification
                    if ( departureMoment > _lastDepartureSchedule[ next ] && departureMoment < _firstDepartureSchedule[ next ] )
                        departureMoment = _firstDepartureSchedule[ next ];

                    if ( departureMoment > maxDepartureMoment )
                        return UNKNOWN_VALUE;

                    // Check for reservation possibility
                    if ( _line->getService( next )->isReservationPossible( departureMoment, calculationMoment ) )
                    {
                        if ( departureMoment < _lastDepartureSchedule[ next ] )
                        {
                            if (_line->getService (next)->isProvided ( departureMoment.getDate(),
								       _lastDepartureSchedule[ next ].getDaysSinceDeparture () ) )
                                return next;
                        }
                        else
                            if ( _line->getService( next )->isProvided( departureMoment.getDate(), 
								      _firstDepartureSchedule[ next ].getDaysSinceDeparture () ) )
                                return next;
                    }
                } 
                else // Normal case
                {
                    // If too early, not convenient
                    if ( departureMoment <= _lastDepartureSchedule[ next ] )
                    {
                        if ( departureMoment < _firstDepartureSchedule[ next ] )
                            departureMoment = _firstDepartureSchedule[ next ];

                        if ( departureMoment > maxDepartureMoment )
                            return UNKNOWN_VALUE;

                        if ( _line->getService( next )->isProvided( departureMoment.getDate(), _firstDepartureSchedule[ next ].getDaysSinceDeparture () ) )
                        {
                            if ( _line->getService( next )->isReservationPossible( departureMoment, calculationMoment ) )
                            {
                                return next;
                            }
                        }
                    }
                }

                next++;

            } //end while

        } //end if

        departureMoment++;
        departureMoment.updateHour ( 0, 0 );

        next = _departureIndex[ 0 ];
    }

    return UNKNOWN_VALUE;
}







int 
LineStop::getNextService ( synthese::time::DateTime& departureMoment, 
			   const synthese::time::DateTime& maxDepartureMoment,
			   int& continuousServiceAmplitude, 
			   int minNextServiceNumber,
			   const synthese::time::DateTime& calculationMoment ) const
{
    int next = getNextService( departureMoment, 
			       maxDepartureMoment, 
			       calculationMoment, 
			       minNextServiceNumber );

    if ( next != UNKNOWN_VALUE && _line->getService ( next )->isContinuous () )
    {
        if ( departureMoment > _lastDepartureSchedule[ next ] )
            continuousServiceAmplitude = 1440 - ( departureMoment.getHour() - _lastDepartureSchedule[ next ].getHour() );
        else
            continuousServiceAmplitude = _lastDepartureSchedule[ next ].getHour() - departureMoment.getHour();
    } 
    else
        continuousServiceAmplitude = 0;

    return next;
}





int 
LineStop::getPreviousService ( synthese::time::DateTime& arrivalMoment, 
			       const synthese::time::DateTime& minArrivalMoment ) const

{
    int previous;

    previous = _arrivalIndex[ arrivalMoment.getHours () ];

    while ( arrivalMoment >= minArrivalMoment )  // Loop over dates
    {
        if ( _line->isInService( arrivalMoment.getDate() ) )
            while ( previous >= 0 )  // Loop over services
            {
                // Case != continuous service
                if ( _line->getService( previous )->isContinuous() && _firstArrivalSchedule[ previous ].getDaysSinceDeparture () != _lastArrivalSchedule[ previous ].getDaysSinceDeparture () )
                {
                    // if service after departure moment then modification
                    if ( arrivalMoment > _lastArrivalSchedule[ previous ] && arrivalMoment < _firstArrivalSchedule[ previous ] )
                        arrivalMoment = _lastArrivalSchedule[ previous ];

                    if ( arrivalMoment < minArrivalMoment )
                        return UNKNOWN_VALUE;

                    if ( arrivalMoment > _firstDepartureSchedule[ previous ] )
                    {
                        if ( _line->getService( previous )->isProvided( arrivalMoment.getDate(), _firstArrivalSchedule[ previous ].getDaysSinceDeparture () ) )
                            return previous;
                    }
                    else
                        if ( _line->getService( previous )->isProvided( arrivalMoment.getDate(), _lastArrivalSchedule[ previous ].getDaysSinceDeparture () ) )
                            return previous;
                }
                else
                {
                    if ( arrivalMoment >= _firstArrivalSchedule[ previous ] )
                    {
                        if ( arrivalMoment > _lastArrivalSchedule[ previous ] )
                            arrivalMoment = _lastArrivalSchedule[ previous ];

                        if ( arrivalMoment < minArrivalMoment )
                            return UNKNOWN_VALUE;

                        if ( _line->getService( previous )->isProvided( arrivalMoment.getDate(), _lastArrivalSchedule[ previous ].getDaysSinceDeparture () ) )
                            return previous;
                    }
                }
                previous--;
            }

        arrivalMoment--;
        arrivalMoment.updateHour ( 23, 59 );
        previous = _arrivalIndex[ 23 ];
    }

    return UNKNOWN_VALUE;
}







int 
LineStop::getPreviousService ( synthese::time::DateTime& arrivalMoment, 
			       const synthese::time::DateTime& minArrivalMoment,
			       int continuousServiceAmplitude ) const
{
    
    int previous = getPreviousService ( arrivalMoment, minArrivalMoment );

    if ( previous != UNKNOWN_VALUE && 
	 _line->getService( previous )->isContinuous() )
    {
        if ( arrivalMoment > _lastArrivalSchedule[ previous ] )
            continuousServiceAmplitude = 1440 - ( arrivalMoment.getHour() - _lastArrivalSchedule[ previous ].getHour () );
        else
            continuousServiceAmplitude = _lastArrivalSchedule[ previous ].getHour() - arrivalMoment.getHour ();
    }
    else
        continuousServiceAmplitude = 0;
    return previous;
}




void 
LineStop::linkWithNextSchedule (const LineStop& previous, 
				const LineStop& following, 
				int position, 
				int number, 
				int serviceNumber)
{
    // Coefficient for interpolation
    float coeffPosition;
    if ( previous._metricOffset == following._metricOffset )
        coeffPosition = ( ( float ) position ) / number;
    else
        coeffPosition = ( ( ( float ) _metricOffset - previous._metricOffset ) ) / ( following._metricOffset - previous._metricOffset );

    float durationToAdd;

    for ( int s = serviceNumber == -1 ? 0 : serviceNumber;
	  serviceNumber != -1 && 
	      s == serviceNumber || serviceNumber == -1 && 
	      s < _line->getServices().size();
	  ++s )
    {
        // Schedule calculation
        durationToAdd = coeffPosition * ( following._firstDepartureSchedule[ s ] - previous._firstDepartureSchedule[ s ] );

        _firstDepartureSchedule[ s ] = previous._firstDepartureSchedule[ s ];
        _firstDepartureSchedule[ s ] += int( ( int ) floor( durationToAdd ) );
        _realFirstDepartureSchedule[ s ] = _firstDepartureSchedule[ s ];
        _lastDepartureSchedule[ s ] = previous._lastDepartureSchedule[ s ];
        _lastDepartureSchedule[ s ] += int( ( int ) floor( durationToAdd ) );

        durationToAdd = coeffPosition * ( following._firstArrivalSchedule[ s ] - previous._firstArrivalSchedule[ s ] );

        _firstArrivalSchedule[ s ] = previous._firstArrivalSchedule[ s ];
        _firstArrivalSchedule[ s ] += int( ( int ) ceil( durationToAdd ) );
        _realFirstArrivalSchedule[ s ] = _firstArrivalSchedule[ s ];
        _lastArrivalSchedule[ s ] = previous._lastArrivalSchedule[ s ];
        _lastArrivalSchedule[ s ] += int( ( int ) ceil( durationToAdd ) );
    }

    updateArrivalIndex();
    updateDepartureIndex();

}





void 
LineStop::setSchedules ( const std::string& buffer, 
			 int position, 
			 int columnWidth,
			 bool departurePassageDifferent )
{
    for ( int s = 0; s < _line->getServices().size(); s++ )
    {
	const Service* service = _line->getService (s);

	int duration = 0;
	int periodicity = 0;
	
	if (service->isContinuous ()) {
	    const ContinuousService* continuousService = 
		dynamic_cast<const ContinuousService*> (service);
	    duration = continuousService->getRange ();
	    periodicity = continuousService->getMaxWaitingTime ();
	}
	

        _firstDepartureSchedule[ s ] = buffer.substr ( position );
        _realFirstDepartureSchedule[ s ] = _firstDepartureSchedule[ s ];
        _lastDepartureSchedule[ s ] = _firstDepartureSchedule[ s ];
	_lastDepartureSchedule[ s ] += duration;


        if ( !departurePassageDifferent )
        {
            _firstArrivalSchedule[ s ] = _firstDepartureSchedule[ s ];
            _firstArrivalSchedule[ s ] += periodicity;
            _realFirstArrivalSchedule[ s ] = _firstArrivalSchedule[ s ];
            _realFirstArrivalSchedule[ s ] += periodicity;
            _lastArrivalSchedule[ s ] = _lastDepartureSchedule[ s ];
            _lastArrivalSchedule[ s ] += periodicity;
        }

        if (_line->getLineStops().front() == this)
	{
	    // MJ constness pb
            ((Service*) service)->setDepartureSchedule ( &_firstDepartureSchedule[ s ] );
	}

        position += columnWidth;
    }

    updateArrivalIndex();
    updateDepartureIndex();


}




void 
LineStop::allocateSchedules ()
{
    _lastArrivalSchedule = new synthese::time::Schedule[ _line->getServices().size() ];
    _firstArrivalSchedule = new synthese::time::Schedule[ _line->getServices().size() ];
    _realFirstArrivalSchedule = new synthese::time::Schedule[ _line->getServices().size() ];
    _lastDepartureSchedule = new synthese::time::Schedule[ _line->getServices().size() ];
    _firstDepartureSchedule = new synthese::time::Schedule[ _line->getServices().size() ];
    _realFirstDepartureSchedule = new synthese::time::Schedule[ _line->getServices().size() ];
}









}
}
