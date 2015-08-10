
/** DisplayScreenContentFunction class implementation.
	@file DisplayScreenContentFunction.cpp

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

#include "DisplayScreenContentFunction.h"

#include "AccessParameters.h"
#include "RequestException.h"
#include "Request.h"
#include "CommercialLineTableSync.h"
#include "StopPointTableSync.hpp"
#include "ServicePointer.h"
#include "DisplayScreen.h"
#include "DisplayScreenTableSync.h"
#include "DisplayType.h"
#include "DisplayTypeTableSync.h"
#include "DeparturesTableInterfacePage.h"
#include "StopAreaTableSync.hpp"
#include "Interface.h"
#include "Env.h"
#include "LinePhysicalStop.hpp"
#include "PTModule.h"
#include "ContinuousService.h"
#include "SchedulesBasedService.h"
#include "ContinuousService.h"
#include "JourneyPattern.hpp"
#include "RollingStock.hpp"
#include "RollingStockFilter.h"
#include "City.h"
#include "Alarm.h"
#include "Webpage.h"
#include "RoadModule.h"
#include "PTUseRule.h"
#include "Destination.hpp"
#ifdef WITH_MYSQL
	#ifdef __gnu_linux__
		#define MYSQL_CONNECTOR_AVAILABLE 1
		#include "Connector.hpp"
	#endif
#endif
#include "RoutePlanningTableGenerator.h"
#include "ServerModule.h"
#include "InterfacePageException.h"
#include "MimeTypes.hpp"
#include "TransportNetwork.h"
#ifdef WITH_SCOM
	#include "SCOMModule.h"
	#include "SCOMData.h"
#endif

#include <sstream>
#include <boost/optional/optional_io.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace server;
	using namespace vehicle;
	using namespace pt;
	using namespace pt_website;
	using namespace interfaces;
	using namespace db;
	using namespace geography;
	using namespace departure_boards;
	using namespace cms;
	using namespace road;

	template<> const string util::FactorableTemplate<DisplayScreenContentFunction::_FunctionWithSite,DisplayScreenContentFunction>::FACTORY_KEY("tdg");

	namespace departure_boards
	{
		const string DisplayScreenContentFunction::PARAMETER_DATE = "date";
		const string DisplayScreenContentFunction::PARAMETER_WAY = "way";
		const string DisplayScreenContentFunction::PARAMETER_TB = "tb";
		const string DisplayScreenContentFunction::PARAMETER_INTERFACE_ID("i");
		const string DisplayScreenContentFunction::PARAMETER_MAC_ADDRESS("m");
		const string DisplayScreenContentFunction::PARAMETER_OPERATOR_CODE("oc");
		const string DisplayScreenContentFunction::PARAMETER_ROWS_NUMBER("rn");
		const string DisplayScreenContentFunction::PARAMETER_CITY_NAME("cn");
		const string DisplayScreenContentFunction::PARAMETER_STOP_NAME("sn");
		const string DisplayScreenContentFunction::PARAMETER_LINE_ID("lineid");
		const string DisplayScreenContentFunction::PARAMETER_ROLLING_STOCK_FILTER_ID = "tm";
		const string DisplayScreenContentFunction::PARAMETER_GENERATION_METHOD = "generation_method";
		const string DisplayScreenContentFunction::PARAMETER_USE_SAE_DIRECT_CONNECTION("use_sae_direct_connection");
		const string DisplayScreenContentFunction::PARAMETER_STOPS_LIST("stops_list");
		const string DisplayScreenContentFunction::PARAMETER_TIMETABLE_GROUPED_BY_AREA("timetable_grouped_by_area");
		const string DisplayScreenContentFunction::PARAMETER_DATA_SOURCE_FILTER("data_source_filter");
		const string DisplayScreenContentFunction::PARAMETER_SPLIT_CONTINUOUS_SERVICES("split_continuous_services");
		const string DisplayScreenContentFunction::PARAMETER_MAX_DAYS_NEXT_DEPARTURES("max_days_next_departures");
		const string DisplayScreenContentFunction::PARAMETER_USE_SCOM("use_scom");

		const string DisplayScreenContentFunction::DATA_MAC("mac");
		const string DisplayScreenContentFunction::DATA_DISPLAY_SERVICE_NUMBER("display_service_number");
		const string DisplayScreenContentFunction::DATA_DISPLAY_TEAM("display_team");
		const string DisplayScreenContentFunction::DATA_DISPLAY_TRACK_NUMBER("display_track_number");
		const string DisplayScreenContentFunction::DATA_INTERMEDIATE_STOPS_NUMBER("intermediate_stops_number");
		const string DisplayScreenContentFunction::DATA_STOP_NAME("stop_name");
		const string DisplayScreenContentFunction::DATA_TITLE("title");
		const string DisplayScreenContentFunction::DATA_WIRING_CODE("wiring_code");
		const string DisplayScreenContentFunction::DATA_DISPLAY_CLOCK("display_clock");
		const string DisplayScreenContentFunction::DATA_ROWS("rows");
		const string DisplayScreenContentFunction::DATA_DATE("date");
		const string DisplayScreenContentFunction::DATA_SUBSCREEN_("subscreen_");
		const string DisplayScreenContentFunction::DATA_FIRST_DEPARTURE_TIME("first_departure_time");
		const string DisplayScreenContentFunction::DATA_LAST_DEPARTURE_TIME("last_departure_time");
		const string DisplayScreenContentFunction::DATA_ONLY_REAL_TIME("onlyRealTime");

		const string DisplayScreenContentFunction::DATA_STOP_ID("stop_id");
		const string DisplayScreenContentFunction::DATA_OPERATOR_CODE("operatorCode");
		const string DisplayScreenContentFunction::DATA_NETWORK_ID("networkId");
		const string DisplayScreenContentFunction::DATA_NETWORK_NAME("networkName");
		const string DisplayScreenContentFunction::DATA_STOP_AREA_ID("stop_area_id");
		const string DisplayScreenContentFunction::DATA_STOP_AREA_NAME("stop_area_name");
		const string DisplayScreenContentFunction::DATA_STOP_AREA_CITY_NAME("stop_area_city_name");
		const string DisplayScreenContentFunction::DATA_STOP_AREA_CITY_ID("stop_area_city_id");

		const string DisplayScreenContentFunction::DATA_JOURNEYS("journeys");

		const string DisplayScreenContentFunction::DATA_ROW_RANK("row_rank");
		const string DisplayScreenContentFunction::DATA_PAGE_NUMBER("page_number");
		const string DisplayScreenContentFunction::DATA_BLINKS("blinks");
		const string DisplayScreenContentFunction::DATA_TIME("time");
		const string DisplayScreenContentFunction::DATA_WAITING_TIME("waiting_time");
		const string DisplayScreenContentFunction::DATA_PLANNED_TIME("planned_time");
		const string DisplayScreenContentFunction::DATA_DELAY("delay");
		const string DisplayScreenContentFunction::DATA_SERVICE_ID("service_id");
		const string DisplayScreenContentFunction::DATA_SERVICE_NUMBER("service_number");
		const string DisplayScreenContentFunction::DATA_TRACK("track");
		const string DisplayScreenContentFunction::DATA_TEAM("team");
		const string DisplayScreenContentFunction::DATA_TRANSPORT_MODE("transport_mode");
		const string DisplayScreenContentFunction::DATA_RANK_IN_PATH("rank_in_path");
		const string DisplayScreenContentFunction::DATA_DESTINATIONS("destinations");
		const string DisplayScreenContentFunction::DATA_DIRECTION("direction");
		const string DisplayScreenContentFunction::DATA_IS_CANCELED("is_canceled");

		const string DisplayScreenContentFunction::DATA_IS_SAME_CITY("is_same_city");
		const string DisplayScreenContentFunction::DATA_IS_END_STATION("is_end_station");
		const string DisplayScreenContentFunction::DATA_DESTINATION_RANK("destination_rank");
		const string DisplayScreenContentFunction::DATA_DESTINATION_GLOBAL_RANK("destination_global_rank");
		const string DisplayScreenContentFunction::DATA_TRANSFERS("transfers");
		const string DisplayScreenContentFunction::DATA_IS_CONTINUATION("is_continuation");
		const string DisplayScreenContentFunction::DATA_CONTINUATION_STARTS_AT_END("continuation_starts_at_end");

		const string DisplayScreenContentFunction::DATA_DEPARTURE_TIME("departure_time");
		const string DisplayScreenContentFunction::DATA_ARRIVAL_TIME("arrival_time");
		const string DisplayScreenContentFunction::DATA_TRANSFER_RANK("transfer_rank");

		const string DisplayScreenContentFunction::DATA_WITH_TRANSFER("with_transfer");

		const string DisplayScreenContentFunction::DATA_SECOND_TRACK("second_track");
		const string DisplayScreenContentFunction::DATA_SECOND_SERVICE_NUMBER("second_service_number");
		const string DisplayScreenContentFunction::DATA_SECOND_TIME("second_time");
		const string DisplayScreenContentFunction::DATA_SECOND_("second_");
		const string DisplayScreenContentFunction::DATA_SECOND_TRANSPORT_MODE("second_transport_mode");
		const string DisplayScreenContentFunction::DATA_TRANSFER_STOP_NAME("transfer_stop_name");

		const string DisplayScreenContentFunction::PARAMETER_MAIN_PAGE_ID("main_page_id");
		const string DisplayScreenContentFunction::PARAMETER_ROW_PAGE_ID("row_page_id");
		const string DisplayScreenContentFunction::PARAMETER_DESTINATION_PAGE_ID("destination_page_id");
		const string DisplayScreenContentFunction::PARAMETER_TRANSFER_DESTINATION_PAGE_ID("transfer_destination_page_id");

		const string DisplayScreenContentFunction::DATA_TIMETABLE("timetable");
		const string DisplayScreenContentFunction::DATA_SCHEDULE("schedule");
		const string DisplayScreenContentFunction::DATA_JOURNEY("journey");
		const string DisplayScreenContentFunction::DATA_DESTINATION("destination");
		const string DisplayScreenContentFunction::DATA_ROUTE_ID("route_id");
		const string DisplayScreenContentFunction::DATA_DATE_TIME("date_time");
		const string DisplayScreenContentFunction::DATA_ROLLING_STOCK("rolling_stock");
		const string DisplayScreenContentFunction::DATA_COMMERCIAL_LINE("commercial_line");
		const string DisplayScreenContentFunction::DATA_STOP_AREA("stop_area");
		const string DisplayScreenContentFunction::DATA_STOP_POINT("stop_point");

		const string DisplayScreenContentFunction::DATA_IS_REAL_TIME("realTime");
		const string DisplayScreenContentFunction::DATA_DUMMY_KEY("0");

		const time_duration DisplayScreenContentFunction::endOfService = hours(3);
		const string DisplayScreenContentFunction::DATA_HANDICAPPED_ACCESS("handicapped_access");


		const string PIPO_KEY("00");

		DisplayScreenContentFunction::SAELine DisplayScreenContentFunction::_SAELine;
		ptime DisplayScreenContentFunction::_nextUpdateLine = second_clock::local_time();

		FunctionAPI DisplayScreenContentFunction::getAPI() const
		{
			FunctionAPI api(
				"Departure Boards",
				"Display Screen Generation",
				"Generate a display screen for a stop area or a physical stop"
			);

			api.openParamGroup("Loading pre-configured display screen by id");
			api.addParams(Request::PARAMETER_OBJECT_ID, "A display screen roid", true);

			api.openParamGroup("Load of pre-configured display screen by mac address");
			api.addParams(DisplayScreenContentFunction::PARAMETER_MAC_ADDRESS,
						  "MAC address of the display screen. "
						  "Note : the MAC address must correspond to the content of the 'MAC address' "
						  "field of the board in the database. Actually, the content of this field is "
						  "free and must not necessarily be a MAC address.", false);

			api.openParamGroup("Generating from a stop area by id.");
			api.addParams(Request::PARAMETER_OBJECT_ID, "A stop area roid.", true);

			api.openParamGroup("Generating from a stop area by name");
			api.addParams(DisplayScreenContentFunction::PARAMETER_CITY_NAME, "City name.", true);
			api.addParams(DisplayScreenContentFunction::PARAMETER_STOP_NAME, "Stop name.", true);

			api.openParamGroup("Generating from a physical stop by id.");
			api.addParams(Request::PARAMETER_OBJECT_ID, "A physical stop roid.", true);

			api.openParamGroup("Generating from a physical stop by operator code");
			api.addParams(Request::PARAMETER_OBJECT_ID,
						  "Id of the stop area which the stop belongs to.", true);
			api.addParams(DisplayScreenContentFunction::PARAMETER_OPERATOR_CODE,
						  "Operator code of the physical stop.", true);

			api.openParamGroup("Content tweak");
			api.addParams(DisplayScreenContentFunction::PARAMETER_DATE,
						  "Date of the search (iso format : YYYY-MM-DD HH:II). Default is the actual time of the request.",
						  false);
			api.addParams(DisplayScreenContentFunction::PARAMETER_WAY,
						  "'backward' or 'forward'. If 'way' is 'forward' the answer will be the 'rn' "
						  "next departures after 'date'. "
						  "If 'way' is 'backward' the answer will be the 'rn' "
						  "previous departures just before 'date'.\n"
						  "WARNING : rn is the number of departures which have different start time. "
						  "Consequently, if two service starts at the same minutes this will count "
						  "for a single start!", false);
			api.addParams(DisplayScreenContentFunction::PARAMETER_INTERFACE_ID, "Deprecated. "
						  "id of display interface to use. If not specified, the standard XML output is used",
						  false);
			api.addParams(DisplayScreenContentFunction::PARAMETER_ROWS_NUMBER,
						  "Maximal returned number of results per physical stops. "
						  "Default value is 10.", false);
			api.addParams(DisplayScreenContentFunction::PARAMETER_LINE_ID,
						  "Commercial line ID : if given then results are only for this line", false);
			api.addParams(DisplayScreenContentFunction::PARAMETER_ROLLING_STOCK_FILTER_ID,
						  "Rolling stock filter ID.", false);
			api.addParams(DisplayScreenContentFunction::PARAMETER_GENERATION_METHOD,
						  "Specify the generation method. Must be one of 'standard_method', "
						  "'with_forced_destinations_method', 'route_planning' or "
						  "'display_children_only'. The default is 'standard_method'",
						  false);
			api.addParams(DisplayScreenContentFunction::PARAMETER_DATA_SOURCE_FILTER,
						  "Filter the stops in the answer linked with this data source",
						  false);
			api.addParams(DisplayScreenContentFunction::PARAMETER_USE_SAE_DIRECT_CONNECTION,
						  "Specify if SAE Direct Connection is needed for RealTime, "
						  "this will open unix sockets to a MySQL BDSI INEO server. \n"
						  "Works only on Linux",
						  false);
			api.addParams(DisplayScreenContentFunction::PARAMETER_TIMETABLE_GROUPED_BY_AREA,
						  "Group the results by StopArea, displaying next departures for each <Line, Destination> pair."
						  "Pairs are ordered by default Commercial Line order and destination order (alphanumeric).",
						  false);
			api.addParams(DisplayScreenContentFunction::PARAMETER_USE_SCOM,
						  "Specify if the data from Ineo's SCOM server should be used."
						  "Synthese must have been built with SCOM support and the SCOM listener setup for this to work.\n"
						  "For now, only the call using _displayDepartureBoardRow will be impacted."
						  "See the SCOM documentation for more information.\n"
						  "By default, SCOM is disabled."
						  "Possible values : true or false",
						  false);
			return api;
		}

		ParametersMap DisplayScreenContentFunction::_getParametersMap() const
		{
			ParametersMap map(FunctionWithSiteBase::_getParametersMap());
			if(_date && !_date->is_not_a_date_time()) map.insert(PARAMETER_DATE, *_date);
			if(_screen.get()) map.insert(Request::PARAMETER_OBJECT_ID, _screen->getKey());
			if(_mainPage.get())
			{
				map.insert(PARAMETER_MAIN_PAGE_ID, _mainPage->getKey());
			}
			else
			{
				map.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}
			if(_rowPage.get())
			{
				map.insert(PARAMETER_ROW_PAGE_ID, _rowPage->getKey());
			}
			if(_destinationPage.get())
			{
				map.insert(PARAMETER_DESTINATION_PAGE_ID, _destinationPage->getKey());
			}
			if(_transferDestinationPage.get())
			{
				map.insert(PARAMETER_TRANSFER_DESTINATION_PAGE_ID, _transferDestinationPage->getKey());
			}
			if(_rollingStockFilter.get() != NULL)
			{
				map.insert(PARAMETER_ROLLING_STOCK_FILTER_ID, _rollingStockFilter->getKey());
			}
			if(_dataSourceFilter)
			{
				map.insert(PARAMETER_DATA_SOURCE_FILTER, _dataSourceFilter->getKey());
			}
			return map;
		}



		void DisplayScreenContentFunction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				RegistryKeyType id(
					map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0)
				);
				if (!id)
				{
					id = map.getDefault<RegistryKeyType>(PARAMETER_TB, 0);
				}

				// CMS pages load

				optional<RegistryKeyType> pid(map.getOptional<RegistryKeyType>(PARAMETER_MAIN_PAGE_ID));
				if(pid) try
				{
					_mainPage = Env::GetOfficialEnv().get<Webpage>(*pid);
				}
				catch (ObjectNotFoundException<Webpage>&)
				{
					throw RequestException("No such main page");
				}
				if(!_mainPage.get())
				{
					setOutputFormatFromMap(map, MimeTypes::XML);
				}

				optional<RegistryKeyType> rid(map.getOptional<RegistryKeyType>(PARAMETER_ROW_PAGE_ID));
				if(rid) try
				{
					_rowPage = Env::GetOfficialEnv().get<Webpage>(*rid);
				}
				catch (ObjectNotFoundException<Webpage>&)
				{
					throw RequestException("No such row page");
				}

				optional<RegistryKeyType> did(map.getOptional<RegistryKeyType>(PARAMETER_DESTINATION_PAGE_ID));
				if(did) try
				{
					_destinationPage = Env::GetOfficialEnv().get<Webpage>(*did);
				}
				catch (ObjectNotFoundException<Webpage>&)
				{
					throw RequestException("No such destination page");
				}

				optional<RegistryKeyType> tid(map.getOptional<RegistryKeyType>(PARAMETER_TRANSFER_DESTINATION_PAGE_ID));
				if(tid) try
				{
					_transferDestinationPage = Env::GetOfficialEnv().get<Webpage>(*tid);
				}
				catch (ObjectNotFoundException<Webpage>&)
				{
					throw RequestException("No such transfer destination page");
				}

				if(map.getOptional<RegistryKeyType>(PARAMETER_DATA_SOURCE_FILTER)) try
				{
					_dataSourceFilter = Env::GetOfficialEnv().get<impex::DataSource>(map.get<RegistryKeyType>(PARAMETER_DATA_SOURCE_FILTER));
				}
				catch (ObjectNotFoundException<impex::DataSource>&)
				{
					throw RequestException("No such data source");
				}

				// Way 1 : pre-configured display screen

				// 1.1 by id
				if (decodeTableId(id) == DisplayScreenTableSync::TABLE.ID)
				{
					_screen = DisplayScreenTableSync::Get(id, *_env);
				}

				// 1.2 by mac address
				else if(!map.getDefault<string>(PARAMETER_MAC_ADDRESS).empty())
				{
					_screen = DisplayScreenTableSync::GetByMACAddress(
						*_env,
						map.get<string>(PARAMETER_MAC_ADDRESS),
						UP_LINKS_LOAD_LEVEL
					);
				}
				else
				{
					_FunctionWithSite::_setFromParametersMap(map);

					DisplayScreen* screen(new DisplayScreen);

					// Screen Generation Type
					string generationMethodStr(map.getDefault<string>(PARAMETER_GENERATION_METHOD, "standard_method"));
					DisplayScreen::GenerationMethod generationMethod = DisplayScreen::STANDARD_METHOD;
					if(generationMethodStr == "with_forced_destinations_method")
					{
						generationMethod = DisplayScreen::WITH_FORCED_DESTINATIONS_METHOD;
					}
					else if(generationMethodStr == "route_planning")
					{
						generationMethod = DisplayScreen::ROUTE_PLANNING;
					}
					else if(generationMethodStr == "display_children_only")
					{
						generationMethod = DisplayScreen::DISPLAY_CHILDREN_ONLY;
					}
					//Generation Method of the pannel is "STANDARD_METHOD"
					screen->setGenerationMethod(generationMethod);
					_type.reset(new DisplayType);
					_type->set<RowsNumber>(map.getDefault<size_t>(PARAMETER_ROWS_NUMBER, 10));


					// Way 3 : physical stop

					// 3.1 by id
					if(decodeTableId(id) == StopPointTableSync::TABLE.ID)
					{
						boost::shared_ptr<const StopPoint> stop(
								Env::GetOfficialEnv().get<StopPoint>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID))
						);

						if(!_dataSourceFilter || stop->hasLinkWithSource(*_dataSourceFilter))
						{
							screen->set<BroadCastPoint>(*(const_cast<StopArea*>(stop->getConnectionPlace())));
							screen->set<AllPhysicalDisplayed>(false);
							ArrivalDepartureTableGenerator::PhysicalStops stopsFilter;
							stopsFilter.insert(make_pair(stop->getKey(), stop.get()));
							screen->setStops(stopsFilter);
						}
					}
					// 5 by id list
					else if(!map.getDefault<string>(PARAMETER_STOPS_LIST).empty())
					{
						string stopsStr(map.getDefault<string>(PARAMETER_STOPS_LIST));
						try
						{
							if(!stopsStr.empty())
							{
								vector<string> stopsVect;
								split(stopsVect, stopsStr, is_any_of(",; "));

								screen->set<AllPhysicalDisplayed>(false);
								ArrivalDepartureTableGenerator::PhysicalStops stopsFilter;

								BOOST_FOREACH(string& stopItem, stopsVect)
								{
									vector<string> tripletVect;
									split(tripletVect, stopItem, is_any_of("|"));
									const CommercialLine* lineFilter = NULL;
									const StopArea* destinationFilter = NULL;

									if(tripletVect.size() > 1)
									{
										RegistryKeyType lineId = lexical_cast<RegistryKeyType>(tripletVect[1]);
										if(decodeTableId(lineId) == CommercialLineTableSync::TABLE.ID)
										{
											boost::shared_ptr<const CommercialLine> commercialLine(
												Env::GetOfficialEnv().get<CommercialLine>(lineId)
											);
											lineFilter = commercialLine.get();
										}
									}
									if(tripletVect.size() > 2)
									{
										RegistryKeyType destId = lexical_cast<RegistryKeyType>(tripletVect[2]);
										if(decodeTableId(destId) == StopAreaTableSync::TABLE.ID)
										{
											boost::shared_ptr<const StopArea> stopArea(
												Env::GetOfficialEnv().get<StopArea>(destId)
											);
											destinationFilter = stopArea.get();
										}
									}

									RegistryKeyType stopId = lexical_cast<RegistryKeyType>(tripletVect[0]);
									if(decodeTableId(stopId) == StopPointTableSync::TABLE.ID)
									{
										boost::shared_ptr<const StopPoint> stop(
											Env::GetOfficialEnv().get<StopPoint>(stopId)
										);

										if(_dataSourceFilter && !stop->hasLinkWithSource(*_dataSourceFilter))
											continue;

										screen->set<BroadCastPoint>(*(const_cast<StopArea*>(stop->getConnectionPlace())));
										stopsFilter.insert(make_pair(stop->getKey(), stop.get()));
										_lineDestinationFilter.insert(LineDestinationFilter::value_type(stop.get(), make_pair(lineFilter, destinationFilter)));
									}
									else if (decodeTableId(stopId) == StopAreaTableSync::TABLE.ID)
									{
										boost::shared_ptr<const StopArea> stop(
											Env::GetOfficialEnv().get<StopArea>(stopId)
										);
										BOOST_FOREACH(const StopArea::PhysicalStops::value_type& itStop, stop->getPhysicalStops())
										{
											const StopPoint& stop(*itStop.second);
											if(_dataSourceFilter && !stop.hasLinkWithSource(*_dataSourceFilter))
												continue;

											screen->set<BroadCastPoint>(*(const_cast<StopArea*>(stop.getConnectionPlace())));
											stopsFilter.insert(make_pair(stop.getKey(), &stop));
											_lineDestinationFilter.insert(LineDestinationFilter::value_type(&stop, make_pair(lineFilter, destinationFilter)));
										}
									}
								}
								screen->setStops(stopsFilter);
							}
						}
						catch(bad_lexical_cast&)
						{
							throw RequestException("Stops List is unreadable");
						}
					}
					// 3.2 by operator code
					//4.1 by operator code
					else if(!map.getDefault<string>(PARAMETER_OPERATOR_CODE).empty())
					{
						//If an oc was given we search corresponding physical stop
						string oc(map.get<string>(PARAMETER_OPERATOR_CODE));

						set<string> stopsSet;
						split(stopsSet, oc, is_any_of(",; "));

						//Get StopPoint Global Registry
						typedef const pair<const RegistryKeyType, boost::shared_ptr<StopPoint> > myType;
						ArrivalDepartureTableGenerator::PhysicalStops pstops;
						BOOST_FOREACH(myType& myStop,Env::GetOfficialEnv().getRegistry<StopPoint>())
						{
							BOOST_FOREACH(const impex::Importable::DataSourceLinks::value_type& dataSourceLink, myStop.second->getDataSourceLinks())
							{
								BOOST_FOREACH(string code, stopsSet)
								{
									if(dataSourceLink.second == code)
									{
										if(_dataSourceFilter && dataSourceLink.first->getKey() != _dataSourceFilter->getKey())
											continue;

										stopsSet.erase(code);
										pstops.insert(make_pair(myStop.second->getKey(), myStop.second.get()));
										screen->set<BroadCastPoint>(*(const_cast<StopArea*>(myStop.second->getConnectionPlace())));
										break;
									}
								}
							}
						}

						screen->set<AllPhysicalDisplayed>(false);
						screen->setStops(pstops);
					}

					// Way 2 : connection place

					// 2.1 by id
					else if(decodeTableId(id) == StopAreaTableSync::TABLE.ID)
					{
						screen->set<BroadCastPoint>(*(const_cast<StopArea*>(Env::GetOfficialEnv().getRegistry<StopArea>().get(id).get())));
						screen->set<AllPhysicalDisplayed>(true);
					}

					// 2.2 by name
					else if (!map.getDefault<string>(PARAMETER_CITY_NAME).empty() && !map.getDefault<string>(PARAMETER_STOP_NAME).empty())
					{
						boost::shared_ptr<geography::Place> place(RoadModule::FetchPlace(map.get<string>(PARAMETER_CITY_NAME), map.get<string>(PARAMETER_STOP_NAME)));
						if(!dynamic_cast<StopArea*>(place.get()))
						{
							throw RequestException("This place is not a stop area");
						}
						screen->set<BroadCastPoint>(*(const_cast<StopArea*>(dynamic_cast<StopArea*>(place.get()))));
						screen->set<AllPhysicalDisplayed>(true);
					}
					else // Failure
					{
						throw RequestException("Not a display screen nor a connection place");
					}


					// Way
					_wayIsBackward = false;
					optional<string> way(map.getOptional<string>(PARAMETER_WAY));
					if(way && (*way) == "backward")
					{
						_wayIsBackward = true;
					}

					_lineToDisplay = map.getOptional<RegistryKeyType>(PARAMETER_LINE_ID);

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

					// Check if need to use SAE RealTime Direct Connection
					_useSAEDirectConnection = false;
					optional<string> rtStr(map.getOptional<string>(PARAMETER_USE_SAE_DIRECT_CONNECTION));
					if(rtStr && (*rtStr) == "1")
					{
					#ifdef MYSQL_CONNECTOR_AVAILABLE
						_useSAEDirectConnection = true;
					#else
						throw RequestException("SAE Direct Connection works only on Linux platforms and build with MySQL enabled");
					#endif
					}

					_timetableGroupedByArea = map.getDefault<bool>(PARAMETER_TIMETABLE_GROUPED_BY_AREA, false);
					_splitContinuousServices = map.getDefault<bool>(PARAMETER_SPLIT_CONTINUOUS_SERVICES, false);
					
					screen->set<DisplayTypePtr>(*(_type.get()));
					//If request contains an interface : build a screen, else prepare custom xml answer
					optional<RegistryKeyType> idReg(map.getOptional<RegistryKeyType>(PARAMETER_INTERFACE_ID));
					if(idReg)
					{
						try
						{
							_type->set<DisplayInterface>(*(const_cast<Interface*>(Env::GetOfficialEnv().getRegistry<Interface>().get(*idReg).get())));
						}
						catch (ObjectNotFoundException<Interface>&)
						{
							throw RequestException("No such screen type");
						}
					}
					else if(_mainPage.get() && !_useSAEDirectConnection && !_timetableGroupedByArea)
					{
						_type->set<DisplayMainPage>(*(const_cast<Webpage*>(_mainPage.get())));
						_type->set<DisplayRowPage>(*(const_cast<Webpage*>(_rowPage.get())));
						_type->set<DisplayDestinationPage>(*(const_cast<Webpage*>(_destinationPage.get())));
						_type->set<DisplayTransferDestinationPage>(*(const_cast<Webpage*>(_transferDestinationPage.get())));

						if(_lineToDisplay)
						{
							LineFilter lf;
							lf.insert(
								make_pair(
									_env->getRegistry<CommercialLine>().get(*_lineToDisplay).get(),
									false
							)	);
							screen->setAllowedLines(lf);
						}
					}
					_screen.reset(screen);
				}

				// Date
				if(!map.getDefault<string>(PARAMETER_DATE).empty() && map.getDefault<string>(PARAMETER_DATE) != "A")
				{
					_date = time_from_string(map.get<string>(PARAMETER_DATE));

					// SAE real time only for the current service day
					ptime now = second_clock::local_time();
					time_period period(ptime(now.date(), endOfService), hours(24));

					if(now.time_of_day() < endOfService)
						period.shift(hours(-24));

					if(!period.contains(*_date))
						_useSAEDirectConnection = false;
				}

				_maxDaysNextDepartures = map.getDefault<int>(PARAMETER_MAX_DAYS_NEXT_DEPARTURES, 1);

				// Type control
				if(!_screen->get<DisplayTypePtr>())
				{
					throw RequestException("The screen "+ lexical_cast<string>(id) +" has no type.");
				}

				// Enable SCOM if available
				_scom = map.isDefined(PARAMETER_USE_SCOM) && map.get<string>(PARAMETER_USE_SCOM) == "true";
				#ifndef WITH_SCOM
				if (_scom)
				{
					Log::GetInstance().debug("SCOM disabled in compilation (no -DWITH_SCOM), it will not be used");
					_scom = false;
				}
				#endif

			}
			catch (ObjectNotFoundException<DisplayScreen> e)
			{
				throw RequestException("Display screen not found "+ e.getMessage());
			}
			catch (ObjectNotFoundException<StopArea>& e)
			{
				throw RequestException("Connection place not found "+ e.getMessage());
			}
			catch (ObjectNotFoundException<StopPoint>& e)
			{
				throw RequestException("Physical stop not found "+ e.getMessage());
			}
		}



		void DisplayScreenContentFunction::concatXMLResult(
			std::ostream& stream,
			ServicePointer& servicePointer
		) const {
			const SchedulesBasedService* service = static_cast<const SchedulesBasedService*>(servicePointer.getService());
			const JourneyPattern* journeyPattern = static_cast<const JourneyPattern*>(service->getPath());
			const StopPoint* stop = static_cast<const StopPoint*>(servicePointer.getDepartureEdge()->getFromVertex());
			const StopArea* connPlace(stop->getConnectionPlace());

			//Here we got our service !
			stream <<"<journey routeId=\""<< journeyPattern->getKey() <<
				"\" dateTime=\""    << servicePointer.getDepartureDateTime() <<
				"\" blink=\"" << "0" <<
					"\" "<< DATA_IS_REAL_TIME <<"=\"" << (service->hasRealTimeData() ? "yes":"no") <<
				"\" "<< DATA_WAITING_TIME <<"=\"" << to_simple_string(servicePointer.getDepartureDateTime() - second_clock::local_time()) <<
				"\">";

			stream << "<stop id=\"" << stop->getKey() <<
				"\" operatorCode=\""<< stop->getCodeBySources() <<
				"\" name=\""        << stop->getName() <<
				"\" />";

			RollingStock* rs = journeyPattern->getRollingStock();
			if(rs)
			{
				stream <<"<transportMode id=\""<< rs->getKey() <<
					"\" name=\""               << rs->getName() <<
					"\" article=\""            << rs->get<Article>()<<
					"\" />";
			}

			const CommercialLine* commercialLine(journeyPattern->getCommercialLine());

			if(commercialLine)
			{
				stream <<"<line id=\""<< commercialLine->getKey() <<
					"\" creatorId=\"";
				if(!commercialLine->getDataSourceLinks().empty())
				{
					stream << commercialLine->getDataSourceLinks().begin()->second;
				}

				stream <<
					"\" name=\""      << commercialLine->getName() <<
					"\" shortName=\"" << commercialLine->getShortName() <<
					"\" longName=\""  << commercialLine->getLongName() <<
					"\" color=\""  << (
						commercialLine->getColor() ?
						commercialLine->getColor()->toString():
						""
					) <<
					"\" xmlColor=\""  << (
						commercialLine->getColor() ?
						commercialLine->getColor()->toXMLColor():
						""
					) <<
					"\" style=\""     << commercialLine->getStyle() <<
					"\" image=\""     << commercialLine->getImage() <<
					"\" direction=\"" << (
						journeyPattern->getDirection().empty() && journeyPattern->getDirectionObj() ?
						journeyPattern->getDirectionObj()->get<DisplayedText>() :
						journeyPattern->getDirection()
					) <<
					"\" wayback=\"" << (journeyPattern->getWayBack() ? "1" : "0") <<
					"\" >";
				stream <<"<network id=\""<< commercialLine->getNetwork()->getKey() <<
					"\" name=\""      << commercialLine->getNetwork()->getName() <<
					"\" image=\""      << commercialLine->getNetwork()->getImage() <<
					"\" />";
				stream << "</line>";
			}

			const StopArea& origin(
					*journeyPattern->getOrigin()->getConnectionPlace()
			);
			stream << "<origin id=\""  << origin.getKey() <<
					"\" name=\""           << origin.getName() <<
					"\" cityName=\""       << origin.getCity()->getName() <<
					"\" />";

			const StopArea& destination(
					*journeyPattern->getDestination()->getConnectionPlace()
			);
			stream << "<destination id=\"" << destination.getKey() <<
					"\" name=\""           << destination.getName() <<
					"\" cityName=\""       << destination.getCity()->getName() <<
					"\" />";

			stream << "<stopArea id=\""<< connPlace->getKey()<<
				"\" name=\""           << connPlace->getName() <<
				"\" cityId=\""         << connPlace->getCity()->getKey() <<
				"\" cityName=\""       << connPlace->getCity()->getName() <<
				"\" directionAlias=\"" << connPlace->getName26() <<
				"\" />";

			stream << "</journey>";
		}



		void DisplayScreenContentFunction::concatXMLResultRealTime(
				std::ostream& stream,
				RealTimeService& serviceReal
		) const {
			boost::shared_ptr<const StopPoint> stop = serviceReal.stop;
			const StopArea* connPlace(stop->getConnectionPlace());

			stream <<"<journey routeId=\""<< DATA_DUMMY_KEY <<
				"\" dateTime=\""    << serviceReal.datetime <<
				"\" blink=\"" << "0" <<
				"\" "<< DATA_IS_REAL_TIME <<"=\"" << (serviceReal.realTime ? "yes" : "no") <<
				"\" "<< DATA_WAITING_TIME <<"=\"" << to_simple_string(serviceReal.datetime - second_clock::local_time()) <<
				"\">";

			stream << "<stop id=\"" << stop->getKey() <<
				"\" operatorCode=\""<< stop->getCodeBySources() <<
				"\" name=\""        << stop->getName() <<
				"\" />";

			if(serviceReal.commercialLine)
			{
				stream <<"<line id=\""<< serviceReal.commercialLine->getKey() <<
					"\" creatorId=\"";
				if(!serviceReal.commercialLine->getDataSourceLinks().empty())
				{
					stream << serviceReal.commercialLine->getDataSourceLinks().begin()->second;
				}
				stream <<
					"\" name=\""      << serviceReal.commercialLine->getName() <<
					"\" shortName=\"" << serviceReal.commercialLine->getShortName() <<
					"\" longName=\""  << serviceReal.commercialLine->getLongName() <<
					"\" color=\""     << (serviceReal.commercialLine->getColor() ? serviceReal.commercialLine->getColor()->toString() : "") <<
					"\" xmlColor=\""  << (serviceReal.commercialLine->getColor() ? serviceReal.commercialLine->getColor()->toXMLColor() : "") <<
					"\" style=\""     << serviceReal.commercialLine->getStyle() <<
					"\" image=\""     << serviceReal.commercialLine->getImage() <<
					"\" >";
				stream <<"<network id=\""<< serviceReal.commercialLine->getNetwork()->getKey() <<
					"\" name=\""      << serviceReal.commercialLine->getNetwork()->getName() <<
					"\" image=\""      << serviceReal.commercialLine->getNetwork()->getImage() <<
					"\" />";
				stream << "</line>";
			}

			stream << "<destination id=\"" << serviceReal.destination->getKey() <<
					"\" name=\""           << serviceReal.destination->getName() <<
					"\" cityName=\""       << serviceReal.destination->getCity()->getName() <<
					"\" />";

			stream << "<stopArea id=\""<< connPlace->getKey()<<
				"\" name=\""           << connPlace->getName() <<
				"\" cityId=\""         << connPlace->getCity()->getKey() <<
				"\" cityName=\""       << connPlace->getCity()->getName() <<
				"\" directionAlias=\"" << connPlace->getName26() <<
				"\" />";

			stream << "</journey>";
		}



		void DisplayScreenContentFunction::addJourneyToParametersMap(
			ParametersMap& pm,
			ServicePointer& servicePointer
		) const {
			const SchedulesBasedService* service = static_cast<const SchedulesBasedService*>(servicePointer.getService());
			const JourneyPattern* journeyPattern = static_cast<const JourneyPattern*>(service->getPath());
			const StopPoint* stop = static_cast<const StopPoint*>(servicePointer.getDepartureEdge()->getFromVertex());
			const StopArea* connPlace(stop->getConnectionPlace());

			pm.insert(DATA_STOP_ID, stop->getKey());
			pm.insert(DATA_STOP_NAME, stop->getName());
			pm.insert(DATA_OPERATOR_CODE, stop->getCodeBySources());

			pm.insert(DATA_STOP_AREA_ID, connPlace->getKey());
			pm.insert(DATA_STOP_AREA_NAME, connPlace->getName());
			pm.insert(DATA_STOP_AREA_CITY_NAME, connPlace->getCity()->getName());
			pm.insert(DATA_STOP_AREA_CITY_ID, connPlace->getCity()->getKey());

			boost::shared_ptr<ParametersMap> journeyPm(new ParametersMap());
			journeyPm->insert("route_id", journeyPattern->getKey());
			journeyPm->insert("date_time", servicePointer.getDepartureDateTime());
			journeyPm->insert(DATA_IS_REAL_TIME, (_useSAEDirectConnection ? string("no") : string("yes")));
			journeyPm->insert(DATA_WAITING_TIME, to_simple_string(servicePointer.getDepartureDateTime() - second_clock::local_time()));

			journeyPm->insert(DATA_PLANNED_TIME, servicePointer.getTheoreticalDepartureDateTime());

			if(servicePointer.getRealTimeDepartureVertex())
			{
				journeyPm->insert(
					DATA_TRACK,
					static_cast<const StopPoint*>(servicePointer.getRealTimeDepartureVertex())->getName()
				);
			}

			if(journeyPattern && journeyPattern->getNetwork())
			{
				journeyPm->insert(DATA_NETWORK_ID, journeyPattern->getNetwork()->getKey());
				journeyPm->insert(DATA_NETWORK_NAME, journeyPattern->getNetwork()->getName());
			}

			// Waiting time
			time_duration waitingTime(servicePointer.getDepartureDateTime() - second_clock::local_time());
			journeyPm->insert(DATA_WAITING_TIME, to_simple_string(waitingTime));

			// Handicapped access
			const PTUseRule* handicappedUserRule = dynamic_cast<const PTUseRule*>(
				&(service)->getUseRule(USER_HANDICAPPED - USER_CLASS_CODE_OFFSET)
			);
			journeyPm->insert(DATA_HANDICAPPED_ACCESS, handicappedUserRule ? handicappedUserRule->getAccessCapacity().get_value_or(9999) != 0 : true);

			// Is realtime
			journeyPm->insert(DATA_IS_REAL_TIME, service->hasRealTimeData());

			boost::shared_ptr<ParametersMap> stopPM(new ParametersMap);
			stop->toParametersMap(*stopPM, false);
			journeyPm->insert("stop", stopPM);

			RollingStock* rs = journeyPattern->getRollingStock();
			if(rs)
			{
				boost::shared_ptr<ParametersMap> rsPM(new ParametersMap);
				rs->toParametersMap(*rsPM, true);
				journeyPm->insert("rollingStock", rsPM);
			}

			const CommercialLine* commercialLine(journeyPattern->getCommercialLine());

			if(commercialLine)
			{
				boost::shared_ptr<ParametersMap> linePM(new ParametersMap);
				commercialLine->toParametersMap(*linePM, true);
				linePM->insert("wayback",journeyPattern->getWayBack());
				journeyPm->insert("line", linePM);
			}

			const StopArea& origin(
				*journeyPattern->getOrigin()->getConnectionPlace()
			);
			boost::shared_ptr<ParametersMap> originPM(new ParametersMap);
			origin.toParametersMap(*originPM, true);
			journeyPm->insert("origin", originPM);


			const StopArea& destination(
				*journeyPattern->getDestination()->getConnectionPlace()
			);
			boost::shared_ptr<ParametersMap> destinationPM(new ParametersMap);
			destination.toParametersMap(*destinationPM, true);
			journeyPm->insert("destination", destinationPM);

			boost::shared_ptr<ParametersMap> connPlacePM(new ParametersMap);
			connPlace->toParametersMap(*connPlacePM, true);
			journeyPm->insert("stopArea", connPlacePM);

			pm.insert("journey", journeyPm);
		}



		void DisplayScreenContentFunction::addJourneyToParametersMapRealTime(
			ParametersMap& pm,
			RealTimeService& serviceReal
		) const {
			boost::shared_ptr<const StopPoint> stop = serviceReal.stop;
			const StopArea* connPlace(stop->getConnectionPlace());

			pm.insert(DATA_STOP_ID, stop->getKey());
			pm.insert(DATA_STOP_NAME, stop->getName());
			pm.insert(DATA_OPERATOR_CODE, stop->getCodeBySources());

			pm.insert(DATA_STOP_AREA_ID, connPlace->getKey());
			pm.insert(DATA_STOP_AREA_NAME, connPlace->getName());
			pm.insert(DATA_STOP_AREA_CITY_NAME, connPlace->getCity()->getName());
			pm.insert(DATA_STOP_AREA_CITY_ID, connPlace->getCity()->getKey());

			boost::shared_ptr<ParametersMap> journeyPm(new ParametersMap());
			journeyPm->insert("route_id", DATA_DUMMY_KEY);
			journeyPm->insert("date_time", serviceReal.datetime);
			journeyPm->insert(DATA_IS_REAL_TIME, (serviceReal.realTime ? string("yes") : string("no")));
			journeyPm->insert(DATA_WAITING_TIME, to_simple_string(serviceReal.datetime - second_clock::local_time()));

			boost::shared_ptr<ParametersMap> stopPM(new ParametersMap);
			stop->toParametersMap(*stopPM, false);
			journeyPm->insert("stop", stopPM);

			if(serviceReal.commercialLine)
			{
				boost::shared_ptr<ParametersMap> linePM(new ParametersMap);
				serviceReal.commercialLine->toParametersMap(*linePM, true);
				journeyPm->insert("line", linePM);
			}

			boost::shared_ptr<ParametersMap> destinationPM(new ParametersMap);
			serviceReal.destination->toParametersMap(*destinationPM, true);
			journeyPm->insert("destination", destinationPM);

			boost::shared_ptr<ParametersMap> connPlacePM(new ParametersMap);
			connPlace->toParametersMap(*connPlacePM, true);
			journeyPm->insert("stopArea", connPlacePM);

			pm.insert("journey", journeyPm);
		}



		util::ParametersMap DisplayScreenContentFunction::run( std::ostream& stream, const Request& request ) const
		{
			if(!_screen->get<DisplayTypePtr>())
			{
				return ParametersMap();
			}
			if(_screen->get<DisplayTypePtr>()->get<DisplayInterface>() || _screen->get<DisplayTypePtr>()->get<DisplayMainPage>()
			){
				ptime date(_date ? *_date : second_clock::local_time());
				if(!_screen->get<MaintenanceIsOnline>() ||
					!(_screen->get<BroadCastPoint>())
				){
					return ParametersMap();
				}

				try
				{
					// Start time
					ptime realStartDateTime(date);
					realStartDateTime -= minutes(_screen->get<ClearingDelay>());

					// End time
					ptime endDateTime(realStartDateTime);
					endDateTime += minutes(_screen->get<MaxDelay>());

					VariablesMap variables;

					if(_screen->getGenerationMethod() == DisplayScreen::ROUTE_PLANNING)
					{
						RoutePlanningTableGenerator generator(
							*_screen->get<BroadCastPoint>(),
							_screen->getDisplayedPlaces(),
							realStartDateTime,
							endDateTime,
							_screen->get<RoutePlanningWithTransfer>()
						);

						RoutePlanningList displayedObject(generator.run());
						if(_screen->get<DisplayTypePtr>()->get<DisplayMainPage>())
						{
							_displayRoutePlanningBoard(
								stream,
								request,
								Env::GetOfficialEnv().getSPtr(_screen->get<DisplayTypePtr>()->get<DisplayMainPage>().get_ptr()),
								Env::GetOfficialEnv().getSPtr(_screen->get<DisplayTypePtr>()->get<DisplayRowPage>().get_ptr()),
								Env::GetOfficialEnv().getSPtr(_screen->get<DisplayTypePtr>()->get<DisplayDestinationPage>().get_ptr()),
								realStartDateTime,
								displayedObject,
								*_screen
							);
						}
					}
					else if(_screen->getGenerationMethod() == DisplayScreen::DISPLAY_CHILDREN_ONLY)
					{
						// computes nothing, children will be called by the cms method
					}
					else
					{
						ArrivalDepartureList displayedObject(
							_screen->generateStandardScreen(realStartDateTime, endDateTime, true, _scom)
						);

						if(_screen->get<DisplayTypePtr>()->get<DisplayInterface>() &&
							_screen->get<DisplayTypePtr>()->get<DisplayInterface>()->getPage<DeparturesTableInterfacePage>()
						){
							_screen->get<DisplayTypePtr>()->get<DisplayInterface>()->getPage<DeparturesTableInterfacePage>()->display(
								stream,
								variables,
								_screen->get<Title>(),
								_screen->get<WiringCode>(),
								_screen->get<ServiceNumberDisplay>(),
								_screen->get<TrackNumberDisplay>(),
								_screen->get<DisplayTeam>(),
								_screen->get<DisplayTypePtr>()->get<MaxStopsNumber>(),
								_screen->get<BlinkingDelay>(),
								_screen->get<DisplayClock>(),
								_screen->get<BroadCastPoint>().get_ptr(),
								displayedObject,
								&request
							);
						}
						else
						{
							assert(_screen->get<DisplayTypePtr>()->get<DisplayMainPage>());

							_displayDepartureBoard(
								stream,
								request,
								Env::GetOfficialEnv().getSPtr(_screen->get<DisplayTypePtr>()->get<DisplayMainPage>().get_ptr()),
								Env::GetOfficialEnv().getSPtr(_screen->get<DisplayTypePtr>()->get<DisplayRowPage>().get_ptr()),
								Env::GetOfficialEnv().getSPtr(_screen->get<DisplayTypePtr>()->get<DisplayDestinationPage>().get_ptr()),
								Env::GetOfficialEnv().getSPtr(_screen->get<DisplayTypePtr>()->get<DisplayTransferDestinationPage>().get_ptr()),
								realStartDateTime,
								displayedObject,
								*_screen
							);
						}
					}
				}
				catch (InterfacePageException&)
				{
				}

				return util::ParametersMap();
			}

			ptime now = second_clock::local_time();
			bool noRealTime(true);

		#ifdef MYSQL_CONNECTOR_AVAILABLE
			string SAEDateString, SAEHourString;
			string SAELimit = boost::lexical_cast<string>(_screen->get<DisplayTypePtr>()->get<RowsNumber>());
			ptime SAEDate;

			boost::shared_ptr<const impex::DataSource> SAEDataSource;
			try
			{
				SAEDataSource = util::Env::GetOfficialEnv().get<impex::DataSource>(DeparturesTableModule::GetIneoStopsDataSourceId());
			}
			catch (ObjectNotFoundException<impex::DataSource>&)
			{
			}

			if(_useSAEDirectConnection)
			{
				shared_ptr<MySQLconnector> connector(new MySQLconnector());

				SAEDate = (_date ? *_date : now);
				// SAE system only work on one day, representing service after midnight at its start
				if(SAEDate.time_of_day() < endOfService)
					SAEDate -= hours(24);

				SAEDateString = boost::gregorian::to_iso_extended_string(SAEDate.date());
				SAEHourString = boost::posix_time::to_simple_string(SAEDate.time_of_day());

				// First realTime request, cache list of lines handled by the SAE
				if(_SAELine.empty() || (now >= _nextUpdateLine))
				{
					try
					{
						// Standard output, without leading 0 lowercase
						string lineListRequest = "SELECT LOWER(TRIM(LEADING '0' FROM mnemo)) AS mnemo FROM LIGNE where jour = '" + SAEDateString + "';";
						shared_ptr<MySQLResult> result = connector->execQuery(lineListRequest);

						while(result->next())
							_SAELine.insert(SAELine::value_type(result->getInfo("mnemo"), SAELine::mapped_type()));

						// Retrieve commercial line from short name
						BOOST_FOREACH(const Registry<CommercialLine>::value_type& curLine, Env::GetOfficialEnv().getRegistry<CommercialLine>())
						{
							// Case insensitive and remove leading 0 : 02S = 2s
							string curShortName = boost::algorithm::to_lower_copy(
								trim_left_copy_if(curLine.second->getShortName(), is_any_of("0"))
							);
							BOOST_FOREACH(SAELine::value_type& linePair, _SAELine)
							{
								if(curShortName == linePair.first)
									_SAELine[linePair.first].insert(curLine.second->getKey());
							}
						}

						_nextUpdateLine = ptime(_nextUpdateLine.date(), hours(4));
						if(SAEDate.time_of_day() >= hours(4))
							_nextUpdateLine += hours(24);
					}
					catch(Exception &e)
					{
						util::Log::GetInstance().warn(string("Exception while querying lines list : ") + e.what());
					}
				}
			}
		#endif

			if(_timetableGroupedByArea)
			{
				ParametersMap result;
				AccessParameters ap;

				typedef set<ServicePointer, Spointer_comparator> OrderedServiceSet;
				typedef set<RealTimeService, Spointer_comparator> OrderedRealTimeServiceSet;
				typedef pair<OrderedRealTimeServiceSet, OrderedServiceSet> OrderedServices;

				typedef map<LineDestinationKey, OrderedServices, LineDestinationKey_comparator> DeparturesByDestination;
				typedef map<const StopArea*, DeparturesByDestination> DestinationsServicesByArea;

				set<RegistryKeyType> linesWithRealTime;
				DestinationsServicesByArea serviceMap;

			#ifdef MYSQL_CONNECTOR_AVAILABLE
				if(_useSAEDirectConnection)
				{
					stringstream operatorCodes, filters, request;

					BOOST_FOREACH(const ArrivalDepartureTableGenerator::PhysicalStops::value_type& it, _screen->getPhysicalStops())
					{
						BOOST_FOREACH(const impex::Importable::DataSourceLinks::value_type& dataSourceLink, it.second->getDataSourceLinks())
						{
							if(SAEDataSource && (SAEDataSource.get() != dataSourceLink.first))
								continue;

							if(!dataSourceLink.second.empty())
							{
								operatorCodes << (operatorCodes.str().empty() ? "" : ","); 
								operatorCodes << "'" << setfill('0') << setw(5) << dataSourceLink.second;
								operatorCodes << "'";
							}
						}
					}

					if(!operatorCodes.str().empty())
					{
						noRealTime = false;
						shared_ptr<MySQLconnector> connector(new MySQLconnector());

					// Limit the research to X hour after date, X is 1 hour times the number of result we wish for
					time_duration searchDepth(hours(1) * _screen->get<DisplayTypePtr>()->get<RowsNumber>());

					filters << "((h.hrd > '" << SAEHourString << "' AND h.hrd <= ADDTIME('" << SAEHourString << "', '" << to_simple_string(searchDepth) << "')) ";
					// If after midnight, check if we do not cross over end of service
					if(SAEDate.time_of_day() < endOfService)
						filters << "AND h.hrd <= '" << to_simple_string(endOfService) << "')";
					// Otherwise make sure, if we are just before midnight that we get after midnight services, without crossing end of service
					else
						filters << "OR (h.hrd <= LEAST(TIMEDIFF(ADDTIME('" << SAEHourString + "', '" << to_simple_string(searchDepth) << "') , '24:00:00'), '" << to_simple_string(endOfService) << "')))";

					if(_lineToDisplay)
					{
						shared_ptr<const CommercialLine> commercialLine(_env->getRegistry<CommercialLine>().get(*_lineToDisplay));
						string shortNameToDisplay = boost::algorithm::to_lower_copy(
							trim_left_copy_if(commercialLine->getShortName(), is_any_of("0"))
						);
						filters << " AND LOWER(TRIM(LEADING '0' FROM h.ligne)) = '" << shortNameToDisplay << "'";
					}

					try
					{
						string requestInitVars("SET @num = 0, @itineraire_dest = '', @itineraire_lign_com = '', @itineraire_arret = ''");
						connector->execQuery(requestInitVars);

						request << " SELECT"
								<< "	@num := IF(@itineraire_lign_com = ligne, IF(@itineraire_dest = oc_arrivee, IF(@itineraire_arret = arret_oc, @num + 1, 1), 1), 1) AS row_number,"
								<< "	horaire, fiable, @itineraire_arret := arret_oc AS arret_oc, @itineraire_dest := oc_arrivee AS oc_arrivee, @itineraire_lign_com := ligne AS ligne"
								<< " FROM ("
								<< "	SELECT"
								<< "		h.hrd AS horaire, fiable, a.nol AS arret_oc, a_arrivee.nol AS oc_arrivee, LOWER(TRIM(LEADING '0' FROM h.ligne)) AS ligne"
								<< "	FROM HORAIRE h FORCE INDEX (idx_horaire_arret)"
								<< "	JOIN ARRET a"
								<< "		ON a.ref = h.ref_arret AND a.jour = h.jour"
								<< "	JOIN ARRETCHN achn"
								<< "		ON achn.ref = h.ref_arretchn AND achn.jour = h.jour"
								<< "	JOIN ARRETCHN achn_arrivee"
								<< "		ON achn.jour = achn_arrivee.jour AND achn_arrivee.chainage = achn.chainage"
								<< "	JOIN ARRET a_arrivee"
								<< "		ON a_arrivee.ref = achn_arrivee.arret AND a_arrivee.jour = achn_arrivee.jour"
								<< " 	WHERE"
								<< "			a.mnemol IN (" << operatorCodes.str() << ")"
								<< "		AND " << filters.str()
								<< "		AND h.jour = '" << SAEDateString << "'"
								<< "		AND h.isArretNeutralise = 0"
								<< "		AND h.typeCourse = 'C'"
								<< "		AND h.isVehiculeNeutralise = 0"
								<< "		AND h.isInhibHoraire = 0"
								<< "		AND h.isDeviation = 0"
								<< "		AND h.isInhibMontee = 0"
								<< "		AND achn_arrivee.type = 'A'"
								<< "		AND a.nol != a_arrivee.nol"
								<< " 	ORDER BY"
								<< "		oc_arrivee, ligne, arret_oc, hrd < '" << boost::posix_time::to_simple_string(endOfService) << "', hrd"
								<< " ) AS result"
								<< " WHERE"
								<< "	IF(@num >= " << SAELimit << ", IF(@itineraire_lign_com = ligne, IF(@itineraire_dest = oc_arrivee, IF(@itineraire_arret = arret_oc, FALSE, TRUE), TRUE), TRUE), TRUE)"
						;

						shared_ptr<MySQLResult> result = connector->execQuery(request.str());

						typedef map<string, boost::shared_ptr<StopPoint> > OCStops;
						OCStops ocStops;

						while(result->next())
						{
							ocStops[result->getInfo("oc_arrivee")];
							ocStops[result->getInfo("arret_oc")];
						}

						result->reset();
						BOOST_FOREACH(const Registry<StopPoint>::value_type& curStop, Env::GetOfficialEnv().getRegistry<StopPoint>())
						{
							BOOST_FOREACH(const impex::Importable::DataSourceLinks::value_type& dataSourceLink, curStop.second->getDataSourceLinks())
							{
									if(SAEDataSource && (SAEDataSource.get() != dataSourceLink.first))
										continue;

								if(ocStops.find(dataSourceLink.second) != ocStops.end())
									ocStops[dataSourceLink.second] = curStop.second;
							}
						}

						while(result->next())
						{
							RealTimeService realTimeService;

							realTimeService.stop = ocStops.find(result->getInfo("arret_oc"))->second;
							if(ocStops.find(result->getInfo("oc_arrivee"))->second)
								realTimeService.destination = ocStops.find(result->getInfo("oc_arrivee"))->second->getConnectionPlace();

							const StopArea* area = realTimeService.stop->getConnectionPlace();
							if(_SAELine.find(result->getInfo("ligne")) == _SAELine.end() || !realTimeService.destination)
								continue;

							// Ignore when destination and stop are on the same stop area
							if(area->getKey() == realTimeService.destination->getKey())
								continue;

							BOOST_FOREACH(SAELine::mapped_type::value_type idLine, _SAELine.find(result->getInfo("ligne"))->second)
							{
								try
								{
									boost::shared_ptr<const CommercialLine> curLine = Env::GetOfficialEnv().getRegistry<CommercialLine>().get(idLine);
									BOOST_FOREACH(Path* path, curLine->getPaths())
									{
										const StopPoint* destination = dynamic_cast<const StopPoint*>(path->getLastEdge()->getFromVertex());
										if(destination)
										{
											BOOST_FOREACH(const impex::Importable::DataSourceLinks::value_type& dataSourceLink, destination->getDataSourceLinks())
											{
													if(SAEDataSource && (SAEDataSource.get() != dataSourceLink.first))
														continue;

												if(dataSourceLink.second == result->getInfo("oc_arrivee"))
												{
													realTimeService.commercialLine = curLine;
													break;
												}
											}

											if(realTimeService.commercialLine)
												break;
										}
									}

									if(realTimeService.commercialLine)
										break;
								}
								catch(util::ObjectNotFoundInRegistryException<CommercialLine>)
								{
									continue;
								}
							}

							if(!realTimeService.commercialLine)
								continue;

							linesWithRealTime.insert(realTimeService.commercialLine->getKey());
							if(_lineDestinationFilter.find(realTimeService.stop.get()) != _lineDestinationFilter.end())
							{
								bool displayDeparture(false);
								typedef pair<LineDestinationFilter::const_iterator, LineDestinationFilter::const_iterator> RangeIt;
								RangeIt range = _lineDestinationFilter.equal_range(realTimeService.stop.get());

								for(LineDestinationFilter::const_iterator it = range.first ; it != range.second ; it++)
								{
									bool goodLine(!it->second.first || (it->second.first == realTimeService.commercialLine.get()));
									bool goodDest(!it->second.second || (it->second.second == realTimeService.destination));
									displayDeparture = goodLine & goodDest;

									if(displayDeparture)
										break;
								}

								if(!displayDeparture)
									continue;
							}

							if(result->getInfo("fiable") == "F")
								realTimeService.realTime = true;

							ptime SAEResultPtime(time_from_string(SAEDateString + " " + result->getInfo("horaire")));
							time_duration tod = SAEResultPtime.time_of_day();

							if(tod < endOfService)
								SAEResultPtime += boost::gregorian::days(1);

							realTimeService.datetime = SAEResultPtime;

							LineDestinationKey ldKey = make_pair(realTimeService.commercialLine.get(), realTimeService.destination);
							bool serviceInserted(false);
							// StopArea already in the map
							if(serviceMap.find(area) != serviceMap.end())
							{
								BOOST_FOREACH(DeparturesByDestination::value_type& departureByDestination, serviceMap.find(area)->second)
								{
									const DeparturesByDestination::key_type& key = departureByDestination.first;
									if(key == ldKey)
									{
										serviceMap[area][key].first.insert(realTimeService);
										serviceInserted = true;
										break;
									}
								}
							}

							// Destination or StopArea doesn't exist yet
							if(!serviceInserted)
							{
								OrderedServices tmpSetPair;
								tmpSetPair.first.insert(realTimeService);
								serviceMap[area][ldKey] = tmpSetPair;
							}
						}
					}
					catch(Exception &e)
					{
						// If an exception is thrown, disable realTime
						noRealTime = true;
						util::Log::GetInstance().warn(string("Exception while querying next departures : ") + e.what());
					}
				}
				}
			#endif

				BOOST_FOREACH(const ArrivalDepartureTableGenerator::PhysicalStops::value_type& it, _screen->getPhysicalStops())
				{
					const StopPoint* stop = it.second;
					const StopArea* area = stop->getConnectionPlace();

					ptime startDateTime,endDateTime;
					if(_wayIsBackward)
					{
						endDateTime = (_date ? *_date : now);
						startDateTime = endDateTime.time_of_day().hours() < 3 ?
							endDateTime - endDateTime.time_of_day() - hours(_maxDaysNextDepartures * 24) + hours(3) :
							endDateTime - endDateTime.time_of_day() - hours((_maxDaysNextDepartures * 24) - 1) + hours(3);
					}
					else
					{
						startDateTime = (_date ? *_date : now);
						endDateTime = startDateTime.time_of_day().hours() < 3 ?
							startDateTime - startDateTime.time_of_day() + hours((_maxDaysNextDepartures - 1) * 24) + hours(3) :
							startDateTime - startDateTime.time_of_day() + hours(_maxDaysNextDepartures * 24) + hours(3);
					}

					BOOST_FOREACH(const Vertex::Edges::value_type& edge, stop->getDepartureEdges())
					{
						// Jump over junctions
						if(!dynamic_cast<const LinePhysicalStop*>(edge.second))
						{
							continue;
						}

						const LinePhysicalStop* ls = static_cast<const LinePhysicalStop*>(edge.second);

						const UseRule& useRule(ls->getParentPath()->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET));
						if(dynamic_cast<const PTUseRule*>(&useRule) && static_cast<const PTUseRule&>(useRule).getForbiddenInDepartureBoards())
						{
							continue;
						}

						BOOST_FOREACH(const Path::ServiceCollections::value_type& itCollection, ls->getParentPath()->getServiceCollections())
						{
							ptime departureDateTime = startDateTime;
							optional<Edge::DepartureServiceIndex::Value> index;

							while(true)
							{
								ServicePointer servicePointer(
									ls->getNextService(
										*itCollection,
										ap,
										departureDateTime,
										endDateTime,
										false,
										index,
										false,
										false,
										false,
										PTModule::isTheoreticalAllowed(),
										PTModule::isRealTimeAllowed()
									)
								);

								if (!servicePointer.getService())
									break;

								++*index;

								departureDateTime = servicePointer.getDepartureDateTime() + servicePointer.getServiceRange();
								if(stop->getKey() != servicePointer.getRealTimeDepartureVertex()->getKey())
								{
									continue;
								}

								const JourneyPattern* journeyPattern = static_cast<const JourneyPattern*>(servicePointer.getService()->getPath());
								const CommercialLine* commercialLine(journeyPattern->getCommercialLine());
								const StopArea* destination = journeyPattern->getDestination()->getConnectionPlace();

								if(_lineDestinationFilter.find(stop) != _lineDestinationFilter.end())
								{
									bool displayDeparture(false);
									typedef pair<LineDestinationFilter::const_iterator, LineDestinationFilter::const_iterator> RangeIt;
									RangeIt range = _lineDestinationFilter.equal_range(stop);

									for(LineDestinationFilter::const_iterator it = range.first ; it != range.second ; it++)
									{
										bool goodLine(!it->second.first || (it->second.first == commercialLine));
										bool goodDest(!it->second.second || (it->second.second == destination));
										displayDeparture = goodLine && goodDest;

										if(displayDeparture)
											break;
									}

									if(!displayDeparture)
										continue;
								}

								//If a lineid arg was passed : only one line will be displayed
								if(_lineToDisplay)
								{
									if(commercialLine->getKey()!=(*_lineToDisplay))
										continue;
								}

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

								LineDestinationKey ldKey = make_pair(commercialLine, destination);
								bool serviceInserted(false);
								// StopArea already in the map
								if(serviceMap.find(area) != serviceMap.end())
								{
									BOOST_FOREACH(DeparturesByDestination::value_type& departureByDestination, serviceMap.find(area)->second)
									{
										const DeparturesByDestination::key_type& key = departureByDestination.first;
										if(key == ldKey)
										{
											serviceMap[area][ldKey].second.insert(servicePointer);
											serviceInserted = true;
											break;
										}
									}
								}

								// Destination or StopArea doesn't exist yet
								if(!serviceInserted)
								{
									OrderedServices tmpSetPair;
									tmpSetPair.second.insert(servicePointer);
									serviceMap[area][ldKey] = tmpSetPair;
								}
							}
						}
					}
				}

				BOOST_FOREACH(DestinationsServicesByArea::value_type& areaMap, serviceMap)
				{
					boost::shared_ptr<ParametersMap> stopAreaPM(new ParametersMap());
					areaMap.first->toParametersMap(*stopAreaPM, true);

					BOOST_FOREACH(DeparturesByDestination::value_type& destinationMap, areaMap.second)
					{
						const CommercialLine* commercialLine(destinationMap.first.first);
						const StopArea* destination(destinationMap.first.second);

						boost::shared_ptr<ParametersMap> schedulePM(new ParametersMap());

						boost::shared_ptr<ParametersMap> linePM(new ParametersMap());
						commercialLine->toParametersMap(*linePM, true);

						boost::shared_ptr<ParametersMap> destinationPM(new ParametersMap());
						destination->toParametersMap(*destinationPM, true);

						boost::shared_ptr<ParametersMap> journeysPM(new ParametersMap());

						const StopPoint* stop(NULL);

						if(noRealTime || linesWithRealTime.find(commercialLine->getKey()) == linesWithRealTime.end())
						{
							for(OrderedServices::second_type::const_iterator it = destinationMap.second.second.begin() ; it != destinationMap.second.second.end() ; it++)
							{
								if(journeysPM->getSubMaps(DATA_JOURNEY).size() >= _screen->get<DisplayTypePtr>()->get<RowsNumber>())
								{
									break;
								}

								const ServicePointer& servicePointer = *it;
								stop = static_cast<const StopPoint*>(servicePointer.getDepartureEdge()->getFromVertex());

								const SchedulesBasedService* service = static_cast<const SchedulesBasedService*>(servicePointer.getService());
								const JourneyPattern* journeyPattern = static_cast<const JourneyPattern*>(service->getPath());

								const ContinuousService* continuousService = dynamic_cast<const ContinuousService*>(servicePointer.getService());

								if(_splitContinuousServices && continuousService)
								{
									boost::posix_time::ptime firstDeparture = servicePointer.getDepartureDateTime() + continuousService->getMaxWaitingTime();
									boost::posix_time::ptime lastDeparture = firstDeparture + servicePointer.getServiceRange();
									RollingStock* rs = journeyPattern->getRollingStock();

									it++;
									if(it != destinationMap.second.second.end())
									{
										lastDeparture = (lastDeparture > it->getDepartureDateTime() ? it->getDepartureDateTime() : lastDeparture);
									}
									it--;

									for(boost::posix_time::ptime departureTime = firstDeparture ; departureTime < lastDeparture ; departureTime += continuousService->getMaxWaitingTime())
									{
										if(journeysPM->getSubMaps(DATA_JOURNEY).size() >= _screen->get<DisplayTypePtr>()->get<RowsNumber>())
										{
											break;
										}

								boost::shared_ptr<ParametersMap> journeyPM(new ParametersMap());
										journeyPM->insert(DATA_DATE_TIME, departureTime);
										journeyPM->insert(DATA_WAITING_TIME, to_simple_string(departureTime - now));
										journeyPM->insert(DATA_IS_REAL_TIME, false);

										if(rs)
										{
											boost::shared_ptr<ParametersMap> rsPM(new ParametersMap);
											rs->toParametersMap(*rsPM, true);
											journeyPM->insert(DATA_ROLLING_STOCK, rsPM);
										}
										journeysPM->insert(DATA_JOURNEY, journeyPM);
									}
								}
								else
								{
									boost::shared_ptr<ParametersMap> journeyPM(new ParametersMap());
								journeyPM->insert(DATA_DATE_TIME, servicePointer.getDepartureDateTime());
								journeyPM->insert(DATA_WAITING_TIME, to_simple_string(servicePointer.getDepartureDateTime() - now));
								journeyPM->insert(DATA_IS_REAL_TIME, noRealTime || !_useSAEDirectConnection);

								RollingStock* rs = journeyPattern->getRollingStock();
								if(rs)
								{
									boost::shared_ptr<ParametersMap> rsPM(new ParametersMap);
									rs->toParametersMap(*rsPM, true);
									journeyPM->insert(DATA_ROLLING_STOCK, rsPM);
								}

								journeysPM->insert(DATA_JOURNEY, journeyPM);
							}
						}
						}
						else
						{
							BOOST_FOREACH(const OrderedServices::first_type::value_type& realTimeService, destinationMap.second.first)
							{
								if(journeysPM->getSubMaps(DATA_JOURNEY).size() >= _screen->get<DisplayTypePtr>()->get<RowsNumber>())
								{
									break;
								}

								boost::shared_ptr<ParametersMap> journeyPM(new ParametersMap());
								journeyPM->insert(DATA_DATE_TIME, realTimeService.datetime);
								journeyPM->insert(DATA_WAITING_TIME, to_simple_string(realTimeService.datetime - now));
								journeyPM->insert(DATA_IS_REAL_TIME, realTimeService.realTime);
								journeysPM->insert(DATA_JOURNEY, journeyPM);

								stop = realTimeService.stop.get();
								}
							}

						// No journey or no real time journeys for a line handled by SAE
						if(journeysPM->getSubMaps(DATA_JOURNEY).size() == 0)
							continue;

						if(stop)
						{
							boost::shared_ptr<ParametersMap> stopPointPM(new ParametersMap());
							stop->toParametersMap(*stopPointPM, false);
							schedulePM->insert(DATA_STOP_POINT, stopPointPM);
						}

						schedulePM->insert(DATA_DESTINATION, destinationPM);
						schedulePM->insert(DATA_JOURNEYS, journeysPM);
						schedulePM->insert(DATA_COMMERCIAL_LINE, linePM);

						stopAreaPM->insert(DATA_SCHEDULE, schedulePM);
					}

					result.insert(DATA_STOP_AREA, stopAreaPM);
				}

				if(_mainPage.get())
				{
					result.merge(getTemplateParameters());
					_mainPage->display(stream, request, result);
				}
				else
				{
					outputParametersMap(
						result,
						stream,
						DATA_TIMETABLE,
						""
					);
				}

				return result;
			}
			else
			{
				ParametersMap result;
				bool isOutputXML = _outputFormat == MimeTypes::XML;
				AccessParameters ap;
				typedef map<int, pair<vector<RealTimeService>, vector<ServicePointer> > > OrderedDeparturesMap;
				OrderedDeparturesMap servicePointerAll;

				if(isOutputXML)
				{
					// XML header
					stream <<
						"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
						"<timeTable xsi:noNamespaceSchemaLocation=\"http://synthese.rcsmobility.com/include/54_departures_table/DisplayScreenContentFunction.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" type=\"departure\">"
						;
				}

				// Calculate start time and end time of the searched interval
				ptime startDateTime, endDateTime;
				if(_wayIsBackward)
				{
					endDateTime = (_date ? *_date : now);
					if(endDateTime.time_of_day() > endOfService)
					{
						startDateTime = ptime(endDateTime.date(), endOfService);
					}
					else
					{
						startDateTime = ptime(endDateTime.date() - boost::gregorian::days(1), endOfService);
					}
				}
				else
				{
					startDateTime = (_date ? *_date : now);
					if(startDateTime.time_of_day() > endOfService)
					{
						if(_screen.get() && _screen->get<MaxDelay>() != 0)
						{
							endDateTime = ptime(startDateTime);
							endDateTime += minutes(_screen->get<MaxDelay>());
							time_period period(startDateTime, endDateTime);
							// Limit the endDateTime to the date of the next day, at time 'endOfService'.
							if (period.contains(ptime(startDateTime.date() + boost::gregorian::days(1), endOfService)))
							{
								endDateTime = ptime(startDateTime.date() + boost::gregorian::days(1), endOfService);
							}
						}
						else
						{
							endDateTime = ptime(startDateTime.date() + boost::gregorian::days(1), endOfService);
						}
					}
					else
					{
						endDateTime = ptime(startDateTime.date(), endOfService);
					}
				}

			#ifdef MYSQL_CONNECTOR_AVAILABLE
				// RealTime part
				if(_useSAEDirectConnection)
				{
					stringstream operatorCodes, filters, request;

					BOOST_FOREACH(const ArrivalDepartureTableGenerator::PhysicalStops::value_type& it, _screen->getPhysicalStops())
					{
						BOOST_FOREACH(const impex::Importable::DataSourceLinks::value_type& dataSourceLink, it.second->getDataSourceLinks())
						{
							if(SAEDataSource && (SAEDataSource.get() != dataSourceLink.first))
								continue;

							if(!dataSourceLink.second.empty())
							{
								operatorCodes << (operatorCodes.str().empty() ? "" : ","); 
								operatorCodes << "'" << setfill('0') << setw(5) << dataSourceLink.second;
								operatorCodes << "'";
							}
						}
					}

					if(!operatorCodes.str().empty())
					{
						noRealTime = false;
					shared_ptr<MySQLconnector> connector(new MySQLconnector());

					string sortingOrder(_wayIsBackward ? "DESC" : "ASC");

					if(_wayIsBackward)
					{
						filters << "(h.hrd < '" << SAEHourString << "' ";
						filters << (SAEDate.time_of_day() < endOfService ? "OR" : "AND") <<  " h.hrd > '" << boost::posix_time::to_simple_string(endOfService) << "')";
					}
					else
					{
						filters << "(h.hrd > '" << SAEHourString << "' ";
						filters << (SAEDate.time_of_day() < endOfService ? "AND" : "OR") <<  " h.hrd <= '" << boost::posix_time::to_simple_string(endOfService) << "')";
					}

					if(_lineToDisplay)
					{
						shared_ptr<const CommercialLine> commercialLine(_env->getRegistry<CommercialLine>().get(*_lineToDisplay));
						string shortNameToDisplay = boost::algorithm::to_lower_copy(
							trim_left_copy_if(commercialLine->getShortName(), is_any_of("0"))
						);
						filters << " AND LOWER(TRIM(LEADING '0' FROM h.ligne)) = '" << shortNameToDisplay << "'";
					}

					request << " SELECT"
							<< "	hrd AS horaire, fiable, LOWER(TRIM(LEADING '0' FROM ligne)) AS ligne,"
							<< "	a.nol AS arret_oc, a_arrivee.nol AS oc_arrivee"
							<< " FROM HORAIRE h FORCE INDEX(idx_horaire_arret)"
							<< " JOIN ARRET a"
							<< "	 ON a.ref = h.ref_arret AND a.jour = h.jour"
							<< " JOIN ARRETCHN achn"
							<< "	 ON achn.ref = h.ref_arretchn AND achn.jour = h.jour"
							<< " JOIN ARRETCHN achn_arrivee"
							<< "	 ON achn.jour = achn_arrivee.jour AND achn_arrivee.chainage = achn.chainage"
							<< " JOIN ARRET a_arrivee"
								<< "	 ON a_arrivee.ref = achn_arrivee.arret AND a_arrivee.jour = achn_arrivee.jour AND a.ref != a_arrivee.ref"
							<< " WHERE"
							<< "		a.mnemol IN (" << operatorCodes.str() << ")"
							<< "	AND " << filters.str()
							<< "	AND h.jour = '" << SAEDateString << "'"
							<< "	AND h.typeCourse = 'C'"
							<< "	AND h.isVehiculeNeutralise = 0"
							<< "	AND h.isInhibHoraire = 0"
							<< "	AND h.isDeviation = 0"
							<< "	AND h.isInhibMontee = 0"
							<< "	AND h.isArretNeutralise = 0"
							<< "	AND achn_arrivee.type = 'A'"
							<< " ORDER BY"
							<< "	hrd <= '" << boost::posix_time::to_simple_string(endOfService) << "' " << sortingOrder << ", horaire " << sortingOrder
							<< " LIMIT " << SAELimit
					;

					try
					{
						shared_ptr<MySQLResult> result = connector->execQuery(request.str());

						typedef map<string, boost::shared_ptr<StopPoint> > OCStops;
						OCStops ocStops;

						while(result->next())
						{
							ocStops[result->getInfo("oc_arrivee")];
							ocStops[result->getInfo("arret_oc")];
						}

						result->reset();
						BOOST_FOREACH(const Registry<StopPoint>::value_type& curStop, Env::GetOfficialEnv().getRegistry<StopPoint>())
						{
							BOOST_FOREACH(const impex::Importable::DataSourceLinks::value_type& dataSourceLink, curStop.second->getDataSourceLinks())
							{
									if(SAEDataSource && (SAEDataSource.get() != dataSourceLink.first))
										continue;

								if(ocStops.find(dataSourceLink.second) != ocStops.end())
									ocStops[dataSourceLink.second] = curStop.second;
							}
						}

						while(result->next())
						{
							RealTimeService realTimeService;

							realTimeService.stop = ocStops.find(result->getInfo("arret_oc"))->second;
							if(ocStops.find(result->getInfo("oc_arrivee"))->second)
								realTimeService.destination = ocStops.find(result->getInfo("oc_arrivee"))->second->getConnectionPlace();

							if(_SAELine.find(result->getInfo("ligne")) == _SAELine.end() || !realTimeService.destination)
								continue;

							// Ignore when destination and stop are on the same stop area
							if(realTimeService.stop->getConnectionPlace()->getKey() == realTimeService.destination->getKey())
								continue;

							BOOST_FOREACH(SAELine::mapped_type::value_type idLine, _SAELine.find(result->getInfo("ligne"))->second)
							{
								try
								{
									boost::shared_ptr<const CommercialLine> curLine = Env::GetOfficialEnv().getRegistry<CommercialLine>().get(idLine);
									BOOST_FOREACH(Path* path, curLine->getPaths())
									{
										const StopPoint* destination = dynamic_cast<const StopPoint*>(path->getLastEdge()->getFromVertex());
										if(destination)
										{
											BOOST_FOREACH(const impex::Importable::DataSourceLinks::value_type& dataSourceLink, destination->getDataSourceLinks())
											{
													if(SAEDataSource && (SAEDataSource.get() != dataSourceLink.first))
														continue;

												if(dataSourceLink.second == result->getInfo("oc_arrivee"))
												{
													realTimeService.commercialLine = curLine;
													break;
												}
											}

											if(realTimeService.commercialLine)
												break;
										}
									}

									if(realTimeService.commercialLine)
										break;
								}
								catch(util::ObjectNotFoundInRegistryException<CommercialLine>)
								{
									continue;
								}
							}

							if(!realTimeService.commercialLine)
								continue;

							if(_lineDestinationFilter.find(realTimeService.stop.get()) != _lineDestinationFilter.end())
							{
								bool displayDeparture(false);
								typedef pair<LineDestinationFilter::const_iterator, LineDestinationFilter::const_iterator> RangeIt;
								RangeIt range = _lineDestinationFilter.equal_range(realTimeService.stop.get());

								for(LineDestinationFilter::const_iterator it = range.first ; it != range.second ; it++)
								{
									bool goodLine(!it->second.first || (it->second.first == realTimeService.commercialLine.get()));
									bool goodDest(!it->second.second || (it->second.second == realTimeService.destination));
									displayDeparture = goodLine && goodDest;

									if(displayDeparture)
										break;
								}

								if(!displayDeparture)
									continue;
							}

							if(result->getInfo("fiable") == "F")
								realTimeService.realTime = true;

							ptime SAEResultPtime(time_from_string(SAEDateString + " " + result->getInfo("horaire")));
							time_duration tod = SAEResultPtime.time_of_day();
							int mapKeyMinutes = tod.seconds() + tod.minutes() * 60 + tod.hours() * 3600;

							if(tod < endOfService)
							{
								SAEResultPtime += boost::gregorian::days(1);
								mapKeyMinutes += 86400;
							}

							realTimeService.datetime = SAEResultPtime;

							OrderedDeparturesMap::iterator it(servicePointerAll.find(mapKeyMinutes));
							if(it == servicePointerAll.end())
							{
								vector<RealTimeService> tmpVect;
								tmpVect.push_back(realTimeService);
								servicePointerAll[mapKeyMinutes].first = tmpVect;
							}
							else
							{
								servicePointerAll[mapKeyMinutes].first.push_back(realTimeService);
							}
						}
					}
					catch(Exception &e)
					{
						// If an exception is thrown, disable realTime
						noRealTime = true;
						util::Log::GetInstance().warn(string("Exception while querying next departures : ") + e.what());
					}
				}
				}
			#endif

				BOOST_FOREACH(const ArrivalDepartureTableGenerator::PhysicalStops::value_type& it, _screen->getPhysicalStops())
				{
					const StopPoint * stop = it.second;

					// Theoretical time part
					BOOST_FOREACH(const Vertex::Edges::value_type& edge, stop->getDepartureEdges())
					{
						// Jump over junctions
						if(!dynamic_cast<const LinePhysicalStop*>(edge.second))
						{
							continue;
						}

						const LinePhysicalStop* ls = static_cast<const LinePhysicalStop*>(edge.second);

						const UseRule& useRule(ls->getParentPath()->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET));
						if(dynamic_cast<const PTUseRule*>(&useRule) && static_cast<const PTUseRule&>(useRule).getForbiddenInDepartureBoards())
						{
							continue;
						}

						boost::shared_lock<util::shared_recursive_mutex> sharedServicesLock(
							*(ls->getParentPath())->sharedServicesMutex
						);
						BOOST_FOREACH(const Path::ServiceCollections::value_type& itCollection, ls->getParentPath()->getServiceCollections())
						{
							ptime departureDateTime = startDateTime;
							// Loop on services
							optional<Edge::DepartureServiceIndex::Value> index;
							while(true)
							{
								ServicePointer servicePointer(
									ls->getNextService(
										*itCollection,
										ap,
										departureDateTime,
										endDateTime,
										false,
										index,
										false,
										false,
										false,
										PTModule::isTheoreticalAllowed(),
										PTModule::isRealTimeAllowed()
								)	);

								if (!servicePointer.getService())
									break;
								++*index;

								departureDateTime = servicePointer.getDepartureDateTime() + servicePointer.getServiceRange();
								if(stop->getKey() != servicePointer.getRealTimeDepartureVertex()->getKey())
									continue;

								const JourneyPattern* journeyPattern = static_cast<const JourneyPattern*>(servicePointer.getService()->getPath());
								const CommercialLine * commercialLine(journeyPattern->getCommercialLine());
								const StopArea* destination = journeyPattern->getDestination()->getConnectionPlace();

								if(_lineDestinationFilter.find(stop) != _lineDestinationFilter.end())
								{
									bool displayDeparture(false);
									typedef pair<LineDestinationFilter::const_iterator, LineDestinationFilter::const_iterator> RangeIt;
									RangeIt range = _lineDestinationFilter.equal_range(stop);

									for(LineDestinationFilter::const_iterator it = range.first ; it != range.second ; it++)
									{
										bool goodLine(!it->second.first || (it->second.first == commercialLine));
										bool goodDest(!it->second.second || (it->second.second == destination));
										displayDeparture = goodLine && goodDest;

										if(displayDeparture)
											break;
									}

									if(!displayDeparture)
										continue;
								}

								//If a lineid arg was passed : only one line will be displayed
								if(_lineToDisplay)
								{
									if(commercialLine->getKey()!=(*_lineToDisplay))
										continue;
								}

								// Filter by Rolling stock id
								if(_rollingStockFilter.get())
								{
									const JourneyPattern* journeyPattern = static_cast<const JourneyPattern*>(servicePointer.getService()->getPath());
									const CommercialLine * commercialLine(journeyPattern->getCommercialLine());
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

								time_duration tod = servicePointer.getDepartureDateTime().time_of_day();
								int mapKeyMinutes = tod.seconds() + tod.minutes() * 60 + tod.hours() * 3600;
								if(servicePointer.getDepartureDateTime().date() > startDateTime.date())
								{
									mapKeyMinutes += 86400;
								}

								OrderedDeparturesMap::iterator it(servicePointerAll.find(mapKeyMinutes));
								//Check if a service is already inserted for this date
								if(it == servicePointerAll.end())
								{
									vector<ServicePointer> tmpVect;
									tmpVect.push_back(servicePointer);
									servicePointerAll[mapKeyMinutes].second = tmpVect;
								}
								else
								{
									servicePointerAll[mapKeyMinutes].second.push_back(servicePointer);
								}
							}
						}
					}
				}

				OrderedDeparturesMap::iterator it = servicePointerAll.begin();
				OrderedDeparturesMap::iterator itStartTheoretical;
				int maxRow(_screen->get<DisplayTypePtr>()->get<RowsNumber>());
				int displayedResults(0);
				bool startDisplayTheoretical(false);

				if(_wayIsBackward)
				{
					// Realtime is already limited and ordered at maxRow results, theoretical is requested on the full day
					int count(0);
					for(itStartTheoretical = servicePointerAll.end() ; itStartTheoretical != servicePointerAll.begin() ; itStartTheoretical--)
					{
						// Only consider entry of servicePointerAll which have theoretical services
						if(itStartTheoretical->second.second.size() && ++count >= maxRow)
							break;
					}
				}
				else
					itStartTheoretical = servicePointerAll.begin();

				typedef map<graph::ServicePointer, pair<int, int>, Spointer_comparator> ContinuousServicesToMerge;
				ContinuousServicesToMerge continuousServicesToMerge;

				for(; it != servicePointerAll.end() ; it++)
				{
					if(itStartTheoretical == it)
						startDisplayTheoretical = true;

					bool serviceDisplayed(false);
					do
					{
						serviceDisplayed = false;
						for(ContinuousServicesToMerge::iterator itCs = continuousServicesToMerge.begin() ; itCs != continuousServicesToMerge.end() ; itCs++)
						{
							ContinuousServicesToMerge::value_type continuousToDisplay(*itCs);
							if(continuousToDisplay.second.first < it->first && continuousToDisplay.second.first <= continuousToDisplay.second.second)
							{
								ServicePointer sp(continuousToDisplay.first);

								const JourneyPattern* journeyPattern = static_cast<const JourneyPattern*>(sp.getService()->getPath());
								const CommercialLine * commercialLine(journeyPattern->getCommercialLine());
								const ContinuousService* continuousService = static_cast<const ContinuousService*>(sp.getService());

								string curShortName = boost::algorithm::to_lower_copy(
									trim_left_copy_if(commercialLine->getShortName(), is_any_of("0"))
								);

								// If realTime request didn't output any results or line isn't in the SAE
								if(noRealTime || _SAELine.find(curShortName) == _SAELine.end())
								{
									if(displayedResults >= maxRow)
										break;
									else if(isOutputXML)
										concatXMLResult(stream,	sp);
									else
										addJourneyToParametersMap(result, sp);

									displayedResults++;
									serviceDisplayed = true;

									sp.setDepartureInformations(
										*(sp.getDepartureEdge()),
										sp.getDepartureDateTime() + continuousService->getMaxWaitingTime(),
										sp.getTheoreticalDepartureDateTime() + continuousService->getMaxWaitingTime(),
										*(sp.getRealTimeDepartureVertex())
									);

									if(continuousToDisplay.second.first + continuousService->getMaxWaitingTime().total_seconds() < continuousToDisplay.second.second)
									{
										continuousServicesToMerge.insert(
											ContinuousServicesToMerge::value_type(
												sp,
												make_pair(
													continuousToDisplay.second.first + continuousService->getMaxWaitingTime().total_seconds(),
													continuousToDisplay.second.second
										)	)	);
									}
								}

								continuousServicesToMerge.erase(itCs);
								break;
							}
							else if(continuousToDisplay.second.first > continuousToDisplay.second.second)
							{
								continuousServicesToMerge.erase(itCs);
								break;
							}
						}
					} while(serviceDisplayed);

					BOOST_FOREACH(RealTimeService& rts, it->second.first)
					{
						if(displayedResults >= maxRow)
							break;
						else if(isOutputXML)
							concatXMLResultRealTime(stream, rts);
						else
						    addJourneyToParametersMapRealTime(result, rts);

						displayedResults++;
					}

					if(startDisplayTheoretical)
					{
						BOOST_FOREACH(ServicePointer sp, it->second.second)
						{
							const JourneyPattern* journeyPattern = static_cast<const JourneyPattern*>(sp.getService()->getPath());
							const CommercialLine * commercialLine(journeyPattern->getCommercialLine());
							const ContinuousService* continuousService = dynamic_cast<const ContinuousService*>(sp.getService());

							if(_splitContinuousServices && continuousService)
							{
								sp.setDepartureInformations(
									*(sp.getDepartureEdge()),
									sp.getDepartureDateTime() + continuousService->getMaxWaitingTime(),
									sp.getTheoreticalDepartureDateTime() + continuousService->getMaxWaitingTime(),
									*(sp.getRealTimeDepartureVertex())
								);
								continuousServicesToMerge.insert(
									ContinuousServicesToMerge::value_type(
										sp,
										make_pair(
											it->first + continuousService->getMaxWaitingTime().total_seconds(),
											it->first + sp.getServiceRange().total_seconds()
								)	)	);
								continue;
							}

							string curShortName = boost::algorithm::to_lower_copy(
								trim_left_copy_if(commercialLine->getShortName(), is_any_of("0"))
							);

							// If realTime request didn't output any results or line isn't in the SAE
							if(noRealTime || _SAELine.find(curShortName) == _SAELine.end())
							{
								if(displayedResults >= maxRow)
									break;
								else if(isOutputXML)
									concatXMLResult(stream,	sp);
								else
									addJourneyToParametersMap(result, sp);

								displayedResults++;
							}
						}
					}

					if(displayedResults >= maxRow)
						break;
				}

				if(isOutputXML)
				{
					// XML footer
					stream << "</timeTable>";
				}
				else if(_mainPage.get())
				{
					if(result.hasSubMaps("journey") && _rowPage.get())
					{
						vector<boost::shared_ptr<ParametersMap> > journeys = result.getSubMaps("journey");
						if(!journeys.empty())
						{
							result.insert(DATA_FIRST_DEPARTURE_TIME, (*journeys.begin())->getValue("date_time"));
							result.insert(DATA_LAST_DEPARTURE_TIME, (*journeys.rbegin())->getValue("date_time"));
						}

						size_t rank(0);
						bool onlyRealTime = true;
						stringstream rowStream;
						BOOST_FOREACH(const boost::shared_ptr<ParametersMap>& journey, journeys)
						{
							if(journey->hasSubMaps("destination") && _destinationPage.get())
							{
								stringstream destinationStream;
								BOOST_FOREACH(const boost::shared_ptr<ParametersMap>& dest, journey->getSubMaps("destination"))
								{
									dest->merge(getTemplateParameters());
									_destinationPage->display(destinationStream, request, *dest);
								}
								journey->insert(DATA_DESTINATIONS, destinationStream.str());
							}
							
							if(journey->hasSubMaps("line"))
							{
								BOOST_FOREACH(const boost::shared_ptr<ParametersMap>& linemerge, journey->getSubMaps("line"))
								{
									journey->merge(*linemerge);
								}
							}

							if(!journey->isDefined(DATA_IS_REAL_TIME) || journey->getValue(DATA_IS_REAL_TIME) == "no")
							{
								onlyRealTime = false;
							}
							journey->merge(getTemplateParameters());
							journey->insert("rank", ++rank);
							_rowPage->display(rowStream, request, *journey);
						}
						result.insert(DATA_ONLY_REAL_TIME, (onlyRealTime ? 1 : 0));
						result.insert(DATA_ROWS, rowStream.str());
					}
					result.merge(getTemplateParameters());
					_mainPage->display(stream, request, result);
				}
				else
				{
					outputParametersMap(
						result,
						stream,
						DATA_JOURNEYS,
						"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/54_departure_boards/DisplayScreenContentFunction.xsd"
					);
				}


				return result;
			}

			return util::ParametersMap();
		}



		bool DisplayScreenContentFunction::isAuthorized(const Session* session
		) const {
			return true;
		}



		std::string DisplayScreenContentFunction::getOutputMimeType() const
		{
			if(_screen->get<DisplayTypePtr>()->get<DisplayInterface>())
			{
				return
					(   _screen.get() &&
						_screen->get<DisplayTypePtr>() &&
						_screen->get<DisplayTypePtr>()->get<DisplayInterface>() &&
						_screen->getGenerationMethod() != DisplayScreen::ROUTE_PLANNING &&
						_screen->get<DisplayTypePtr>()->get<DisplayInterface>()->hasPage<DeparturesTableInterfacePage>()
					) ?
						_screen->get<DisplayTypePtr>()->get<DisplayInterface>()->getPage<DeparturesTableInterfacePage>()->getMimeType() :
					"text/plain"
				;
			}
			if(_screen->get<DisplayTypePtr>()->get<DisplayMainPage>())
			{
				return _screen->get<DisplayTypePtr>()->get<DisplayMainPage>()->getMimeType();
			}
			else if(_mainPage.get())
			{
				return _mainPage->getMimeType();
			}

			return getOutputMimeTypeFromOutputFormat(MimeTypes::XML);
		}



		void DisplayScreenContentFunction::setScreen(
			boost::shared_ptr<const DisplayScreen> value
		){
			_screen = value;
		}



		void DisplayScreenContentFunction::_displayDepartureBoard(
			std::ostream& stream,
			const server::Request& request,
			boost::shared_ptr<const cms::Webpage> page,
			boost::shared_ptr<const cms::Webpage> rowPage,
			boost::shared_ptr<const cms::Webpage> destinationPage,
			boost::shared_ptr<const cms::Webpage> transferPage,
			const boost::posix_time::ptime& date,
			const ArrivalDepartureList& rows,
			const DisplayScreen& screen
		) const {
			ParametersMap pm(getTemplateParameters());
			pm.insert(Request::PARAMETER_OBJECT_ID, screen.getKey());
			pm.insert(DATA_MAC, screen.get<MacAddress>());
			pm.insert(DATA_DATE, to_iso_extended_string(date.date()) + " " + to_simple_string(date.time_of_day()));
			pm.insert(DATA_TITLE, screen.get<Title>());
			pm.insert(DATA_WIRING_CODE, screen.get<WiringCode>());
			pm.insert(DATA_DISPLAY_SERVICE_NUMBER, screen.get<ServiceNumberDisplay>());
			pm.insert(DATA_DISPLAY_TRACK_NUMBER, screen.get<TrackNumberDisplay>());
			if(screen.get<DisplayTypePtr>())
			{
				pm.insert(DATA_INTERMEDIATE_STOPS_NUMBER, screen.get<DisplayTypePtr>()->get<MaxStopsNumber>());
			}
			pm.insert(DATA_DISPLAY_TEAM, screen.get<DisplayTeam>());
			pm.insert(DATA_STOP_NAME, screen.get<BroadCastPoint>() ? screen.get<BroadCastPoint>()->getFullName() : string());
			pm.insert(DATA_DISPLAY_CLOCK, screen.get<DisplayClock>());

			// Stop Point
			if(!screen.get<AllPhysicalDisplayed>() && (screen.getPhysicalStops().size() == 1))
			{
				const ArrivalDepartureTableGenerator::PhysicalStops::const_iterator it = screen.getPhysicalStops().begin();
				const StopPoint * stop = it->second;

				pm.insert(DATA_STOP_ID, stop->getKey());
				pm.insert(DATA_STOP_NAME, stop->getName());
				pm.insert(DATA_OPERATOR_CODE, stop->getCodeBySources());
			}

			// StopArea
			if(screen.get<BroadCastPoint>())
			{
				const StopArea* connPlace(screen.get<BroadCastPoint>().get_ptr());

				pm.insert(DATA_STOP_AREA_ID, connPlace->getKey());
				pm.insert(DATA_STOP_AREA_NAME, connPlace->getName());
				pm.insert(DATA_STOP_AREA_CITY_NAME, connPlace->getCity()->getName());
				pm.insert(DATA_STOP_AREA_CITY_ID, connPlace->getCity()->getKey());
			}

			// Rows
			if(rowPage.get())
			{
				stringstream rowStream;
				
				if(!rows.empty())
				{
					displayFullDate(
						DATA_FIRST_DEPARTURE_TIME,
						(*rows.begin()).first.getDepartureDateTime(),
						pm
					);

					displayFullDate(
						DATA_LAST_DEPARTURE_TIME,
						(*rows.rbegin()).first.getDepartureDateTime(),
						pm
					);
				}

				// Link between an adapted time and a service
				std::multimap<ptime,ArrivalDepartureList::const_iterator> times;

				// Adapting the time using SCOM if activated
				#ifdef WITH_SCOM
				if (_scom)
				{
					for (ArrivalDepartureList::const_iterator it = rows.begin(); it != rows.end(); ++it)
					{
						// Fetch the time from SCOM
						const JourneyPattern* journeyPattern = static_cast<const JourneyPattern*>(it->first.getService()->getPath());

						std::string dest =
							journeyPattern->getDirection().empty() && journeyPattern->getDirectionObj() ?
							journeyPattern->getDirectionObj()->get<DisplayedText>() :
							journeyPattern->getDirection();

						ptime adaptedTime = it->first.getDepartureDateTime();

						// Check object before calling them
						adaptedTime = scom::SCOMModule::GetSCOMData()->GetWaitingTime(
							_screen.get()->getCodeBySources(),
							journeyPattern->getCommercialLine()->getShortName(),
							dest,
							adaptedTime,
							date
						);

						// Save the adapted time
						times.insert(make_pair(adaptedTime,it));
					}

				}
				#endif

				// No SCOM, just use the service time
				if (
					!_scom
				#ifndef WITH_SCOM
					|| true
				#endif
				)
				{
					for (ArrivalDepartureList::const_iterator it = rows.begin(); it != rows.end(); ++it)
					{
						times.insert(make_pair(it->first.getDepartureDateTime(),it));
					}
				}

				/// @todo replace by parameters or something else
				int __Pages(0);
				int departuresToHide(0);
				string __SeparateurPage;
				int __MultiplicateurRangee(1);

				// Gestion des pages
				size_t __NombrePages(1);
				if(__Pages != 0)
				{
					int departuresNumber = rows.size() - departuresToHide;
					for (ArrivalDepartureList::const_iterator it = rows.begin(); departuresNumber && (it != rows.end()); ++it, --departuresNumber)
					{
						const ActualDisplayedArrivalsList& displayedList = it->second;
						if (displayedList.size() > __NombrePages + 2)
						{
							__NombrePages = displayedList.size () - 2;
						}
					}
					if (__Pages != UNKNOWN_VALUE && __NombrePages > (size_t)__Pages)
					{
						__NombrePages = __Pages;
					}
				}

				// Boucle sur les pages
				for(size_t __NumeroPage = 1; __NumeroPage <= __NombrePages; __NumeroPage++ )
				{
					// Separateur de page
					if ( __NumeroPage > 1 )
					{
						rowStream << __SeparateurPage;
					}

					// Boucle sur les rangees
					int __Rangee = __MultiplicateurRangee;
					int departuresNumber = rows.size() - departuresToHide;
					for (std::multimap<ptime,ArrivalDepartureList::const_iterator>::const_iterator it = times.begin(); departuresNumber && (it != times.end()); ++it, --departuresNumber)
					{
						const ArrivalDepartureRow& row(*it->second);

						int __NombrePagesRangee = row.second.size () - 2;
						int pageNumber = ( !__NombrePagesRangee || __NumeroPage > __NombrePagesRangee * ( __NombrePages / __NombrePagesRangee ) )
							? __NumeroPage
							: (1 + __NumeroPage % __NombrePagesRangee);		// 1 : Numero de page

						// Lancement de l'affichage de la rangee
						_displayDepartureBoardRow(
							rowStream,
							request,
							rowPage,
							destinationPage,
							transferPage,
							date,
							__Rangee,
							pageNumber,
							row,
							screen
						);

						// Incrementation du numero de rangee
						__Rangee += __MultiplicateurRangee;
					}
				}

				pm.insert(DATA_ROWS, rowStream.str());
			}

			// Subscreens
			size_t subScreenRank(0);
			BOOST_FOREACH(const DisplayScreen::ChildrenType::value_type& it, screen.getChildren())
			{
				if(it.second->getSubScreenType() == DisplayScreen::SUB_CONTENT)
				{
					pm.insert(DATA_SUBSCREEN_ + lexical_cast<string>(subScreenRank++), it.second->getKey());
				}
			}

			// Launch of the display
			page->display(stream, request, pm);
		}



		void DisplayScreenContentFunction::_displayDepartureBoardRow(
			std::ostream& stream,
			const server::Request& request,
			boost::shared_ptr<const cms::Webpage> page,
			boost::shared_ptr<const cms::Webpage> destinationPage,
			boost::shared_ptr<const cms::Webpage> transferPage,
			const boost::posix_time::ptime& requestTime,
			size_t rowRank,
			size_t pageNumber,
			const ArrivalDepartureRow& row,
			const DisplayScreen& screen
		) const {
			ParametersMap pm(getTemplateParameters());
			pm.insert(Request::PARAMETER_OBJECT_ID, screen.getKey());
			pm.insert(DATA_ROW_RANK, rowRank);
			pm.insert(DATA_PAGE_NUMBER, pageNumber);
			pm.insert(DATA_DISPLAY_TRACK_NUMBER, screen.get<TrackNumberDisplay>());
			pm.insert(DATA_DISPLAY_SERVICE_NUMBER, screen.get<ServiceNumberDisplay>());

			if(screen.get<DisplayTypePtr>())
			{
				pm.insert(DATA_INTERMEDIATE_STOPS_NUMBER, screen.get<DisplayTypePtr>()->get<MaxStopsNumber>());
			}
			pm.insert(DATA_DISPLAY_TEAM, screen.get<DisplayTeam>());
			if(row.first.getService())
			{
				static_cast<const StopPoint*>(row.first.getDepartureEdge()->getFromVertex())->getConnectionPlace()->toParametersMap(pm, true);

				// Waiting time
				time_duration waitingTime(row.first.getDepartureDateTime() - requestTime);
				pm.insert(DATA_WAITING_TIME, to_simple_string(waitingTime));

				time_duration blinkingDelay(minutes(screen.get<BlinkingDelay>()));
				if(	blinkingDelay.total_seconds() > 0 &&
					waitingTime <= blinkingDelay
				){
					pm.insert(DATA_BLINKS, true);
				}

				// Handicapped access (true by default)
				const PTUseRule* handicappedUserRule = dynamic_cast<const PTUseRule*>(
					&(row.first.getService())->getUseRule(USER_HANDICAPPED - USER_CLASS_CODE_OFFSET)
				);
				pm.insert(DATA_HANDICAPPED_ACCESS, handicappedUserRule ? handicappedUserRule->getAccessCapacity().get_value_or(9999) != 0 : true);

				// Time
				pm.insert(DATA_TIME, to_iso_extended_string(row.first.getDepartureDateTime().date()) + " " + to_simple_string(row.first.getDepartureDateTime().time_of_day()));
				pm.insert(DATA_PLANNED_TIME, to_iso_extended_string(row.first.getTheoreticalDepartureDateTime().date()) + " " + to_simple_string(row.first.getTheoreticalDepartureDateTime().time_of_day()));

				// Delay
				pm.insert(
					DATA_DELAY,
					(row.first.getDepartureDateTime() - row.first.getTheoreticalDepartureDateTime()).total_seconds() / 60
				);

				// Service
				pm.insert(DATA_SERVICE_ID, row.first.getService()->getKey());
				pm.insert(DATA_SERVICE_NUMBER, row.first.getService()->getServiceNumber());

				// Is canceled
				pm.insert(DATA_IS_CANCELED, row.first.getCanceled());

				// Is realtime
				const SchedulesBasedService* schedulesBasedService(dynamic_cast<const SchedulesBasedService*>(row.first.getService()));
				if (schedulesBasedService)
				{
					pm.insert(DATA_IS_REAL_TIME, schedulesBasedService->hasRealTimeData());
				}
				else
				{
					const ContinuousService* continuousService(dynamic_cast<const ContinuousService*>(row.first.getService()));
					if(continuousService)
					{
						pm.insert(DATA_IS_REAL_TIME, continuousService->hasRealTimeData());
					}
				}

				// Direction
				const JourneyPattern* jp(dynamic_cast<const JourneyPattern*>(row.first.getService()->getPath()));
				string lineDirection(
					jp->getDirection().empty() && jp->getDirectionObj() ?
					jp->getDirectionObj()->get<DisplayedText>() :
					jp->getDirection()
				);
				pm.insert(
					DATA_DIRECTION,
					lineDirection
				);

				pm.insert(
					DATA_TRACK,
					static_cast<const StopPoint*>(row.first.getRealTimeDepartureVertex())->getName()
				);
				pm.insert(
					DATA_TEAM,
					row.first.getService()->getTeam()
				);

				// Line
				dynamic_cast<const CommercialLine&>(*row.first.getService()->getPath()->getPathGroup()).toParametersMap(pm, true);

				// Transport mode
				const JourneyPattern* line(static_cast<const JourneyPattern*>(row.first.getService()->getPath()));
				if(line->getRollingStock())
				{
					pm.insert(DATA_TRANSPORT_MODE, line->getRollingStock()->getKey());
				}

				// Path
				pm.insert(DATA_RANK_IN_PATH, row.first.getDepartureEdge()->getRankInPath());

				// Destinations
				if(destinationPage.get())
				{
					stringstream destinationsStream;
					const City* lastCity = dynamic_cast<const NamedPlace*>(row.second.at(0).place)->getCity();
					size_t totalTransferRank(0);

					for(size_t rank(1); rank < row.second.size(); ++rank)
					{
						const IntermediateStop& stop(row.second.at(rank));

						_displayDepartureBoardDestination(
							destinationsStream,
							request,
							destinationPage,
							transferPage,
							stop.serviceUse,
							stop.place->getCity() == lastCity,
							rank + 1 == row.second.size(),
							rank,
							totalTransferRank,
							stop.transferDestinations,
							screen,
							false,
							false
						);

						lastCity = stop.place->getCity();

						if(stop.continuationService.getService())
						{
							// Introduction row (is associated with the preceding one : rank does not increment)
							const IntermediateStop& substop(stop.destinationsReachedByContinuationService.at(0));
							_displayDepartureBoardDestination(
								destinationsStream,
								request,
								destinationPage,
								transferPage,
								substop.serviceUse,
								true,
								false,
								0,
								totalTransferRank,
								substop.transferDestinations,
								screen,
								true,
								rank + 1 == row.second.size()
							);

							for(size_t subrank(1); subrank < stop.destinationsReachedByContinuationService.size(); ++subrank)
							{
								const IntermediateStop& substop(stop.destinationsReachedByContinuationService.at(subrank));

								_displayDepartureBoardDestination(
									destinationsStream,
									request,
									destinationPage,
									transferPage,
									substop.serviceUse,
									substop.place->getCity() == lastCity,
									subrank + 1 == stop.destinationsReachedByContinuationService.size(),
									subrank,
									totalTransferRank,
									substop.transferDestinations,
									screen,
									true,
									rank + 1 == row.second.size()
								);
								++totalTransferRank;

								lastCity = substop.place->getCity();
							}
						}
						else
						{
							++totalTransferRank;
						}
					}
					pm.insert(DATA_DESTINATIONS, destinationsStream.str());
				}
			}

			// Subscreens
			size_t subScreenRank(0);
			BOOST_FOREACH(const DisplayScreen::ChildrenType::value_type& it, screen.getChildren())
			{
				if(it.second->getSubScreenType() == DisplayScreen::SUB_CONTENT)
				{
					pm.insert(DATA_SUBSCREEN_ + lexical_cast<string>(subScreenRank++), it.second->getKey());
				}
			}

			// Launch of the display
			page->display(stream, request, pm);
		}



		void DisplayScreenContentFunction::_displayDepartureBoardDestination(
			std::ostream& stream,
			const server::Request& request,
			boost::shared_ptr<const cms::Webpage> page,
			boost::shared_ptr<const cms::Webpage> transferPage,
			const graph::ServicePointer& object,
			bool lastDisplayedStopWasInTheSameCity,
			bool isTheEndStation,
			size_t rank,
			size_t globalRank,
			const IntermediateStop::TransferDestinations& transferDestinations,
			const DisplayScreen& screen,
			bool isContinuation,
			bool continuationStartsAtEnd
		) const {
			const StopArea* place(
				dynamic_cast<const StopArea*>(
					(rank ? object.getArrivalEdge() : object.getDepartureEdge())->getHub()
			)	);

			ParametersMap pm(getTemplateParameters());
			place->toParametersMap(pm, true);
			pm.insert(DATA_IS_SAME_CITY, lastDisplayedStopWasInTheSameCity);
			pm.insert(DATA_TIME, to_iso_extended_string((rank ? object.getArrivalDateTime() : object.getDepartureDateTime()).date()) +" "+ to_simple_string((rank ? object.getArrivalDateTime() : object.getDepartureDateTime()).time_of_day()));
			pm.insert(DATA_IS_END_STATION, isTheEndStation);
			pm.insert(DATA_DESTINATION_RANK, rank);
			pm.insert(DATA_DESTINATION_GLOBAL_RANK, globalRank);

			const JourneyPattern* line(dynamic_cast<const JourneyPattern*>(object.getService()->getPath()));
			string lineDirection(
				line->getDirection().empty() && line->getDirectionObj() ?
				line->getDirectionObj()->get<DisplayedText>() :
				line->getDirection()
			);
			pm.insert(
				DATA_DIRECTION,
				lineDirection.empty() ? line->getDestination()->getConnectionPlace()->getFullName() : lineDirection
			);

			// Continuation
			pm.insert(DATA_IS_CONTINUATION, isContinuation);
			if(isContinuation)
			{
				pm.insert(DATA_CONTINUATION_STARTS_AT_END, continuationStartsAtEnd);
			}

			// Transfers
			if(transferPage.get())
			{
				stringstream transferStream;

				// Loop on the transfer pages
				size_t localTransferRank(0);
				BOOST_FOREACH(const IntermediateStop::TransferDestinations::value_type& transferServiceUse, transferDestinations)
				{
					_displayDepartureBoardTrandferDestination(
						transferStream,
						request,
						transferPage,
						transferServiceUse,
						localTransferRank++,
						screen
					);
				}

				pm.insert(DATA_TRANSFERS, transferStream.str());
			}

			// Subscreens
			size_t subScreenRank(0);
			BOOST_FOREACH(const DisplayScreen::ChildrenType::value_type& it, screen.getChildren())
			{
				if(it.second->getSubScreenType() == DisplayScreen::SUB_CONTENT)
				{
					pm.insert(DATA_SUBSCREEN_ + lexical_cast<string>(subScreenRank++), it.second->getKey());
				}
			}

			// Launch of the display
			page->display(stream, request, pm);
		}



		void DisplayScreenContentFunction::_displayDepartureBoardTrandferDestination(
			std::ostream& stream,
			const server::Request& request,
			boost::shared_ptr<const cms::Webpage> page,
			const graph::ServicePointer& object,
			size_t localTransferRank,
			const DisplayScreen& screen
		) const {
			ParametersMap pm(getTemplateParameters());

			const JourneyPattern* line(dynamic_cast<const JourneyPattern*>(object.getService()->getPath()));
			const StopArea* place(dynamic_cast<const StopArea*>(object.getArrivalEdge()->getFromVertex()->getHub()));

			// Transport mode
			if(line->getRollingStock())
			{
				pm.insert(DATA_TRANSPORT_MODE, line->getRollingStock()->getKey());
			}

			line->getCommercialLine()->toParametersMap(pm, true);
			place->toParametersMap(pm, true);

			{ // Departure time
				stringstream s;
				s <<
					setw(2) << setfill('0') <<
					object.getDepartureDateTime().time_of_day().hours() <<
					":" << setw(2) << setfill('0') <<
					object.getDepartureDateTime().time_of_day().minutes()
				;
				pm.insert(DATA_DEPARTURE_TIME, s.str());
			}
			{
				stringstream s;
				s <<
					setw(2) << setfill('0') <<
					object.getArrivalDateTime().time_of_day().hours() <<
					":" << setw(2) << setfill('0') <<
					object.getArrivalDateTime().time_of_day().minutes()
				;
				pm.insert(DATA_ARRIVAL_TIME, s.str());
			}
			pm.insert(DATA_TRANSFER_RANK, localTransferRank);

			// Subscreens
			size_t subScreenRank(0);
			BOOST_FOREACH(const DisplayScreen::ChildrenType::value_type& it, screen.getChildren())
			{
				if(it.second->getSubScreenType() == DisplayScreen::SUB_CONTENT)
				{
					pm.insert(DATA_SUBSCREEN_ + lexical_cast<string>(subScreenRank++), it.second->getKey());
				}
			}

			// Launch of the display
			page->display(stream, request, pm);
		}



		void DisplayScreenContentFunction::_displayRoutePlanningBoard(
			std::ostream& stream,
			const server::Request& request,
			boost::shared_ptr<const cms::Webpage> page,
			boost::shared_ptr<const cms::Webpage> rowPage,
			boost::shared_ptr<const cms::Webpage> destinationPage,
			const boost::posix_time::ptime& date,
			const RoutePlanningList& rows,
			const DisplayScreen& screen
		) const {
			ParametersMap pm(getTemplateParameters());

			pm.insert(Request::PARAMETER_OBJECT_ID, screen.getKey());
			pm.insert(DATA_MAC, screen.get<MacAddress>());
			pm.insert(DATA_DATE, to_iso_extended_string(date.date()) + " " + to_simple_string(date.time_of_day()));
			pm.insert(DATA_TITLE, screen.get<Title>());
			pm.insert(DATA_WIRING_CODE, screen.get<WiringCode>());
			pm.insert(DATA_DISPLAY_SERVICE_NUMBER, screen.get<ServiceNumberDisplay>());
			pm.insert(DATA_DISPLAY_TRACK_NUMBER, screen.get<TrackNumberDisplay>());
			if(screen.get<DisplayTypePtr>())
			{
				pm.insert(DATA_INTERMEDIATE_STOPS_NUMBER, screen.get<DisplayTypePtr>()->get<MaxStopsNumber>());
			}
			pm.insert(DATA_DISPLAY_TEAM, screen.get<DisplayTeam>());
			pm.insert(DATA_STOP_NAME, screen.get<BroadCastPoint>() ? screen.get<BroadCastPoint>()->getFullName() : string());
			pm.insert(DATA_DISPLAY_CLOCK, screen.get<DisplayClock>());
			pm.insert(DATA_WITH_TRANSFER, screen.get<RoutePlanningWithTransfer>());
			screen.get<BroadCastPoint>()->toParametersMap(pm, true);

			// Rows
			if(rowPage.get())
			{
				stringstream rowsStream;
				
				// Sort of the rows
				typedef map<string,RoutePlanningList::const_iterator> SortedRows;
				SortedRows sortedRows;
				for(RoutePlanningList::const_iterator it = rows.begin(); it != rows.end(); ++it)
				{
					stringstream s;
					if(destinationPage.get())
					{
						_displayRoutePlanningBoardDestination(
							s,
							request,
							destinationPage,
							*it->first // place
						);
					}
					else
					{
						s << algorithm::to_lower_copy(it->first->getFullName());
					}
					sortedRows.insert(make_pair(s.str(), it));
				}

				// Boucle sur les rangees
				size_t rank(0);
				for (SortedRows::const_iterator it = sortedRows.begin(); it != sortedRows.end(); ++it, ++rank)
				{
					_displayRoutePlanningBoardRow(
						rowsStream,
						request,
						rowPage,
						destinationPage,
						rank,
						*it->second,
						screen
					);
				}

				pm.insert(DATA_ROWS, rowsStream.str());
			}

			// Subscreens
			size_t subScreenRank(0);
			BOOST_FOREACH(const DisplayScreen::ChildrenType::value_type& it, screen.getChildren())
			{
				if(it.second->getSubScreenType() == DisplayScreen::SUB_CONTENT)
				{
					pm.insert(DATA_SUBSCREEN_ + lexical_cast<string>(subScreenRank++), it.second->getKey());
				}
			}

			// Launch of the display
			page->display(stream, request, pm);
		}



		void DisplayScreenContentFunction::_displayRoutePlanningBoardRow(
			std::ostream& stream,
			const server::Request& request,
			boost::shared_ptr<const cms::Webpage> page,
			boost::shared_ptr<const cms::Webpage> destinationPage,
			size_t rowId,
			const RoutePlanningRow& row,
			const DisplayScreen& screen
		) const {

			ParametersMap pm(getTemplateParameters());

			pm.insert(DATA_ROW_RANK, rowId);
			pm.insert(DATA_WITH_TRANSFER, screen.get<RoutePlanningWithTransfer>());
			pm.insert(DATA_DISPLAY_TRACK_NUMBER, screen.get<TrackNumberDisplay>());
			pm.insert(DATA_DISPLAY_SERVICE_NUMBER, screen.get<ServiceNumberDisplay>());

			{ // Destination
				stringstream str;
				if(destinationPage.get())
				{
					_displayRoutePlanningBoardDestination(
						str,
						request,
						destinationPage,
						*row.first
					);
				}
				else
				{
					str << row.first->getFullName();
				}
				pm.insert(DATA_STOP_NAME, str.str());
			}

			if(!row.second.empty())
			{
				const ServicePointer& s(row.second.getFirstJourneyLeg());

				pm.insert(DATA_BLINKS, s.getDepartureDateTime() - second_clock::local_time() <= minutes(screen.get<BlinkingDelay>()));
				pm.insert(DATA_TRACK, static_cast<const StopPoint*>(s.getRealTimeDepartureVertex())->getName());
				pm.insert(DATA_SERVICE_NUMBER, s.getService()->getServiceNumber());

				{ // Time
					stringstream str;
					str <<
						setw(2) << setfill('0') <<
						s.getDepartureDateTime().time_of_day().hours() <<
						":" << setw(2) << setfill('0') <<
						s.getDepartureDateTime().time_of_day().minutes()
					;
					pm.insert(DATA_TIME, str.str());
				}

				// Line
				static_cast<const JourneyPattern*>(s.getDepartureEdge()->getParentPath())->getCommercialLine()->toParametersMap(pm, true);

				// Transport mode
				const JourneyPattern* jp(static_cast<const JourneyPattern*>(s.getDepartureEdge()->getParentPath()));
				if(jp->getRollingStock())
				{
					pm.insert(DATA_TRANSPORT_MODE, jp->getRollingStock()->getKey());
				}

				if(row.second.getServiceUses().size() > 1)
				{
					const ServicePointer& s(row.second.getLastJourneyLeg());
					pm.insert(DATA_SECOND_TRACK, static_cast<const StopPoint*>(s.getRealTimeDepartureVertex())->getName());
					pm.insert(DATA_SECOND_SERVICE_NUMBER, s.getService()->getServiceNumber());

					stringstream str;
					str << setw(2) << setfill('0') << s.getDepartureDateTime().time_of_day().hours() << ":" << setw(2) << setfill('0') << s.getDepartureDateTime().time_of_day().minutes();
					pm.insert(DATA_SECOND_TIME, str.str());

					static_cast<const JourneyPattern*>(s.getDepartureEdge()->getParentPath())->getCommercialLine()->toParametersMap(pm, true, boost::logic::indeterminate, DATA_SECOND_);

					const JourneyPattern* jp(static_cast<const JourneyPattern*>(s.getDepartureEdge()->getParentPath()));
					if(jp->getRollingStock())
					{
						pm.insert(DATA_SECOND_TRANSPORT_MODE, jp->getRollingStock()->getKey());
					}

					{ // Destination
						stringstream str;
						const StopArea* p(static_cast<const StopArea*>(s.getDepartureEdge()->getFromVertex()->getHub()));
						if(destinationPage.get())
						{
							_displayRoutePlanningBoardDestination(
								str,
								request,
								destinationPage,
								*p
							);
						}
						else
						{
							str << p->getFullName();
						}
						pm.insert(DATA_TRANSFER_STOP_NAME, str.str());
					}
				}
			}

			// Subscreens
			size_t subScreenRank(0);
			BOOST_FOREACH(const DisplayScreen::ChildrenType::value_type& it, screen.getChildren())
			{
				if(it.second->getSubScreenType() == DisplayScreen::SUB_CONTENT)
				{
					pm.insert(DATA_SUBSCREEN_ + lexical_cast<string>(subScreenRank++), it.second->getKey());
				}
			}

			// Launch of the display
			page->display(stream, request, pm);
		}



		void DisplayScreenContentFunction::_displayRoutePlanningBoardDestination(
			std::ostream& stream,
			const server::Request& request,
			boost::shared_ptr<const cms::Webpage> page,
			const pt::StopArea& place
		) const {
			ParametersMap pm(getTemplateParameters());
			place.toParametersMap(pm, true);

			// Launch of the display
			page->display(stream, request, pm);
		}

		void DisplayScreenContentFunction::displayFullDate(
			const string & datafieldName,
			const ptime & time,
			ParametersMap & pm
		) const {
			stringstream s;
			s << to_iso_extended_string(time.date()) << ' ';
			s << setw(2) << setfill('0') << time.time_of_day().hours() << ":" << setw(2) << setfill('0') << time.time_of_day().minutes();
			pm.insert(datafieldName, s.str());
		}
}	}
