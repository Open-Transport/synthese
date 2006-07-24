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


LineStop::LineStop (const uid& id,
		    const Line* line,
		    int rankInPath,
		    double metricOffset,
		    const PhysicalStop* physicalStop)
    : synthese::util::Registrable<uid,LineStop> (id)
    , Edge (EDGE_TYPE_PASSAGE, line, rankInPath)
    , _metricOffset (metricOffset)
    , _physicalStop (physicalStop)
{

}






LineStop::~LineStop()
{

}




const synthese::time::Schedule& 
LineStop::getDepartureBeginSchedule (int serviceNumber) const
{
    return _departureBeginSchedule[serviceNumber];
}




const synthese::time::Schedule& 
LineStop::getDepartureEndSchedule (int serviceNumber) const
{
    return _departureEndSchedule[serviceNumber];
}



const synthese::time::Schedule& 
LineStop::getArrivalBeginSchedule (int serviceNumber) const
{
    return _arrivalBeginSchedule[serviceNumber];
}





const synthese::time::Schedule& 
LineStop::getArrivalEndSchedule (int serviceNumber) const
{
    return _arrivalEndSchedule[serviceNumber];
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
        if ( getParentPath ()->isInService ( startDate ) )
            return true;
    return false;
}





void 
LineStop::calculateArrival (const LineStop& departureLineStop, 
			    int serviceNumber,
			    const synthese::time::DateTime& departureMoment, 
			    synthese::time::DateTime& arrivalMoment ) const
{
    if ( getParentPath ()->getService (serviceNumber)->isContinuous () )
    {
        arrivalMoment = departureMoment;
        arrivalMoment += ( _arrivalBeginSchedule[serviceNumber ] - departureLineStop._departureBeginSchedule[ serviceNumber ] );
    } 
    else
    {
        arrivalMoment = _arrivalBeginSchedule[ serviceNumber ];
        arrivalMoment.addDaysDuration( _arrivalBeginSchedule[ serviceNumber ].getDaysSinceDeparture () - departureLineStop._departureBeginSchedule[ serviceNumber ].getDaysSinceDeparture () );
    }
    
}



void 
LineStop::calculateDeparture (const LineStop& arrivalLineStop, 
			      int serviceNumber,
			      const synthese::time::DateTime& arrivalMoment, 
			      synthese::time::DateTime& departureMoment ) const
{
    if ( getParentPath ()->getService( serviceNumber )->isContinuous() )
    {
        departureMoment = arrivalMoment;
        departureMoment -= ( arrivalLineStop._arrivalBeginSchedule[ serviceNumber ] - _departureBeginSchedule[ serviceNumber ] );
    } 
    else
    {
        departureMoment = _departureBeginSchedule[ serviceNumber ];
        departureMoment.subDaysDuration( arrivalLineStop._arrivalBeginSchedule[ serviceNumber ].getDaysSinceDeparture () - _departureBeginSchedule[ serviceNumber ].getDaysSinceDeparture () );
    }
}



