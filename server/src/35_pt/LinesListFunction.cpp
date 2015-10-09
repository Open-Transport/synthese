////////////////////////////////////////////////////////////////////////////////
/// LinesListFunction class implementation.
///	@file LinesListFunction.cpp
///	@author Hugues Romain
///	@date 2008
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "LinesListFunction.h"

#include "alphanum.hpp"
#include "CalendarTemplate.h"
#include "CityTableSync.h"
#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "CustomBroadcastPoint.hpp"
#include "Destination.hpp"
#include "GetMessagesFunction.hpp"
#include "ImportableTableSync.hpp"
#include "JourneyPattern.hpp"
#include "LineAlarmRecipient.hpp"
#include "MimeTypes.hpp"
#include "Profile.h"
#include "PTUseRule.h"
#include "Right.h"
#include "Session.h"
#include "User.h"
#include "Path.h"
#include "Request.h"
#include "RequestException.h"
#include "ReservationContact.h"
#include "RollingStock.hpp"
#include "RollingStockFilter.h"
#include "TransportNetwork.h"
#include "StopArea.hpp"
#include "TransportNetworkTableSync.h"
#include "TreeFolder.hpp"
#include "Vertex.h"
#include "Webpage.h"
#include "StopPoint.hpp"

#include <geos/geom/LineString.h>
#include <geos/geom/GeometryCollection.h>
#include <geos/io/WKTWriter.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace geos::io;
using namespace boost::algorithm;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace calendar;
	using namespace db;
	using namespace util;
	using namespace server;
	using namespace pt;
	using namespace vehicle;
	using namespace security;
	using namespace cms;
	using namespace geography;
	using namespace graph;
	using namespace impex;
	using namespace messages;
	using namespace pt_website;
	using namespace util;
	using namespace security;
	using namespace tree;
	using namespace vehicle;

	template<> const string util::FactorableTemplate<server::Function,pt::LinesListFunction>::FACTORY_KEY(
		"LinesListFunction2"
	);

	namespace pt
	{
		const string LinesListFunction::PARAMETER_NETWORK_ID("ni");
		const string LinesListFunction::PARAMETER_PAGE_ID("pi");
		const string LinesListFunction::PARAMETER_SRID("srid");
		const string LinesListFunction::PARAMETER_OUTPUT_STOPS("os");
		const string LinesListFunction::PARAMETER_OUTPUT_TERMINUSES("ot");
		const string LinesListFunction::PARAMETER_OUTPUT_GEOMETRY("og");
		const string LinesListFunction::PARAMETER_IGNORE_TIMETABLE_EXCLUDED_LINES = "ittd";
		const string LinesListFunction::PARAMETER_IGNORE_JOURNEY_PLANNER_EXCLUDED_LINES = "ijpd";
		const string LinesListFunction::PARAMETER_IGNORE_DEPARTURES_BOARD_EXCLUDED_LINES = "idbd";
		const string LinesListFunction::PARAMETER_ROLLING_STOCK_FILTER_ID = "tm";
		const string LinesListFunction::PARAMETER_SORT_BY_TRANSPORT_MODE = "sort_by_transport_mode";
		const string LinesListFunction::PARAMETER_RIGHT_CLASS = "right_class";
		const string LinesListFunction::PARAMETER_RIGHT_LEVEL = "right_level";
		const string LinesListFunction::PARAMETER_CONTACT_CENTER_ID = "contact_center_id";
		const string LinesListFunction::PARAMETER_CITY_FILTER = "city_filter";
		const string LinesListFunction::PARAMETER_STOP_AREA_TERMINUS_PAGE_ID ="terminus_page";
		const string LinesListFunction::PARAMETER_DATE_FILTER = "date_filter";
		const string LinesListFunction::PARAMETER_CALENDAR_FILTER = "calendar_filter";
		const string LinesListFunction::PARAMETER_RUNS_SOON_FILTER = "runs_soon_filter";
		const string LinesListFunction::PARAMETER_DISPLAY_DURATION_BEFORE_FIRST_DEPARTURE_FILTER = "display_duration_before_first_departure_filter";
		const string LinesListFunction::PARAMETER_BROADCAST_POINT_ID = "broadcast_point_id";
		const string LinesListFunction::PARAMETER_WITH_DIRECTIONS = "with_directions";
		const string LinesListFunction::PARAMETER_SHORT_NAME_FILTER = "short_name_filter";

		const string LinesListFunction::FORMAT_WKT("wkt");

		const string LinesListFunction::TAG_LINE = "line";
		const string LinesListFunction::TAG_FORWARD_DIRECTION = "forward_direction";
		const string LinesListFunction::TAG_BACKWARD_DIRECTION = "backward_direction";
		const string LinesListFunction::ATTR_DIRECTION = "direction";
		const string LinesListFunction::DATA_LINES("lines");
		const string LinesListFunction::DATA_STOP_AREAS("stopAreas");
		const string LinesListFunction::DATA_STOP_AREA("stopArea");
		const string LinesListFunction::DATA_TRANSPORT_MODE("transportMode");
		const string LinesListFunction::DATA_GEOMETRY("geometry");
		const string LinesListFunction::DATA_WKT("wkt");
		const string LinesListFunction::DATA_EDGE("edge");
		const string LinesListFunction::DATA_POINT("point");
		const string LinesListFunction::DATA_RANK("rank");
		const string LinesListFunction::DATA_X("x");
		const string LinesListFunction::DATA_Y("y");
		const string LinesListFunction::DATA_TERMINUS("terminus");
		const string LinesListFunction::DATA_STOP_AREA_TERMINUS("stopAreaTerminus");

		ParametersMap LinesListFunction::_getParametersMap() const
		{
			ParametersMap result;

			// Network
			stringstream networksStr;
			bool first(true);
			BOOST_FOREACH(const pt::TransportNetwork* network, _networks)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					networksStr << ",";
				}
				networksStr << network->getKey();
			}
			result.insert(PARAMETER_NETWORK_ID, networksStr.str());

			// SRID
			if(_coordinatesSystem)
			{
				result.insert(PARAMETER_SRID, static_cast<int>(_coordinatesSystem->getSRID()));
			}

			// Output geometry
			result.insert(PARAMETER_OUTPUT_GEOMETRY, _outputGeometry);

			// Output stops
			result.insert(PARAMETER_OUTPUT_STOPS, _outputStops);

			// Output terminuses
			result.insert(PARAMETER_OUTPUT_TERMINUSES, _outputTerminuses);

			// With directions
			result.insert(PARAMETER_WITH_DIRECTIONS, _withDirections);

			// Output messages
			if(_broadcastPoint)
			{
				result.insert(PARAMETER_BROADCAST_POINT_ID, _broadcastPoint->getKey());
			}

			// Page or output format
			if(_page.get())
			{
				result.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			else
			{
				result.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}

			if(_stopAreaTerminusPage.get())
			{
				result.insert(PARAMETER_STOP_AREA_TERMINUS_PAGE_ID, _stopAreaTerminusPage->getKey());
			}

			// Ignore excluded lines
			result.insert(PARAMETER_IGNORE_DEPARTURES_BOARD_EXCLUDED_LINES, _ignoreDeparturesBoardExcludedLines);
			result.insert(PARAMETER_IGNORE_JOURNEY_PLANNER_EXCLUDED_LINES, _ignoreJourneyPlannerExcludedLines);
			result.insert(PARAMETER_IGNORE_TIMETABLE_EXCLUDED_LINES, _ignoreTimetableExcludedLines);

			// Rolling stock filter
			if(_rollingStockFilter.get() != NULL)
			{
				result.insert(PARAMETER_ROLLING_STOCK_FILTER_ID, _rollingStockFilter->getKey());
			}

			// Transport mode sorting
			if(_sortByTransportMode.size() > 1)
			{
				stringstream s;
				bool first(true);
				BOOST_FOREACH(const boost::shared_ptr<const RollingStock>& tm, _sortByTransportMode)
				{
					if(!tm.get())
					{
						continue;
					}
					if(first)
					{
						first = false;
					}
					else
					{
						s << ",";
					}
					s << tm->getKey();
				}
				result.insert(PARAMETER_SORT_BY_TRANSPORT_MODE, s.str());
			}

			// Right class
			if(!_rightClass.empty())
			{
				result.insert(PARAMETER_RIGHT_CLASS, _rightClass);
			}

			// Right level
			if(_rightLevel)
			{
				result.insert(PARAMETER_RIGHT_LEVEL, static_cast<int>(*_rightLevel));
			}

			return result;
		}



		void LinesListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Selection by line id
			RegistryKeyType lineId(
				map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0)
			);
			if(lineId > 0 && decodeTableId(lineId) == CommercialLineTableSync::TABLE.ID) try
			{
				_line = Env::GetOfficialEnv().get<CommercialLine>(lineId);
			}
			catch(ObjectNotFoundException<CommercialLine>&)
			{
				throw RequestException("Line " + lexical_cast<string>(lineId) + " not found");
			}

			// Selection by line short name
			string shortName(
				map.getDefault<string>(PARAMETER_SHORT_NAME_FILTER)
			);
			if(!shortName.empty())
			{
				BOOST_FOREACH(const CommercialLine::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<CommercialLine>())
                                {
                                        if(it.second->getShortName() == shortName)
                                        {
                                                _line = it.second;
                                        	break;
					}
                                }
				if(!_line.get())
					throw RequestException("Line " + shortName + " not found");
			}

			// Networks and tree selectors
			string networks(
				map.getDefault<string>(PARAMETER_NETWORK_ID)
			);
			_networks.clear();
			if(!networks.empty())
			{
				vector<string> ids;
				split(ids, networks, is_any_of(","));
				BOOST_FOREACH(const string& idstr, ids)
				{
					try
					{
						RegistryKeyType id(lexical_cast<RegistryKeyType>(idstr));
						if(decodeTableId(id) == TransportNetworkTableSync::TABLE.ID)
						{
							_networks.insert(
								Env::GetOfficialEnv().get<TransportNetwork>(id).get()
							);
						}
						else if(decodeTableId(id) == TreeFolder::CLASS_NUMBER)
						{
							_folders.insert(
								Env::GetOfficialEnv().get<TreeFolder>(id).get()
							);
						}
						else if(id == 0)
						{
							_folders.insert(NULL);
						}
					}
					catch(bad_lexical_cast)
					{
						throw RequestException("Bad network id : "+ idstr);
					}
					catch(ObjectNotFoundException<TransportNetwork>&)
					{
						throw RequestException("Transport network " + idstr + " not found");
					}
					catch(ObjectNotFoundException<TreeFolder>&)
					{
						throw RequestException("Folder " + idstr + " not found");
					}
				}
			}

			// Right class
			_rightClass = map.getDefault<string>(PARAMETER_RIGHT_CLASS);
			if(	!_rightClass.empty() &&
				!Factory<Right>::contains(_rightClass)
			){
				throw RequestException("No such right class");
			}

			// Right level
			if(map.getOptional<int>(PARAMETER_RIGHT_LEVEL))
			{
				_rightLevel = static_cast<RightLevel>(map.get<int>(PARAMETER_RIGHT_LEVEL));
			}

			// Output messages
			RegistryKeyType broadcastPointId(
				map.getDefault<RegistryKeyType>(PARAMETER_BROADCAST_POINT_ID, 0)
			);
			if(broadcastPointId)
			{
				try
				{
					_broadcastPoint = Env::GetOfficialEnv().get<CustomBroadcastPoint>(broadcastPointId).get();
				}
				catch(ObjectNotFoundException<CustomBroadcastPoint>&)
				{
					throw RequestException("No such broadcast point");
				}
			}

			// With directions
			_withDirections = map.getDefault<bool>(PARAMETER_WITH_DIRECTIONS, false);

			// Output
			optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_PAGE_ID));
			if(id) try
			{
				_page = Env::GetOfficialEnv().get<Webpage>(*id);
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such page");
			}
			if(!_page.get())
			{
				setOutputFormatFromMap(map, string());
			}
			
			if(map.getOptional<RegistryKeyType>(PARAMETER_STOP_AREA_TERMINUS_PAGE_ID)) try
			{
				_stopAreaTerminusPage = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_STOP_AREA_TERMINUS_PAGE_ID));
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such stopAreaTerminusPage");
			}
			
			_outputGeometry = map.getDefault<string>(PARAMETER_OUTPUT_GEOMETRY);
			_outputStops = map.isTrue(PARAMETER_OUTPUT_STOPS);
			_outputTerminuses = map.isTrue(PARAMETER_OUTPUT_TERMINUSES);
			CoordinatesSystem::SRID srid(
				map.getDefault<CoordinatesSystem::SRID>(PARAMETER_SRID, CoordinatesSystem::GetInstanceCoordinatesSystem().getSRID())
			);
			_coordinatesSystem = &CoordinatesSystem::GetCoordinatesSystem(srid);

			// Parameters
			_ignoreDeparturesBoardExcludedLines = map.isTrue(PARAMETER_IGNORE_DEPARTURES_BOARD_EXCLUDED_LINES);
			_ignoreJourneyPlannerExcludedLines = map.isTrue(PARAMETER_IGNORE_JOURNEY_PLANNER_EXCLUDED_LINES);
			_ignoreTimetableExcludedLines = map.isTrue(PARAMETER_IGNORE_TIMETABLE_EXCLUDED_LINES);

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

			// Transport mode sorting
			_sortByTransportMode.clear();
			string strSBTM(map.getDefault<string>(PARAMETER_SORT_BY_TRANSPORT_MODE));
			if(!strSBTM.empty())
			{
				vector<string> tms;
				split(tms, strSBTM, is_any_of(","));
				BOOST_FOREACH(const string& tmstr, tms)
				{
					try
					{
						boost::shared_ptr<const RollingStock> tm(
							Env::GetOfficialEnv().get<RollingStock>(lexical_cast<RegistryKeyType>(tmstr))
						);
						_sortByTransportMode.push_back(tm);
					}
					catch(bad_lexical_cast)
					{
						throw RequestException("Bad transport mode id");
					}
					catch (ObjectNotFoundException<RollingStock>&)
					{
						throw RequestException("No rolling stock");
					}
				}
			}
			_sortByTransportMode.push_back(boost::shared_ptr<RollingStock>()); // NULL pointer at end

			// Contact center filter
			_contactCenterFilter.reset();
			if(!map.getDefault<string>(PARAMETER_CONTACT_CENTER_ID).empty())
			{
				RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_CONTACT_CENTER_ID));
				if(id > 0) try
				{
					_contactCenterFilter = Env::GetOfficialEnv().get<ReservationContact>(id);
				}
				catch(ObjectNotFoundException<ReservationContact>&)
				{
					throw RequestException("No such contact center");
				}
				else
				{
					_contactCenterFilter = boost::shared_ptr<ReservationContact>();
				}
			}

			// Cities filter
			string cities(
				map.getDefault<string>(PARAMETER_CITY_FILTER)
			);
			_cities.clear();
			if(!cities.empty())
			{
				vector<string> ids;
				split(ids, cities, is_any_of(","));
				BOOST_FOREACH(const string& id, ids)
				{
					try
					{
						_cities.insert(
							Env::GetOfficialEnv().get<City>(
								lexical_cast<RegistryKeyType>(id)
							).get()
						);
					}
					catch(bad_lexical_cast)
					{
						throw RequestException("Bad city id");
					}
					catch(ObjectNotFoundException<City>&)
					{
						throw RequestException("City " + lexical_cast<string>(id) + " not found");
					}
				}
			}

			// Date filter
			string dateFilterStr(map.getDefault<string>(PARAMETER_DATE_FILTER));
			if(!dateFilterStr.empty())
			{
				if(dateFilterStr == "t")
				{
					_dateFilter = day_clock::local_day();
				}
				else
				{
					_dateFilter = from_string(dateFilterStr);
				}
			}

			// Calendar template filter
			RegistryKeyType calendarTemplateFilter(map.getDefault<RegistryKeyType>(PARAMETER_CALENDAR_FILTER, 0));
			if(calendarTemplateFilter)
			{
				try
				{
					_calendarFilter = Env::GetOfficialEnv().get<CalendarTemplate>(calendarTemplateFilter);
				}
				catch(ObjectNotFoundException<CalendarTemplate>&)
				{
					throw RequestException("Calendar filter not found");
				}
				if((*_calendarFilter)->isLimited())
				{
					_calendarDaysFilter = (*_calendarFilter)->getResult();
				}
				else
				{
					throw RequestException("The calendar filter must be limited");
				}

			}


			// Runs soon filter
			long duration(map.getDefault<long>(PARAMETER_RUNS_SOON_FILTER, 0));
			if(duration)
			{
				_runsSoonFilter = minutes(duration);
			}

			_displayDurationBeforeFirstDepartureFilter = map.getDefault<bool>(PARAMETER_DISPLAY_DURATION_BEFORE_FIRST_DEPARTURE_FILTER, false);
		}



		bool LinesListFunction::_lineIsSelected(
			const CommercialLine& line,
			const server::Request& request
		) const	{

			// Network filter
			if(!_networks.empty())
			{
				bool result(false);
				BOOST_FOREACH(const TransportNetwork* network, _networks)
				{
					if(line.getNetwork() == network)
					{
						result = true;
						break;
					}
				}
				if(!result)
				{
					return false;
				}
			}

			// Folder filter
			if(!_folders.empty())
			{
				bool result(false);
				BOOST_FOREACH(const TreeFolder* folder, _folders)
				{
					if(!folder && line._getParent() == line.getNetwork())
					{
						result = true;
						break;
					}
					if(line.isChildOf(*folder))
					{
						result = true;
						break;
					}
				}
				if(!result)
				{
					return false;
				}
			}

			// City filter
			if(!_cities.empty())
			{
				bool result(false);
				BOOST_FOREACH(const City* city, _cities)
				{
					if(line.callsAtCity(*city))
					{
						result = true;
						break;
					}
				}
				if(!result)
				{
					return false;
				}
			}

			// Rights filter
			if(_rightLevel && _rights && request.getUser())
			{
				// All ?
				RightsOfSameClassMap::const_iterator it(
					_rights->find(GLOBAL_PERIMETER)
				);
				if (it == _rights->end() ||
					it->second->getPublicRightLevel() < *_rightLevel
				){
					bool result(false);
					BOOST_FOREACH(const RightsOfSameClassMap::value_type& it, *_rights)
					{
						if(	!it.second->perimeterIncludes(
								lexical_cast<string>(line.getKey()),
								Env::GetOfficialEnv()
							)
						){
							continue;
						}

						if (it.second->getPublicRightLevel() < *_rightLevel)
						{
							return false;
						}
						else
						{
							result = true;
							break;
						}
					}
					if(!result)
					{
						return false;
					}
			}	}

			// Filter by Rolling stock id
			if(_rollingStockFilter.get())
			{
				// Set the boolean to true or false depending on whether filter is inclusive or exclusive
				bool result = !(_rollingStockFilter->getAuthorizedOnly());
				set<const RollingStock*> rollingStocksList = _rollingStockFilter->getList();
				BOOST_FOREACH(const RollingStock* rollingStock, rollingStocksList)
				{
					if(line.usesTransportMode(*rollingStock))
					{
						result = _rollingStockFilter->getAuthorizedOnly();
						break;
					}
				}

				// If the line doesn't respect the filter, skip it
				if(!result)
				{
					return false;
				}
			}

			// Use rule tests
			const UseRule& useRule(line.getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET));
			if(	dynamic_cast<const PTUseRule*>(&useRule) &&
				(	(_ignoreJourneyPlannerExcludedLines && static_cast<const PTUseRule&>(useRule).getForbiddenInJourneyPlanning()) ||
					(_ignoreTimetableExcludedLines && static_cast<const PTUseRule&>(useRule).getForbiddenInTimetables()) ||
					(_ignoreDeparturesBoardExcludedLines && static_cast<const PTUseRule&>(useRule).getForbiddenInDepartureBoards())
			)	){
				return false;
			}

			// Contact center filter
			if(	_contactCenterFilter &&
				_contactCenterFilter->get() &&
				_contactCenterFilter->get() != line.getReservationContact()
			){
				return false;
			}

			// Date filter
			if(	_dateFilter &&
				!line.runsAtDate(*_dateFilter)
			){
				return false;
			}

			// Calendar filter
			if(	_calendarFilter &&
				!line.runsOnCalendar(_calendarDaysFilter)
			){
				return false;
			}

			// Runs soon ?
			if(	_runsSoonFilter &&
				!line.runsSoon(*_runsSoonFilter)
			){
				return false;
			}

			// displayDurationBeforeFirstDepartureFilter
			if(	_displayDurationBeforeFirstDepartureFilter &&
				!line.getDisplayDurationBeforeFirstDeparture().is_not_a_date_time() &&
				line.getDisplayDurationBeforeFirstDeparture() != boost::posix_time::time_duration(0,-1,0) &&
				!line.runsSoon(line.getDisplayDurationBeforeFirstDeparture())
			){
				return false;
			}

			return true;
		}



		util::ParametersMap LinesListFunction::run(
			std::ostream& stream,
			const Request& request
		) const	{

			// Rights caching
			if((!_rightClass.empty() || !_rightLevel) && request.getUser())
			{
				synthese::security::RightsOfSameClassMap rights(request.getUser()->getProfile()->getRights(_rightClass));
				_rights = rights;
			}

			typedef set<boost::shared_ptr<const CommercialLine>, CommercialLine::PointerComparator> SortedItems;
			typedef std::map<const RollingStock*, SortedItems> LinesMapType;
			LinesMapType linesMap;

			// Specified line ID
			if(_line.get())
			{
				linesMap[NULL].insert(_line);
			}
			else
			{
				vector<boost::shared_ptr<CommercialLine> > lines;
				BOOST_FOREACH(const CommercialLine::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<CommercialLine>())
				{
					if(!_lineIsSelected(*it.second, request))
					{
						continue;
					}

					lines.push_back(it.second);
				}

				set<CommercialLine*> alreadyShownLines;
				BOOST_FOREACH(const boost::shared_ptr<const RollingStock>& tm, _sortByTransportMode)
				{
					BOOST_FOREACH(const boost::shared_ptr<CommercialLine>& line, lines)
					{
						// Avoid to return a line twice
						if(alreadyShownLines.find(line.get()) != alreadyShownLines.end())
						{
							continue;
						}

						// Transport mode check
						if(!tm.get() || line->usesTransportMode(*tm))
						{
							linesMap[tm.get()].insert(line);
							alreadyShownLines.insert(line.get());
						}
				}	}
			}

			// Populating the parameters map
			ParametersMap pm;
			BOOST_FOREACH(const boost::shared_ptr<const RollingStock>& tm, _sortByTransportMode)
			{
				BOOST_FOREACH(const LinesMapType::mapped_type::value_type& it, linesMap[tm.get()])
				{
					boost::shared_ptr<const CommercialLine> line = it;
					boost::shared_ptr<ParametersMap> linePM(new ParametersMap);
					line->toParametersMap(*linePM, true);

					// Rolling stock
					set<RollingStock *> rollingStocks;
					BOOST_FOREACH(Path* path, line->getPaths())
					{
						const JourneyPattern* jp(dynamic_cast<const JourneyPattern*>(path));
						if(!jp || !jp->getRollingStock())
						{
							continue;
						}

						rollingStocks.insert(
							jp->getRollingStock()
						);
					}
					BOOST_FOREACH(RollingStock * rs, rollingStocks)
					{
						boost::shared_ptr<ParametersMap> rsPM(new ParametersMap);
						rs->toParametersMap(*rsPM, true);
						linePM->insert(DATA_TRANSPORT_MODE, rsPM);
					}

					// Directions
					if(_withDirections)
					{
						// Loop on wayback
						for(size_t wayback(0); wayback!=2; ++wayback)
						{
							set<string> directions;

							BOOST_FOREACH(Path* path, line->getPaths())
							{
								const JourneyPattern* jp(dynamic_cast<const JourneyPattern*>(path));
								if(!jp || !jp->getMain() || jp->getWayBack() != (wayback == 1))
								{
									continue;
								}

								string direction;
								if(jp->getDirectionObj())
								{
									direction = jp->getDirectionObj()->get<DisplayedText>();
								}
								else if(!jp->getDirection().empty())
								{
									direction = jp->getDirection();
								}
								else if(dynamic_cast<const NamedPlace*>(jp->getDestination()->getHub()))
								{
									direction = dynamic_cast<const NamedPlace*>(jp->getDestination()->getHub())->getFullName();
								}
								directions.insert(direction);
							}

							BOOST_FOREACH(const string& direction, directions)
							{
								boost::shared_ptr<ParametersMap> directionPM(new ParametersMap);
								directionPM->insert(ATTR_DIRECTION, direction);
								linePM->insert(wayback ? TAG_BACKWARD_DIRECTION : TAG_FORWARD_DIRECTION, directionPM);
							}
						}
					}

					if(_outputStops || _outputTerminuses)
					{
						set<const StopArea*> stopAreas;
						set<const StopArea*> stopAreasTerminus;
						BOOST_FOREACH(Path* path, line->getPaths())
						{
							if(!dynamic_cast<const JourneyPattern*>(path))
							{
								continue;
							}

							if(_outputStops)
							{
								BOOST_FOREACH(Edge* edge, path->getEdges())
								{
									if(!edge->getFromVertex())
									{
										break;
									}
									const StopArea* stopArea(
										dynamic_cast<const StopArea*>(edge->getFromVertex()->getHub())
									);
									if(stopArea)
									{
										stopAreas.insert(stopArea);
									}
								}
							}

							if(_outputTerminuses)
							{
								stopAreasTerminus.insert(static_cast<const StopPoint*>(path->getDestination())->getConnectionPlace());
							}
						}

						if(_outputStops)
						{
							boost::shared_ptr<ParametersMap> stopAreasPM(new ParametersMap);
							BOOST_FOREACH(const StopArea* stopArea, stopAreas)
							{
								boost::shared_ptr<ParametersMap> stopAreaPM(new ParametersMap);
								stopArea->toParametersMap(*stopAreaPM, _coordinatesSystem);
								stopAreasPM->insert(DATA_STOP_AREA, stopAreaPM);
							}
							linePM->insert(DATA_STOP_AREAS, stopAreasPM);
						}

						if(_outputTerminuses)
						{
							//Terminus
							BOOST_FOREACH(const StopArea* stopArea, stopAreasTerminus)
							{
								boost::shared_ptr<ParametersMap> stopAreaTerminusPM(new ParametersMap);
								stopArea->toParametersMap(*stopAreaTerminusPM, _coordinatesSystem);
								linePM->insert(DATA_STOP_AREA_TERMINUS, stopAreaTerminusPM);
							}
						}
					}
					if(!_outputGeometry.empty())
					{
						typedef map<pair<Vertex*, Vertex*>, boost::shared_ptr<Geometry> > VertexPairs;
						VertexPairs geometries;
						BOOST_FOREACH(Path* path, line->getPaths())
						{
							if(!dynamic_cast<const JourneyPattern*>(path))
							{
								continue;
							}

							BOOST_FOREACH(Edge* edge, path->getEdges())
							{
								if(!edge->getNext())
								{
									break;
								}
								VertexPairs::key_type od(make_pair(edge->getFromVertex(), edge->getNext()->getFromVertex()));
								if(geometries.find(od) == geometries.end())
								{
									boost::shared_ptr<LineString> lineGeometry = edge->getRealGeometry();
									if (lineGeometry)
										geometries.insert(make_pair(od, lineGeometry));
								}
							}
						}

						boost::shared_ptr<ParametersMap> geometryPM(new ParametersMap);
						if(_outputGeometry == FORMAT_WKT)
						{
							vector<boost::shared_ptr<Geometry> > vec;
							vector<Geometry*> vecd;
							BOOST_FOREACH(const VertexPairs::value_type& it, geometries)
							{
								boost::shared_ptr<Geometry> prGeom(
									_coordinatesSystem->convertGeometry(*it.second)
								);
								vec.push_back(prGeom);
								vecd.push_back(prGeom.get());
							}
							boost::shared_ptr<GeometryCollection> mls(
								_coordinatesSystem->getGeometryFactory().createGeometryCollection(vecd)
							);
							geometryPM->insert(DATA_WKT, WKTWriter().write(mls.get()));
						}
						else
						{
							BOOST_FOREACH(const VertexPairs::value_type& it, geometries)
							{
								boost::shared_ptr<ParametersMap> edgePM(new ParametersMap);
								boost::shared_ptr<Geometry> prGeom(
									_coordinatesSystem->convertGeometry(*it.second)
								);
								for(size_t i(0); i<prGeom->getNumPoints(); ++i)
								{
									const Coordinate& pt(prGeom->getCoordinates()->getAt(i));
									boost::shared_ptr<ParametersMap> pointPM(new ParametersMap);
									pointPM->insert(DATA_X, pt.x);
									pointPM->insert(DATA_Y, pt.y);
									edgePM->insert(DATA_POINT, pointPM);
								}
								geometryPM->insert(DATA_EDGE, edgePM);
							}
						}
						linePM->insert(DATA_GEOMETRY, geometryPM);
					}

					// Messages output
					if(_broadcastPoint)
					{
						// Parameters map
						ParametersMap parameters;
						parameters.insert("line_id", linePM->get<RegistryKeyType>(TABLE_COL_ID));

						GetMessagesFunction f(
							_broadcastPoint,
							parameters
						);
						linePM->merge(
							f.run(stream, request),
							string(),
							true
						);
					}

					pm.insert(TAG_LINE, linePM);
			}	}

			if(_page.get()) // CMS output
			{
				size_t rank(0);
				BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmLine, pm.getSubMaps(TAG_LINE))
				{
					// Template parameters
					pmLine->merge(getTemplateParameters());

					// Rank
					pmLine->insert(DATA_RANK, rank++);

					//Add Rolling stock information with a prefix
					if(pmLine->hasSubMaps(DATA_TRANSPORT_MODE))
					{
						pmLine->merge(*pmLine->getSubMaps(DATA_TRANSPORT_MODE)[0].get(),string("rs_"));
					}
					
					//Add Terminus
					if(_stopAreaTerminusPage.get() && pmLine->hasSubMaps(DATA_STOP_AREA_TERMINUS))
					{
						boost::shared_ptr<ParametersMap> stopAreasTerminusPM(new ParametersMap);
						stringstream stopAreasTerminusStream;

						BOOST_FOREACH(const boost::shared_ptr<ParametersMap>& stopAreaPM, pmLine->getSubMaps(DATA_STOP_AREA_TERMINUS))
						{
							stopAreaPM->merge(getTemplateParameters());
							_stopAreaTerminusPage->display(stopAreasTerminusStream, request, *stopAreaPM);
						}
						pmLine->insert(DATA_TERMINUS, stopAreasTerminusStream.str());
					}

					// Display
					_page->display(stream, request, *pmLine);
				}
			}
			else if(_outputFormat == MimeTypes::CSV)
			{
				// Hand made formatting for CSV.
				BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmLine, pm.getSubMaps(TAG_LINE))
				{
					stream << pmLine->get<string>(CommercialLine::DATA_LINE_ID) << ";" << pmLine->get<string>(CommercialLine::DATA_LINE_SHORT_NAME) << "\n";
				}
			}
			else
			{
				outputParametersMap(
					pm,
					stream,
					DATA_LINES,
					"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/35_pt/LinesListFunction.xsd"
				);
			}
			return pm;
		}



		bool LinesListFunction::isAuthorized(
			const Session*
		) const {
			return true;
		}



		std::string LinesListFunction::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : getOutputMimeTypeFromOutputFormat();
		}



		LinesListFunction::LinesListFunction():
			_outputStops(false),
			_outputTerminuses(false),
			_ignoreTimetableExcludedLines(false),
			_ignoreJourneyPlannerExcludedLines(false),
			_ignoreDeparturesBoardExcludedLines(false),
			_withDirections(false),
			_displayDurationBeforeFirstDepartureFilter(false),
			_broadcastPoint(NULL)
		{}



		void LinesListFunction::setNetwork(
			const TransportNetwork* value
		){
			_networks.clear();
			_networks.insert(value);
		}
}	}
