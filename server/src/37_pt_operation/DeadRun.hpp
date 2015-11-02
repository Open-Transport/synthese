
/** DeadRun class header.
	@file DeadRun.hpp

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

#ifndef SYNTHESE_pt_operation_DeadRun_hpp__
#define SYNTHESE_pt_operation_DeadRun_hpp__

#include "Object.hpp"

#include "ImportableTemplate.hpp"
#include "Path.h"
#include "SchedulesBasedService.h"
#include "WithGeometry.hpp"

namespace synthese
{
	namespace pt
	{
		class StopPoint;
		class TransportNetwork;
	}

	namespace pt_operation
	{
		class Depot;
		class OperationUnit;

		FIELD_POINTER(DeadRunNetwork, pt::TransportNetwork)
		FIELD_ID(DepotId)
		FIELD_ID(StopId)
		FIELD_BOOL(Direction)
		FIELD_DOUBLE(Length)
		FIELD_DATASOURCE_LINKS(DeadRunDataSource)
		FIELD_POINTER(DeadRunOperationUnit, OperationUnit)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(DeadRunNetwork),
			FIELD(DepotId),
			FIELD(StopId),
			FIELD(Direction),
			FIELD(pt::ServiceSchedules),
			FIELD(pt::ServiceDates),
			FIELD(pt::ServiceNumber),
			FIELD(Length),
			FIELD(DeadRunDataSource),
			FIELD(DeadRunOperationUnit)
		> DeadRunSchema;

		/** Dead run class.
			@ingroup m37
		*/
		class DeadRun:
			public Object<DeadRun, DeadRunSchema>,
			public pt::SchedulesBasedService,
			public graph::Path,
			public impex::ImportableTemplate<DeadRun>
		{
		public:
			typedef util::Registry<DeadRun> Registry;

		private:
			static const std::string ATTR_DEP_PLACE_NAME;
			static const std::string ATTR_ARR_PLACE_NAME;
			static const std::string ATTR_DEP_SCHEDULE;
			static const std::string ATTR_ARR_SCHEDULE;


		public:
			DeadRun(
				util::RegistryKeyType id = 0,
				std::string number = std::string()
			);
			~DeadRun();

			//! @name Modifiers
			//@{
				void setRoute(
					Depot& depot,
					pt::StopPoint& stop,
					double length,
					bool fromDepotToStop
				);

				void setUndefined();
			//@}

			//! @name Getters
			//@{
				pt::TransportNetwork* getTransportNetwork() const;
				const boost::optional<OperationUnit&> getOperationUnit() const;
			//@}

			//! @name Setters
			//@{
				void setTransportNetwork(pt::TransportNetwork* value);
				void setOperationUnit(const boost::optional<pt_operation::OperationUnit&>& value);
				virtual void setServiceNumber(std::string serviceNumber);
			//@}

			//! @name Services
			//@{
				bool getFromDepotToStop() const;
				bool isUndefined() const;
				Depot* getDepot() const;
				pt::StopPoint* getStop() const;

				virtual bool isContinuous(void) const;
				virtual boost::posix_time::time_duration getDepartureBeginScheduleToIndex(bool,size_t) const;
				virtual boost::posix_time::time_duration getDepartureEndScheduleToIndex(bool,size_t) const;
				virtual boost::posix_time::time_duration getArrivalBeginScheduleToIndex(bool,size_t) const;
				virtual boost::posix_time::time_duration getArrivalEndScheduleToIndex(bool,size_t) const;
				virtual graph::ServicePointer getFromPresenceTime(const graph::AccessParameters&, bool, bool,bool, const synthese::graph::Edge &,const boost::posix_time::ptime &,bool,bool,bool,bool, graph::UseRule::ReservationDelayType = graph::UseRule::RESERVATION_EXTERNAL_DELAY) const;
				virtual void completeServicePointer(synthese::graph::ServicePointer &,const synthese::graph::Edge &,const synthese::graph::AccessParameters &) const;
				virtual bool isPedestrianMode(void) const;
				virtual bool isActive(const boost::gregorian::date &) const;
				virtual std::string getRuleUserName() const;

				virtual void toParametersMap(
					util::ParametersMap& pm,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;

				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();

				virtual LinkedObjectsIds getLinkedObjectsIds(
					const Record& record
				) const;
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
	}
}

#endif
