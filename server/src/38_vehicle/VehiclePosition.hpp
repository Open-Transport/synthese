
/** VehiclePosition class header.
	@file VehiclePosition.hpp

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

#ifndef SYNTHESE_pt_operation_VehiclePosition_hpp__
#define SYNTHESE_pt_operation_VehiclePosition_hpp__

#include "Object.hpp"

#include "EnumObjectField.hpp"
#include "GeometryField.hpp"
#include "PtimeField.hpp"
#include "StringField.hpp"
#include "WithGeometry.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/optional.hpp>

namespace geos
{
	namespace geom
	{
		class Point;
	}
}

namespace synthese
{
	namespace pt
	{
		class StopPoint;
		class ScheduledService;
	}

	namespace pt_operation
	{
		class Depot;
	}

	namespace util
	{
		class ParametersMap;
	}

	namespace vehicle
	{
		class Vehicle;

		typedef enum
		{
			TRAINING = 0,
			DEAD_RUN_DEPOT = 1,
			DEAD_RUN_TRANSFER = 2,
			SERVICE = 3,
			COMMERCIAL = 4,
			NOT_IN_SERVICE = 5,
			OUT_OF_SERVICE = 6,
			UNKNOWN_STATUS = 999,
			REFUELING = 8,
			TERMINUS_START = 9
		} VehiclePositionStatusEnum;

		FIELD_ENUM(VehiclePositionStatus, VehiclePositionStatusEnum)
		FIELD_POINTER(VehiclePositionVehicle, Vehicle)
		FIELD_DOUBLE(MeterOffset)
		FIELD_ID(StopPointId)
		FIELD_STRING(Comment)
		FIELD_POINTER(VehiclePositionService, pt::ScheduledService)
		FIELD_SIZE_T(VehiclePositionRankInPath)
		FIELD_SIZE_T(Passengers)
		FIELD_BOOL(InStopArea)
		FIELD_PTIME(NextStopFoundTime)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(VehiclePositionStatus),
			FIELD(VehiclePositionVehicle),
			FIELD(Time),
			FIELD(MeterOffset),
			FIELD(StopPointId),
			FIELD(Comment),
			FIELD(VehiclePositionService),
			FIELD(VehiclePositionRankInPath),
			FIELD(Passengers),
			FIELD(InStopArea),
			FIELD(NextStopFoundTime),
			FIELD(PointGeometry)
		> VehiclePositionSchema;

		/** VehiclePosition class.
			@ingroup m38
		*/
		class VehiclePosition:
			public Object<VehiclePosition, VehiclePositionSchema>,
			public WithGeometry<geos::geom::Point>
		{
		private:
			static const std::string ATTR_X;
			static const std::string ATTR_Y;
			static const std::string ATTR_METER_OFFSET;
			static const std::string ATTR_STATUS;
			static const std::string ATTR_IN_STOP_AREA;
			static const std::string ATTR_STOP_FOUND_TIME;
			static const std::string ATTR_VEHICLE_NUMBER;
			static const std::string TAG_STOP;
			static const std::string TAG_SERVICE;

		public:
			typedef double Meters;

			static std::string GetStatusName(VehiclePositionStatusEnum value);
			static std::vector<std::pair<boost::optional<VehiclePositionStatusEnum>, std::string> > GetStatusList();


		private:
			std::string _vehicleNumber;
			pt::StopPoint* _stopPoint;
			pt_operation::Depot* _depot;

		public:
			VehiclePosition(
				util::RegistryKeyType id = 0
			);

			void setStatus(VehiclePositionStatusEnum value){ set<VehiclePositionStatus>(value); }
			void setVehicle(Vehicle* value);
			void setTime(const boost::posix_time::ptime& value){ set<Time>(value); }
			void setMeterOffset(const Meters& value){ set<MeterOffset>(value); }
			void setStopPoint(pt::StopPoint* value);
			void setComment(const std::string& value){ set<Comment>(value); }
			void setService(pt::ScheduledService* value);
			void setRankInPath(boost::optional<std::size_t> value);
			void setPassengers(std::size_t value){ set<Passengers>(value); }
			void setDepot(pt_operation::Depot* value);
			void setInStopArea(bool value){ set<InStopArea>(value); }
			void setVehicleNumber(const std::string& value){ _vehicleNumber = value; }
			void setNextStopFoundTime(const boost::posix_time::ptime& value){ set<NextStopFoundTime>(value); }

			const VehiclePositionStatusEnum& getStatus() const { return get<VehiclePositionStatus>(); }
			Vehicle* getVehicle() const;
			const boost::posix_time::ptime& getTime() const { return get<Time>(); }
			const Meters& getMeterOffset() const { return get<MeterOffset>(); }
			pt::StopPoint* getStopPoint() const { return _stopPoint; }
			const std::string& getComment() const { return get<Comment>(); }
			pt::ScheduledService* getService() const;
			boost::optional<std::size_t> getRankInPath() const;
			std::size_t getPassengers() const { return get<Passengers>(); }
			pt_operation::Depot* getDepot() const { return _depot; }
			bool getInStopArea() const { return get<InStopArea>(); }
			const std::string& getVehicleNumber() const { return _vehicleNumber; }
			const boost::posix_time::ptime& getNextStopFoundTime() const { return get<NextStopFoundTime>(); }

			virtual void toParametersMap(util::ParametersMap& pm,
				bool withAdditionalParameters,
				boost::logic::tribool withFiles = boost::logic::indeterminate,
				std::string prefix = std::string()) const;

			virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_pt_operation_VehiclePosition_hpp__
