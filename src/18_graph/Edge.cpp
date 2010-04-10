
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

using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace geometry;

	namespace graph
	{
		const size_t Edge::INDICES_NUMBER(24);

		Edge::Edge(
			const Path* parentPath,
			int rankInPath,
			Vertex* fromVertex,
			double metricOffset
		):	Registrable(0),
			_parentPath (parentPath),
			_rankInPath (rankInPath),
			_previousConnectionDeparture(NULL),
			_previousDepartureForFineSteppingOnly(NULL),
			_followingConnectionArrival(NULL),
			_followingArrivalForFineSteppingOnly(NULL),
			_serviceIndexUpdateNeeded (true),
			_fromVertex(fromVertex),
			_metricOffset(metricOffset),
			_departureIndex(INDICES_NUMBER),
			_arrivalIndex(INDICES_NUMBER)
		{}


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



		Vertex* Edge::getFromVertex() const
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
			ptime departureMoment,
			const ptime& maxDepartureMoment,
			bool controlIfTheServiceIsReachable,
			optional<DepartureServiceIndex::Value>& minNextServiceIndex,
			bool inverted
		) const	{

			const ServiceSet& services(getParentPath()->getServices());

			if(services.empty())
			{
				return ServicePointer(false, DEPARTURE_TO_ARRIVAL, userClass);
			}

			bool RTData(departureMoment < posix_time::second_clock().local_time() + posix_time::hours(23));

			// Search schedule
			DepartureServiceIndex::Value next(getDepartureFromIndex(RTData, departureMoment.time_of_day().hours()));

			if(	minNextServiceIndex &&
				(*minNextServiceIndex == services.end() || services.value_comp()(*next, **minNextServiceIndex))
			){
				next = *minNextServiceIndex;
			}

			while ( departureMoment <= maxDepartureMoment )  // boucle sur les dates
			{
				// Look in schedule for when the line is in service
				if(	getParentPath()->isActive(departureMoment.date()))
				{
					for (; next != services.end(); ++next)  // boucle sur les services
					{
						// Saving of the used service
						ServicePointer servicePointer(
							(*next)->getFromPresenceTime(
								RTData,
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
							return ServicePointer(RTData, DEPARTURE_TO_ARRIVAL, userClass);

						// Store the service rank in edge
						minNextServiceIndex = next;

						// The service is now returned
						return servicePointer;
				}	}
				
				departureMoment = ptime(departureMoment.date(), hours(24));

				next = _departureIndex[0].get(RTData);
			}

			return ServicePointer(RTData, DEPARTURE_TO_ARRIVAL, userClass);
		}



		ServicePointer Edge::getPreviousService(
			UserClassCode userClass,
			ptime arrivalMoment,
			const ptime& minArrivalMoment,
			bool controlIfTheServiceIsReachable,
			optional<ArrivalServiceIndex::Value>& maxPreviousServiceIndex,
			bool inverted
		) const	{

			const ServiceSet& services(getParentPath()->getServices());

			if(services.empty())
			{
				return ServicePointer(false, ARRIVAL_TO_DEPARTURE, userClass);
			}

			bool RTData(arrivalMoment < posix_time::second_clock().local_time() + posix_time::hours(23));

			ArrivalServiceIndex::Value previous(getArrivalFromIndex(RTData, arrivalMoment.time_of_day().hours()));

			if(	maxPreviousServiceIndex &&
				(*maxPreviousServiceIndex == services.rend() || services.value_comp()(**maxPreviousServiceIndex, *previous))
			){
				previous = *maxPreviousServiceIndex;
			}

			while ( arrivalMoment >= minArrivalMoment )  // Loop over dates
			{
				if(	getParentPath()->isActive(arrivalMoment.date()))
				{
					for (; previous != services.rend(); ++previous)  // Loop over services
					{
						// Saving of the used service
						ServicePointer servicePointer(
							(*previous)->getFromPresenceTime(
								RTData,
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
							return ServicePointer(RTData, ARRIVAL_TO_DEPARTURE, userClass);

						// Store service rank in edge
						maxPreviousServiceIndex = previous;

						// The service is now returned
						return servicePointer;
				}	}

				arrivalMoment = ptime(arrivalMoment.date(), -seconds(1));
				previous = _arrivalIndex[INDICES_NUMBER - 1].get(RTData);
			}

			return ServicePointer(RTData, ARRIVAL_TO_DEPARTURE, userClass);
		}



		void Edge::_updateServiceIndex(
			bool RTData
		) const {

			const ServiceSet& services(getParentPath()->getServices());
			size_t numHour;

			// Reset
			for ( numHour = 0; numHour < INDICES_NUMBER; ++numHour)
			{
				_departureIndex[numHour].set(RTData, services.end());
				_arrivalIndex[numHour].set(RTData, services.rend());
			}
			if(RTData)
			{
				_RTserviceIndexUpdateNeeded = false;
			}
			else
			{
				_serviceIndexUpdateNeeded = false;
			}

			if(services.empty()) return;

			// Departures
			for(ServiceSet::const_iterator it(services.begin()); it!=services.end(); ++it)
			{
				const Service* service = *it;
				time_duration endHour(Service::GetTimeOfDay(service->getDepartureEndScheduleToIndex(RTData, getRankInPath())));
				int endHours(endHour.hours());
				time_duration beginHour(Service::GetTimeOfDay(service->getDepartureBeginScheduleToIndex(RTData, getRankInPath())));

				for (numHour = 0; numHour <= endHours; ++numHour)
				{
					if(	_departureIndex[numHour].get(RTData) == services.end() ||
						Service::GetTimeOfDay((*_departureIndex[numHour].get(RTData))->getDepartureBeginScheduleToIndex(RTData, getRankInPath())) > endHour
					){
						_departureIndex[numHour].set(RTData, it);
					}
				}
				if (endHour < beginHour)
				{
					for (numHour = endHours; numHour < 24; ++numHour)
					{
						if(	_departureIndex[numHour].get(RTData) == services.end())
						{
							_departureIndex[numHour].set(RTData, it);
						}
					}
				}
			}

			// Arrivals
			for(ServiceSet::const_reverse_iterator it(services.rbegin()); it != services.rend(); ++it)
			{
				const Service* service = *it;
				time_duration endHour(Service::GetTimeOfDay(service->getArrivalEndScheduleToIndex(RTData, getRankInPath())));
				time_duration beginHour(Service::GetTimeOfDay(service->getArrivalBeginScheduleToIndex(RTData, getRankInPath())));
				int beginHours(beginHour.hours());

				for (numHour = 23; numHour >= beginHours; --numHour)
				{
					if(	_arrivalIndex[numHour].get(RTData) == services.rend()	||
						Service::GetTimeOfDay((*_arrivalIndex[numHour].get(RTData))->getArrivalBeginScheduleToIndex(RTData, getRankInPath())) < beginHour
					){
						_arrivalIndex[numHour].set(RTData, it);
					}
					if(numHour == 0) break;
				}
				if (endHour < beginHour)
				{
					for (numHour = endHour.hours(); numHour >= 0; --numHour)
					{
						if(	_arrivalIndex[numHour].get(RTData) == services.rend())
						{
							_arrivalIndex[numHour].set(RTData, it);
						}
						if(numHour == 0) break;
					}
				}
			}
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



	    void Edge::markServiceIndexUpdateNeeded(
			bool RTDataOnly
		) const {
			if(!RTDataOnly)
			{
				_serviceIndexUpdateNeeded = true;
			}
			_RTserviceIndexUpdateNeeded = true;
	    }



	    Edge::DepartureServiceIndex::Value Edge::getDepartureFromIndex(
			bool RTData,
			size_t hour
		) const {
			if (_getServiceIndexUpdateNeeded(RTData)) _updateServiceIndex(RTData);
			return  _departureIndex[hour].get(RTData);
	    }


	    Edge::ArrivalServiceIndex::Value Edge::getArrivalFromIndex(
			bool RTData,
			size_t hour
		) const {
			if (_getServiceIndexUpdateNeeded(RTData)) _updateServiceIndex(RTData);
			return _arrivalIndex[hour].get(RTData);
	    }



		bool Edge::isConnectingEdge() const
		{
			assert(_fromVertex);
			assert(_fromVertex->getHub());
			return _fromVertex->getHub()->isConnectionPossible();
		}



		bool Edge::_getServiceIndexUpdateNeeded( bool RTData ) const
		{
			return RTData ? _RTserviceIndexUpdateNeeded : _serviceIndexUpdateNeeded;
		}



		const Edge::DepartureServiceIndices& Edge::getDepartureIndices() const
		{
			return _departureIndex;
		}



		const Edge::ArrivalServiceIndices& Edge::getArrivalIndices() const
		{
			return _arrivalIndex;
		}
}	}
