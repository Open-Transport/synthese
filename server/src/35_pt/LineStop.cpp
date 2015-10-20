
/** LineStop class implementation.
	@file LineStop.cpp

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

#include "LineStop.h"

#include "AreaGeneratedLineStop.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "DRTArea.hpp"
#include "SchedulesBasedService.h"
#include "StopPoint.hpp"

using namespace boost;
using namespace geos::geom;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace pt;

	CLASS_DEFINITION(LineStop, "t010_line_stops", 10)
	FIELD_DEFINITION_OF_OBJECT(LineStop, "line_stop_id", "line_stop_ids")

	FIELD_DEFINITION_OF_TYPE(LineNode, "physical_stop_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(Line, "line_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(RankInPath, "rank_in_path", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(IsDeparture, "is_departure", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(IsArrival, "is_arrival", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(MetricOffsetField, "metric_offset", SQL_DOUBLE)
	FIELD_DEFINITION_OF_TYPE(ScheduleInput, "schedule_input", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(InternalService, "internal_service", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(ReservationNeeded, "reservation_needed", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(ReverseDRTArea, "reverse_drt_area", SQL_BOOLEAN)

	namespace pt
	{
		LineStop::LineStop(
			RegistryKeyType id,
				JourneyPattern* line,
				std::size_t rankInPath,
				bool isDeparture,
				bool isArrival,
				double metricOffset,
				boost::optional<util::Registrable&> node
		):	Registrable(id),
			Object<LineStop, LineStopSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_VALUE_CONSTRUCTOR(LineNode, node),
					FIELD_VALUE_CONSTRUCTOR(Line, line ? optional<JourneyPattern&>(*line) : optional<JourneyPattern&>()),
					FIELD_VALUE_CONSTRUCTOR(RankInPath, rankInPath),
					FIELD_VALUE_CONSTRUCTOR(IsDeparture, isDeparture),
					FIELD_VALUE_CONSTRUCTOR(IsArrival, isArrival),
					FIELD_VALUE_CONSTRUCTOR(MetricOffsetField, metricOffset),
					FIELD_VALUE_CONSTRUCTOR(ScheduleInput, true),
					FIELD_VALUE_CONSTRUCTOR(InternalService, true),
					FIELD_VALUE_CONSTRUCTOR(ReservationNeeded, true),
					FIELD_VALUE_CONSTRUCTOR(ReverseDRTArea, false),
					FIELD_DEFAULT_CONSTRUCTOR(LineStringGeometry)
			)	)
		{}



		LineStop::~LineStop()
		{
			unlink();
		}



/*		bool LineStop::seemsGeographicallyConsistent (const LineStop& other) const
		{
			double deltaMO; // meters
			if ( getMetricOffset () > other.getMetricOffset () )
				deltaMO = ( getMetricOffset () - other.getMetricOffset () );
			else
				deltaMO = ( other.getMetricOffset () - getMetricOffset () );

			const double deltaGPS(
				getFromVertex()->getGeometry()->distance(other.getFromVertex()->getGeometry().get())
			);

			if ( deltaMO > 10 * deltaGPS && deltaMO - deltaGPS > 1 )
			{
				return false;
			}
			if ( deltaMO < deltaGPS && deltaGPS - deltaMO > 1 )
			{
				return false;
			}

			return true;
		}
*/


		void LineStop::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(get<Line>())
			{
				get<Line>()->addLineStop(*this);

				if(get<LineNode>())
				{
					if(dynamic_cast<StopPoint*>(&*get<LineNode>()))
					{
						bool generatedLineStoptoCreate(true);
						BOOST_FOREACH(GeneratedLineStops::value_type generatedLineStop, _generatedLineStops)
						{
							if (generatedLineStop->getLineStop() == this)
							{
								generatedLineStoptoCreate = false;
							}
						}
						
						if (generatedLineStoptoCreate)
						{
							boost::shared_ptr<LinePhysicalStop> generatedLineStop(
								new DesignatedLinePhysicalStop(
									*this
							)	);
							generatedLineStop->link();
							_generatedLineStops.push_back(generatedLineStop);
						}
					}
					else if(dynamic_cast<DRTArea*>(&*get<LineNode>()))
					{
						// Add links and generated line stops here
						if(get<IsArrival>() && !get<InternalService>())
						{
							_generateDRTAreaAllStops(true);
						}
						if(get<IsDeparture>())
						{
							_generateDRTAreaAllStops(false);
						}
						if(get<IsArrival>() && get<InternalService>())
						{
							_generateDRTAreaAllStops(true);
						}
					}
			}	}
		}



		void LineStop::unlink()
		{
			_generatedLineStops.clear();

			// Clear cache in case of non detected change in external objects (like path edges number)
			if(get<Line>())
			{
				get<Line>()->removeLineStop(*this);

				ServiceSet services(get<Line>()->getAllServices());
				BOOST_FOREACH(const ServiceSet::value_type& service, services)
				{
					if(dynamic_cast<SchedulesBasedService*>(service))
					{
						static_cast<SchedulesBasedService*>(service)->_clearGeneratedSchedules();
					}
				}
			}
		}



		void LineStop::_generateDRTAreaAllStops( bool isForArrival ) const
		{
			BOOST_FOREACH(StopArea* stopArea, dynamic_cast<DRTArea&>(*get<LineNode>()).get<Stops>())
			{
				BOOST_FOREACH(const StopArea::PhysicalStops::value_type& stopPoint, stopArea->getPhysicalStops())
				{
					bool generatedLineStoptoCreate(true);
					BOOST_FOREACH(GeneratedLineStops::value_type generatedLineStop, _generatedLineStops)
					{
						if (generatedLineStop->getLineStop() == this &&
							generatedLineStop->getPhysicalStop() == stopPoint.second &&
							generatedLineStop->isArrival() == isForArrival &&
							generatedLineStop->isDeparture() == !isForArrival)
						{
							generatedLineStoptoCreate = false;
						}
					}
					
					if (generatedLineStoptoCreate)
					{
						boost::shared_ptr<AreaGeneratedLineStop> generatedLineStop(
							new AreaGeneratedLineStop(
								const_cast<LineStop&>(*this),
								const_cast<StopPoint&>(*stopPoint.second),
								!isForArrival,
								isForArrival
						)	);
						generatedLineStop->link();
						_generatedLineStops.push_back(generatedLineStop);
					}
				}
			}
		}
}	}
