
/** StopPointsListFunction class implementation.
	@file StopPointsListFunction.cpp
	@author Hugues Romain
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "ServicePointer.h"
#include "StopPointsListFunction.hpp"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "RollingStock.hpp"
#include "Edge.h"
#include "LineStop.h"
#include "SchedulesBasedService.h"
#include "JourneyPattern.hpp"
#include "CommercialLine.h"
#include "City.h"
#include "Webpage.h"

#include <sstream>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace geos::geom;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::algorithm;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace server;
	using namespace security;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function,pt::StopPointsListFunction>::FACTORY_KEY("StopPointsListFunction");

	namespace pt
	{
		const string StopPointsListFunction::PARAMETER_LINE_ID = "lineid";
		const string StopPointsListFunction::PARAMETER_DATE = "date";
		const string StopPointsListFunction::PARAMETER_PAGE_ID = "page_id";
		const string StopPointsListFunction::PARAMETER_BBOX = "bbox";
		const string StopPointsListFunction::PARAMETER_SRID = "srid";

		const string StopPointsListFunction::TAG_PHYSICAL_STOP = "physicalStop";
		const string StopPointsListFunction::TAG_DESTINATION = "destination";
		const string StopPointsListFunction::DATA_STOP_AREA_PREFIX = "stopArea_";
		const string StopPointsListFunction::DATA_STOPAREA_NAME = "stopAreaName";
		const string StopPointsListFunction::DATA_STOPAREA_CITY_NAME = "stopAreaCityName";



		ParametersMap StopPointsListFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_date && _date->is_not_a_date_time()) map.insert(PARAMETER_DATE, *_date);
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			if(_bbox)
			{
				stringstream s;
				s << _bbox->getMinX() << "," << _bbox->getMinY() << "," <<
					_bbox->getMaxX() << "," << _bbox->getMaxY();
				map.insert(PARAMETER_BBOX, s.str());
			}
			if(_coordinatesSystem)
			{
				map.insert(PARAMETER_SRID, static_cast<int>(_coordinatesSystem->getSRID()));
			}
			return map;
		}



		void StopPointsListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Coordinate system
			CoordinatesSystem::SRID srid(
				map.getDefault<CoordinatesSystem::SRID>(
					PARAMETER_SRID,
					CoordinatesSystem::GetInstanceCoordinatesSystem().getSRID()
			)	);
			_coordinatesSystem = &CoordinatesSystem::GetCoordinatesSystem(srid);
 
			// Stop area request
			if(map.getOptional<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)) try
			{
				_stopArea = Env::GetOfficialEnv().getRegistry<StopArea>().get(
					map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID)
				);
			}
			catch (ObjectNotFoundException<StopArea>&)
			{
				throw RequestException("No such stop area");
			}

			// Line request
			_commercialLineID = map.getOptional<RegistryKeyType>(PARAMETER_LINE_ID);

			// Date
			if(_commercialLineID) // Date is useful only if a line was given
			{
				if(!map.getDefault<string>(PARAMETER_DATE).empty() && map.getDefault<string>(PARAMETER_DATE) != "A")
				{
					_date = time_from_string(map.get<string>(PARAMETER_DATE));
				}
			}

			// BBox selection request
			string bbox(map.getDefault<string>(PARAMETER_BBOX));
			if(!bbox.empty())
			{
				vector< string > parsed_bbox;
				split(parsed_bbox, bbox, is_any_of(",; ") );

				if(parsed_bbox.size() != 4)
				{
					throw RequestException("Malformed bbox.");
				}

				shared_ptr<Point> pt1(
					_coordinatesSystem->createPoint(lexical_cast<double>(parsed_bbox[0]), lexical_cast<double>(parsed_bbox[1]))
				);
				shared_ptr<Point> pt2(
					_coordinatesSystem->createPoint(lexical_cast<double>(parsed_bbox[2]), lexical_cast<double>(parsed_bbox[3]))
				);
				pt1 = CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*pt1);
				pt2 = CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(*pt2);

				_bbox = Envelope(
					pt1->getX(),
					pt2->getX(),
					pt1->getY(),
					pt2->getY()
				);
			}

			// CMS output
			if(map.getOptional<RegistryKeyType>(PARAMETER_PAGE_ID)) try
			{
				_page = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_PAGE_ID));
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such page");
			}
		}



		void StopPointsListFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			ptime startDateTime,endDateTime;

			ptime date = _date ? *_date : second_clock::local_time();

			// and startDateTime is begin of the day (a day begin at 3:00):
			startDateTime = date - date.time_of_day() + hours(3);
			// and endDateTime is end of the day (a day end at 27:00):
			endDateTime = date - date.time_of_day() + hours(27);

			// Search for stopPoints
			StopPointMapType stopPointMap;

			if(_stopArea) // If a stopArea is provided : display only stop points of this stopArea
			{
				const StopArea::PhysicalStops& stops(_stopArea->get()->getPhysicalStops());
				BOOST_FOREACH(const StopArea::PhysicalStops::value_type& stopPoint, stops)
				{
					addStop(stopPointMap, *stopPoint.second, startDateTime, endDateTime);
				}
			}
			else
			{
				BOOST_FOREACH(const Registry<StopPoint>::value_type& stopPoint, Env::GetOfficialEnv().getRegistry<StopPoint>())
				{
					if(_bbox &&
						(!stopPoint.second->getGeometry() ||
						!_bbox->contains(*stopPoint.second->getGeometry()->getCoordinate())))
					{
						continue;
					}
					addStop(stopPointMap, *stopPoint.second, startDateTime, endDateTime);
				}
			}

			// Filling in the result parameters map
			ParametersMap pm;
			BOOST_FOREACH(const StopPointMapType::value_type& sp, stopPointMap)
			{
				// Declarations
				shared_ptr<ParametersMap> stopPM(new ParametersMap);

				// Main attributes
				sp.first->toParametersMap(
					*stopPM,
					false,
					*_coordinatesSystem
				);

				// Destinations
				BOOST_FOREACH(const StopAreaDestinationMapType::value_type& destination, sp.second)
				{
					// Main parameters
					shared_ptr<ParametersMap> destinationPM(new ParametersMap);
					destinationPM->insert("id", destination.first);
					destinationPM->insert("name", destination.second.first->getName());
					destinationPM->insert("cityName", destination.second.first->getCity()->getName());

					// Lines
					if(!_commercialLineID)
					{
						BOOST_FOREACH(const CommercialLineMapType::value_type& line, destination.second.second)
						{
							// Declaration
							shared_ptr<ParametersMap> linePM(new ParametersMap);

							// Main parameters
							line.second->toParametersMap(*linePM);

							// Rolling stock
							set<RollingStock *> rollingStocks;
							BOOST_FOREACH(Path* path, line.second->getPaths())
							{
								if(!dynamic_cast<const JourneyPattern*>(path))
									continue;

								if(!static_cast<const JourneyPattern*>(path)->getRollingStock())
									continue;

								rollingStocks.insert(
									static_cast<const JourneyPattern*>(path)->getRollingStock()
								);
							}
							BOOST_FOREACH(RollingStock * rs, rollingStocks)
							{
								shared_ptr<ParametersMap> transportModePM(new ParametersMap);
								rs->toParametersMap(*transportModePM);
								linePM->insert("transportMode", transportModePM);
							}

							destinationPM->insert("line", linePM);
						}
					}

					stopPM->insert(TAG_DESTINATION, destinationPM);
				}

				pm.insert(TAG_PHYSICAL_STOP, stopPM);
			}

			// Output
			if(_page.get()) // CMS output
			{
				// Declaration
				size_t rank(0);

				// Loop on each stop
				BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type subMap, pm.getSubMaps(TAG_PHYSICAL_STOP))
				{
					// Merge of CMS template parameters
					subMap->merge(getTemplateParameters());

					// Merge of stop area data
					vector<shared_ptr<ParametersMap> > stopAreaMap(subMap->getSubMaps(StopPoint::TAG_STOP_AREA));
					if(!stopAreaMap.empty())
					{
						subMap->merge(**stopAreaMap.begin(), DATA_STOP_AREA_PREFIX);
					}

					// Add rank data
					subMap->insert("rank", rank);
					++rank;

					// Display of the stop by the template
					_page->display(stream, request, *subMap);
				}
			}
			else
			{
				// Additional attributes for XML response
				if(_stopArea)
				{
					pm.insert(DATA_STOPAREA_NAME, _stopArea->get()->getName());
					pm.insert(DATA_STOPAREA_CITY_NAME, _stopArea->get()->getCity()->getName());
				}
				if(_commercialLineID) // destination of this line will be displayed
				{
					pm.insert("lineName", Env::GetOfficialEnv().getRegistry<CommercialLine>().get(*_commercialLineID)->getName());
					pm.insert("lineShortName", Env::GetOfficialEnv().getRegistry<CommercialLine>().get(*_commercialLineID)->getShortName());
					pm.insert("lineStyle", Env::GetOfficialEnv().getRegistry<CommercialLine>().get(*_commercialLineID)->getStyle());
				}

				pm.outputXML(
					stream,
					"physicalStops",
					true,
					"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/35_pt/StopPointsListFunction.xsd"
				);
			}
		}



		void StopPointsListFunction::addStop(
			StopPointMapType & stopPointMap,
			const StopPoint & sp,
			ptime & startDateTime,
			ptime & endDateTime
		) const {
			bool spHaveZeroDestination = true;
			BOOST_FOREACH(const Vertex::Edges::value_type& edge, sp.getDepartureEdges())
			{
				const LineStop* ls = static_cast<const LineStop*>(edge.second);

				ptime departureDateTime = startDateTime;
				// Loop on services
				optional<Edge::DepartureServiceIndex::Value> index;
				while(true)
				{
					ServicePointer servicePointer(
						ls->getNextService(
							USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET,
							departureDateTime,
							endDateTime,
							false,
							index,
							false,
							false
					)	);
					if (!servicePointer.getService())
						break;
					++*index;
					departureDateTime = servicePointer.getDepartureDateTime();
					if(sp.getKey() != servicePointer.getRealTimeDepartureVertex()->getKey())
						continue;

					const JourneyPattern* journeyPattern = dynamic_cast<const JourneyPattern*>(servicePointer.getService()->getPath());
					if(journeyPattern == NULL) // Could be a junction
						continue;

					const CommercialLine * commercialLine(journeyPattern->getCommercialLine());
					if(_commercialLineID && (commercialLine->getKey() != _commercialLineID))// only physicalStop used by the commercial line will be displayed
						continue;

					const StopArea * destination = journeyPattern->getDestination()->getConnectionPlace();

					if(_stopArea)
					{
						//Ignore if destination is the _stopArea himself
						if(destination->getKey() == _stopArea->get()->getKey())
							continue;
					}

					if(spHaveZeroDestination)
					{
						StopAreaDestinationMapType stopAreaMap;
						stopPointMap[&sp] = stopAreaMap;
						spHaveZeroDestination = false;
					}

					StopAreaDestinationMapType::iterator it = stopPointMap[&sp].find(destination->getKey());
					if(it == stopPointMap[&sp].end()) // test if destination stop already in the map
					{
						CommercialLineMapType lineMap;
						lineMap[commercialLine->getKey()] = commercialLine;
						stopPointMap[&sp][destination->getKey()] = make_pair(destination, lineMap);
					}
					else // destination stop is already in the map
					{
						CommercialLineMapType::iterator lineIt = stopPointMap[&sp][destination->getKey()].second.find(commercialLine->getKey());
						if(lineIt == stopPointMap[&sp][destination->getKey()].second.end()) // test if commercialLine already in the sub map
							stopPointMap[&sp][destination->getKey()].second[commercialLine->getKey()] = commercialLine;
					}
				}
			}
		}



		bool StopPointsListFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		string StopPointsListFunction::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : "text/xml";
		}
}	}
