
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

#include "Hub.h"
#include "Edge.h"
#include "Path.h"
#include "Service.h"
#include "Vertex.h"
#include "Schedule.h"

namespace synthese
{
	using namespace time;
	using namespace geometry;

	namespace graph
	{
		Edge::Edge(
			const Path* parentPath,
			int rankInPath,
			Vertex* fromVertex,
			double metricOffset
		):	Registrable(UNKNOWN_VALUE),
			_parentPath (parentPath),
			_rankInPath (rankInPath),
			_previousConnectionDeparture(NULL),
			_previousDepartureForFineSteppingOnly(NULL),
			_followingConnectionArrival(NULL),
			_followingArrivalForFineSteppingOnly(NULL),
			_serviceIndexUpdateNeeded (true),
			_fromVertex(fromVertex),
			_metricOffset(metricOffset)
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



		double Edge::getMetricOffset () const
		{
			return _metricOffset;
		}



		void Edge::setMetricOffset(
			double metricOffset
		){
			_metricOffset = metricOffset;
		}



		bool Edge::isArrival () const
		{
			return _previousDepartureForFineSteppingOnly && isArrivalAllowed();
		}



		bool Edge::isDeparture () const
		{
			return _followingArrivalForFineSteppingOnly && isDepartureAllowed();
		}



		const Vertex* Edge::getFromVertex() const
		{
			return _fromVertex;
		}



		Edge* Edge::getPreviousConnectionDeparture () const
		{
			return _previousConnectionDeparture;
		}



		Edge* Edge::getPreviousDepartureForFineSteppingOnly () const
		{
			return _previousDepartureForFineSteppingOnly;
		}




		Edge* Edge::getFollowingConnectionArrival () const
		{
			return _followingConnectionArrival;
		}



		Edge* Edge::getFollowingArrivalForFineSteppingOnly () const
		{
			return _followingArrivalForFineSteppingOnly;
		}



		void Edge::setPreviousConnectionDeparture(Edge* previousConnectionDeparture)
		{
			_previousConnectionDeparture = previousConnectionDeparture;
		}



		void Edge::setPreviousDepartureForFineSteppingOnly(Edge* previousDeparture)
		{
			_previousDepartureForFineSteppingOnly = previousDeparture;
		}



		void Edge::setFollowingConnectionArrival(Edge* followingConnectionArrival)
		{
			_followingConnectionArrival = followingConnectionArrival;
		}



		void Edge::setFollowingArrivalForFineSteppingOnly(Edge* followingArrival)
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



/*		int 
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



		ServicePointer Edge::getNextService(
			UserClassCode userClass,
			DateTime departureMoment
			, const DateTime& maxDepartureMoment
			, bool controlIfTheServiceIsReachable
			, int minNextServiceIndex
			, bool inverted
		) const	{
			// Search schedule
			int next(getDepartureFromIndex (departureMoment.getHours ()));

			if ( next == UNKNOWN_VALUE )
				next = getParentPath ()->getServices().size();

			if (minNextServiceIndex > next )
				next = minNextServiceIndex;

			while ( departureMoment <= maxDepartureMoment )  // boucle sur les dates
			{
				// Look in schedule for when the line is in service
				if (getParentPath()->isActive(departureMoment.getDate()))
				{
					for (; next < getParentPath ()->getServices().size(); ++next)  // boucle sur les services
					{
						// Saving of the used service
						ServicePointer servicePointer(
							getParentPath ()->getService(next)->getFromPresenceTime(
								DEPARTURE_TO_ARRIVAL,
								userClass
								, this
								, departureMoment
								, controlIfTheServiceIsReachable
								, inverted
							)
						);

						if (!servicePointer.getService())
							continue;

						// Control of validity of departure date time
						if (servicePointer.getActualDateTime() > maxDepartureMoment )
							return ServicePointer(DEPARTURE_TO_ARRIVAL, userClass);

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

			return ServicePointer(DEPARTURE_TO_ARRIVAL, userClass);
		}



		ServicePointer Edge::getPreviousService(
			UserClassCode userClass,
			DateTime arrivalMoment
			, const DateTime& minArrivalMoment
			, bool controlIfTheServiceIsReachable
			, int maxPreviousServiceIndex
			, bool inverted
		) const	{
			int previous(getArrivalFromIndex (arrivalMoment.getHours ()));

			if (maxPreviousServiceIndex < previous)
				previous = maxPreviousServiceIndex;

			while ( arrivalMoment >= minArrivalMoment )  // Loop over dates
			{
				if (getParentPath()->isActive(arrivalMoment.getDate()))
				{
					for (; previous >= 0; --previous)  // Loop over services
					{
						// Saving of the used service
						ServicePointer servicePointer(
							getParentPath ()->getService(previous)->getFromPresenceTime(
								ARRIVAL_TO_DEPARTURE,
								userClass
								, this
								, arrivalMoment
								, controlIfTheServiceIsReachable
								, inverted
							)
						);

						if (!servicePointer.getService())
							continue;

						// Control of validity of departure date time
						if (servicePointer.getActualDateTime() < minArrivalMoment)
							return ServicePointer(ARRIVAL_TO_DEPARTURE, userClass);

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

			return ServicePointer(ARRIVAL_TO_DEPARTURE, userClass);
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
				const Hour& endHour(service->getDepartureEndScheduleToIndex(getRankInPath()).getHour());
				int endHours(endHour.getHours());
				const Hour& beginHour(service->getDepartureBeginScheduleToIndex(getRankInPath()).getHour());

				for (numHour = 0; numHour <= endHours; ++numHour)
				{
					if(	_departureIndex[numHour] == UNKNOWN_VALUE
					||	getParentPath()->getService(_departureIndex[numHour])->getDepartureBeginScheduleToIndex(getRankInPath()).getHour() > endHour
					)
						_departureIndex[numHour] = i;
				}
				if (endHour < beginHour)
				{
					for (numHour = endHours; numHour < HOURS_PER_DAY; ++numHour)
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
				const Hour& endHour(service->getArrivalEndScheduleToIndex(getRankInPath()).getHour());
				const Hour& beginHour(service->getArrivalBeginScheduleToIndex(getRankInPath()).getHour());
				int beginHours(beginHour.getHours());

				for (numHour = HOURS_PER_DAY-1; numHour >= beginHours; --numHour)
				{
					if (_arrivalIndex[numHour] == UNKNOWN_VALUE
						|| getParentPath()->getService(_arrivalIndex[numHour])->getArrivalBeginScheduleToIndex(getRankInPath()).getHour() < beginHour
						)
						_arrivalIndex[numHour] = i;
				}
				if (endHour < beginHour)
				{
					for (numHour = endHour.getHours(); numHour >= 0; --numHour)
					{
						if (_arrivalIndex[numHour] == UNKNOWN_VALUE)
							_arrivalIndex[numHour] = i;
					}
				}
			}
			_serviceIndexUpdateNeeded = false;

		}



		const Hub* Edge::getHub() const
		{
			return getFromVertex()->getHub();
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

		bool Edge::isConnectingEdge() const
		{
			assert(_fromVertex);
			assert(_fromVertex->getHub());
			return _fromVertex->getHub()->isConnectionPossible();
		}
	}
}
