
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

#include "RequestException.h"
#include "Request.h"
#include "StopPointTableSync.hpp"
#include "ServicePointer.h"
#include "DisplayScreenContentFunction.h"
#include "DisplayScreen.h"
#include "DisplayScreenTableSync.h"
#include "DisplayType.h"
#include "DisplayTypeTableSync.h"
#include "DeparturesTableInterfacePage.h"
#include "DeparturesTableRoutePlanningInterfacePage.h"
#include "StopAreaTableSync.hpp"
#include "Interface.h"
#include "Env.h"
#include "LineStop.h"
#include "SchedulesBasedService.h"
#include "JourneyPattern.hpp"
#include "RollingStock.hpp"
#include "CommercialLine.h"
#include "City.h"
#include "Alarm.h"
#include "Webpage.h"
#include "RoadModule.h"
#include "PTUseRule.h"
#include "Destination.hpp"
#include "RoutePlanningTableGenerator.h"
#include "DisplayScreenAlarmRecipient.h"
#include "InterfacePageException.h"
#include "MimeTypes.hpp"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace server;
	using namespace pt;
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
		const string DisplayScreenContentFunction::DATA_MESSAGE_LEVEL("message_level");
		const string DisplayScreenContentFunction::DATA_MESSAGE_CONTENT("message_content");
		const string DisplayScreenContentFunction::DATA_DATE("date");
		const string DisplayScreenContentFunction::DATA_SUBSCREEN_("subscreen_");

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
					setOutputFormatFromMap(map, "");
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
					//Generation Method of the pannel is "STANDARD_METHOD"
					screen->setGenerationMethod(DisplayScreen::STANDARD_METHOD);
					_type.reset(new DisplayType);
					_type->setRowNumber(map.getDefault<size_t>(PARAMETER_ROWS_NUMBER, 10));


					// Way 3 : physical stop

					// 3.1 by id
					if(decodeTableId(id) == StopPointTableSync::TABLE.ID)
					{
						shared_ptr<const StopPoint> stop(
								Env::GetOfficialEnv().get<StopPoint>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID))
						);

						screen->setDisplayedPlace(stop->getConnectionPlace());
						screen->setAllPhysicalStopsDisplayed(false);
						ArrivalDepartureTableGenerator::PhysicalStops stopsFilter;
						stopsFilter.insert(make_pair(stop->getKey(), stop.get()));
						screen->setStops(stopsFilter);
					}

					// 3.2 by operator code
					//4.1 by operator code
					else if(!map.getDefault<string>(PARAMETER_OPERATOR_CODE).empty())
					{
						//If an oc was given we search corresponding physical stop
						string oc(map.get<string>(PARAMETER_OPERATOR_CODE));

						//Get StopPoint Global Registry
						typedef const pair<const RegistryKeyType, shared_ptr<StopPoint> > myType;
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
					else if(_mainPage.get())
					{
						_type->setDisplayMainPage(_mainPage.get());
						_type->setDisplayRowPage(_rowPage.get());
						_type->setDisplayDestinationPage(_destinationPage.get());
						_type->setDisplayTransferDestinationPage(_transferDestinationPage.get());
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
			ServicePointer& servicePointer,
			const StopPoint* stop
		) const {

			const SchedulesBasedService* service = static_cast<const SchedulesBasedService*>(servicePointer.getService());
			const JourneyPattern* journeyPattern = static_cast<const JourneyPattern*>(service->getPath());

			//Here we got our service !
			stream <<"<journey routeId=\""<< journeyPattern->getKey() <<
				"\" dateTime=\""    << servicePointer.getDepartureDateTime() <<
				"\" blink=\"" << "0" <<
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
					"\" color=\""     << commercialLine->getColor() <<
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



		void DisplayScreenContentFunction::addJourneyToParametersMap(
			ParametersMap& pm,
			ServicePointer& servicePointer,
			const StopPoint* stop
		) const {

			const SchedulesBasedService* service = static_cast<const SchedulesBasedService*>(servicePointer.getService());
			const JourneyPattern* journeyPattern = static_cast<const JourneyPattern*>(service->getPath());

			shared_ptr<ParametersMap> journeyPm(new ParametersMap());
			journeyPm->insert("route_id", journeyPattern->getKey());
			journeyPm->insert("date_time", servicePointer.getDepartureDateTime());

			shared_ptr<ParametersMap> stopPM(new ParametersMap);
			stop->toParametersMap(*stopPM, false);
			journeyPm->insert("stop", stopPM);

			RollingStock* rs = journeyPattern->getRollingStock();
			if(rs)
			{
				shared_ptr<ParametersMap> rsPM(new ParametersMap);
				rs->toParametersMap(*rsPM);
				journeyPm->insert("rollingStock", rsPM);
			}

			const CommercialLine* commercialLine(journeyPattern->getCommercialLine());

			if(commercialLine)
			{
				shared_ptr<ParametersMap> linePM(new ParametersMap);
				commercialLine->toParametersMap(*linePM);
				journeyPm->insert("line", linePM);
			}

			const StopArea& origin(
				*journeyPattern->getOrigin()->getConnectionPlace()
			);
			shared_ptr<ParametersMap> originPM(new ParametersMap);
			origin.toParametersMap(*originPM);
			journeyPm->insert("origin", originPM);


			const StopArea& destination(
				*journeyPattern->getDestination()->getConnectionPlace()
			);
			shared_ptr<ParametersMap> destinationPM(new ParametersMap);
			destination.toParametersMap(*destinationPM);
			journeyPm->insert("destination", destinationPM);

			const StopArea* connPlace(stop->getConnectionPlace());
			shared_ptr<ParametersMap> connPlacePM(new ParametersMap);
			connPlace->toParametersMap(*connPlacePM);
			journeyPm->insert("stopArea", connPlacePM);

			pm.insert("journey", journeyPm);
		}



		util::ParametersMap DisplayScreenContentFunction::run( std::ostream& stream, const Request& request ) const
		{
			if(!_screen->getType())
			{
				return ParametersMap();
			}
			if(	_screen->getType()->getDisplayInterface() || _screen->getType()->getDisplayMainPage()
			){
				ptime date(_date ? *_date : second_clock::local_time());
				if(	!_screen->getIsOnline() ||
					!_screen->getDisplayedPlace()
				){
					return ParametersMap();
				}

				try
				{
					// End time
					ptime realStartDateTime(date);
					realStartDateTime -= minutes(_screen->getClearingDelay());
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

						RoutePlanningListWithAlarm displayedObject;
						displayedObject.map = generator.run();
						displayedObject.alarm = DisplayScreenAlarmRecipient::getAlarm(_screen.get(), date);

						if(	_screen->getType()->getDisplayInterface() &&
							_screen->getType()->getDisplayInterface()->getPage<DeparturesTableRoutePlanningInterfacePage>()
						){
							_screen->getType()->getDisplayInterface()->getPage<DeparturesTableRoutePlanningInterfacePage>()->display(
								stream,
								variables,
								_screen->getTitle(),
								_screen->getWiringCode(),
								_screen->getServiceNumberDisplay(),
								_screen->getTrackNumberDisplay(),
								_screen->getRoutePlanningWithTransfer(),
								_screen->getBlinkingDelay(),
								_screen->getDisplayClock(),
								*_screen->getDisplayedPlace(),
								displayedObject,
								&request
							);
						}
						else
						{
							assert(_screen->getType()->getDisplayMainPage());

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
						ArrivalDepartureListWithAlarm displayedObject;
						displayedObject.map = _screen->generateStandardScreen(realStartDateTime, endDateTime);
						displayedObject.alarm = DisplayScreenAlarmRecipient::getAlarm(_screen.get(), date);

						if(	_screen->getType()->getDisplayInterface() &&
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
				bool isOutputXML = _outputFormat.empty();

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
						startDateTime = endDateTime - endDateTime.time_of_day() + hours(3);
					}
					else //Way is forward
					{
						// If way is forward : realStartDateTime = date
						startDateTime = (_date ? *_date : second_clock::local_time());
						//startDateTime -= minutes(0); //substract clearing delay
						//and endDateTime is end of the day (a day end at 27:00):
						endDateTime = startDateTime - startDateTime.time_of_day() + hours(27);
					}

					//We populate a map of vector : key is minutes of departures
					//each vector contain all service wich pass in the same minute
					map<long long,vector<ServicePointer> > servicePointerMap;

					BOOST_FOREACH(const Vertex::Edges::value_type& edge, stop->getDepartureEdges())
					{
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

							long long mapKeyMinutes = departureDateTime.time_of_day().minutes()
										+ departureDateTime.time_of_day().hours()*60 //Add hour *60
										+ (departureDateTime.date().day_number()-startDateTime.date().day_number())*1440;//Add 0 or 1 day * 1440

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
						int minutesCounter=0;
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
						(	_screen->getGenerationMethod() == DisplayScreen::ROUTE_PLANNING ?
							_screen->getType()->getDisplayInterface()->hasPage<DeparturesTableRoutePlanningInterfacePage>()	:
							_screen->getType()->getDisplayInterface()->hasPage<DeparturesTableInterfacePage>()
						)
					) ?
						(	_screen->getGenerationMethod() == DisplayScreen::ROUTE_PLANNING ?
							_screen->getType()->getDisplayInterface()->getPage<DeparturesTableRoutePlanningInterfacePage>()->getMimeType() :
							_screen->getType()->getDisplayInterface()->getPage<DeparturesTableInterfacePage>()->getMimeType()
						) :
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
			shared_ptr<const DisplayScreen> value
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
			const ArrivalDepartureListWithAlarm& rows,
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

			// Rows
			if(rowPage.get())
			{
				stringstream rowStream;
				const ArrivalDepartureList& ptds(rows.map);

				/// @todo replace by parameters or something else
				int __Pages(0);
				int departuresToHide(0);
				string __SeparateurPage;
				int __MultiplicateurRangee(1);

				// Gestion des pages
				size_t __NombrePages(1);
				if(__Pages != 0)
				{
					int departuresNumber = ptds.size() - departuresToHide;
					for (ArrivalDepartureList::const_iterator it = ptds.begin(); departuresNumber && (it != ptds.end()); ++it, --departuresNumber)
					{
						const ActualDisplayedArrivalsList& displayedList = it->second;
						if (displayedList.size() > __NombrePages + 2)
						{
							__NombrePages = displayedList.size () - 2;
						}
					}
					if (__Pages != UNKNOWN_VALUE && __NombrePages > __Pages)
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
					int departuresNumber = ptds.size() - departuresToHide;
					for (ArrivalDepartureList::const_iterator it = ptds.begin(); departuresNumber && (it != ptds.end()); ++it, --departuresNumber)
					{
						const ArrivalDepartureRow& row(*it);

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

			// Messages
			if(rows.alarm)
			{
				pm.insert(DATA_MESSAGE_LEVEL, rows.alarm->getLevel());
				pm.insert(DATA_MESSAGE_CONTENT, rows.alarm->getLongMessage());
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
				static_cast<const StopPoint*>(row.first.getDepartureEdge()->getFromVertex())->getConnectionPlace()->toParametersMap(pm);

				// Waiting time
				time_duration waitingTime(row.first.getDepartureDateTime() - requestTime);
				pm.insert(DATA_WAITING_TIME, to_simple_string(waitingTime));

				time_duration blinkingDelay(minutes(screen.getBlinkingDelay()));
				if(	blinkingDelay.total_seconds() > 0 &&
					waitingTime <= blinkingDelay
				){
					pm.insert(DATA_BLINKS, true);
				}

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

				// Direction
				const JourneyPattern* jp(dynamic_cast<const JourneyPattern*>(row.first.getService()->getPath()));
				string lineDirection(
					jp->getDirection().empty() && jp->getDirectionObj() ?
					jp->getDirectionObj()->getDisplayedText() :
					jp->getDirection()
				);
				pm.insert(
					DATA_DIRECTION,
					lineDirection.empty() ? jp->getDestination()->getConnectionPlace()->getFullName() : lineDirection
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
				dynamic_cast<const CommercialLine&>(*row.first.getService()->getPath()->getPathGroup()).toParametersMap(pm);

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
			std::size_t rank,
			std::size_t globalRank,
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
			place->toParametersMap(pm);
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
			std::size_t localTransferRank,
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

			line->getCommercialLine()->toParametersMap(pm);
			place->toParametersMap(pm);

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
			const RoutePlanningListWithAlarm& rows,
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
			screen.getDisplayedPlace()->toParametersMap(pm);

			// Rows
			if(rowPage.get())
			{
				stringstream rowsStream;
				const RoutePlanningList& ptds(rows.map);

				// Sort of the rows
				typedef map<string,RoutePlanningList::const_iterator> SortedRows;
				SortedRows sortedRows;
				for(RoutePlanningList::const_iterator it = ptds.begin(); it != ptds.end(); ++it)
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

			// Messages
			if(rows.alarm)
			{
				pm.insert(DATA_MESSAGE_LEVEL, rows.alarm->getLevel());
				pm.insert(DATA_MESSAGE_CONTENT, rows.alarm->getLongMessage());
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
			std::size_t rowId,
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
				static_cast<const JourneyPattern*>(s.getDepartureEdge()->getParentPath())->getCommercialLine()->toParametersMap(pm);

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

					static_cast<const JourneyPattern*>(s.getDepartureEdge()->getParentPath())->getCommercialLine()->toParametersMap(pm, DATA_SECOND_);

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
			place.toParametersMap(pm);

			// Launch of the display
			page->display(stream, request, pm);
		}
}	}
