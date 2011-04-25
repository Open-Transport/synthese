
/** VehiclePosition class header.
	@file VehiclePosition.hpp

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
		class Vehicle;
		class Depot;

		/** VehiclePosition class.
			@ingroup m37
		*/
		class VehiclePosition:
			public util::Registrable,
			public WithGeometry<geos::geom::Point>
		{
		public:
			typedef util::Registry<Vehicle> Registry;
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
				UNKNOWN_STATUS = 7,
				REFUELING = 8
			} Status;

			static std::string GetStatusName(Status value);
			static std::vector<std::pair<boost::optional<Status>, std::string> > GetStatusList();


		private:
			Status _status;
			Vehicle* _vehicle;
			boost::posix_time::ptime _time;
			Meters _meterOffset;
			pt::StopPoint* _stopPoint;
			Depot* _depot;
			std::string _comment;
			pt::ScheduledService* _service;
			boost::optional<std::size_t> _rankInPath;
			std::size_t _passengers;

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
			void setDepot(Depot* value){ _depot = value; }

			const Status& getStatus() const { return _status; }
			Vehicle* getVehicle() const { return _vehicle; }
			const boost::posix_time::ptime& getTime() const { return _time; }
			const Meters& getMeterOffset() const { return _meterOffset; }
			pt::StopPoint* getStopPoint() const { return _stopPoint; }
			const std::string& getComment() const { return _comment; }
			pt::ScheduledService* getService() const { return _service; }
			boost::optional<std::size_t> getRankInPath() const { return _rankInPath; }
			std::size_t getPassengers() const { return _passengers; }
			Depot* getDepot() const { return _depot; }
		};
}	}

#endif // SYNTHESE_pt_operation_VehiclePosition_hpp__
