
/** VDVClientSubscription class header.
	@file VDVClientSubscription.hpp

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

#ifndef SYNTHESE_data_exchange_VDVClientSubscription_hpp__
#define SYNTHESE_data_exchange_VDVClientSubscription_hpp__

#include "ServicePointer.h"
#include "StandardArrivalDepartureTableGenerator.h"
#include "DeparturesTableTypes.h"

namespace synthese
{
	namespace data_exchange
	{
		class VDVClient;



		/** Subscription to a VDV stream by a client.
			@ingroup m61
		*/
		class VDVClientSubscription
		{
		public:
			static const std::string ATTR_ID;
			static const std::string ATTR_END_TIME;
			static const std::string TAG_STOP_AREA;
			static const std::string TAG_LINE;
			static const std::string ATTR_TIME_SPAN;
			static const std::string ATTR_HYSTERESIS;
			static const std::string ATTR_DIRECTION_FILTER;

			typedef std::map<util::RegistryKeyType, graph::ServicePointer> ServicesList;

		private:
			const std::string _id;
			boost::posix_time::ptime _endTime;
			pt::StopArea* _stopArea;
			pt::CommercialLine* _line;
			std::string _directionFilter;
			boost::posix_time::time_duration _timeSpan;
			boost::posix_time::time_duration _hysteresis;
			const VDVClient* _vdvClient;
			
			mutable ServicesList _lastResult;
			mutable ServicesList _result;
			mutable ServicesList _addings;
			mutable ServicesList _deletions;
			mutable boost::shared_ptr<departure_boards::StandardArrivalDepartureTableGenerator> _generator;

		public:
			/// @name Getters
			//@{
				const std::string& getId() const { return _id; }
				const ServicesList& getAddings() const { return _addings; }
				const ServicesList& getDeletions() const { return _deletions; }
				pt::StopArea* getStopArea() const { return _stopArea; }
				const std::string& getDirectionFilter() const { return _directionFilter; }
			//@}

			/// @name Setters
			//@{
				void setStopArea(pt::StopArea* value){ _stopArea = value; }
				void setLine(pt::CommercialLine* value){ _line = value; }
				void setTimeSpan(const boost::posix_time::time_duration& value){ _timeSpan = value; }
				void setHysteresis(const boost::posix_time::time_duration& value){ _hysteresis = value; }
				void setDirectionFilter(const std::string& value){ _directionFilter = value; }
			//@}

			void buildGenerator() const;
			bool checkUpdate() const;
			void declareSending() const { _lastResult = _result; }

			void toParametersMap(util::ParametersMap& pm) const;

		public:
			VDVClientSubscription(
				const std::string& id,
				const VDVClient& vdvClient
			);
		};
}	}

#endif // SYNTHESE_data_exchange_VDVClientSubscription_hpp__
