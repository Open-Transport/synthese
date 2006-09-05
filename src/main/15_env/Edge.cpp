#include "Edge.h"
#include "Path.h"
#include "Service.h"
#include "ContinuousService.h"

#include "04_time/Schedule.h"


using synthese::time::Schedule;


namespace synthese
{
namespace env
{



Edge::Edge (const EdgeType& type,
	    const Path* parentPath,
	    int rankInPath) 
: _type (type)
, _parentPath (parentPath)
, _rankInPath (rankInPath)
{
    
}


Edge::~Edge ()
{
    // Delete via points
    for (std::vector<const Point*>::iterator iter = _viaPoints.begin (); 
	 iter != _viaPoints.end (); 
	 ++iter)
    {
	delete (*iter);
    }
    _viaPoints.clear ();
    
}



const Edge::EdgeType&
Edge::getType () const
{
    return _type;
}



void 
Edge::setType ( const EdgeType& type )
{
    _type = type;
}



bool 
Edge::isArrival () const
{
    return ( _type == EDGE_TYPE_PASSAGE || _type == EDGE_TYPE_ARRIVAL );
}



bool 
Edge::isDeparture () const
{
    return ( _type == EDGE_TYPE_PASSAGE || _type == EDGE_TYPE_DEPARTURE );
}



const Edge* 
Edge::getNextInPath () const
{
    return _nextInPath;
}



void 
Edge::setNextInPath (const Edge* nextInPath)
{
    _nextInPath = nextInPath;
}




const Edge* 
Edge::getPreviousDeparture () const
{
    return _previousDeparture;
}



const Edge* 
Edge::getFollowingArrival () const
{
    return _followingArrival;
}


const Edge* 
Edge::getPreviousConnectionDeparture () const
{
    return _previousConnectionDeparture;
}



const Edge* 
Edge::getPreviousDepartureForFineSteppingOnly () const
{
    return _previousDepartureForFineSteppingOnly;
}




const Edge* 
Edge::getFollowingConnectionArrival () const
{
    return _followingConnectionArrival;
}



const Edge* 
Edge::getFollowingArrivalForFineSteppingOnly () const
{
    return _followingArrivalForFineSteppingOnly;
}




void 
Edge::setPreviousDeparture ( const Edge* previousDeparture)
{
    _previousDeparture = previousDeparture;
}




void 
Edge::setPreviousConnectionDeparture( const Edge* previousConnectionDeparture)
{
    _previousConnectionDeparture = previousConnectionDeparture;
}


void 
Edge::setPreviousDepartureForFineSteppingOnly ( const Edge* previousDeparture)
{
    _previousDepartureForFineSteppingOnly = previousDeparture;
}




void 
Edge::setFollowingArrival ( const Edge* followingArrival)
{
    _followingArrival = followingArrival;
}




void 
Edge::setFollowingConnectionArrival( const Edge* followingConnectionArrival)
{
    _followingConnectionArrival = followingConnectionArrival;
}




void 
Edge::setFollowingArrivalForFineSteppingOnly ( const Edge* followingArrival)
{
    _followingArrivalForFineSteppingOnly = followingArrival;
}




const std::vector<const Point*>& 
Edge::getViaPoints () const
{
    return _viaPoints;
}



void 
Edge::addViaPoint (const Point& viaPoint)
{
    _viaPoints.push_back (new Point (viaPoint));
}


void 
Edge::clearViaPoints ()
{
    _viaPoints.clear ();
}




int 
Edge::getRankInPath () const
{
    return _rankInPath;
}


const Path* 
Edge::getParentPath () const
{
    return _parentPath;
}




double 
Edge::getLength () const
{
    if (_nextInPath == 0) return 0;
    return _nextInPath->getMetricOffset () - getMetricOffset (); 
}






const synthese::time::Schedule& 
Edge::getDepartureBeginSchedule (int serviceNumber) const
{
    return _departureBeginSchedule[serviceNumber];
}




const synthese::time::Schedule& 
Edge::getDepartureEndSchedule (int serviceNumber) const
{
    return _departureEndSchedule[serviceNumber];
}



const synthese::time::Schedule& 
Edge::getArrivalBeginSchedule (int serviceNumber) const
{
    return _arrivalBeginSchedule[serviceNumber];
}





const synthese::time::Schedule& 
Edge::getArrivalEndSchedule (int serviceNumber) const
{
    return _arrivalEndSchedule[serviceNumber];
}





bool 
Edge::isRunning( const synthese::time::DateTime& startMoment, 
		     const synthese::time::DateTime& endMoment ) const
{
    synthese::time::Date startDate;
    for ( startDate = startMoment; startDate <= endMoment; startDate++ )
        if ( getParentPath ()->isInService ( startDate ) )
            return true;
    return false;
}





void 
Edge::calculateArrival (const Edge& departureEdge, 
			int serviceNumber,
			const synthese::time::DateTime& departureMoment, 
			synthese::time::DateTime& arrivalMoment ) const
{
    if ( getParentPath ()->getService (serviceNumber)->isContinuous () )
    {
        arrivalMoment = departureMoment;
        arrivalMoment += ( _arrivalBeginSchedule[serviceNumber ] - 
			   departureEdge._departureBeginSchedule[ serviceNumber ] );
    } 
    else
    {
        arrivalMoment = _arrivalBeginSchedule[ serviceNumber ];
        arrivalMoment.addDaysDuration( _arrivalBeginSchedule[ serviceNumber ].getDaysSinceDeparture () - 
				       departureEdge._departureBeginSchedule[ serviceNumber ].getDaysSinceDeparture () );
    }
    
}



void 
Edge::calculateDeparture (const Edge& arrivalEdge, 
			  int serviceNumber,
			  const synthese::time::DateTime& arrivalMoment, 
			  synthese::time::DateTime& departureMoment ) const
{
    if ( getParentPath ()->getService( serviceNumber )->isContinuous() )
    {
        departureMoment = arrivalMoment;
        departureMoment -= ( arrivalEdge._arrivalBeginSchedule[ serviceNumber ] - _departureBeginSchedule[ serviceNumber ] );
    } 
    else
    {
        departureMoment = _departureBeginSchedule[ serviceNumber ];
        departureMoment.subDaysDuration( arrivalEdge._arrivalBeginSchedule[ serviceNumber ].getDaysSinceDeparture () - _departureBeginSchedule[ serviceNumber ].getDaysSinceDeparture () );
    }
}





int 
Edge::getBestRunTime (const Edge& other ) const
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
Edge::checkSchedule (const Edge* edgeWithPreviousSchedule ) const
{
    // Vertical chronology check
    if ( edgeWithPreviousSchedule != NULL )
    {
        for ( int s = 0; s < getParentPath ()->getServices().size(); s++ )
            if ( _departureBeginSchedule[ s ] < edgeWithPreviousSchedule->_departureBeginSchedule[ s ] )
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




int 
Edge::getNextService (synthese::time::DateTime& departureMoment, 
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
Edge::getPreviousService ( synthese::time::DateTime& arrivalMoment, 
			   const synthese::time::DateTime& minArrivalMoment,
			   int maxPreviousServiceNumber) const

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






/*
int 
Edge::getPreviousService ( synthese::time::DateTime& arrivalMoment, 
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

*/





void 
Edge::setSchedules ( const std::string& buffer, 
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
Edge::allocateSchedules ()
{
    // Passer en vecteur
    _arrivalEndSchedule = new synthese::time::Schedule[ getParentPath ()->getServices().size() ];
    _arrivalBeginSchedule = new synthese::time::Schedule[ getParentPath ()->getServices().size() ];
    _departureEndSchedule = new synthese::time::Schedule[ getParentPath ()->getServices().size() ];
    _departureBeginSchedule = new synthese::time::Schedule[ getParentPath ()->getServices().size() ];
}





void 
Edge::updateDepartureIndex ()
{
    int numHour;

    int lastHour = 25; 
    int serviceOverMidnightNumber = 0;
    
    // Reset
    for ( numHour = 0; numHour < synthese::time::HOURS_PER_DAY; numHour++ )
        _departureIndex[ numHour ] = -1;


    for (int i = 0; i < _parentPath->getServices().size (); ++i)
    {
        if ( _departureEndSchedule[i].getHours () < lastHour )
            serviceOverMidnightNumber = i;
	
        if ( _departureEndSchedule[i].getHours () >= _departureBeginSchedule[i].getHours () )
        {
            for ( numHour = 0; numHour <= _departureEndSchedule[i].getHours (); ++numHour )
	    {
                if ( (_departureIndex[numHour] == -1) || 
		     (_departureIndex[numHour] < serviceOverMidnightNumber) ) 
		{
		    _departureIndex[numHour] = i;
		}
	    }
        }
        else
        {
            for (numHour = 0; numHour < synthese::time::HOURS_PER_DAY; ++numHour)
	    {
                if (_departureIndex[numHour] == -1)
		{
                    _departureIndex[ numHour ] = i;
		}
	    }
        }
        lastHour = _departureEndSchedule[i].getHours ();

    }
    
}





void 
Edge::updateArrivalIndex ()
{
    int numHour;

    int lastHour = 25; 
    int serviceOverMidnightNumber = _parentPath->getServices ().size ();

    // Reset
    for (numHour = 0; numHour < synthese::time::HOURS_PER_DAY; ++numHour)
        _arrivalIndex[numHour] = -1;
    
    
    for (int i = _parentPath->getServices().size () - 1; i >= 0; --i)
    {
        if ( _arrivalBeginSchedule[i].getHours () > lastHour )
            serviceOverMidnightNumber = i;
	
        if ( _arrivalEndSchedule[i].getHours () >= _arrivalBeginSchedule[i].getHours () )
        {
            for ( numHour = _arrivalBeginSchedule[i].getHours (); 
		  numHour < synthese::time::HOURS_PER_DAY; numHour++ )
	    {
                if ( (_arrivalIndex[numHour] == -1) || 
		     (_arrivalIndex[numHour] > serviceOverMidnightNumber) )
		{
                    _arrivalIndex[numHour] = i;
		}
	    }
        }
        else
        {
            for (numHour = 0; numHour < synthese::time::HOURS_PER_DAY; ++numHour)
	    {
                if ( _arrivalIndex[numHour] == -1 )
		{
                    _arrivalIndex[numHour] = i;
		}
	    }
        }
        lastHour = _arrivalBeginSchedule[i].getHours ();
    }
    
}







/*  => moved in integral search
int 
Edge::getNextService ( synthese::time::DateTime& departureMoment, 
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
*/




}
}
