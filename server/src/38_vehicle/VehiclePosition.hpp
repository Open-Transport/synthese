
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

#include "Registrable.h"
#include "Registry.h"
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

		/** VehiclePosition class.
			@ingroup m38
		*/
		class VehiclePosition:
			public util::Registrable,
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
			typedef util::Registry<VehiclePosition> Registry;

			typedef double Meters;

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
			} Status;

			static std::string GetStatusName(Status value);
			static std::vector<std::pair<boost::optional<Status>, std::string> > GetStatusList();


		private:
			Status _status;
			Vehicle* _vehicle;
			std::string _vehicleNumber;
			boost::posix_time::ptime _time;
			Meters _meterOffset;
			pt::StopPoint* _stopPoint;
			pt_operation::Depot* _depot;
			std::string _comment;
			pt::ScheduledService* _service;
			boost::optional<std::size_t> _rankInPath;
			std::size_t _passengers;
			bool _inStopArea;
			boost::posix_time::ptime _nextStopFoundTime;

		public:
			VehiclePosition(
				util::RegistryKeyType id = 0
			);

			void setStatus(Status value){ _status = value; }
			void setVehicle(Vehicle* value){ _vehicle = value; }
			void setTime(const boost::posix_time::ptime& value){ _time = value; }
			void setMeterOffset(const Meters& value){ _meterOffset = value; }
			void setStopPoint(pt::StopPoint* value){ _stopPoint = value; }
			void setComment(const std::string& value){ _comment = value; }
			void setService(pt::ScheduledService* value){ _service = value; }
			void setRankInPath(boost::optional<std::size_t> value){ _rankInPath = value; }
			void setPassangers(std::size_t value){ _passengers = value; }
			void setDepot(pt_operation::Depot* value){ _depot = value; }
			void setInStopArea(bool value){ _inStopArea = value; }
			void setVehicleNumber(const std::string& value){ _vehicleNumber = value; }
			void setNextStopFoundTime(const boost::posix_time::ptime& value){ _nextStopFoundTime = value; }

			const Status& getStatus() const { return _status; }
			Vehicle* getVehicle() const { return _vehicle; }
			const boost::posix_time::ptime& getTime() const { return _time; }
			const Meters& getMeterOffset() const { return _meterOffset; }
			pt::StopPoint* getStopPoint() const { return _stopPoint; }
			const std::string& getComment() const { return _comment; }
			pt::ScheduledService* getService() const { return _service; }
			boost::optional<std::size_t> getRankInPath() const { return _rankInPath; }
			std::size_t getPassengers() const { return _passengers; }
			pt_operation::Depot* getDepot() const { return _depot; }
			bool getInStopArea() const { return _inStopArea; }
			const std::string& getVehicleNumber() const { return _vehicleNumber; }
			const boost::posix_time::ptime& getNextStopFoundTime() const { return _nextStopFoundTime; }

			virtual void toParametersMap(util::ParametersMap& pm,
				bool withAdditionalParameters,
				boost::logic::tribool withFiles = boost::logic::indeterminate,
				std::string prefix = std::string()) const;
		};
}	}

#endif // SYNTHESE_pt_operation_VehiclePosition_hpp__
