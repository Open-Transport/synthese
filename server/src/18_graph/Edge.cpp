
/** Edge class implementation.
	@file Edge.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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
#include "DBModule.h"
#include "AllowedUseRule.h"
#include "ForbiddenUseRule.h"

#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <geos/geom/LineString.h>
#include <geos/geom/Point.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateSequenceFactory.h>

using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace geos::geom;

using namespace std;

namespace synthese
{
	using namespace db;

	namespace graph
	{
		const size_t Edge::INDICES_NUMBER(24);

		Edge::Edge(
			Path* parentPath,
			size_t rankInPath,
			Vertex* fromVertex,
			double metricOffset
		):	Registrable(0),
			_fromVertex(fromVertex),
			_parentPath (parentPath),
			_metricOffset(metricOffset),
			_rankInPath (rankInPath),
			_previous(NULL),
			_previousConnectionDeparture(NULL),
			_previousDepartureForFineSteppingOnly(NULL),
			_followingConnectionArrival(NULL),
			_followingArrivalForFineSteppingOnly(NULL),
			_next(NULL)
		{
			// Default accessibility
			RuleUser::Rules rules(RuleUser::GetEmptyRules());
			rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_CAR - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			setRules(rules);
		}


		Edge::~Edge ()
		{
		}



		bool Edge::isArrival() const
		{
			return _previousDepartureForFineSteppingOnly && isArrivalAllowed();
		}



		bool Edge::isDeparture() const
		{
			return _followingArrivalForFineSteppingOnly && isDepartureAllowed();
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
			const ChronologicalServicesCollection& collection,
			const AccessParameters& accessParameters,
			ptime departureMoment,
			const ptime& maxDepartureMoment,
			bool checkIfTheServiceIsReachable,
			optional<DepartureServiceIndex::Value>& minNextServiceIndex,
			bool inverted,
			bool ignoreReservation,
			bool allowCanceled,
			bool enableTheoretical,
			bool enableRealTime,
			UseRule::ReservationDelayType reservationRulesDelayType,
			bool maxDepartureMomentConcernsTheorical
		) const	{
			boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
						*getParentPath()->sharedServicesMutex
			);
			const ServiceSet& services(collection.getServices());

			if(services.empty() || (!enableTheoretical && !enableRealTime))
			{
				return ServicePointer();
			}

			bool RTData(enableRealTime && departureMoment < posix_time::second_clock().local_time() + posix_time::hours(23));

			// Search schedule
			DepartureServiceIndex::Value next(
				getDepartureFromIndex(
					collection,
					RTData,
					departureMoment.time_of_day().hours()
			)	);

			if(minNextServiceIndex)
			{
				if(*minNextServiceIndex == services.end())
				{
					next = *minNextServiceIndex;
				}

				else if((next != services.end()) && services.value_comp()(*next, **minNextServiceIndex))
				{
					next = *minNextServiceIndex;
				}
			}

			DepartureServiceIndices::mapped_type& departureIndex(getDepartureIndex(collection));
			while ( departureMoment <= maxDepartureMoment )  // boucle sur les dates
			{
				// Look in schedule for when the line is in service
				if(	getParentPath()->isActive(departureMoment.date()) ||
					(departureMoment.time_of_day() < hours(3) && getParentPath()->isActive(departureMoment.date() - days(1)))
				)
				{
					for (; next != services.end(); ++next)  // boucle sur les services
					{
						// Saving of the used service
						ServicePointer servicePointer(
							(*next)->getFromPresenceTime(
								accessParameters,
								enableTheoretical,
								RTData,
								true,
								*this,
								departureMoment,
								checkIfTheServiceIsReachable,
								inverted,
								ignoreReservation,
								allowCanceled,
								reservationRulesDelayType
							)
						);

						if (!servicePointer.getService())
							continue;

						// Check of validity of departure date time
						if ((!maxDepartureMomentConcernsTheorical && servicePointer.getDepartureDateTime() > maxDepartureMoment ) ||
							(maxDepartureMomentConcernsTheorical && servicePointer.getTheoreticalDepartureDateTime() > maxDepartureMoment))
						{
							return ServicePointer();
						}

						// Limitation of the continuous service range at the specified bounds
						if(servicePointer.getDepartureDateTime() + servicePointer.getServiceRange() > maxDepartureMoment)
						{
							servicePointer.setServiceRange(maxDepartureMoment - servicePointer.getDepartureDateTime());
						}

						// Store the service rank in edge
						minNextServiceIndex = next;

						// The service is now returned
						return servicePointer;
				}	}

				if (departureMoment.time_of_day().hours() < 3)
				{
					departureMoment = ptime(departureMoment.date(), hours(3));
				}
				else
				{
					departureMoment = ptime(departureMoment.date(), hours(27));
				}

				next = departureIndex[0].get(RTData);
			}

			return ServicePointer();
		}



		ServicePointer Edge::getPreviousService(
			const ChronologicalServicesCollection& collection,
			const AccessParameters& accessParameters,
			ptime arrivalMoment,
			const ptime& minArrivalMoment,
			bool checkIfTheServiceIsReachable,
			optional<ArrivalServiceIndex::Value>& maxPreviousServiceIndex,
			bool inverted,
			bool ignoreReservation,
			bool allowCanceled,
			bool enableTheoretical,
			bool enableRealTime,
			UseRule::ReservationDelayType reservationRulesDelayType
		) const {
			boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
						*getParentPath()->sharedServicesMutex
			);
			const ServiceSet& services(collection.getServices());

			if(services.empty())
			{
				return ServicePointer();
			}

			bool RTData(enableRealTime && arrivalMoment < posix_time::second_clock().local_time() + posix_time::hours(23));

			ArrivalServiceIndex::Value previous(
				getArrivalFromIndex(
					collection,
					RTData,
					arrivalMoment.time_of_day().hours()
			)	);

			if(	maxPreviousServiceIndex &&
				(*maxPreviousServiceIndex == services.rend() || services.value_comp()(**maxPreviousServiceIndex, *previous))
			){
				previous = *maxPreviousServiceIndex;
			}

			ArrivalServiceIndices::mapped_type& arrivalIndex(getArrivalIndex(collection));
			while ( arrivalMoment >= minArrivalMoment )  // Loop over dates
			{
				if(	getParentPath()->isActive(arrivalMoment.date()))
				{
					for (; previous != services.rend(); ++previous)  // Loop over services
					{
						// Saving of the used service
						ServicePointer servicePointer(
							(*previous)->getFromPresenceTime(
								accessParameters,
								enableTheoretical,
								RTData,
								false,
								*this,
								arrivalMoment,
								checkIfTheServiceIsReachable,
								inverted,
								ignoreReservation,
								allowCanceled,
								reservationRulesDelayType
							)
						);

						if (!servicePointer.getService())
							continue;

						// Check of validity of departure date time
						if (servicePointer.getArrivalDateTime() + servicePointer.getServiceRange() < minArrivalMoment)
						{
							return ServicePointer();
						}

						// Limitation of the continuous service range at the specified bounds
						if(servicePointer.getArrivalDateTime() < minArrivalMoment)
						{
							time_duration toShift(minArrivalMoment - servicePointer.getArrivalDateTime());
							servicePointer.shift(toShift);
							servicePointer.setServiceRange(servicePointer.getServiceRange() - toShift);
						}

						// Store service rank in edge
						maxPreviousServiceIndex = previous;

						// The service is now returned
						return servicePointer;
				}	}

				arrivalMoment = ptime(arrivalMoment.date(), -seconds(1));
				previous = arrivalIndex[INDICES_NUMBER - 1].get(RTData);
			}

			return ServicePointer();
		}



		void Edge::_updateServiceIndex(
			const ChronologicalServicesCollection& collection,
			bool RTData
		) const {

			boost::recursive_mutex::scoped_lock lock(_indexMutex);
			boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
						*getParentPath()->sharedServicesMutex
			);

			// Get the indices
			DepartureServiceIndices::mapped_type& departureIndex(getDepartureIndex(collection));
			ArrivalServiceIndices::mapped_type& arrivalIndex(getArrivalIndex(collection));

			const ServiceSet& services(collection.getServices());
			size_t numHour;

			// Reset
			for ( numHour = 0; numHour < INDICES_NUMBER; ++numHour)
			{
				departureIndex[numHour].set(RTData, services.end());
				arrivalIndex[numHour].set(RTData, services.rend());
			}
			if(RTData)
			{
				_RTserviceIndexUpdateNeeded[&collection] = false;
			}
			else
			{
				_serviceIndexUpdateNeeded[&collection] = false;
			}

			if(services.empty()) return;

			// Departures
			for(ServiceSet::const_iterator it(services.begin()); it!=services.end(); ++it)
			{
				const Service* service = *it;
				time_duration endHour(service->getDepartureEndScheduleToIndex(RTData, getRankInPath()));
				size_t endHours(std::min((size_t)endHour.hours(), (size_t)23));
				time_duration beginHour(service->getDepartureBeginScheduleToIndex(RTData, getRankInPath()));

				for (numHour = 0; numHour <= endHours; ++numHour)
				{
					if(	departureIndex[numHour].get(RTData) == services.end() ||
						(*departureIndex[numHour].get(RTData)) \
							->getDepartureBeginScheduleToIndex(RTData, getRankInPath()) > endHour
					){
						departureIndex[numHour].set(RTData, it);
					}
				}
				if (endHour < beginHour)
				{
					for (numHour = endHours; numHour < 24; ++numHour)
					{
						if(	departureIndex[numHour].get(RTData) == services.end())
						{
							departureIndex[numHour].set(RTData, it);
						}
					}
				}
			}

			// Arrivals
			for(ServiceSet::const_reverse_iterator it(services.rbegin()); it != services.rend(); ++it)
			{
				const Service* service = *it;
				time_duration endHour(service->getArrivalEndScheduleToIndex(RTData, getRankInPath()));
				time_duration beginHour(service->getArrivalBeginScheduleToIndex(RTData, getRankInPath()));
				size_t beginHours(std::min((size_t)beginHour.hours(), (size_t)23));

				for (numHour = 23; numHour >= beginHours; --numHour)
				{
					if(	arrivalIndex[numHour].get(RTData) == services.rend()	||
						(*arrivalIndex[numHour].get(RTData))->getArrivalBeginScheduleToIndex(RTData, getRankInPath()) < beginHour
					){
						arrivalIndex[numHour].set(RTData, it);						
					}
					if(numHour == 0) break;
				}
				if (endHour < beginHour)
				{
					for (numHour = endHour.hours(); true; --numHour)
					{
						if(	arrivalIndex[numHour].get(RTData) == services.rend())
						{
							arrivalIndex[numHour].set(RTData, it);
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



		void Edge::markServiceIndexUpdateNeeded(
			const ChronologicalServicesCollection& collection,
			bool RTDataOnly
		) const {
			if(!RTDataOnly)
			{
				_serviceIndexUpdateNeeded[&collection] = true;
			}
			_RTserviceIndexUpdateNeeded[&collection] = true;
		}



		Edge::DepartureServiceIndex::Value Edge::getDepartureFromIndex(
			const ChronologicalServicesCollection& collection,
			bool RTData,
			size_t hour
		) const {
			if (_getServiceIndexUpdateNeeded(collection, RTData)) _updateServiceIndex(collection, RTData);
			return getDepartureIndex(collection).at(hour).get(RTData);
		}



		Edge::ArrivalServiceIndex::Value Edge::getArrivalFromIndex(
			const ChronologicalServicesCollection& collection,
			bool RTData,
			size_t hour
		) const {
			if (_getServiceIndexUpdateNeeded(collection, RTData)) _updateServiceIndex(collection, RTData);
			return getArrivalIndex(collection).at(hour).get(RTData);
		}



		bool Edge::isConnectingEdge() const
		{
			assert(_fromVertex);
			assert(_fromVertex->getHub());
			return _fromVertex->getHub()->isConnectionPossible();
		}



		bool Edge::_getServiceIndexUpdateNeeded(
			const ChronologicalServicesCollection& collection,
			bool RTData
		) const	{
			if(RTData)
		{
				ServicesIndexUpdateNeeded::const_iterator it(_RTserviceIndexUpdateNeeded.find(&collection));
				if(it == _RTserviceIndexUpdateNeeded.end())
				{
					_RTserviceIndexUpdateNeeded.insert(make_pair(&collection, true));
					return true;
		}
				return it->second;
			}
			else
			{
				ServicesIndexUpdateNeeded::const_iterator it(_serviceIndexUpdateNeeded.find(&collection));
				if(it == _serviceIndexUpdateNeeded.end())
				{
					_serviceIndexUpdateNeeded.insert(make_pair(&collection, true));
					return true;
				}
				return it->second;
			}
		}



		boost::shared_ptr<LineString> Edge::getRealGeometry(
		) const	{

			assert(_fromVertex);
			const GeometryFactory& geometryFactory(
				CoordinatesSystem::GetDefaultGeometryFactory()
			);

			if(	_next &&
				_fromVertex->hasGeometry() &&
				_next->_fromVertex->hasGeometry()
			){
				CoordinateSequence* cs(geometryFactory.getCoordinateSequenceFactory()->create(0, 2));
				cs->add(*_fromVertex->getGeometry()->getCoordinate(), false);
				cs->add(*_next->_fromVertex->getGeometry()->getCoordinate(), false);
				if(cs->size() != 2)
				{
					return boost::shared_ptr<LineString>();
				}
				else
				{
					return boost::shared_ptr<LineString>(geometryFactory.createLineString(cs));
				}
			}

			return boost::shared_ptr<LineString>(geometryFactory.createLineString());
		}



		MetricOffset Edge::getEndMetricOffset() const
		{
			Edge* nextEdge(getNext());
			return nextEdge ? nextEdge->getMetricOffset() : getMetricOffset();
		}



		Edge::DepartureServiceIndices::mapped_type& Edge::getDepartureIndex(
			const ChronologicalServicesCollection& collection
		) const	{
			DepartureServiceIndices::iterator it(_departureIndex.find(&collection));
			if(it == _departureIndex.end())
		{
				it = _departureIndex.insert(
					make_pair(
						&collection,
						DepartureServiceIndices::mapped_type(INDICES_NUMBER)
				)	).first;
		}
			return it->second;
		}



		Edge::ArrivalServiceIndices::mapped_type& Edge::getArrivalIndex(
			const ChronologicalServicesCollection& collection
		) const	{
			ArrivalServiceIndices::iterator it(_arrivalIndex.find(&collection));
			if(it == _arrivalIndex.end())
			{
				it = _arrivalIndex.insert(
					make_pair(
						&collection,
						ArrivalServiceIndices::mapped_type(INDICES_NUMBER)
				)	).first;
			}
			return it->second;
		}
}	}
