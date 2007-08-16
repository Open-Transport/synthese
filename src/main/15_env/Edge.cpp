
/** Edge class implementation.
	@file Edge.cpp

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

#include "Edge.h"

#include "15_env/Path.h"
#include "15_env/Service.h"
#include "15_env/Vertex.h"
#include "15_env/ContinuousService.h"

#include "04_time/Schedule.h"

namespace synthese
{
	using namespace time;
	using namespace geometry;

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
		, _nextInPath(NULL)
		, _previousConnectionDeparture(NULL)
		, _previousDepartureForFineSteppingOnly(NULL)
		, _followingConnectionArrival(NULL)
		, _followingArrivalForFineSteppingOnly(NULL)
		, _serviceIndexUpdateNeeded (true)
		{ }


		Edge::~Edge ()
		{
			// Delete via points
			for (std::vector<const Point2D*>::iterator iter = _viaPoints.begin (); 
			iter != _viaPoints.end (); 
			++iter)
			{
				delete (*iter);
			}
			_viaPoints.clear ();
		    
		}



		bool Edge::isArrival () const
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




		const std::vector<const Point2D*>& 
		Edge::getViaPoints () const
		{
			return _viaPoints;
		}



		void 
		Edge::addViaPoint (const Point2D& viaPoint)
		{
			_viaPoints.push_back (new Point2D (viaPoint));
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






		bool Edge::isRunning(
			const DateTime& startMoment
			, const DateTime& endMoment
		) const	{
			for (Date startDate(startMoment.getDate()); startDate <= endMoment; startDate++ )
				if ( getParentPath ()->isInService ( startDate ) )
					return true;
			return false;
		}





/*
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

*/




