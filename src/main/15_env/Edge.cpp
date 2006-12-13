#include "Edge.h"
#include "Path.h"
#include "Service.h"
#include "Vertex.h"
#include "ContinuousService.h"

#include "04_time/Schedule.h"


using synthese::time::Schedule;


namespace synthese
{
namespace env
{


    
Edge::Edge (bool isDeparture,
	    bool isArrival,
	    const Path* parentPath,
	    int rankInPath) 
: _isDeparture (isDeparture)
, _isArrival (isArrival)
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






bool 
Edge::isArrival () const
{
    return _isArrival;
}



bool 
Edge::isDeparture () const
{
    return _isDeparture;
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
Edge::getDepartureBeginSchedule (int serviceIndex) const
{
    return _departureBeginSchedule.at (serviceIndex);
}




const synthese::time::Schedule& 
Edge::getDepartureEndSchedule (int serviceIndex) const
{
    return _departureEndSchedule.at (serviceIndex);
}



const synthese::time::Schedule& 
Edge::getArrivalBeginSchedule (int serviceIndex) const
{
    return _arrivalBeginSchedule.at (serviceIndex);
}





const synthese::time::Schedule& 
Edge::getArrivalEndSchedule (int serviceIndex) const
{
    return _arrivalEndSchedule.at (serviceIndex);
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
			int serviceIndex,
			const synthese::time::DateTime& departureMoment, 
			synthese::time::DateTime& arrivalMoment ) const
{
    if ( getParentPath ()->getService (serviceIndex)->isContinuous () )
    {
        arrivalMoment = departureMoment;
        arrivalMoment += ( _arrivalBeginSchedule[serviceIndex] - 
			   departureEdge._departureBeginSchedule[serviceIndex] );
    } 
    else
    {
        arrivalMoment = _arrivalBeginSchedule[serviceIndex];
        arrivalMoment.addDaysDuration( _arrivalBeginSchedule[serviceIndex].getDaysSinceDeparture () - 
				       departureEdge._departureBeginSchedule[serviceIndex].getDaysSinceDeparture () );
    }
    
}



void 
Edge::calculateDeparture (const Edge& arrivalEdge, 
			  int serviceIndex,
			  const synthese::time::DateTime& arrivalMoment, 
			  synthese::time::DateTime& departureMoment ) const
{
    if ( getParentPath ()->getService( serviceIndex )->isContinuous() )
    {
        departureMoment = arrivalMoment;
        departureMoment -= ( arrivalEdge._arrivalBeginSchedule[ serviceIndex ] - _departureBeginSchedule[ serviceIndex ] );
    } 
    else
    {
        departureMoment = _departureBeginSchedule[ serviceIndex ];
        departureMoment.subDaysDuration( arrivalEdge._arrivalBeginSchedule[ serviceIndex ].getDaysSinceDeparture () - _departureBeginSchedule[ serviceIndex ].getDaysSinceDeparture () );
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
			  int minNextServiceIndex ) const
{
    int next;

    // Search schedule
    next = _departureIndex[ departureMoment.getHours () ];
    if ( next == UNKNOWN_VALUE )
        next = getParentPath ()->getServices().size();

    if ( minNextServiceIndex > next )
        next = minNextServiceIndex;

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
			   int maxPreviousServiceIndex) const

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





void 
Edge::insertDepartureSchedule (int index, const Schedule& schedule)
{
    const Service* service = _parentPath->getService (index);

    std::vector<synthese::time::Schedule>::iterator itInsertBegin = _departureBeginSchedule.begin ();
    advance (itInsertBegin, index);

    _departureBeginSchedule.insert (itInsertBegin, schedule);

    std::vector<synthese::time::Schedule>::iterator itInsertEnd = _departureEndSchedule.begin ();
    advance (itInsertEnd, index);

    if (service->isContinuous ())
    {
	const ContinuousService* continuousService = dynamic_cast<const ContinuousService*> (service);
	_departureEndSchedule.insert (itInsertEnd, 
				      _departureBeginSchedule[index] + continuousService->getRange ());
    }
    else 
    {
	_departureEndSchedule.insert (itInsertEnd,
				      _departureBeginSchedule[index]);
    }
    updateDepartureIndex ();
}




void 
Edge::insertArrivalSchedule (int index, const Schedule& schedule)
{
    const Service* service = _parentPath->getService (index);
    
    std::vector<synthese::time::Schedule>::iterator itInsertBegin = _arrivalBeginSchedule.begin ();
    advance (itInsertBegin, index);

    _arrivalBeginSchedule.insert (itInsertBegin, schedule);

    std::vector<synthese::time::Schedule>::iterator itInsertEnd = _arrivalEndSchedule.begin ();
    advance (itInsertEnd, index);

    _arrivalEndSchedule.insert (itInsertEnd, schedule);

    if (service->isContinuous ())
    {
	const ContinuousService* continuousService = dynamic_cast<const ContinuousService*> (service);
	_arrivalBeginSchedule[index] += continuousService->getMaxWaitingTime ();
	_arrivalEndSchedule[index] = _arrivalBeginSchedule[index] + continuousService->getRange ();
    }

    updateArrivalIndex ();
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




const AddressablePlace* 
Edge::getPlace () const
{
    return getFromVertex ()->getPlace ();
}



const ConnectionPlace* 
Edge::getConnectionPlace () const
{
    return getFromVertex ()->getConnectionPlace ();
}







}
}

