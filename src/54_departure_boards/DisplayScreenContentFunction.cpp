
/** DisplayScreenContentFunction class implementation.
	@file DisplayScreenContentFunction.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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
#include "RollingStock.h"
#include "CommercialLine.h"
#include "City.h"

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
	using namespace pt;
	using namespace departure_boards;

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
		
		
		
		ParametersMap DisplayScreenContentFunction::_getParametersMap() const
		{
			ParametersMap map(FunctionWithSiteBase::_getParametersMap());
			if(_date && _date->is_not_a_date_time()) map.insert(PARAMETER_DATE, *_date);
			if(_screen.get()) map.insert(Request::PARAMETER_OBJECT_ID, _screen->getKey());
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


						// Way 3 : physical stop

						// 3.1 by id
						if(decodeTableId(id) == StopPointTableSync::TABLE.ID)
						{
							shared_ptr<const StopPoint> stop(
									Env::GetOfficialEnv().get<StopPoint>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID))
							);

							screen->setLocalization(stop->getConnectionPlace());
							screen->setAllPhysicalStopsDisplayed(false);
							screen->addPhysicalStop(stop.get());
						}

						// 3.2 by operator code
						else if(!map.getDefault<string>(PARAMETER_OPERATOR_CODE).empty())
						{
							string oc(map.get<string>(PARAMETER_OPERATOR_CODE));
							shared_ptr<const StopArea> place(
									Env::GetOfficialEnv().get<StopArea>(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID))
							);

							screen->setLocalization(place.get());
							screen->setAllPhysicalStopsDisplayed(false);
							const ArrivalDepartureTableGenerator::PhysicalStops& stops(place->getPhysicalStops());
							BOOST_FOREACH(const ArrivalDepartureTableGenerator::PhysicalStops::value_type& it, stops)
							{
								if(it.second->getCodeBySource() == oc)
									screen->addPhysicalStop(it.second);
							}
						}

						// Way 2 : connection place

						// 2.1 by id
						else if(decodeTableId(id) == StopAreaTableSync::TABLE.ID)
						{
							screen->setLocalization(Env::GetOfficialEnv().getRegistry<StopArea>().get(id).get());
							screen->setAllPhysicalStopsDisplayed(true);
						}

						// 2.2 by name
						else if (!map.getDefault<string>(PARAMETER_CITY_NAME).empty() && !map.getDefault<string>(PARAMETER_STOP_NAME).empty())
						{
							screen->setAllPhysicalStopsDisplayed(true);

						}


						else // Failure
						{
							throw RequestException("Not a display screen nor a connection place");
						}

					}else{//answer will be XML

						//4.1 by operator code
						if(!map.getDefault<string>(PARAMETER_OPERATOR_CODE).empty())
						{
							//If an oc was given we search corresponding physical stop
							string oc(map.get<string>(PARAMETER_OPERATOR_CODE));

							//Get StopPoint Global Registry
							typedef const pair<const RegistryKeyType, shared_ptr<StopPoint> > myType;
							BOOST_FOREACH(myType&  myStop,Env::GetOfficialEnv().getRegistry<StopPoint>())
							{
								if(myStop.second->getCodeBySource() == oc)
								{
									screen->addPhysicalStop(myStop.second.get());
									break;
								}
							}
						}
						else //4.2 by physical stop
						{
							RegistryKeyType id = map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID);
							shared_ptr<const StopPoint> stop(
									Env::GetOfficialEnv().get<StopPoint>(id)
							);
							screen->addPhysicalStop(stop.get());
						}
						// Way
						_wayIsBackward = false;
						optional<string> way(map.getOptional<string>(PARAMETER_WAY));
						if(way && (*way) == "backward")
						{
							_wayIsBackward = true;
						}

						_lineToDisplay = map.getOptional<RegistryKeyType>(PARAMETER_LINE_ID);
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
			)const
		{
			const SchedulesBasedService * service = static_cast<const SchedulesBasedService *>(servicePointer.getService());
			const JourneyPattern* journeyPattern = static_cast<const JourneyPattern*>(service->getPath());

			//Here we got our service !
			stream <<"<journey routeId=\""<< journeyPattern->getKey() <<
				"\" dateTime=\""    << servicePointer.getDepartureDateTime() <<
				"\" blink=\"" << "0" <<
				"\">";

			RollingStock * rs = journeyPattern->getRollingStock();

			stream << "<stop id=\"" << stop->getKey() <<
				"\" operatorCode=\""<< stop->getCodeBySource() <<
				"\" name=\""        << stop->getName() <<
				"\" />";

			stream <<"<transportMode id=\""<< rs->getKey() <<
				"\" name=\""               << rs->getName() <<
				"\" article=\""            << rs->getArticle()<<
				"\" />";

			const CommercialLine * commercialLine(journeyPattern->getCommercialLine());

			stream <<"<line id=\""<< commercialLine->getKey() <<
				"\" creatorId=\"" << commercialLine->getCreatorId() <<
				"\" name=\""      << commercialLine->getName() <<
				"\" shortName=\"" << commercialLine->getShortName() <<
				"\" longName=\""  << commercialLine->getLongName() <<
				"\" color=\""     << commercialLine->getColor() <<
				"\" style=\""     << commercialLine->getStyle() <<
				"\" image=\""     << commercialLine->getImage() <<
				"\" />";

			const StopArea & origin(
					*journeyPattern->getOrigin()->getConnectionPlace()
			);
			stream << "<origin id=\""  << origin.getKey() <<
					"\" name=\""           << origin.getName() <<
					"\" cityName=\""       << origin.getCity()->getName() <<
					"\" />";

			const StopArea & destination(
					*journeyPattern->getDestination()->getConnectionPlace()
			);
			stream << "<destination id=\"" << destination.getKey() <<
					"\" name=\""           << destination.getName() <<
					"\" cityName=\""       << destination.getCity()->getName() <<
					"\" />";

			const StopArea * connPlace(stop->getConnectionPlace());

			stream << "<stopArea id=\""<< connPlace->getKey()<<
				"\" name=\""           << connPlace->getName() <<
				"\" cityId=\""         << connPlace->getCity()->getKey() <<
				"\" cityName=\""       << connPlace->getCity()->getName() <<
				"\" directionAlias=\"" << connPlace->getName26() <<
				"\" />";

			stream << "</journey>";
		}

		void DisplayScreenContentFunction::run( std::ostream& stream, const Request& request ) const
		{
			if(_type->getDisplayInterface()!=NULL)
			{
				_screen->display(stream, _date ? *_date : second_clock::local_time(), &request);
			}
			else
			{

				// XML header
				stream <<
					"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" <<
					"<timeTable xsi:noNamespaceSchemaLocation=\"http://synthese.rcsmobility.com/include/54_departures_table/DisplayScreenContentFunction.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" type=\"departure\">"
					;

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

						ptime departureDateTime = startDateTime;
						// Loop on services
						optional<Edge::DepartureServiceIndex::Value> index;
						while(true)
						{
							ServicePointer servicePointer(
									ls->getNextService(
											USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET,
											departureDateTime
											, endDateTime
											, false
											, index
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
								for(int i=0;i<it->second.size();i++)//For each service at this minute
								{
									concatXMLResult(
											stream,
											it->second[i],
											stop
									);
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
							for(int i=0;i<it->second.size();i++)//For each service at this minute
							{
								concatXMLResult(
										stream,
										it->second[i],
										stop
								);
							}
						}
					}
				}

				// XML footer
				stream << "</timeTable>";
			}
		}



		bool DisplayScreenContentFunction::isAuthorized(const Session* session
		) const {
			return true;
		}

		std::string DisplayScreenContentFunction::getOutputMimeType() const
		{
			return (_screen->getType()->getDisplayInterface()==NULL) ? "text/xml" :
					(
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
					)
			;
		}

		void DisplayScreenContentFunction::setScreen(
			shared_ptr<const DisplayScreen> value
		){
			_screen = value;
		}
	}
}