/*		bool 
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
*/



		ServicePointer Edge::getNextService (
			DateTime departureMoment, 
			const DateTime& maxDepartureMoment,
			const DateTime& calculationMoment
			, int minNextServiceIndex
		) const
		{
			// Search schedule
			int next(getDepartureFromIndex (departureMoment.getHours ()));

			if ( next == UNKNOWN_VALUE )
				next = getParentPath ()->getServices().size();

			if (minNextServiceIndex > next )
				next = minNextServiceIndex;

			while ( departureMoment <= maxDepartureMoment )  // boucle sur les dates
			{
				// Look in schedule for when the line is in service
				if (getParentPath ()->isInService( departureMoment.getDate()))
				{
					for (; next < getParentPath ()->getServices().size(); ++next)  // boucle sur les services
					{
						// Saving of the used service
						ServicePointer servicePointer(
							getParentPath ()->getService(next)->getFromPresenceTime(ServicePointer::DEPARTURE_TO_ARRIVAL, this, departureMoment, calculationMoment)
							);

						if (!servicePointer.getService())
							continue;

						// Control of validity of departure date time
						if (servicePointer.getActualDateTime() > maxDepartureMoment )
							return ServicePointer();

						// Store the service rank in edge
						servicePointer.setServiceIndex(next);

						// The service is now returned
						return servicePointer;

					} //end while
				}
				
				departureMoment++;
				departureMoment.setHour(Hour(TIME_MIN));

				next = _departureIndex[ 0 ];
			}

			return ServicePointer();
		}



		ServicePointer Edge::getPreviousService(
			DateTime arrivalMoment
			, const DateTime& minArrivalMoment
			, const DateTime& computingDateTime
			, int maxPreviousServiceIndex
		) const	{
			int previous(getArrivalFromIndex (arrivalMoment.getHours ()));
			
			while ( arrivalMoment >= minArrivalMoment )  // Loop over dates
			{
				if (getParentPath ()->isInService( arrivalMoment.getDate()))
				{
					for (; previous >= 0; --previous)  // Loop over services
					{
						// Saving of the used service
						ServicePointer servicePointer(
							getParentPath ()->getService(previous)->getFromPresenceTime(ServicePointer::ARRIVAL_TO_DEPARTURE, this, arrivalMoment, computingDateTime)
							);

						if (!servicePointer.getService())
							continue;

						// Control of validity of departure date time
						if (servicePointer.getActualDateTime() < minArrivalMoment)
							return ServicePointer();

						// Store service rank in edge
						servicePointer.setServiceIndex(previous);

						// The service is now returned
						return servicePointer;
					}
				}

				arrivalMoment--;
				arrivalMoment.setHour(Hour(TIME_MAX));
				previous = _arrivalIndex[ 23 ];
			}

			return ServicePointer();
		}



		void Edge::updateServiceIndex() const
		{
			int numHour;
			int i;

			// Reset
			for ( numHour = 0; numHour < HOURS_PER_DAY; ++numHour)
			{
				_departureIndex[ numHour ] = UNKNOWN_VALUE;
				_arrivalIndex[numHour] = UNKNOWN_VALUE;
			}

			// Departures
			for (i=0; i<getParentPath()->getServices().size(); ++i)
			{
				const Service* service = getParentPath()->getService(i);
				const Schedule& endSchedule(service->getDepartureEndScheduleToIndex(this));
				const Schedule& beginSchedule(service->getDepartureBeginScheduleToIndex(this));

				for (numHour = 0; numHour <= endSchedule.getHours(); ++numHour)
				{
					if (_departureIndex[numHour] == UNKNOWN_VALUE
						|| getParentPath()->getService(_departureIndex[numHour])->getDepartureBeginScheduleToIndex(this).getHour() > endSchedule.getHour()
						)
						_departureIndex[numHour] = i;
				}
				if (endSchedule.getHour() < beginSchedule.getHour())
				{
					for (numHour = endSchedule.getHours(); numHour < HOURS_PER_DAY; ++numHour)
					{
						if (_departureIndex[numHour] == UNKNOWN_VALUE)
							_departureIndex[numHour] = i;
					}
				}
			}

			// Arrivals
			for (i=getParentPath()->getServices().size()-1; i>=0; --i)
			{
				const Service* service = getParentPath()->getService(i);
				const Schedule& endSchedule(service->getArrivalEndScheduleToIndex(this));
				const Schedule& beginSchedule(service->getArrivalBeginScheduleToIndex(this));

				for (numHour = HOURS_PER_DAY-1; numHour >= endSchedule.getHours(); --numHour)
				{
					if (_arrivalIndex[numHour] == UNKNOWN_VALUE
						|| getParentPath()->getService(_arrivalIndex[numHour])->getArrivalBeginScheduleToIndex(this).getHour() < endSchedule.getHour()
						)
						_arrivalIndex[numHour] = i;
				}
				if (endSchedule.getHour() < beginSchedule.getHour())
				{
					for (numHour = endSchedule.getHours(); numHour >= 0; --numHour)
					{
						if (_arrivalIndex[numHour] == UNKNOWN_VALUE)
							_arrivalIndex[numHour] = i;
					}
				}
			}
			_serviceIndexUpdateNeeded = false;

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

		void Edge::setIsArrival( bool value )
		{
			_isArrival = value;
		}

		void Edge::setIsDeparture( bool value )
		{
			_isDeparture = value;
		}

		void Edge::setRankInPath( int value )
		{
			_rankInPath = value;
		}

		void Edge::setParentPath( const Path* path )
		{
			_parentPath = path;
		}



	    void 
	    Edge::markServiceIndexUpdateNeeded ()
	    {
		_serviceIndexUpdateNeeded = true;
	    }



	    int Edge::getDepartureFromIndex (int hour) const
	    {
		if (_serviceIndexUpdateNeeded) updateServiceIndex ();
		return _departureIndex[hour];
	    }


	    int Edge::getArrivalFromIndex (int hour) const
	    {
		if (_serviceIndexUpdateNeeded) updateServiceIndex ();
		return _arrivalIndex[hour];
	    }

	}
}
