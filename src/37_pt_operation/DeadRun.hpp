
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

#include "SchedulesBasedService.h"
#include "Path.h"
#include "WithGeometry.hpp"
#include "ImportableTemplate.hpp"

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

		/** Dead run class.
			@ingroup m37
		*/
		class DeadRun:
			public pt::SchedulesBasedService,
			public graph::Path,
			public impex::ImportableTemplate<DeadRun>
		{
		public:
			typedef util::Registry<DeadRun> Registry;

		private:
			pt::TransportNetwork* _network;
			boost::optional<pt_operation::OperationUnit&> _operationUnit;


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
				pt::TransportNetwork* getTransportNetwork() const { return _network; }
				const boost::optional<pt_operation::OperationUnit&>& getOperationUnit() const { return _operationUnit; }
			//@}

			//! @name Setters
			//@{
				void setTransportNetwork(pt::TransportNetwork* value){ _network = value; }
				void setOperationUnit(const boost::optional<pt_operation::OperationUnit&>& value){ _operationUnit = value; }
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

				
				virtual bool loadFromRecord(
					const Record& record,
					util::Env& env
				);

				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();

				virtual LinkedObjectsIds getLinkedObjectsIds(
					const Record& record
				) const;
			//@}
		};
	}
}

#endif
