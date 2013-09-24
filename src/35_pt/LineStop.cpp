
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

#include "DBConstants.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "DRTAreaTableSync.hpp"
#include "JourneyPatternCopy.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineArea.hpp"
#include "LineStopTableSync.h"
#include "PTModule.h"
#include "SchedulesBasedService.h"
#include "StopArea.hpp"
#include "StopPointTableSync.hpp"
#include "Vertex.h"

#include <geos/geom/LineString.h>

using namespace boost;
using namespace geos::geom;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace graph;
	using namespace pt;

	namespace util
	{
		template<> const string Registry<pt::LineStop>::KEY("LineStop");
	}

	namespace pt
	{
		LineStop::LineStop(
			RegistryKeyType id,
			JourneyPattern* line,
			size_t rankInPath,
			bool isDeparture,
			bool isArrival,
			double metricOffset,
			Vertex* vertex
		):	Registrable(id),
			Edge(line, rankInPath, vertex, metricOffset),
			_isDeparture(isDeparture),
			_isArrival(isArrival)
		{}



		LineStop::~LineStop()
		{
		}



		bool LineStop::seemsGeographicallyConsistent (const LineStop& other) const
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



		JourneyPattern* LineStop::getLine() const
		{
			return static_cast<JourneyPattern*>(getParentPath());
		}


		void LineStop::setLine(JourneyPattern* line )
		{
			setParentPath(static_cast<Path*>(line));
		}



		bool LineStop::isDepartureAllowed() const
		{
			return _isDeparture;
		}



		bool LineStop::isArrivalAllowed() const
		{
			return _isArrival;
		}



		bool LineStop::loadFromRecord( const Record& record, util::Env& env )
		{
			bool result(false);

			// Geometry
			if(record.isDefined(TABLE_COL_GEOMETRY))
			{
				boost::shared_ptr<LineString> value;
				string viaPointsStr(record.get<string>(TABLE_COL_GEOMETRY));
				if(!viaPointsStr.empty())
				{
					value = boost::dynamic_pointer_cast<LineString, geos::geom::Geometry>(
						record.getGeometryFromWKT(TABLE_COL_GEOMETRY)
					);
				}
				if(	(!value && getGeometry()) ||
					(value && !getGeometry()) ||
					(value && getGeometry() && !value->equalsExact(getGeometry().get(), 0.01))
				){
					setGeometry(value);
					result = true;
				}
			}

			// Metric offset
			if(record.isDefined(LineStopTableSync::COL_METRICOFFSET))
			{
				double metricOffset(
					record.getDefault<MetricOffset>(
						LineStopTableSync::COL_METRICOFFSET,
						0
				)	);
				if(metricOffset != _metricOffset)
				{
					_metricOffset = metricOffset;
					result = true;
				}
			}

			// Is arrival
			if(record.isDefined(LineStopTableSync::COL_ISARRIVAL))
			{
				bool isArrival(
					record.getDefault<bool>(
						LineStopTableSync::COL_ISARRIVAL,
						true
				)	);
				if(isArrival != _isArrival)
				{
					_isArrival = isArrival;
					result = true;
				}
			}

			// Is departure
			if(record.isDefined(LineStopTableSync::COL_ISDEPARTURE))
			{
				bool isDeparture(
					record.getDefault<bool>(
						LineStopTableSync::COL_ISDEPARTURE,
						true
				)	);
				if(isDeparture != _isDeparture)
				{
					_isDeparture = isDeparture;
					result = true;
				}
			}

			// Rank in path
			if(record.isDefined(LineStopTableSync::COL_RANKINPATH))
			{
				size_t rankInPath(
					record.getDefault<int>(
						LineStopTableSync::COL_RANKINPATH,
						0
				)	);
				if(rankInPath != getRankInPath())
				{
					setRankInPath(rankInPath);
					result = true;
				}
			}

			// Line
			if(record.isDefined(LineStopTableSync::COL_LINEID))
			{
				JourneyPattern* value(NULL);
				util::RegistryKeyType lineId(
					record.getDefault<RegistryKeyType>(
						LineStopTableSync::COL_LINEID,
						0
				)	);
				if(lineId > 0)
				{
					value = JourneyPatternTableSync::GetEditable(lineId, env).get();
				}
				if(value != getLine())
				{
					setLine(value);
					result = true;
				}
			}

			// Stop / DRT area
			if(dynamic_cast<DesignatedLinePhysicalStop*>(this))
			{
				DesignatedLinePhysicalStop& dls(static_cast<DesignatedLinePhysicalStop&>(*this));

				// Schedule input
				if(record.isDefined(LineStopTableSync::COL_SCHEDULEINPUT))
				{
					bool value(
						record.getDefault<bool>(
							LineStopTableSync::COL_SCHEDULEINPUT,
							true
					)	);
					if(value != getScheduleInput())
					{
						dls.setScheduleInput(value);
						result = true;
					}
				}

				// Reservation needed
				if(record.isDefined(LineStopTableSync::COL_RESERVATION_NEEDED))
				{
					bool value(
						record.getDefault<bool>(
							LineStopTableSync::COL_RESERVATION_NEEDED,
							true
					)	);
					if(value != getReservationNeeded())
					{
						dls.setReservationNeeded(value);
						result = true;
					}
				}

//				if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
//				{
					// Stop point
					if(record.isDefined(LineStopTableSync::COL_PHYSICALSTOPID))
					{
						util::RegistryKeyType fromPhysicalStopId(
							record.getDefault<RegistryKeyType>(LineStopTableSync::COL_PHYSICALSTOPID, 0)
						);
						StopPoint* value(
							StopPointTableSync::GetEditable(fromPhysicalStopId, env).get()
						);

						if(dls.getPhysicalStop() != value)
						{
							dls.setPhysicalStop(*value);
							result = true;
						}
					}
//				}
			}
			else if(dynamic_cast<LineArea*>(this))
			{
				LineArea& lineArea(static_cast<LineArea&>(*this));

				// Internal service
				if(record.isDefined(LineStopTableSync::COL_INTERNAL_SERVICE))
				{
					bool value(
						record.getDefault<bool>(
							LineStopTableSync::COL_INTERNAL_SERVICE,
							true
					)	);
					if(value != lineArea.getInternalService())
					{
						lineArea.setInternalService(value);
						result = true;
					}
				}

//				if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
//				{
					// DRT Area
					if(record.isDefined(LineStopTableSync::COL_PHYSICALSTOPID))
					{
						RegistryKeyType areaId(
							record.getDefault<RegistryKeyType>(LineStopTableSync::COL_PHYSICALSTOPID, 0)
						);
						DRTArea* value(NULL);
						if(areaId) try
						{
							value = DRTAreaTableSync::GetEditable(areaId, env).get();
						}
						catch(ObjectNotFoundException<DRTArea>& e)
						{
							throw Exception(lexical_cast<string>(LineStopTableSync::COL_PHYSICALSTOPID)+" not found");
						}

						if(value != lineArea.getArea())
						{
							lineArea.setArea(*value);
							result = true;
						}
					}
//				}
			}

			// Clear cache in case of non detected change in external objects (like path edges number)
			if(getLine())
			{
				BOOST_FOREACH(const ServiceSet::value_type& service, getLine()->getServices())
				{
					if(dynamic_cast<SchedulesBasedService*>(service))
					{
						static_cast<SchedulesBasedService*>(service)->_clearGeneratedSchedules();
					}
				}
			}

			return result;
		}



		synthese::LinkedObjectsIds LineStop::getLinkedObjectsIds( const Record& record ) const
		{
			return LinkedObjectsIds();
		}



		void LineStop::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			DesignatedLinePhysicalStop* dls(
				dynamic_cast<DesignatedLinePhysicalStop*>(this)
			);
			LineArea* la(
				dynamic_cast<LineArea*>(this)
			);

			if(getLine())
			{
				// Sublines update
				if(dls)
				{
					// Line update
					getLine()->addEdge(*this);

					BOOST_FOREACH(JourneyPatternCopy* copy, getLine()->getSubLines())
					{
						DesignatedLinePhysicalStop* newEdge(
							new DesignatedLinePhysicalStop(
								0,
								copy,
								getRankInPath(),
								isDeparture(),
								isArrival(),
								getMetricOffset(),
								dls->getPhysicalStop(),
								dls->getScheduleInput()
						)	);
						copy->addEdge(*newEdge);

						// Links from stop to the linestop
						if(dls->getPhysicalStop())
						{
							if(getIsArrival())
							{
								dls->getPhysicalStop()->addArrivalEdge(newEdge);
							}
							if(getIsDeparture())
							{
								dls->getPhysicalStop()->addDepartureEdge(newEdge);
							}
						}
					}
				}
				if(la)
				{
					// Sublines update
					BOOST_FOREACH(JourneyPatternCopy* copy, getLine()->getSubLines())
					{
						LineArea* newEdge(
							new LineArea(
								0,
								copy,
								getRankInPath(),
								isDeparture(),
								isArrival(),
								getMetricOffset(),
								la->getArea(),
								la->getInternalService()
						)	);

						if(la->getArea())
						{
							// Add links and generated line stops here
							if(isArrivalAllowed() && !la->getInternalService())
							{
								newEdge->addAllStops(true);
							}
							if(isDepartureAllowed())
							{
								newEdge->addAllStops(false);
							}
							if(isArrivalAllowed() && la->getInternalService())
							{
								newEdge->addAllStops(true);
							}
						}
	
						// After addAllStops calls because _generatedLineStops must be populated befort addEdge
						copy->addEdge(*newEdge);
					}
				}

			}

			if(dls && dls->getPhysicalStop())
			{
				// Useful transfer calculation
				dls->getPhysicalStop()->getHub()->clearAndPropagateUsefulTransfer(PTModule::GRAPH_ID);

				
				// Links from stop to the linestop
				if(getIsArrival())
				{
					dls->getPhysicalStop()->addArrivalEdge((Edge*) this);
				}
				if(getIsDeparture())
				{
					dls->getPhysicalStop()->addDepartureEdge((Edge*) this);
				}
			}
			if(la && la->getArea())
			{
				// Useful transfer calculation
				BOOST_FOREACH(StopArea* stopArea, la->getArea()->get<Stops>())
				{
					stopArea->clearAndPropagateUsefulTransfer(PTModule::GRAPH_ID);
				}

				// Add links and generated line stops here
				if(isArrivalAllowed() && !la->getInternalService())
				{
					la->addAllStops(true);
				}
				if(isDepartureAllowed())
				{
					la->addAllStops(false);
				}
				if(isArrivalAllowed() && la->getInternalService())
				{
					la->addAllStops(true);
				}

				// Line update
				getLine()->addEdge(*this);
			}
		}
}	}
