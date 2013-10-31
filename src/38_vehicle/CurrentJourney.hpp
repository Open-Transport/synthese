
/** CurrentJourney class header.
	@file CurrentJourney.hpp

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

#ifndef SYNTHESE_vehicle_CurrentJourney_hpp__
#define SYNTHESE_vehicle_CurrentJourney_hpp__

#include "NextStop.hpp"

#include <boost/thread/mutex.hpp>
#include <vector>

namespace synthese
{
	namespace util
	{
		class ParametersMap;
	}

	namespace pt
	{
		class CommercialLine;
	}

	namespace vehicle
	{
		/** CurrentJourney class.
			@ingroup m38
		*/
		class CurrentJourney
		{
		public:
			typedef std::vector<NextStop> NextStops;

		private:
			static const std::string ATTR_STOP_REQUESTED;
			static const std::string ATTR_LINE_NUMBER;
			static const std::string TAG_NEXT_STOP;
			static const std::string TAG_COMMERCIAL_LINE;
			static const std::string TAG_TERMINUS_DEPARTURE_TIME;
			static const std::string TAG_TERMINUS_SECONDS;

			mutable boost::mutex _mutex;
			NextStops _nextStops;
			pt::CommercialLine* _line;
			std::string _lineNumber;
			bool _stopRequested;
			boost::posix_time::time_duration _terminusDeparture;
			
		public:
			CurrentJourney();

			void setNextStops(const NextStops& value);
			void setLine(pt::CommercialLine* value){ _line = value; }
			void setLineNumber(const std::string& value);
			void setStopRequested(bool value){ _stopRequested = value; }
			void setTerminusDeparture(boost::posix_time::time_duration value){ _terminusDeparture = value; }

			boost::mutex& getMutex() const { return _mutex; }

			/// @pre the object mutex must be locked as an iteration is done on the nextstops variable
			const NextStops& getNextStops() const { return _nextStops; }

			/// @pre the object mutex must be locked as an iteration is done on the nextstops variable
			NextStops& getNextStops(){ return _nextStops; }
			pt::CommercialLine* getLine() const { return _line; }

			/// @pre the object mutex must be locked as the line number is used
			const std::string& getLineNumber() const { return _lineNumber; }
			bool getStopRequested() const { return _stopRequested; }
			boost::posix_time::time_duration getTerminusDeparture() const { return _terminusDeparture; }

			void toParametersMap(
				util::ParametersMap& pm
			) const;
		};
	}
}

#endif // SYNTHESE_vehicle_CurrentJourney_hpp__
