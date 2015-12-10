
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
#include "LineStop.h"
#include "PTModule.h"
#include "SchedulesBasedService.h"
#include "ContinuousService.h"
#include "JourneyPattern.hpp"
#include "RollingStock.hpp"
#include "RollingStockFilter.h"
#include "CommercialLine.h"
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

		const string DisplayScreenContentFunction::PARAMETER_DATA_SOURCE_NAME_FILTER("data_source_name_filter");
		const string DisplayScreenContentFunction::PARAMETER_USE_SAE_DIRECT_CONNECTION("use_sae_direct_connection");

		const string DisplayScreenContentFunction::PARAMETER_STOPS_LIST("stops_list");

		const string DisplayScreenContentFunction::DATA_IS_REAL_TIME("realTime");

		const string DisplayScreenContentFunction::DATA_HANDICAPPED_ACCESS("handicapped_access");
		
		const string DisplayScreenContentFunction::DATA_SERVICE_ARRIVAL_COMMENT("service_arrival_comment");
		const string DisplayScreenContentFunction::DATA_SERVICE_DEPARTURE_COMMENT("service_departure_comment");


		const string PIPO_KEY("00");
		vector<string> DisplayScreenContentFunction::_SAELine;

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
			api.addParams(DisplayScreenContentFunction::PARAMETER_DATA_SOURCE_NAME_FILTER,
						  "Specify the datasource stops and lines you want to see in the answer",
						  false);
			api.addParams(DisplayScreenContentFunction::PARAMETER_USE_SAE_DIRECT_CONNECTION,
						  "Specify if SAE Direct Connection is needed for RealTime, "
						  "this will open unix sockets to a MySQL BDSI INEO server. \n"
						  "Works only on Linux",
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
			if(_dataSourceName)
			{
				map.insert(PARAMETER_DATA_SOURCE_NAME_FILTER, *_dataSourceName);
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
					_type->setRowNumber(map.getDefault<size_t>(PARAMETER_ROWS_NUMBER, 10));


					// Way 3 : physical stop

					// 3.1 by id
					if(decodeTableId(id) == StopPointTableSync::TABLE.ID)
					{
						boost::shared_ptr<const StopPoint> stop(
								Env::GetOfficialEnv().get<StopPoint>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID))
						);

						screen->setDisplayedPlace(stop->getConnectionPlace());
						screen->setAllPhysicalStopsDisplayed(false);
						ArrivalDepartureTableGenerator::PhysicalStops stopsFilter;
						stopsFilter.insert(make_pair(stop->getKey(), stop.get()));
						screen->setStops(stopsFilter);
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

								screen->setAllPhysicalStopsDisplayed(false);
								ArrivalDepartureTableGenerator::PhysicalStops stopsFilter;

								BOOST_FOREACH(string& stopItem, stopsVect)
								{
									RegistryKeyType stopId = lexical_cast<RegistryKeyType>(stopItem);
									if(decodeTableId(stopId) == StopPointTableSync::TABLE.ID)
									{
										boost::shared_ptr<const StopPoint> stop(
											Env::GetOfficialEnv().get<StopPoint>(stopId)
										);
										screen->setDisplayedPlace(stop->getConnectionPlace());
										stopsFilter.insert(make_pair(stop->getKey(), stop.get()));
									}
									else if (decodeTableId(stopId) == StopAreaTableSync::TABLE.ID)
									{
										boost::shared_ptr<const StopArea> stop(
											Env::GetOfficialEnv().get<StopArea>(stopId)
										);
										BOOST_FOREACH(const StopArea::PhysicalStops::value_type& itStop, stop->getPhysicalStops())
										{
											const StopPoint& stop(*itStop.second);
											screen->setDisplayedPlace(stop.getConnectionPlace());
											stopsFilter.insert(make_pair(stop.getKey(), &stop));
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

						//Get StopPoint Global Registry
						typedef const pair<const RegistryKeyType, boost::shared_ptr<StopPoint> > myType;
						ArrivalDepartureTableGenerator::PhysicalStops pstops;
						BOOST_FOREACH(myType&  myStop,Env::GetOfficialEnv().getRegistry<StopPoint>())
						{
							if(myStop.second->getCodeBySources() == oc)
							{
								pstops.insert(make_pair(myStop.second->getKey(), myStop.second.get()));
								screen->setDisplayedPlace(myStop.second->getConnectionPlace());
								screen->setAllPhysicalStopsDisplayed(false);
								screen->setStops(pstops);
								break;
							}
						}
					}

					// Way 2 : connection place

					// 2.1 by id
					else if(decodeTableId(id) == StopAreaTableSync::TABLE.ID)
					{
						screen->setDisplayedPlace(Env::GetOfficialEnv().getRegistry<StopArea>().get(id).get());
						screen->setAllPhysicalStopsDisplayed(true);
					}

					// 2.2 by name
					else if (!map.getDefault<string>(PARAMETER_CITY_NAME).empty() && !map.getDefault<string>(PARAMETER_STOP_NAME).empty())
					{
						boost::shared_ptr<geography::Place> place(RoadModule::FetchPlace(map.get<string>(PARAMETER_CITY_NAME), map.get<string>(PARAMETER_STOP_NAME)));
						if(!dynamic_cast<StopArea*>(place.get()))
						{
							throw RequestException("This place is not a stop area");
						}
						screen->setDisplayedPlace(dynamic_cast<StopArea*>(place.get()));
						screen->setAllPhysicalStopsDisplayed(true);
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

					
					screen->setType(_type.get());
					//If request contains an interface : build a screen, else prepare custom xml answer
					optional<RegistryKeyType> idReg(map.getOptional<RegistryKeyType>(PARAMETER_INTERFACE_ID));
					if(idReg)
					{
						try
						{
							_type->setDisplayInterface(Env::GetOfficialEnv().getRegistry<Interface>().get(*idReg).get());
						}
						catch (ObjectNotFoundException<Interface>&)
						{
							throw RequestException("No such screen type");
						}
					}
					else if(_mainPage.get() && !_useSAEDirectConnection)
					{
						_type->setDisplayMainPage(_mainPage.get());
						_type->setDisplayRowPage(_rowPage.get());
						_type->setDisplayDestinationPage(_destinationPage.get());
						_type->setDisplayTransferDestinationPage(_transferDestinationPage.get());

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
				}

				// Type control
				if(!_screen->getType())
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

			_dataSourceName = map.getOptional<string>(PARAMETER_DATA_SOURCE_NAME_FILTER);
		}

		void DisplayScreenContentFunction::concatXMLResult(
			std::ostream& stream,
			ServicePointer& servicePointer,
			const StopPoint* stop
		) const {

			const SchedulesBasedService* service = static_cast<const SchedulesBasedService*>(servicePointer.getService());
			const JourneyPattern* journeyPattern = static_cast<const JourneyPattern*>(service->getPath());

			//Here we got our service !
			stream <<"<journey routeId=\""<< journeyPattern->getKey() <<
				"\" dateTime=\""    << servicePointer.getDepartureDateTime() <<
				"\" blink=\"" << "0" <<
					 "\" "<< DATA_IS_REAL_TIME <<"=\"" << (service->hasRealTimeData() ? "yes":"no") <<
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
					"\" article=\""            << rs->getArticle()<<
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
						journeyPattern->getDirectionObj()->getDisplayedText() :
						journeyPattern->getDirection()
					) <<
					"\" />";
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

			const StopArea* connPlace(stop->getConnectionPlace());

			stream << "<stopArea id=\""<< connPlace->getKey()<<
				"\" name=\""           << connPlace->getName() <<
				"\" cityId=\""         << connPlace->getCity()->getKey() <<
				"\" cityName=\""       << connPlace->getCity()->getName() <<
				"\" directionAlias=\"" << connPlace->getName26() <<
				"\" />";

			stream << "</journey>";
		}

		struct MapValue
		{
			vector<DisplayScreenContentFunction::ServiceRealTime> serviceRealTimeVect;
			vector<graph::ServicePointer> servicePointerVect;
		};	

		// RETURN AN ITERATOR FOR THE FIRST SAE SERVICE AFTER SQL REQUEST
		// but if it reached the maximum number of display => break
		map<long long, MapValue>::iterator findFirstRealTime(
				map<long long, MapValue> &servicePointerAll,
				int NbDisplayMin
		)
		{
			map<long long,MapValue>::iterator it = servicePointerAll.begin();

			for(;it!=servicePointerAll.end();it++)//For each minute
			{
				// IF A REAL TIME SERVICE IS FOUNDED OR THE THE NUMBER OF DISPLAY IS REACHED WE RETURN THE POSITION OF THE ITERATOR
				if(!it->second.serviceRealTimeVect.empty() || distance(it, servicePointerAll.end()) <= NbDisplayMin)
					return it;
			}

			return it;
		}

		void DisplayScreenContentFunction::concatXMLResultRealTime(
				std::ostream& stream,
				ServiceRealTime& serviceReal
		) const {
			// DISPLAY RESULT
			stream <<"<journey routeId=\"" << PIPO_KEY <<
					"\" dateTime=\"" << serviceReal.date <<
					"\" blink=\"" << "0" <<
					"\" "<< DATA_IS_REAL_TIME <<"=\"" << serviceReal.Realtime <<
					"\">";

			// Stop point
			stream << "<stop id=\"" << PIPO_KEY <<
					"\" operatorCode=\"" << serviceReal.oc <<
					"\" name=\"" << serviceReal.arret <<
					"\" />";

			// Transport Mode
			stream <<"<transportMode id=\""<< PIPO_KEY <<
					"\" name=\"" << "bus" <<
					"\" article=\"" << "le" <<
					"\" />";

			// Line ID TODO : get Style ?
			stream <<"<line id=\""<< PIPO_KEY <<
					"\" creatorId=\"" << PIPO_KEY <<
					"\" name=\"" << serviceReal.nom_ligne <<
					"\" shortName=\"" << serviceReal.lineShortName <<
					"\" longName=\"" << "" <<
					"\" color=\"" << serviceReal.lineColor <<
					"\" xmlColor=\"" << serviceReal.lineXmlColor <<
					"\" style=\"" << serviceReal.LineStyle <<
					"\" network_id=\"" << serviceReal.networkId <<
					"\" image=\"" << "" <<
					"\" direction=\"" << "" <<
					"\" />";

			// Origin
			stream << "<origin id=\""  << PIPO_KEY <<
					"\" name=\"" << serviceReal.depart <<
					"\" cityName=\"" << serviceReal.cityName_begin <<
					"\" />";

			// Destination
			stream << "<destination id=\"" << PIPO_KEY <<
					"\" name=\"" << serviceReal.arrivee <<
					"\" cityName=\"" << serviceReal.cityName_end <<
					"\" />";

			// Stop Area
			stream << "<stopArea id=\""<< PIPO_KEY <<
					"\" name=\"" << serviceReal.arret <<
					"\" cityId=\"" << serviceReal.cityId_current <<
					"\" cityName=\"" << serviceReal.cityName_current <<
					"\" directionAlias=\"" << "" <<
					"\" />";

			stream << "</journey>";
		}

		void DisplayScreenContentFunction::addJourneyToParametersMapRealTime(
			ParametersMap& pm,
			ServiceRealTime& serviceReal
		) const {
			//Stop Point
			pm.insert(DATA_STOP_ID, serviceReal.stop_id);
			pm.insert(DATA_OPERATOR_CODE, serviceReal.oc);

			//StopArea
			pm.insert(DATA_STOP_AREA_ID, serviceReal.stopAreaId);
			pm.insert(DATA_STOP_AREA_NAME, serviceReal.arret);
			pm.insert(DATA_STOP_AREA_CITY_NAME, serviceReal.cityName_current);
			pm.insert(DATA_STOP_AREA_CITY_ID, serviceReal.cityId_current);

			shared_ptr<ParametersMap> journeyPm(new ParametersMap());
			journeyPm->insert("route_id", PIPO_KEY);
			journeyPm->insert("date_time", serviceReal.date);
			journeyPm->insert("realTime", serviceReal.Realtime);

			shared_ptr<ParametersMap> stopPM(new ParametersMap);
			stopPM->insert("id", PIPO_KEY);
			stopPM->insert("operatorCode", serviceReal.oc);
			stopPM->insert("name", serviceReal.arret);
			journeyPm->insert("stop", stopPM);
			
			shared_ptr<ParametersMap> rsPM(new ParametersMap);
			rsPM->insert("id", PIPO_KEY);
			rsPM->insert("name", "bus");
			rsPM->insert("article", "le");
			journeyPm->insert("rollingStock", rsPM);
			
			shared_ptr<ParametersMap> linePM(new ParametersMap);
			linePM->insert("id", PIPO_KEY);
			linePM->insert("creatorId", PIPO_KEY);
			linePM->insert("name", serviceReal.nom_ligne);
			linePM->insert("shortName" , serviceReal.lineShortName);
			linePM->insert("color",serviceReal.lineColor);
			linePM->insert("xmlcolor",serviceReal.lineXmlColor);
			linePM->insert("style",serviceReal.LineStyle);
			linePM->insert("network_id",serviceReal.networkId);
			
			journeyPm->insert("line", linePM);
			
			shared_ptr<ParametersMap> originPM(new ParametersMap);
			originPM->insert("id", PIPO_KEY);
			originPM->insert("name", serviceReal.depart);
			originPM->insert("cityName", serviceReal.cityName_begin);
			journeyPm->insert("origin", originPM);

			shared_ptr<ParametersMap> destinationPM(new ParametersMap);
			destinationPM->insert("id", PIPO_KEY);
			destinationPM->insert("name", serviceReal.arrivee);
			destinationPM->insert("cityName", serviceReal.cityName_end);
			journeyPm->insert("destination", destinationPM);

			shared_ptr<ParametersMap> connPlacePM(new ParametersMap);
			connPlacePM->insert("id", PIPO_KEY);
			connPlacePM->insert("name", serviceReal.arret);
			connPlacePM->insert("cityId", serviceReal.cityId_current);
			connPlacePM->insert("cityName", serviceReal.cityName_current);
			journeyPm->insert("stopArea", connPlacePM);
			pm.insert("stop_name", serviceReal.arret);
			pm.insert("journey", journeyPm);
		}

		string formatLineName(string commercialLineName)
		{
			string comLineName = commercialLineName;

			// Set lowercase for comparaison
			std::transform(comLineName.begin(), comLineName.end(), comLineName.begin(), ::tolower);

			// Add zero to line shortName before compare to database value
			if ((comLineName.length() < 2) || ((comLineName.length() == 2) && (comLineName[1]=='s')))
				comLineName = "0" + comLineName;

			return comLineName;
		}

		void DisplayScreenContentFunction::addJourneyToParametersMap(
			ParametersMap& pm,
			ServicePointer& servicePointer,
			const StopPoint* stop
		) const {

			const SchedulesBasedService* service = static_cast<const SchedulesBasedService*>(servicePointer.getService());
			const JourneyPattern* journeyPattern = static_cast<const JourneyPattern*>(service->getPath());

			//Stop Point
			pm.insert(DATA_STOP_ID, stop->getKey());
			pm.insert(DATA_STOP_NAME, stop->getName());
			pm.insert(DATA_OPERATOR_CODE, stop->getCodeBySources());

			//StopArea
			const StopArea* connPlace(stop->getConnectionPlace());

			pm.insert(DATA_STOP_AREA_ID, connPlace->getKey());
			pm.insert(DATA_STOP_AREA_NAME, connPlace->getName());
			pm.insert(DATA_STOP_AREA_CITY_NAME, connPlace->getCity()->getName());
			pm.insert(DATA_STOP_AREA_CITY_ID, connPlace->getCity()->getKey());

			boost::shared_ptr<ParametersMap> journeyPm(new ParametersMap());
			journeyPm->insert("route_id", journeyPattern->getKey());
			journeyPm->insert("date_time", servicePointer.getDepartureDateTime());

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


		util::ParametersMap DisplayScreenContentFunction::run( std::ostream& stream, const Request& request ) const
		{
			if(!_screen->getType())
			{
				return ParametersMap();
			}
			if(_screen->getType()->getDisplayInterface() || _screen->getType()->getDisplayMainPage()
			){
				ptime date(_date ? *_date : second_clock::local_time());
				if(!_screen->getIsOnline() ||
					!_screen->getDisplayedPlace()
				){
					return ParametersMap();
				}

				try
				{
					// Start time
					ptime realStartDateTime(date);
					realStartDateTime -= minutes(_screen->getClearingDelay());

					// End time
					ptime endDateTime(realStartDateTime);
					endDateTime += minutes(_screen->getMaxDelay());

					VariablesMap variables;

					if(_screen->getGenerationMethod() == DisplayScreen::ROUTE_PLANNING)
					{
						RoutePlanningTableGenerator generator(
							*_screen->getDisplayedPlace(),
							_screen->getDisplayedPlaces(),
							realStartDateTime,
							endDateTime,
							_screen->getRoutePlanningWithTransfer()
						);

						RoutePlanningList displayedObject(generator.run());
						if(_screen->getType()->getDisplayMainPage())
						{
							_displayRoutePlanningBoard(
								stream,
								request,
								Env::GetOfficialEnv().getSPtr(_screen->getType()->getDisplayMainPage()),
								Env::GetOfficialEnv().getSPtr(_screen->getType()->getDisplayRowPage()),
								Env::GetOfficialEnv().getSPtr(_screen->getType()->getDisplayDestinationPage()),
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

						if(_screen->getType()->getDisplayInterface() &&
							_screen->getType()->getDisplayInterface()->getPage<DeparturesTableInterfacePage>()
						){
							_screen->getType()->getDisplayInterface()->getPage<DeparturesTableInterfacePage>()->display(
								stream,
								variables,
								_screen->getTitle(),
								_screen->getWiringCode(),
								_screen->getServiceNumberDisplay(),
								_screen->getTrackNumberDisplay(),
								_screen->getDisplayTeam(),
								_screen->getType()->getMaxStopsNumber(),
								_screen->getBlinkingDelay(),
								_screen->getDisplayClock(),
								_screen->getDisplayedPlace(),
								displayedObject,
								&request
							);
						}
						else
						{
							assert(_screen->getType()->getDisplayMainPage());

							_displayDepartureBoard(
								stream,
								request,
								Env::GetOfficialEnv().getSPtr(_screen->getType()->getDisplayMainPage()),
								Env::GetOfficialEnv().getSPtr(_screen->getType()->getDisplayRowPage()),
								Env::GetOfficialEnv().getSPtr(_screen->getType()->getDisplayDestinationPage()),
								Env::GetOfficialEnv().getSPtr(_screen->getType()->getDisplayTransferDestinationPage()),
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

			}
			else
			{
				ParametersMap result;
				bool isOutputXML = _outputFormat == MimeTypes::XML;
				AccessParameters ap;
				map<long long, MapValue> servicePointerAll;

				if(isOutputXML)
				{
					// XML header
					stream <<
						"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
						"<timeTable xsi:noNamespaceSchemaLocation=\"http://synthese.rcsmobility.com/include/54_departures_table/DisplayScreenContentFunction.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" type=\"departure\">"
						;
				}

				BOOST_FOREACH(const ArrivalDepartureTableGenerator::PhysicalStops::value_type& it, _screen->getPhysicalStops())
				{
					const StopPoint * stop = it.second;

					//Calculate start time and end time of the searched interval
					ptime startDateTime,endDateTime;
					if(_wayIsBackward)
					{
						//ptime(gregorian::date d,time_duration_type td):

						//If way is backward : endDateTime = _date
						endDateTime = (_date ? *_date : second_clock::local_time());
						//and startDateTime is begin of the day (a day begin at 3:00):
						startDateTime = endDateTime.time_of_day().hours() < 3 ?
							endDateTime - endDateTime.time_of_day() - hours(21) :
							endDateTime - endDateTime.time_of_day() + hours(3);
					}
					else //Way is forward
					{
						// If way is forward : realStartDateTime = date
						startDateTime = (_date ? *_date : second_clock::local_time());
						//startDateTime -= minutes(0); //substract clearing delay
						//and endDateTime is end of the day (a day end at 27:00):
						endDateTime = startDateTime.time_of_day().hours() < 3 ?
							startDateTime - startDateTime.time_of_day() + hours(3) :
							startDateTime - startDateTime.time_of_day() + hours(27);
					}

					//We populate a map of vector : key is minutes of departures
					//each vector contain all service wich pass in the same minute
					map<long long,vector<ServicePointer> > servicePointerMap;

					BOOST_FOREACH(const Vertex::Edges::value_type& edge, stop->getDepartureEdges())
					{
						// Jump over junctions
						if(!dynamic_cast<const LineStop*>(edge.second))
						{
							continue;
						}

						const LineStop* ls = static_cast<const LineStop*>(edge.second);

						const UseRule& useRule(ls->getLine()->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET));
						if(dynamic_cast<const PTUseRule*>(&useRule) && static_cast<const PTUseRule&>(useRule).getForbiddenInDepartureBoards())
						{
							continue;
						}

						ptime departureDateTime = startDateTime;
						// Loop on services
						optional<Edge::DepartureServiceIndex::Value> index;
						while(true)
						{
							ServicePointer servicePointer(
								ls->getNextService(
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
							departureDateTime = servicePointer.getDepartureDateTime();
							if(stop->getKey() != servicePointer.getRealTimeDepartureVertex()->getKey())
								continue;

							//If a lineid arg was passed : only one line will be displayed
							if(_lineToDisplay)
							{
								const JourneyPattern* journeyPattern = static_cast<const JourneyPattern*>(servicePointer.getService()->getPath());
								const CommercialLine * commercialLine(journeyPattern->getCommercialLine());
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
							long long mapKeyMinutes = departureDateTime.time_of_day().minutes()
								+ departureDateTime.time_of_day().hours()*60 //Add hour *60
								+ (departureDateTime.date().day_number()-startDateTime.date().day_number())*1440;// Add 0 or 1 day * 1440

							map<long long,vector<ServicePointer> >::iterator it;
							it=servicePointerMap.find(mapKeyMinutes);

							//Check if a service is already inserted for this date

							if(it==servicePointerMap.end()) //There is no service for this minute
							{
								vector<ServicePointer> tmpVect;

								tmpVect.push_back(servicePointer);

								//Call copy constructor of vector
								servicePointerMap[mapKeyMinutes] = tmpVect;
							}
							else //A service already exist for this minute
							{
								servicePointerMap[mapKeyMinutes].push_back(servicePointer);
							}
						}
					}

					//If backward : reverse display
					if(_wayIsBackward)
					{
						//Loop backward on vector
						int lastIndex = servicePointerMap.size() - _screen->getType()->getRowNumber();
						if(lastIndex<0)
						{
							lastIndex=0;
						}

						map<long long,vector<ServicePointer> >::iterator it = servicePointerMap.begin();
						int minutesCounter=0;
						for(;it!=servicePointerMap.end();it++)//For each minute
						{
							if(minutesCounter>=lastIndex)
							{
								for(size_t i(0); i<it->second.size(); ++i)//For each service at this minute
								{
									if(isOutputXML)
									{
										concatXMLResult(
											stream,
											it->second[i],
											stop
										);
									}
									else
									{
										addJourneyToParametersMap(result, it->second[i], stop);
									}
								}
							}
							++minutesCounter;
						}
					}
					else//If forward : normal display
					{
						map<long long,vector<ServicePointer> >::iterator it = servicePointerMap.begin();
						size_t minutesCounter=0;
						for(;it!=servicePointerMap.end();it++)//For each minute
						{
							if(minutesCounter >= _screen->getType()->getRowNumber())
								break;
							minutesCounter++;
							for(size_t i(0); i<it->second.size(); ++i)//For each service at this minute
							{
								if(isOutputXML)
								{
									concatXMLResult(
										stream,
										it->second[i],
										stop
									);
								}
								else
								{
									addJourneyToParametersMap(result, it->second[i], stop);
								}
							}
						}
					}
				}

				if(isOutputXML)
				{
					// XML footer
					stream << "</timeTable>";
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
			if(_screen->getType()->getDisplayInterface())
			{
				return
					(   _screen.get() &&
						_screen->getType() &&
						_screen->getType()->getDisplayInterface() &&
						_screen->getGenerationMethod() != DisplayScreen::ROUTE_PLANNING &&
						_screen->getType()->getDisplayInterface()->hasPage<DeparturesTableInterfacePage>()
					) ?
						_screen->getType()->getDisplayInterface()->getPage<DeparturesTableInterfacePage>()->getMimeType() :
					"text/plain"
				;
			}
			if(_screen->getType()->getDisplayMainPage())
			{
				return _screen->getType()->getDisplayMainPage()->getMimeType();
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
			pm.insert(DATA_MAC, screen.getMacAddress());
			pm.insert(DATA_DATE, to_iso_extended_string(date.date()) + " " + to_simple_string(date.time_of_day()));
			pm.insert(DATA_TITLE, screen.getTitle());
			pm.insert(DATA_WIRING_CODE, screen.getWiringCode());
			pm.insert(DATA_DISPLAY_SERVICE_NUMBER, screen.getServiceNumberDisplay());
			pm.insert(DATA_DISPLAY_TRACK_NUMBER, screen.getTrackNumberDisplay());
			if(screen.getType())
			{
				pm.insert(DATA_INTERMEDIATE_STOPS_NUMBER, screen.getType()->getMaxStopsNumber());
			}
			pm.insert(DATA_DISPLAY_TEAM, screen.getDisplayTeam());
			pm.insert(DATA_STOP_NAME, screen.getDisplayedPlace() ? screen.getDisplayedPlace()->getFullName() : string());
			pm.insert(DATA_DISPLAY_CLOCK, screen.getDisplayClock());

			// Stop Point
			if(!screen.getAllPhysicalStopsDisplayed() && (screen.getPhysicalStops().size() == 1))
			{
				const ArrivalDepartureTableGenerator::PhysicalStops::const_iterator it = screen.getPhysicalStops().begin();
				const StopPoint * stop = it->second;

				pm.insert(DATA_STOP_ID, stop->getKey());
				pm.insert(DATA_STOP_NAME, stop->getName());
				pm.insert(DATA_OPERATOR_CODE, stop->getCodeBySources());
			}

			// StopArea
			if(screen.getDisplayedPlace())
			{
				const StopArea* connPlace(screen.getDisplayedPlace());

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
							journeyPattern->getDirectionObj()->getDisplayedText() :
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
			pm.insert(DATA_DISPLAY_TRACK_NUMBER, screen.getTrackNumberDisplay());
			pm.insert(DATA_DISPLAY_SERVICE_NUMBER, screen.getServiceNumberDisplay());

			if(screen.getType())
			{
				pm.insert(DATA_INTERMEDIATE_STOPS_NUMBER, screen.getType()->getMaxStopsNumber());
			}
			pm.insert(DATA_DISPLAY_TEAM, screen.getDisplayTeam());
			if(row.first.getService())
			{
				static_cast<const StopPoint*>(row.first.getDepartureEdge()->getFromVertex())->getConnectionPlace()->toParametersMap(pm, true);

				ptime adaptedTime = row.first.getDepartureDateTime();

				// Fetch the time from SCOM if enabled
				#ifdef WITH_SCOM
				if (_scom)
				{
					const JourneyPattern* journeyPattern = static_cast<const JourneyPattern*>(row.first.getService()->getPath());

					std::string dest =
								journeyPattern->getDirection().empty() && journeyPattern->getDirectionObj() ?
								journeyPattern->getDirectionObj()->getDisplayedText() :
								journeyPattern->getDirection();

					// Check object before calling them
					adaptedTime = scom::SCOMModule::GetSCOMData()->GetWaitingTime(
							_screen.get()->getCodeBySources(),
							journeyPattern->getCommercialLine()->getShortName(),
							dest,
							row.first.getDepartureDateTime(),
							requestTime
					);
				}
				#endif

				// Waiting time
				time_duration waitingTime(adaptedTime - requestTime);
				pm.insert(DATA_WAITING_TIME, to_simple_string(waitingTime));

				time_duration blinkingDelay(minutes(screen.getBlinkingDelay()));
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
				
				// Service comments at this stop
				if (schedulesBasedService && row.first.getDepartureEdge())
				{
					size_t rank = row.first.getDepartureEdge()->getRankInPath();
					pm.insert(DATA_SERVICE_ARRIVAL_COMMENT, schedulesBasedService->getArrivalComment(rank));
					pm.insert(DATA_SERVICE_DEPARTURE_COMMENT, schedulesBasedService->getDepartureComment(rank));
				}

				// Direction
				const JourneyPattern* jp(dynamic_cast<const JourneyPattern*>(row.first.getService()->getPath()));
				string lineDirection(
					jp->getDirection().empty() && jp->getDirectionObj() ?
					jp->getDirectionObj()->getDisplayedText() :
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
				line->getDirectionObj()->getDisplayedText() :
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
			pm.insert(DATA_MAC, screen.getMacAddress());
			pm.insert(DATA_DATE, to_iso_extended_string(date.date()) + " " + to_simple_string(date.time_of_day()));
			pm.insert(DATA_TITLE, screen.getTitle());
			pm.insert(DATA_WIRING_CODE, screen.getWiringCode());
			pm.insert(DATA_DISPLAY_SERVICE_NUMBER, screen.getServiceNumberDisplay());
			pm.insert(DATA_DISPLAY_TRACK_NUMBER, screen.getTrackNumberDisplay());
			if(screen.getType())
			{
				pm.insert(DATA_INTERMEDIATE_STOPS_NUMBER, screen.getType()->getMaxStopsNumber());
			}
			pm.insert(DATA_DISPLAY_TEAM, screen.getDisplayTeam());
			pm.insert(DATA_STOP_NAME, screen.getDisplayedPlace() ? screen.getDisplayedPlace()->getFullName() : string());
			pm.insert(DATA_DISPLAY_CLOCK, screen.getDisplayClock());
			pm.insert(DATA_WITH_TRANSFER, screen.getRoutePlanningWithTransfer());
			screen.getDisplayedPlace()->toParametersMap(pm, true);

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
			pm.insert(DATA_WITH_TRANSFER, screen.getRoutePlanningWithTransfer());
			pm.insert(DATA_DISPLAY_TRACK_NUMBER, screen.getTrackNumberDisplay());
			pm.insert(DATA_DISPLAY_SERVICE_NUMBER, screen.getServiceNumberDisplay());

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

				pm.insert(DATA_BLINKS, s.getDepartureDateTime() - second_clock::local_time() <= minutes(screen.getBlinkingDelay()));
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
