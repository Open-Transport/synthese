
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

#include "StopPointsListFunction.hpp"

#include "alphanum.hpp"
#include "City.h"
#include "DRTArea.hpp"
#include "JourneyPattern.hpp"
#include "LinePhysicalStop.hpp"
#include "MimeTypes.hpp"
#include "PTServiceConfig.hpp"
#include "RequestException.h"
#include "Request.h"
#include "RollingStock.hpp"
#include "RollingStockFilter.h"
#include "SchedulesBasedService.h"
#include "ServicePointer.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "Webpage.h"

#ifndef UNIX
#include <geos/util/math.h>
using namespace geos::util;
#endif
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
	using namespace pt_website;
	using namespace vehicle;

	template<> const string util::FactorableTemplate<Function,pt::StopPointsListFunction>::FACTORY_KEY("StopPointsListFunction");

	namespace pt
	{
		const string StopPointsListFunction::PARAMETER_LINE_ID = "lineid";
		const string StopPointsListFunction::PARAMETER_DATE = "date";
		const string StopPointsListFunction::PARAMETER_PAGE_ID = "page_id";
		const string StopPointsListFunction::PARAMETER_DESTINATION_PAGE_ID = "destination_page_id";
		const string StopPointsListFunction::PARAMETER_LINE_PAGE_ID = "line_page_id";
		const string StopPointsListFunction::PARAMETER_BBOX = "bbox";
		const string StopPointsListFunction::PARAMETER_SRID = "srid";
		const string StopPointsListFunction::PARAMETER_ROLLING_STOCK_FILTER_ID = "tm";
		const string StopPointsListFunction::PARAMETER_SORT_BY_LINE_NAME = "sln";
		const string StopPointsListFunction::PARAMETER_OMIT_SAME_AREA_DESTINATIONS = "omitSameAreaDestinations";
		const string StopPointsListFunction::PARAMETER_SORT_BY_DISTANCE_TO_BBOX_CENTER = "sortByDistance";
		const string StopPointsListFunction::PARAMETER_MAX_SOLUTIONS_NUMBER = "msn";
		const string StopPointsListFunction::PARAMETER_DATA_SOURCE_FILTER = "data_source_filter";
		const string StopPointsListFunction::PARAMETER_DAYS_CHECK_IF_STOP_SERVED = "days_check_if_stop_served";
		const string StopPointsListFunction::PARAMETER_OUTPUT_ONLY_ARRIVAL_STOPS = "output_only_arrival_stops";

		const string StopPointsListFunction::TAG_PHYSICAL_STOPS = "physicalStops";
		const string StopPointsListFunction::TAG_PHYSICAL_STOP = "physicalStop";
		const string StopPointsListFunction::TAG_DESTINATION = "destination";
		const string StopPointsListFunction::TAG_LINE = "line";
		const string StopPointsListFunction::DATA_STOP_AREA_PREFIX = "stopArea_";
		const string StopPointsListFunction::DATA_STOPAREA_NAME = "stopAreaName";
		const string StopPointsListFunction::DATA_STOPAREA_CITY_NAME = "stopAreaCityName";
		const string StopPointsListFunction::DATA_DESTINATIONS = "destinations";
		const string StopPointsListFunction::DATA_DESTINATIONS_NUMBER = "destinationsNumber";
		const string StopPointsListFunction::DATA_LINES = "lines";
		const string StopPointsListFunction::DATA_DISTANCE_TO_BBOX_CENTER = "distanceToBboxCenter";

		FunctionAPI StopPointsListFunction::getAPI() const
		{
			FunctionAPI api(
						"Public Transport",
						"Returns the list of stop points for a commercial line, "
						"a stop area or a bounding box",
						"");
			api.openParamGroup("Select");
			api.addParams(PARAMETER_LINE_ID, "", false);
			api.addParams(PARAMETER_DATE, "", false);
			api.addParams(PARAMETER_PAGE_ID, "", false);
			api.addParams(PARAMETER_DESTINATION_PAGE_ID, "", false);
			api.addParams(PARAMETER_LINE_PAGE_ID, "", false);
			api.openParamGroup("Filter");
			api.addParams(PARAMETER_BBOX,
								   "A bounding box of the form 'x1,y1,x2,y2'", false);
			api.addParams(PARAMETER_SRID, "", false);
			api.addParams(PARAMETER_ROLLING_STOCK_FILTER_ID, "", false);
			api.addParams(PARAMETER_SORT_BY_LINE_NAME,
								   "Sort by line name", false);
			api.addParams(PARAMETER_OMIT_SAME_AREA_DESTINATIONS, "", false);
			api.addParams(PARAMETER_SORT_BY_DISTANCE_TO_BBOX_CENTER, "", false);
			api.addParams(PARAMETER_MAX_SOLUTIONS_NUMBER,
								   "Max number of solution", false);
			return api;
		}


		ParametersMap StopPointsListFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_date && _date->is_not_a_date_time()) map.insert(PARAMETER_DATE, *_date);
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			if(_destinationPage.get())
			{
				map.insert(PARAMETER_DESTINATION_PAGE_ID, _destinationPage->getKey());
			}
			if(_linePage.get())
			{
				map.insert(PARAMETER_LINE_PAGE_ID, _linePage->getKey());
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
			if(_rollingStockFilter.get() != NULL)
			{
				map.insert(PARAMETER_ROLLING_STOCK_FILTER_ID, _rollingStockFilter->getKey());
			}
			map.insert(PARAMETER_SORT_BY_LINE_NAME, _sortByLineName);

			// Max solutions number
			if(_maxSolutionsNumber)
			{
				map.insert(PARAMETER_MAX_SOLUTIONS_NUMBER, *_maxSolutionsNumber);
			}
			
			// dataSourceFilter
			if(_dataSourceFilter)
			{
				map.insert(PARAMETER_DATA_SOURCE_FILTER, _dataSourceFilter->getKey());
			}

			return map;
		}



		void StopPointsListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Max solutions number
			_maxSolutionsNumber = map.getOptional<size_t>(PARAMETER_MAX_SOLUTIONS_NUMBER);

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

				boost::shared_ptr<Point> pt1(
					_coordinatesSystem->createPoint(lexical_cast<double>(parsed_bbox[0]), lexical_cast<double>(parsed_bbox[1]))
				);
				boost::shared_ptr<Point> pt2(
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
			if(map.getOptional<RegistryKeyType>(PARAMETER_DESTINATION_PAGE_ID)) try
			{
				_destinationPage = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_DESTINATION_PAGE_ID));
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such page");
			}
			if(map.getOptional<RegistryKeyType>(PARAMETER_LINE_PAGE_ID)) try
			{
				_linePage = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_LINE_PAGE_ID));
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such page");
			}

			// Other output
			if(!_page.get())
			{
				setOutputFormatFromMap(map, MimeTypes::XML);
			}

			// Omit same area destinations
			_omitSameAreaDestinations = map.getDefault<bool>(PARAMETER_OMIT_SAME_AREA_DESTINATIONS, false);

			// Rolling stock filter
			optional<RegistryKeyType> rs_id(map.getOptional<RegistryKeyType>(PARAMETER_ROLLING_STOCK_FILTER_ID));
			if(rs_id) try
			{
				_rollingStockFilter = Env::GetOfficialEnv().get<RollingStockFilter>(*rs_id);
			}
			catch (ObjectNotFoundException<RollingStockFilter>)
			{
				throw RequestException("No such RollingStockFilter");
			}
			_sortByLineName = map.getDefault<bool>(PARAMETER_SORT_BY_LINE_NAME, false);

			// Sort by distance to bbox center
			_isSortByDistanceToBboxCenter = false;
			optional<string> sortValueStr(map.getOptional<string>(PARAMETER_SORT_BY_DISTANCE_TO_BBOX_CENTER));
			if(sortValueStr && (*sortValueStr) == "1" && _bbox)
			{
				_isSortByDistanceToBboxCenter = true;
			}

			if(map.getOptional<RegistryKeyType>(PARAMETER_DATA_SOURCE_FILTER)) try
			{
				_dataSourceFilter = Env::GetOfficialEnv().get<impex::DataSource>(map.get<RegistryKeyType>(PARAMETER_DATA_SOURCE_FILTER));
			}
			catch (ObjectNotFoundException<impex::DataSource>&)
			{
				throw RequestException("No such data source");
			}

			_daysCheckIfStopServed = boost::gregorian::date_duration(map.getDefault<int>(PARAMETER_DAYS_CHECK_IF_STOP_SERVED, 0));

			_outputOnlyArrivalStops = map.isTrue(PARAMETER_OUTPUT_ONLY_ARRIVAL_STOPS);
		}



		util::ParametersMap StopPointsListFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			ptime startDateTime,endDateTime;

			ptime date = _date ? *_date : second_clock::local_time();

			// and startDateTime is begin of the day (a day begin at 3:00):
			startDateTime = date - date.time_of_day() + hours(3);
			// and endDateTime is end of the day (a day end at 27:00):
			endDateTime = date - date.time_of_day() + _daysCheckIfStopServed + hours(27);

			// Search for stopPoints
			StopPointMapType stopPointMap;

			if(_stopArea) // If a stopArea is provided : display only stop points of this stopArea
			{
				const StopArea::PhysicalStops& stops(_stopArea->get()->getPhysicalStops());
				BOOST_FOREACH(const StopArea::PhysicalStops::value_type& stopPoint, stops)
				{
					if(_dataSourceFilter && !stopPoint.second->hasLinkWithSource(*_dataSourceFilter))
						continue;

					addStop(stopPointMap, *stopPoint.second, startDateTime, endDateTime);
				}
			}
			else
			{
				BOOST_FOREACH(const Registry<StopPoint>::value_type& stopPoint, Env::GetOfficialEnv().getRegistry<StopPoint>())
				{
					if((_bbox &&
						(!stopPoint.second->getGeometry() ||
						!_bbox->contains(*stopPoint.second->getGeometry()->getCoordinate()))) ||
						(_dataSourceFilter && !stopPoint.second->hasLinkWithSource(*_dataSourceFilter))
					){
						continue;
					}

					addStop(stopPointMap, *stopPoint.second, startDateTime, endDateTime);
				}
			}

			// Filling in the result parameters map
			ParametersMap pm;
			size_t nbStops = 0;
			BOOST_FOREACH(const StopPointMapType::value_type& sp, stopPointMap)
			{
				// Declarations
				boost::shared_ptr<ParametersMap> stopPM(new ParametersMap);

				// Main attributes
				sp.first.getStopPoint()->toParametersMap(
					*stopPM,
					true,
					*_coordinatesSystem
				);

				// Distance to bbox center
				if (_isSortByDistanceToBboxCenter)
				{
					int distanceToBboxCenter = sp.first.getDistanceToBboxCenter();
					stopPM->insert(DATA_DISTANCE_TO_BBOX_CENTER, distanceToBboxCenter);
				}

				// Destinations
				BOOST_FOREACH(const StopAreaDestinationMapType::value_type& destination, sp.second)
				{
					// Main parameters
					boost::shared_ptr<ParametersMap> destinationPM(new ParametersMap);
					destinationPM->insert("id", destination.first.getKey());
					destinationPM->insert("name", destination.second.first->getName());
					destinationPM->insert("cityName", destination.second.first->getCity()->getName());

					// Lines
					BOOST_FOREACH(const CommercialLineSetType::value_type& line, destination.second.second)
					{
						// Declaration
						boost::shared_ptr<ParametersMap> linePM(new ParametersMap);

						// Main parameters
						line->toParametersMap(*linePM, true);

						// Rolling stock
						set<RollingStock *> rollingStocks;
						BOOST_FOREACH(Path* path, line->getPaths())
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
							boost::shared_ptr<ParametersMap> transportModePM(new ParametersMap);
							rs->toParametersMap(*transportModePM, true);
							linePM->insert("transportMode", transportModePM);
						}

						destinationPM->insert(TAG_LINE, linePM);
					}

					stopPM->insert(TAG_DESTINATION, destinationPM);
				}

				pm.insert(TAG_PHYSICAL_STOP, stopPM);
				nbStops++;

				if (_maxSolutionsNumber && nbStops >= *_maxSolutionsNumber)
				{
					break;
				}
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
					if(subMap->hasSubMaps(StopPoint::TAG_STOP_AREA))
					{
						vector<boost::shared_ptr<ParametersMap> > stopAreaMap(subMap->getSubMaps(StopPoint::TAG_STOP_AREA));
						subMap->merge(**stopAreaMap.begin(), DATA_STOP_AREA_PREFIX);
					}

					if(_destinationPage.get())
					{
						if(subMap->hasSubMaps(TAG_DESTINATION))
						{
							vector<boost::shared_ptr<ParametersMap> > destinationVect = subMap->getSubMaps(TAG_DESTINATION);
							vector<boost::shared_ptr<ParametersMap> > sortedDestinationVect;

							if(_sortByLineName)
							{
								typedef multimap <string, boost::shared_ptr<ParametersMap>,
									util::alphanum_text_first_less<string> > sortedMapType;
								sortedMapType sortedMap;
								BOOST_FOREACH(const boost::shared_ptr<ParametersMap>& destination, destinationVect)
								{
									if(destination->hasSubMaps(TAG_LINE))
									{
										BOOST_FOREACH(const boost::shared_ptr<ParametersMap>& line, destination->getSubMaps(TAG_LINE))
										{
											//Create a new Destination with only one line
											boost::shared_ptr<ParametersMap> newDestination(new ParametersMap);
											newDestination->merge(*destination);
											newDestination->insert(TAG_LINE, line);
											sortedMap.insert(make_pair(line->get<string>("line_short_name"), newDestination));
										}
									}
								}

								BOOST_FOREACH(sortedMapType::value_type it, sortedMap)
								{
									sortedDestinationVect.push_back(it.second);
								}
							}

							stringstream destinationsStream;
							_displayDestinations(
								destinationsStream,
								(_sortByLineName ? sortedDestinationVect : destinationVect),
								request
							);
							subMap->insert(DATA_DESTINATIONS_NUMBER, (_sortByLineName ? sortedDestinationVect.size() : destinationVect.size()));
							subMap->insert(DATA_DESTINATIONS, destinationsStream.str());
						}
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

				outputParametersMap(
					pm,
					stream,
					TAG_PHYSICAL_STOPS,
					"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/35_pt/StopPointsListFunction.xsd"
				);
			}

			return pm;
		}



		//Sort stopPoint by distance to bbox,or by code operator.Sort by code operator is applied by default. 
		StopPointsListFunction::SortableStopPoint::SortableStopPoint(const StopPoint * sp, int distanceToBboxCenter, bool isSortByDistanceToBboxCenter):
			_sp(sp),
			_opCode(_sp->getCodeBySources(), false),
			_distanceToBboxCenter(distanceToBboxCenter),
			_isSortByDistanceToBboxCenter(isSortByDistanceToBboxCenter)
		{
		}



		bool StopPointsListFunction::SortableStopPoint::operator<(SortableStopPoint const &otherStopPoint) const
		{
			if(	_isSortByDistanceToBboxCenter &&
				_distanceToBboxCenter != otherStopPoint.getDistanceToBboxCenter()
			){
				return _distanceToBboxCenter < otherStopPoint.getDistanceToBboxCenter();
			}

			if(	_opCode != otherStopPoint.getOpCode()
			){
				return util::alphanum_text_first_comp(_opCode, otherStopPoint.getOpCode()) < 0; 
			}

			return _sp < otherStopPoint._sp;
		}



		string StopPointsListFunction::SortableStopPoint::getOpCode() const
		{
			return _opCode;
		}



		int StopPointsListFunction::SortableStopPoint::getDistanceToBboxCenter() const
		{
			return _distanceToBboxCenter;
		}



		const StopPoint* StopPointsListFunction::SortableStopPoint::getStopPoint() const
		{
			return _sp;
		}



		StopPointsListFunction::SortableStopArea::SortableStopArea(RegistryKeyType key, string destinationName):
			_key(key),
			_destinationName(destinationName)
		{
		}



		bool StopPointsListFunction::SortableStopArea::operator<(SortableStopArea const &otherStopArea) const
		{
			return util::alphanum_text_first_comp(_destinationName, otherStopArea.getDestinationName()) < 0;
		}



		string StopPointsListFunction::SortableStopArea::getDestinationName() const
		{
			return _destinationName;
		}



		RegistryKeyType StopPointsListFunction::SortableStopArea::getKey() const
		{
			return _key;
		}



		void StopPointsListFunction::addStop(
			StopPointMapType & stopPointMap,
			const StopPoint & sp,
			ptime & startDateTime,
			ptime & endDateTime
		) const {
			bool spHaveZeroDestination = true;
			Vertex::Edges::const_iterator itDep(sp.getDepartureEdges().begin());
			Vertex::Edges::const_iterator itArr(sp.getArrivalEdges().begin());
			const Edge* edge;
			while(true)
			{
				if(itDep != sp.getDepartureEdges().end())
				{
					edge = itDep->second;
					itDep++;
				}
				else
				{
					if(_outputOnlyArrivalStops && itArr != sp.getArrivalEdges().end())
					{
						edge = itArr->second;
						itArr++;
					}
					else
					{
						break;
					}
				}

				// Jump over junctions
				if(!dynamic_cast<const LinePhysicalStop*>(edge))
				{
					continue;
				}

				const LinePhysicalStop* ls = static_cast<const LinePhysicalStop*>(edge);

				BOOST_FOREACH(const Path::ServiceCollections::value_type& itCollection, ls->getParentPath()->getServiceCollections())
				{

					ptime departureDateTime = startDateTime;
					// Loop on services
					optional<Edge::DepartureServiceIndex::Value> index;
					while(true)
					{
						AccessParameters ap(USER_PEDESTRIAN);
						ServicePointer servicePointer(
							ls->getNextService(
									*itCollection,
								ap,
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
						{
							if (!dynamic_cast<const DRTArea*>(servicePointer.getRealTimeDepartureVertex()))
								continue;
						}

						const JourneyPattern* journeyPattern = dynamic_cast<const JourneyPattern*>(servicePointer.getService()->getPath());
						if(journeyPattern == NULL) // Could be a junction
							continue;

						const CommercialLine * commercialLine(journeyPattern->getCommercialLine());
						if(_commercialLineID && (commercialLine->getKey() != _commercialLineID))// only physicalStop used by the commercial line will be displayed
							continue;

						// Filter by Rolling stock id
						if(_rollingStockFilter.get())
						{
							// Set the boolean to true or false depending on whether filter is inclusive or exclusive
							bool atLeastOneMode = !(_rollingStockFilter->getAuthorizedOnly());
							set<const RollingStock*> rollingStocksList = _rollingStockFilter->getList();
							BOOST_FOREACH(const RollingStock* rollingStock, rollingStocksList)
							{
								if(commercialLine->usesTransportMode(*rollingStock))
								{
									atLeastOneMode = _rollingStockFilter->getAuthorizedOnly();
									break;
								}
							}

							// If the line doesn't respect the filter, skip it
							if(!atLeastOneMode)
							{
								continue;
							}
						}

						const StopArea * destination = journeyPattern->getDestination()->getConnectionPlace();

						if(_omitSameAreaDestinations && _stopArea)
						{
							//Ignore if destination is the _stopArea himself
							if(destination->getKey() == _stopArea->get()->getKey())
							{
								continue;
							}
						}

						int distanceToBboxCenter = CalcDistanceToBboxCenter(sp);
						SortableStopPoint keySP(&sp,distanceToBboxCenter,_isSortByDistanceToBboxCenter);
						SortableStopArea keySA(destination->getKey(), destination->getName());
						if(spHaveZeroDestination)
						{
							StopAreaDestinationMapType stopAreaMap;
							stopPointMap[keySP] = stopAreaMap;
							spHaveZeroDestination = false;
						}

						StopAreaDestinationMapType::iterator it = stopPointMap[keySP].find(keySA);
						if(it == stopPointMap[keySP].end()) // test if destination stop already in the map
						{
							CommercialLineSetType lineSet;
							lineSet.insert(commercialLine);
							stopPointMap[keySP][keySA] = make_pair(destination, lineSet);
						}
						else // destination stop is already in the map
						{
							stopPointMap[keySP][keySA].second.insert(commercialLine);
						}
					}
				}
			}
		}



		void StopPointsListFunction::_displayLines(
			std::ostream& stream,
			const std::vector<boost::shared_ptr<util::ParametersMap> >& lines,
			const std::string destinationName,
			const std::string destinationCityName,
			const server::Request& request
		) const {
			BOOST_FOREACH(const boost::shared_ptr<ParametersMap>& line, lines)
			{
				line->merge(getTemplateParameters());
				line->insert("destinationName", destinationName),
				line->insert("destinationCityName", destinationCityName),
				_linePage->display(stream, request, *line);
			}
		}



		void StopPointsListFunction::_displayDestinations(
			std::ostream& stream,
			const std::vector<boost::shared_ptr<util::ParametersMap> >& destinations,
			const server::Request& request
		) const {
			bool isFirst = true;
			BOOST_FOREACH(const boost::shared_ptr<ParametersMap>& destination, destinations)
			{
				destination->merge(getTemplateParameters());
				destination->insert("isFirstDestination", isFirst);
				isFirst = false;
				if(_linePage.get())
				{
					if(destination->hasSubMaps(TAG_LINE))
					{
						stringstream linesStream;
						_displayLines(
							linesStream,
							destination->getSubMaps(TAG_LINE),
							destination->get<string>("name"),
							destination->get<string>("cityName"),
							request
						);
						destination->insert(DATA_LINES, linesStream.str());
					}
				}
				_destinationPage->display(stream, request, *destination);
			}
		}



		bool StopPointsListFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		string StopPointsListFunction::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : getOutputMimeTypeFromOutputFormat();
		}



		int StopPointsListFunction::CalcDistanceToBboxCenter(const StopPoint & stopPoint) const
		{
			//return value
			int distanceToBboxCenter = 0;

			if(_bbox)
			{
				boost::shared_ptr<Point> gp = stopPoint.getGeometry();

				// Coordinates of bbox center
				double xCenter = (_bbox->getMaxX() + _bbox->getMinX()) / 2.0; 
				double yCenter = (_bbox->getMaxY() + _bbox->getMinY()) / 2.0; 

				if(gp.get())
				{
					distanceToBboxCenter = sqrt((gp->getX() - xCenter) * (gp->getX() - xCenter)+(gp->getY() - yCenter) * (gp->getY() - yCenter));
				}
			}

			//return value
			return static_cast<int>(round(distanceToBboxCenter));
		}
}	}
