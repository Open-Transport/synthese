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


LineStop::LineStop (int id,
			const Line* line,
		    double metricOffset,
		    const EdgeType& type,
		    const PhysicalStop* physicalStop,
		    bool scheduleInput)
    : synthese::util::Registrable<int,LineStop> (id)
    , Edge (type)
    , _line (line)
    , _metricOffset (metricOffset)
    , _physicalStop (physicalStop)
    , _scheduleInput (scheduleInput)
{

}






LineStop::~LineStop()
{

}




const synthese::time::Schedule& 
LineStop::getFirstDepartureSchedule (int serviceNumber) const
{
    return _firstDepartureSchedule[serviceNumber];
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
LineStop::getLastArrivalSchedule (int serviceNumber) const
{
    return _lastArrivalSchedule[serviceNumber];
}



bool 
LineStop::getScheduleInput () const
{
    return _scheduleInput;
}



const Path* 
LineStop::getParentPath () const
{
    return _line;
}




const Vertex* 
LineStop::getFromVertex () const
{
    return _physicalStop;
}





double
LineStop::getMetricOffset () const
{
    return _metricOffset;
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
    double deltaMO; // meters
    if ( getMetricOffset () > other.getMetricOffset () )
        deltaMO = ( getMetricOffset () - other.getMetricOffset () ) / 1000;
    else
        deltaMO = ( other.getMetricOffset () - getMetricOffset () ) / 1000;

    int deltaGPS = SquareDistance ( 
	*getFromVertex (), 
	*other.getFromVertex () ).getDistance(); // kilometers

    if ( deltaMO > 10 * deltaGPS && deltaMO - deltaGPS > 1 )
    {
        return false;
    }
    if ( deltaMO < deltaGPS && deltaGPS - deltaMO > 1 )
    {
        return false;
    }

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
        _lastDepartureSchedule[ s ] = previous._lastDepartureSchedule[ s ];
        _lastDepartureSchedule[ s ] += int( ( int ) floor( durationToAdd ) );

        durationToAdd = coeffPosition * ( following._firstArrivalSchedule[ s ] - previous._firstArrivalSchedule[ s ] );

        _firstArrivalSchedule[ s ] = previous._firstArrivalSchedule[ s ];
        _firstArrivalSchedule[ s ] += int( ( int ) ceil( durationToAdd ) );
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
        _lastDepartureSchedule[ s ] = _firstDepartureSchedule[ s ];
	_lastDepartureSchedule[ s ] += duration;


        if ( !departurePassageDifferent )
        {
            _firstArrivalSchedule[ s ] = _firstDepartureSchedule[ s ];
            _firstArrivalSchedule[ s ] += periodicity;
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
    _lastDepartureSchedule = new synthese::time::Schedule[ _line->getServices().size() ];
    _firstDepartureSchedule = new synthese::time::Schedule[ _line->getServices().size() ];
}





/*! \brief Ecriture des index de d�part de la gare ligne, � partir des horaires des arr�ts.
 \version 2.1
 \author Hugues Romain
 \date 2001-2004
 
Ecrit tous les index d'un coup (contrairement � la version 1)
*/
void 
LineStop::updateDepartureIndex ()
{
/* MJ TO BE MIGRATED !!!
    int iNumeroHeure;

    int DerniereHeure = 25; // MODIF HR
    size_t NumeroServicePassantMinuit = 0; // MODIF HR

    // RAZ
    for ( iNumeroHeure = 0; iNumeroHeure < synthese::time::HOURS_PER_DAY; iNumeroHeure++ )
        vIndexDepart[ iNumeroHeure ] = -1;

    // Ecriture service par service
    for ( size_t iNumeroService = 0; iNumeroService < vLigne->getServices().size(); iNumeroService++ )
    {
        if ( vHoraireDepartDernier[ iNumeroService ].getHours () < DerniereHeure )
            NumeroServicePassantMinuit = iNumeroService;
        if ( vHoraireDepartDernier[ iNumeroService ].getHours () >= vHoraireDepartPremier[ iNumeroService ].getHours () )
        {
            for ( iNumeroHeure = 0; iNumeroHeure <= vHoraireDepartDernier[ iNumeroService ].getHours (); iNumeroHeure++ )
                if ( vIndexDepart[ iNumeroHeure ] == -1 || vIndexDepart[ iNumeroHeure ] < NumeroServicePassantMinuit )
                    vIndexDepart[ iNumeroHeure ] = iNumeroService;
        }
        else
        {
            for ( iNumeroHeure = 0; iNumeroHeure < synthese::time::HOURS_PER_DAY; iNumeroHeure++ )
                if ( vIndexDepart[ iNumeroHeure ] == -1 )
                    vIndexDepart[ iNumeroHeure ] = iNumeroService;
        }
        DerniereHeure = vHoraireDepartDernier[ iNumeroService ].getHours ();

    }

    // Ecriture du temps r�el
    for ( iNumeroHeure = 0; iNumeroHeure < 24; iNumeroHeure++ )
        vIndexDepartReel[ iNumeroHeure ] = vIndexDepart[ iNumeroHeure ];

*/
}





void 
LineStop::updateArrivalIndex ()
{
/* MJ TO BE MIRGATED !!!
    int iNumeroHeure;

    // RAZ
    for ( iNumeroHeure = 0; iNumeroHeure < synthese::time::HOURS_PER_DAY; iNumeroHeure++ )
        vIndexArrivee[ iNumeroHeure ] = -1;

    int DerniereHeure = 25;
    size_t NumeroServicePassantMinuit = vLigne->getServices().size();

    for ( size_t iNumeroService = vLigne->getServices().size() - 1; iNumeroService >= 0; iNumeroService-- )
    {
        if ( vHoraireArriveePremier[ iNumeroService ].getHours () > DerniereHeure )
            NumeroServicePassantMinuit = iNumeroService;
        if ( vHoraireArriveeDernier[ iNumeroService ].getHours () >= vHoraireArriveePremier[ iNumeroService ].getHours () )
        {
            for ( iNumeroHeure = vHoraireArriveePremier[ iNumeroService ].getHours (); iNumeroHeure < synthese::time::HOURS_PER_DAY; iNumeroHeure++ )
                if ( vIndexArrivee[ iNumeroHeure ] == -1 || vIndexArrivee[ iNumeroHeure ] > NumeroServicePassantMinuit )
                    vIndexArrivee[ iNumeroHeure ] = iNumeroService;
        }
        else
        {
            for ( iNumeroHeure = 0; iNumeroHeure < synthese::time::HOURS_PER_DAY; iNumeroHeure++ )
                if ( vIndexArrivee[ iNumeroHeure ] == -1 )
                    vIndexArrivee[ iNumeroHeure ] = iNumeroService;
        }
        DerniereHeure = vHoraireArriveePremier[ iNumeroService ].getHours ();
    }

    // Ecriture du temps r�el
    for ( iNumeroHeure = 0; iNumeroHeure < synthese::time::HOURS_PER_DAY; iNumeroHeure++ )
        vIndexArriveeReel[ iNumeroHeure ] = vIndexArrivee[ iNumeroHeure ];
*/
}






}
}
