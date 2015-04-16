
/** NextStop class header.
	@file NextStop.hpp

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

#ifndef SYNTHESE_vehicle_NextStop_hpp__
#define SYNTHESE_vehicle_NextStop_hpp__

#include <boost/date_time/posix_time/ptime.hpp>

namespace synthese
{
	namespace pt
	{
		class StopPoint;
	}

	namespace util
	{
		class ParametersMap;
	}

	namespace vehicle
	{
		/** NextStop class.
			@ingroup m38
		*/
		class NextStop
		{
		private:
			static const std::string TAG_STOP;
			static const std::string ATTR_STOP_NAME;
			static const std::string ATTR_ARRIVAL_TIME;
			static const std::string ATTR_ARRIVAL_DURATION;
			static const std::string ATTR_IN_STOP_AREA;
			static const std::string ATTR_RANK;

			bool _inStopArea;
			pt::StopPoint* _stop;
			std::string _stopName;
			boost::posix_time::ptime _arrivalTime;
			std::string _stopIdentifier;
			size_t _rank;

		public:
			NextStop();

			void setStop(pt::StopPoint* value){ _stop = value; }
			void setRank(size_t value){ _rank = value; }
			void setStopName(const std::string& value){ _stopName = value; }
			void setArrivalTime(const boost::posix_time::ptime& value){ _arrivalTime = value; }
			void setStopIdentifier(const std::string& value){ _stopIdentifier = value; }
			void setInStopArea(bool value){ _inStopArea = value; }

			const std::string& getStopIdentifier() const { return _stopIdentifier; } 
			bool getInStopArea() const { return _inStopArea; }
			pt::StopPoint* getStop() const { return _stop; }
			size_t getRank() const { return _rank; }
			const std::string& getStopName() const { return _stopName; }
			const boost::posix_time::ptime& getArrivalTime() const { return _arrivalTime; }

			void toParametersMap(
				util::ParametersMap& pm
			) const;
		};
}	}

#endif // SYNTHESE_vehicle_NextStop_hpp__
