
/** VDVClientSubscription class implementation.
	@file VDVClientSubscription.cpp

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

#include "VDVClientSubscription.hpp"

#include "CommercialLine.h"
#include "JourneyPattern.hpp"
#include "ScheduledService.h"
#include "VDVClient.hpp"

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace departure_boards;
	using namespace pt;
	using namespace util;

	

	namespace data_exchange
	{
		const std::string VDVClientSubscription::ATTR_ID = "id";
		const std::string VDVClientSubscription::ATTR_END_TIME = "end_time";
		const std::string VDVClientSubscription::TAG_STOP_AREA = "stop_area";
		const std::string VDVClientSubscription::TAG_LINE = "line";
		const std::string VDVClientSubscription::ATTR_TIME_SPAN ="time_span";
		const std::string VDVClientSubscription::ATTR_HYSTERESIS = "hysteresis";
		const std::string VDVClientSubscription::ATTR_DIRECTION_FILTER = "direction_filter";

		
		
		void VDVClientSubscription::buildGenerator() const
		{
			ArrivalDepartureTableGenerator::PhysicalStops ps(_stopArea->getPhysicalStops());
			DeparturesTableDirection di(DISPLAY_DEPARTURES);
			EndFilter ef(WITH_PASSING);
			LineFilter lf;
			lf.insert(make_pair(_line, optional<bool>()));
			DisplayedPlacesList dp;
			ForbiddenPlacesList fp;
			ptime now(second_clock::local_time());
			ptime end(now + _timeSpan);

			_generator.reset(
				new StandardArrivalDepartureTableGenerator(
					ps,
					di,
					ef,
					lf,
					dp,
					fp,
					now,
					end,
					false
			)	);
		}



		bool VDVClientSubscription::checkUpdate() const
		{
			buildGenerator();

			const ArrivalDepartureList& result(_generator->generate());
			_result.clear();
			BOOST_FOREACH(const ArrivalDepartureList::value_type& it1, result)
			{
				// Jump over non scheduled services
				if(	!dynamic_cast<const ScheduledService*>(it1.first.getService())
				){
					continue;
				}

				// Jump over services filtered by direction
				const JourneyPattern& jp(
					*static_cast<const JourneyPattern*>(it1.first.getService()->getPath())
				);
				if(	!_directionFilter.empty() &&
					_directionFilter != _vdvClient->getDirectionID(jp)
				){
					continue;
				}

				_result.insert(
					make_pair(
						it1.first.getService(),
						it1.first
				)	);
			}

			_addings.clear();
			_deletions.clear();
			
			
			// Addings or upddates
			BOOST_FOREACH(const ServicesList::value_type& it1, _result)
			{
				ServicesList::const_iterator it(_lastResult.find(it1.first));
				if(	it == _lastResult.end() ||
					it->second.hysteresisCompare(it1.second, _hysteresis)
				){
					_addings.insert(it1);
				}
			}

			// Deletions
			BOOST_FOREACH(const ServicesList::value_type& it2, _lastResult)
			{
				ServicesList::const_iterator it(_result.find(it2.first));
				if(it == _result.end())
				{
					_deletions.insert(it2);
				}
			}

			return !_addings.empty() || !_deletions.empty();
		}


		VDVClientSubscription::VDVClientSubscription(
			const std::string& id,
			const VDVClient& vdvClient
		):	_id(id),
			_endTime(not_a_date_time),
			_stopArea(NULL),
			_line(NULL),
			_timeSpan(hours(1)),
			_hysteresis(minutes(1)),
			_vdvClient(&vdvClient)
		{}



		void VDVClientSubscription::toParametersMap( util::ParametersMap& pm ) const
		{
			pm.insert(ATTR_ID, _id);
			if(!_endTime.is_not_a_date_time())
			{
				pm.insert(ATTR_END_TIME, _endTime);
			}
			if(_stopArea)
			{
				boost::shared_ptr<ParametersMap> stopPM(new ParametersMap);
				_stopArea->toParametersMap(*stopPM);
				pm.insert(TAG_STOP_AREA, stopPM);
			}
			if(_line)
			{
				boost::shared_ptr<ParametersMap> linePM(new ParametersMap);
				_line->toParametersMap(*linePM);
				pm.insert(TAG_LINE, linePM);
			}
			pm.insert(ATTR_TIME_SPAN, _timeSpan.total_seconds() / 60);
			pm.insert(ATTR_HYSTERESIS, _hysteresis.total_seconds());
			pm.insert(ATTR_DIRECTION_FILTER, _directionFilter);
		}
}	}

