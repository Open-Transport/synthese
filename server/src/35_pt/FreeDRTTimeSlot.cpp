
/** FreeDRTTimeSlot class implementation.
	@file FreeDRTTimeSlot.cpp

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

#include "FreeDRTTimeSlot.hpp"

#include "CalendarLinkTableSync.hpp"
#include "FreeDRTArea.hpp"
#include "Place.h"
#include "Profile.h"
#include "PTUseRuleTableSync.h"
#include "TransportNetworkRight.h"
#include "User.h"

#include <geos/geom/Point.h>

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace graph;
	using namespace pt;
	using namespace util;

	CLASS_DEFINITION(FreeDRTTimeSlot, "t083_free_drt_time_slots", 83)
	FIELD_DEFINITION_OF_OBJECT(FreeDRTTimeSlot, "free_drt_time_slot_id", "free_drt_time_slot_ids")

	FIELD_DEFINITION_OF_TYPE(Area, "area_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(FreeDRTServiceNumber, "service_number", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(FirstDeparture, "first_departure", SQL_TIME)
	FIELD_DEFINITION_OF_TYPE(LastArrival, "last_arrival", SQL_TIME)
	FIELD_DEFINITION_OF_TYPE(MaxCapacity, "max_capacity", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(CommercialSpeed, "commercial_speed", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(MaxSpeed, "max_speed", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(FreeDRTTimeSlotUseRules, "use_rules", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(FreeDRTTimeSlotDates, "dates", SQL_TEXT)

	namespace pt
	{
		FreeDRTTimeSlot::FreeDRTTimeSlot(
			RegistryKeyType id
		):
			Registrable(id),
			Object<FreeDRTTimeSlot, FreeDRTTimeSlotSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Area),
					FIELD_DEFAULT_CONSTRUCTOR(FreeDRTServiceNumber),
					FIELD_VALUE_CONSTRUCTOR(FirstDeparture, not_a_date_time),
					FIELD_VALUE_CONSTRUCTOR(LastArrival, not_a_date_time),
					FIELD_DEFAULT_CONSTRUCTOR(MaxCapacity),
					FIELD_VALUE_CONSTRUCTOR(CommercialSpeed, 0),
					FIELD_VALUE_CONSTRUCTOR(MaxSpeed, 0),
					FIELD_DEFAULT_CONSTRUCTOR(FreeDRTTimeSlotUseRules),
					FIELD_DEFAULT_CONSTRUCTOR(FreeDRTTimeSlotDates)
			)	),
			NonPermanentService(id)
		{
		}



		boost::posix_time::time_duration FreeDRTTimeSlot::getJourneyDuration(
			const geography::Place& from,
			const geography::Place& to
		) const	{

			// Distance
			double distance(from.getPoint()->distance(to.getPoint().get()));

			// Time
			return minutes(long(0.06 * (distance / getCommercialSpeed())));
		}



		Journey FreeDRTTimeSlot::getJourneyDepartureToArrival(
			const boost::posix_time::ptime time,
			const geography::Place& from,
			const geography::Place& to
		) const	{


			ServicePointer sp;

			// TODO Implementation

			return Journey();
		}



		Journey FreeDRTTimeSlot::getJourneyArrivalToDeparture(
			const boost::posix_time::ptime time,
			const geography::Place& from,
			const geography::Place& to
		) const	{

			ServicePointer sp;

			// TODO Implementation

			return Journey();

		}


		FreeDRTArea* FreeDRTTimeSlot::getArea() const
		{
			return static_cast<FreeDRTArea*>(_path);
		}



		void FreeDRTTimeSlot::setArea( FreeDRTArea* value )
		{
			_path = static_cast<Path*>(value);
			set<Area>(value
				? boost::optional<FreeDRTArea&>(*value)
				: boost::none);
		}



		void FreeDRTTimeSlot::_computeNextRTUpdate()
		{

		}



		bool FreeDRTTimeSlot::isContinuous() const
		{
			return true;
		}



		boost::posix_time::time_duration FreeDRTTimeSlot::getDepartureSchedule( bool RTData, std::size_t rank ) const
		{
			return getFirstDeparture();
		}



		boost::posix_time::time_duration FreeDRTTimeSlot::getArrivalSchedule( bool RTData, std::size_t rank ) const
		{
			return getLastArrival();
		}



		const boost::posix_time::time_duration& FreeDRTTimeSlot::getLastArrivalSchedule( bool RTData ) const
		{
			return getLastArrival();
		}



		ServicePointer FreeDRTTimeSlot::getFromPresenceTime(
			const AccessParameters& accessParameters,
			bool THData,
			bool RTData,
			bool getDeparture,
			const graph::Edge& edge,
			const boost::posix_time::ptime& presenceDateTime,
			bool checkIfTheServiceIsReachable,
			bool inverted,
			bool ignoreReservation,
			bool allowCanceled,
			UseRule::ReservationDelayType reservationRulesDelayType
		) const	{
			return ServicePointer();
		}



		void FreeDRTTimeSlot::completeServicePointer( graph::ServicePointer& servicePointer, const graph::Edge& edge, const graph::AccessParameters& accessParameters ) const
		{

		}



		boost::posix_time::time_duration FreeDRTTimeSlot::getDepartureBeginScheduleToIndex( bool RTData, std::size_t rankInPath ) const
		{
			return getFirstDeparture();
		}



		boost::posix_time::time_duration FreeDRTTimeSlot::getDepartureEndScheduleToIndex( bool RTData, std::size_t rankInPath ) const
		{
			return getLastArrival();
		}



		boost::posix_time::time_duration FreeDRTTimeSlot::getArrivalBeginScheduleToIndex( bool RTData, std::size_t rankInPath ) const
		{
			return getFirstDeparture();
		}



		boost::posix_time::time_duration FreeDRTTimeSlot::getArrivalEndScheduleToIndex( bool RTData, std::size_t rankInPath ) const
		{
			return getLastArrival();
		}



		graph::UseRule::ReservationAvailabilityType FreeDRTTimeSlot::getReservationAbility(
			const boost::gregorian::date& date,
			std::size_t userClassRank
		) const	{

			if(!isActive(date))
			{
				return UseRule::RESERVATION_FORBIDDEN;
			}

			return UseRule::RESERVATION_COMPULSORY_POSSIBLE; // TODO temporary
		}



		boost::posix_time::ptime FreeDRTTimeSlot::getReservationDeadLine(
			const boost::gregorian::date& date,
			std::size_t userClassRank
		) const	{

			return ptime(date, getLastArrival()); // TODO apply reservation rule delay
		}

		void FreeDRTTimeSlot::toParametersMap( util::ParametersMap& map, bool withAdditionalParameters, boost::logic::tribool withFiles /*= boost::logic::indeterminate*/, std::string prefix /*= std::string() */ ) const
		{
			Service::toParametersMap(map, withAdditionalParameters, withFiles, prefix);

			map.insert(TABLE_COL_ID, getKey());
			map.insert(FirstDeparture::FIELD.name, getFirstDeparture());
			map.insert(LastArrival::FIELD.name, getLastArrival());
			map.insert(MaxCapacity::FIELD.name, getMaxCapacity());
			map.insert(CommercialSpeed::FIELD.name, getCommercialSpeed());
			map.insert(MaxSpeed::FIELD.name, getMaxSpeed());
			map.insert(
				BikeComplianceId::FIELD.name,
				(	getRule(USER_BIKE) && dynamic_cast<const PTUseRule*>(getRule(USER_BIKE)) ?
					static_cast<const PTUseRule*>(getRule(USER_BIKE))->getKey() :
					RegistryKeyType(0)
			)	);
			map.insert(
				HandicappedComplianceId::FIELD.name,
				(	getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(getRule(USER_HANDICAPPED)) ?
					static_cast<const PTUseRule*>(getRule(USER_HANDICAPPED))->getKey() :
					RegistryKeyType(0)
			)	);
			map.insert(
				PedestrianComplianceId::FIELD.name,
				(	getRule(USER_PEDESTRIAN) && dynamic_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN)) ?
					static_cast<const PTUseRule*>(getRule(USER_PEDESTRIAN))->getKey() :
					RegistryKeyType(0)
			)	);
			// Dates preparation
			stringstream datesStr;
			serialize(datesStr);
			map.insert(FreeDRTTimeSlotDates::FIELD.name, datesStr.str());
		}

		void FreeDRTTimeSlot::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if (get<Area>())
			{
				_path = static_cast<Path*>(get<Area>().get_ptr());
			}

			RuleUser::setRules(
				PTUseRuleTableSync::UnserializeUseRules(
					get<FreeDRTTimeSlotUseRules>(),
					env
			)	);

			// Search of calendar template links (overrides manually defined calendar)
			calendar::CalendarLinkTableSync::SearchResult links(
				calendar::CalendarLinkTableSync::Search(
					env,
					getKey()
			)	);
			if(links.empty())
			{
				setFromSerializedString(get<FreeDRTTimeSlotDates>());
			}
			else
			{
				BOOST_FOREACH(const boost::shared_ptr<calendar::CalendarLink>& link, links)
				{
					addCalendarLink(*link, false);
				}
			}

			// Registration in the line and in the path
			if(getArea())
			{
				// Registration in the path
				getArea()->addService(*this, false);

				// Registration in the line
				if(getArea()->getLine())
				{
					getArea()->getLine()->registerService(*this);
				}
			}
		}

		void FreeDRTTimeSlot::unlink()
		{
			if(getArea())
			{
				// Unregister from the area
				getPath()->removeService(*this);

				// Unregister from the line
				if (getArea()->getLine())
				{
					getArea()->getLine()->unregisterService(*this);
				}
			}
		}

		void FreeDRTTimeSlot::setRules(const Rules& value)
		{
			RuleUser::setRules(value);
			string strUseRules = PTUseRuleTableSync::SerializeUseRules(value);
			set<FreeDRTTimeSlotUseRules>(strUseRules);
		}

		bool FreeDRTTimeSlot::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::WRITE);
		}

		bool FreeDRTTimeSlot::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::WRITE);
		}

		bool FreeDRTTimeSlot::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(security::DELETE_RIGHT);
		}
}	}