int 
LineStop::getBestRunTime (const LineStop& other ) const
{
    int curT;
    int bestT;

    for ( int s = 0; s != getParentPath ()->getServices().size(); s++ )
    {
        curT = other._arrivalBeginSchedule[ s ] - _departureBeginSchedule[ s ];
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
        for ( int s = 0; s < getParentPath ()->getServices().size(); s++ )
            if ( _departureBeginSchedule[ s ] < lineStopWithPreviousSchedule->_departureBeginSchedule[ s ] )
                return false;
    }

    // Horizontal chronology check
    for ( int s = 1; s < getParentPath ()->getServices().size(); s++ )
        if ( _departureBeginSchedule[ s ] < _departureBeginSchedule[ s - 1 ] )
            return false;

    // Check if hours exist
    for ( size_t s = 1; s < getParentPath ()->getServices().size(); s++ )
        if ( !_arrivalBeginSchedule[ s ].isValid () || !_departureBeginSchedule[ s ].isValid () )
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
        next = getParentPath ()->getServices().size();

    if ( minNextServiceNumber > next )
        next = minNextServiceNumber;

    while ( departureMoment <= maxDepartureMoment )  // boucle sur les dates
    {
        // Look in schedule for when the line is in service
        if ( getParentPath ()->isInService( departureMoment.getDate() ) )
        {
            while ( next < getParentPath ()->getServices().size() )  // boucle sur les services
            {
                // Case != continuous service
                if ( getParentPath ()->getService( next )->isContinuous() && _departureBeginSchedule[ next ].getDaysSinceDeparture () != _departureEndSchedule[ next ].getDaysSinceDeparture () )
                {
                    // if service after departure moment then modification
                    if ( departureMoment > _departureEndSchedule[ next ] && departureMoment < _departureBeginSchedule[ next ] )
                        departureMoment = _departureBeginSchedule[ next ];

                    if ( departureMoment > maxDepartureMoment )
                        return UNKNOWN_VALUE;

                    // Check for reservation possibility
                    if ( getParentPath ()->getService( next )->isReservationPossible( departureMoment, calculationMoment ) )
                    {
                        if ( departureMoment < _departureEndSchedule[ next ] )
                        {
                            if (getParentPath ()->getService (next)->isProvided ( departureMoment.getDate(),
								       _departureEndSchedule[ next ].getDaysSinceDeparture () ) )
                                return next;
                        }
                        else
                            if ( getParentPath ()->getService( next )->isProvided( departureMoment.getDate(), 
								      _departureBeginSchedule[ next ].getDaysSinceDeparture () ) )
                                return next;
                    }
                } 
                else // Normal case
                {
                    // If too early, not convenient
                    if ( departureMoment <= _departureEndSchedule[ next ] )
                    {
                        if ( departureMoment < _departureBeginSchedule[ next ] )
                            departureMoment = _departureBeginSchedule[ next ];

                        if ( departureMoment > maxDepartureMoment )
                            return UNKNOWN_VALUE;

                        if ( getParentPath ()->getService( next )->isProvided( departureMoment.getDate(), _departureBeginSchedule[ next ].getDaysSinceDeparture () ) )
                        {
                            if ( getParentPath ()->getService( next )->isReservationPossible( departureMoment, calculationMoment ) )
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

    if ( next != UNKNOWN_VALUE && getParentPath ()->getService ( next )->isContinuous () )
    {
        if ( departureMoment > _departureEndSchedule[ next ] )
            continuousServiceAmplitude = 1440 - ( departureMoment.getHour() - _departureEndSchedule[ next ].getHour() );
        else
            continuousServiceAmplitude = _departureEndSchedule[ next ].getHour() - departureMoment.getHour();
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
        if ( getParentPath ()->isInService( arrivalMoment.getDate() ) )
            while ( previous >= 0 )  // Loop over services
            {
                // Case != continuous service
                if ( getParentPath ()->getService( previous )->isContinuous() && _arrivalBeginSchedule[ previous ].getDaysSinceDeparture () != _arrivalEndSchedule[ previous ].getDaysSinceDeparture () )
                {
                    // if service after departure moment then modification
                    if ( arrivalMoment > _arrivalEndSchedule[ previous ] && arrivalMoment < _arrivalBeginSchedule[ previous ] )
                        arrivalMoment = _arrivalEndSchedule[ previous ];

                    if ( arrivalMoment < minArrivalMoment )
                        return UNKNOWN_VALUE;

                    if ( arrivalMoment > _departureBeginSchedule[ previous ] )
                    {
                        if ( getParentPath ()->getService( previous )->isProvided( arrivalMoment.getDate(), _arrivalBeginSchedule[ previous ].getDaysSinceDeparture () ) )
                            return previous;
                    }
                    else
                        if ( getParentPath ()->getService( previous )->isProvided( arrivalMoment.getDate(), _arrivalEndSchedule[ previous ].getDaysSinceDeparture () ) )
                            return previous;
                }
                else
                {
                    if ( arrivalMoment >= _arrivalBeginSchedule[ previous ] )
                    {
                        if ( arrivalMoment > _arrivalEndSchedule[ previous ] )
                            arrivalMoment = _arrivalEndSchedule[ previous ];

                        if ( arrivalMoment < minArrivalMoment )
                            return UNKNOWN_VALUE;

                        if ( getParentPath ()->getService( previous )->isProvided( arrivalMoment.getDate(), _arrivalEndSchedule[ previous ].getDaysSinceDeparture () ) )
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
	 getParentPath ()->getService( previous )->isContinuous() )
    {
        if ( arrivalMoment > _arrivalEndSchedule[ previous ] )
            continuousServiceAmplitude = 1440 - ( arrivalMoment.getHour() - _arrivalEndSchedule[ previous ].getHour () );
        else
            continuousServiceAmplitude = _arrivalEndSchedule[ previous ].getHour() - arrivalMoment.getHour ();
    }
    else
        continuousServiceAmplitude = 0;
    return previous;
}








void 
LineStop::setSchedules ( const std::string& buffer, 
			 int position, 
			 int columnWidth,
			 bool departurePassageDifferent )
{
    for ( int s = 0; s < getParentPath ()->getServices().size(); s++ )
    {
	const Service* service = getParentPath ()->getService (s);

	int duration = 0;
	int periodicity = 0;
	
	if (service->isContinuous ()) {
	    const ContinuousService* continuousService = 
		dynamic_cast<const ContinuousService*> (service);
	    duration = continuousService->getRange ();
	    periodicity = continuousService->getMaxWaitingTime ();
	}
	

        _departureBeginSchedule[ s ] = buffer.substr ( position );
        _departureEndSchedule[ s ] = _departureBeginSchedule[ s ];
	_departureEndSchedule[ s ] += duration;


        if ( !departurePassageDifferent )
        {
            _arrivalBeginSchedule[ s ] = _departureBeginSchedule[ s ];
            _arrivalBeginSchedule[ s ] += periodicity;
            _arrivalEndSchedule[ s ] = _departureEndSchedule[ s ];
            _arrivalEndSchedule[ s ] += periodicity;
        }

        // if (getParentPath ()->getLineStops().front() == this)
        if (getRankInPath () == 0)
	{
	    // MJ constness pb
            ((Service*) service)->setDepartureSchedule ( _departureBeginSchedule[ s ] );
	}

        position += columnWidth;
    }

    updateArrivalIndex();
    updateDepartureIndex();


}




void 
LineStop::allocateSchedules ()
{
    // Passer en vecteur
    _arrivalEndSchedule = new synthese::time::Schedule[ getParentPath ()->getServices().size() ];
    _arrivalBeginSchedule = new synthese::time::Schedule[ getParentPath ()->getServices().size() ];
    _departureEndSchedule = new synthese::time::Schedule[ getParentPath ()->getServices().size() ];
    _departureBeginSchedule = new synthese::time::Schedule[ getParentPath ()->getServices().size() ];
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


void 
LineStop::setMetricOffset (double metricOffset)
{
    _metricOffset = metricOffset;
}




}
}
