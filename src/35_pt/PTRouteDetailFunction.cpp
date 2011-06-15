
//////////////////////////////////////////////////////////////////////////////////////////
/// PTRouteDetailFunction class header.
///	@file PTRouteDetailFunction.hpp
///	@author Hugues Romain
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "PTRouteDetailFunction.hpp"
#include "Request.h"
#include "Edge.h"
#include "StopPoint.hpp"
#include "JourneyPattern.hpp"
#include "RequestException.h"
#include "StopArea.hpp"
#include "City.h"
#include "CommercialLine.h"
#include "Webpage.h"
#include "SchedulesBasedService.h"
#include "ScheduledServiceTableSync.h"
#include "ContinuousServiceTableSync.h"
#include "JourneyPatternCopy.hpp"
#include "Destination.hpp"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace graph;
	using namespace security;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function,pt::PTRouteDetailFunction>::FACTORY_KEY(
		"PTRouteDetailFunction"
	);

	namespace pt
	{
		const string PTRouteDetailFunction::PARAMETER_MAIN_PAGE_ID("mp");
		const string PTRouteDetailFunction::PARAMETER_STOP_PAGE_ID("sp");
		const string PTRouteDetailFunction::PARAMETER_DISPLAY_DEPARTURE_STOPS("dd");
		const string PTRouteDetailFunction::PARAMETER_DISPLAY_ARRIVAL_STOPS("da");

		const string PTRouteDetailFunction::TAG_ROUTE("route");
		const string PTRouteDetailFunction::TAG_STOP("stop");

		const string PTRouteDetailFunction::DATA_ID("id");
		const string PTRouteDetailFunction::DATA_NAME("name");
		const string PTRouteDetailFunction::DATA_LINE_ID("lineId");
		const string PTRouteDetailFunction::DATA_LINE_NAME("lineName");
		const string PTRouteDetailFunction::DATA_LINE_SHORT_NAME("lineShortName");
		const string PTRouteDetailFunction::DATA_LINE_LONG_NAME("lineLongName");
		const string PTRouteDetailFunction::DATA_LINE_IMAGE("lineImage");
		const string PTRouteDetailFunction::DATA_LINE_COLOR("lineColor");
		const string PTRouteDetailFunction::DATA_LINE_STYLE("lineStyle");
		const string PTRouteDetailFunction::DATA_DIRECTION("direction");
		const string PTRouteDetailFunction::DATA_STOPS("stops");
		const string PTRouteDetailFunction::DATA_OPERATOR_CODE("operatorCode");
		const string PTRouteDetailFunction::DATA_RANK("rank");
		const string PTRouteDetailFunction::DATA_DEPARTURE_IS_ALLOWED("departureIsAllowed");
		const string PTRouteDetailFunction::DATA_ARRIVAL_IS_ALLOWED("arrivalIsAllowed");
		const string PTRouteDetailFunction::DATA_STOP_AREA_ID("stopAreaId");
		const string PTRouteDetailFunction::DATA_STOP_AREA_NAME("stopAreaName");
		const string PTRouteDetailFunction::DATA_CITY_ID("cityId");
		const string PTRouteDetailFunction::DATA_CITY_NAME("cityName");
		const string PTRouteDetailFunction::DATA_DIRECTION_ALIAS("directionAlias");
		const string PTRouteDetailFunction::DATA_DEPARTURE_TIME("departureTime");
		const string PTRouteDetailFunction::DATA_ARRIVAL_TIME("arrivalTime");



		PTRouteDetailFunction::PTRouteDetailFunction():
		_displayDepartureStops(true),
		_displayArrivalStops(true)
		{}



		ParametersMap PTRouteDetailFunction::_getParametersMap() const
		{
			ParametersMap result;
			if(_service.get())
			{
				result.insert(Request::PARAMETER_OBJECT_ID, _service->getKey());
			}
			else if(_journeyPattern.get())
			{
				result.insert(Request::PARAMETER_OBJECT_ID, _journeyPattern->getKey());
			}
			if(_mainPage.get())
			{
				result.insert(PARAMETER_MAIN_PAGE_ID, _mainPage->getKey());
			}
			if(_stopPage.get())
			{
				result.insert(PARAMETER_STOP_PAGE_ID, _stopPage->getKey());
			}
			result.insert(PARAMETER_DISPLAY_DEPARTURE_STOPS, _displayDepartureStops);
			result.insert(PARAMETER_DISPLAY_ARRIVAL_STOPS, _displayArrivalStops);
			return result;
		}



		void PTRouteDetailFunction::_setFromParametersMap(const ParametersMap& map)
		{
			RegistryKeyType id(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			try
			{
				if(decodeTableId(id) == ScheduledServiceTableSync::TABLE.ID)
				{
					_service = static_pointer_cast<const SchedulesBasedService, const ScheduledService>(Env::GetOfficialEnv().get<ScheduledService>(id));
					_journeyPattern = Env::GetOfficialEnv().getSPtr<JourneyPattern>(
						dynamic_cast<const JourneyPatternCopy*>(_service->getPath()) ?
						dynamic_cast<const JourneyPatternCopy*>(_service->getPath())->getMainLine() :
						static_cast<const JourneyPattern*>(_service->getPath())
					);
				}
				else if(decodeTableId(id) == ContinuousServiceTableSync::TABLE.ID)
				{
					_service = static_pointer_cast<const SchedulesBasedService, const ContinuousService>(Env::GetOfficialEnv().get<ContinuousService>(id));
					_journeyPattern = Env::GetOfficialEnv().getSPtr<JourneyPattern>(
						dynamic_cast<const JourneyPatternCopy*>(_service->getPath()) ?
						dynamic_cast<const JourneyPatternCopy*>(_service->getPath())->getMainLine() :
						static_cast<const JourneyPattern*>(_service->getPath())
					);
				}
				else
				{
					_journeyPattern = Env::GetOfficialEnv().get<JourneyPattern>(id);
				}
			}
			catch (ObjectNotFoundException<ScheduledService>&)
			{
				throw RequestException("No such service");
			}
			catch (ObjectNotFoundException<ContinuousService>&)
			{
				throw RequestException("No such continuous service");
			}
			catch (ObjectNotFoundException<JourneyPattern>&)
			{
				throw RequestException("No such Journey Pattern");
			}

			optional<RegistryKeyType> pid(map.getOptional<RegistryKeyType>(PARAMETER_MAIN_PAGE_ID));
			if(pid) try
			{
				_mainPage = Env::GetOfficialEnv().get<Webpage>(*pid);
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such main page");
			}

			optional<RegistryKeyType> sid(map.getOptional<RegistryKeyType>(PARAMETER_STOP_PAGE_ID));
			if(sid) try
			{
				_stopPage = Env::GetOfficialEnv().get<Webpage>(*sid);
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such stop page");
			}

			_displayDepartureStops = map.getDefault<bool>(PARAMETER_DISPLAY_DEPARTURE_STOPS, true);
			_displayArrivalStops = map.getDefault<bool>(PARAMETER_DISPLAY_ARRIVAL_STOPS, true);
		}


		void PTRouteDetailFunction::run(ostream& stream, const Request& request) const
		{
			const CommercialLine * commercialLine(_journeyPattern->getCommercialLine());

			if(_mainPage.get())
			{
				ParametersMap m(request.getFunction()->getSavedParameters());
				m.insert(DATA_ID, _journeyPattern->getKey());
				m.insert(DATA_NAME, _journeyPattern->getName());
				m.insert(DATA_LINE_ID, commercialLine->getKey());
				m.insert(DATA_LINE_NAME, commercialLine->getName());
				m.insert(DATA_LINE_SHORT_NAME, commercialLine->getShortName());
				m.insert(DATA_LINE_LONG_NAME, commercialLine->getLongName());
				m.insert(DATA_LINE_IMAGE, commercialLine->getImage());
				m.insert(DATA_LINE_STYLE, commercialLine->getStyle());
				if(commercialLine->getColor())
				{
					m.insert(DATA_LINE_COLOR, commercialLine->getColor()->toXMLColor());
				}
				m.insert(
					DATA_DIRECTION,
					_journeyPattern->getDirection().empty() && _journeyPattern->getDirectionObj() ?
						_journeyPattern->getDirectionObj()->getDisplayedText() :
						_journeyPattern->getDirection()
				);

				if(_stopPage.get())
				{
					stringstream s;
					BOOST_FOREACH(const Edge* edge, _journeyPattern->getAllEdges())
					{
						if(	(!edge->isDepartureAllowed() || !_displayDepartureStops) &&
							(!edge->isArrivalAllowed() || !_displayArrivalStops)
						){
							continue;
						}

						const StopPoint* stopPoint(static_cast<const StopPoint *>(edge->getFromVertex()));
						const StopArea* connPlace(stopPoint->getConnectionPlace());

						ParametersMap sm(request.getFunction()->getSavedParameters());
						sm.insert(DATA_ID, stopPoint->getKey());
						sm.insert(DATA_RANK, edge->getRankInPath());
						sm.insert(DATA_DEPARTURE_IS_ALLOWED, edge->isDepartureAllowed());
						sm.insert(DATA_ARRIVAL_IS_ALLOWED, edge->isArrivalAllowed());
						sm.insert(DATA_NAME, stopPoint->getName());
						sm.insert(DATA_OPERATOR_CODE, stopPoint->getCodeBySources());
						sm.insert(DATA_STOP_AREA_ID, connPlace->getKey());
						sm.insert(DATA_STOP_AREA_NAME, connPlace->getName());
						sm.insert(DATA_CITY_ID, connPlace->getCity()->getKey());
						sm.insert(DATA_CITY_NAME, connPlace->getCity()->getName());
						sm.insert(DATA_DIRECTION_ALIAS, connPlace->getName26());
						if(_service.get())
						{
							if(edge->isDepartureAllowed())
							{
								sm.insert(DATA_DEPARTURE_TIME, to_simple_string(Service::GetTimeOfDay(_service->getDepartureSchedule(false, edge->getRankInPath()))));
							}
							if(edge->isArrivalAllowed())
							{
								sm.insert(DATA_ARRIVAL_TIME, to_simple_string(Service::GetTimeOfDay(_service->getArrivalSchedule(false, edge->getRankInPath()))));
							}
						}

						_stopPage->display(s, request, sm);
					}
					m.insert(DATA_STOPS, s.str());
				}

				_mainPage->display(stream, request, m);

			} else {
				stream <<
					"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
					"<" << TAG_ROUTE << " xsi:noNamespaceSchemaLocation=\"https://extranet-rcsmobility.com/attachments/download/14018/PTRouteDetailFunction.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance" <<
					"\" " << DATA_ID << "=\""            << _journeyPattern->getKey() <<
					"\" " << DATA_NAME << "=\""          << _journeyPattern->getName()<<
					"\" " << DATA_LINE_ID << "=\""        << commercialLine->getKey() <<
					"\" " << DATA_LINE_NAME << "=\""      << commercialLine->getName() <<
					"\" " << DATA_LINE_SHORT_NAME << "=\"" << commercialLine->getShortName() <<
					"\" " << DATA_LINE_LONG_NAME << "=\""  << commercialLine->getLongName() <<
					"\" " << DATA_LINE_IMAGE <<"=\""     << commercialLine->getImage() <<
					"\" " << DATA_LINE_STYLE <<"=\""     << commercialLine->getStyle();
				if(commercialLine->getColor())
				{
					stream << "\" " << DATA_LINE_COLOR << "=\""     << commercialLine->getColor()->toXMLColor();
				}
				stream <<
					"\" " << DATA_DIRECTION << "=\"";
				
				if(_journeyPattern->getDirection().empty() && _journeyPattern->getDirectionObj())
				{
					stream << _journeyPattern->getDirectionObj()->getDisplayedText();
				}
				else
				{
					stream << _journeyPattern->getDirection();
				}

				stream << "\">";

				BOOST_FOREACH(const Edge* edge, _journeyPattern->getAllEdges())
				{
					const StopPoint * stopPoint(static_cast<const StopPoint *>(edge->getFromVertex()));

					const StopArea * connPlace(stopPoint->getConnectionPlace());

					stream << "<" << TAG_STOP << " " << DATA_ID << "=\""   << stopPoint->getKey() <<
						"\" " << DATA_RANK << "=\""          << edge->getRankInPath() <<
						"\" " << DATA_DEPARTURE_IS_ALLOWED << "=\""          << edge->isDepartureAllowed() <<
						"\" " << DATA_ARRIVAL_IS_ALLOWED << "=\""          << edge->isArrivalAllowed() <<
						"\" " << DATA_NAME << "=\""          << stopPoint->getName() <<
						"\" " << DATA_OPERATOR_CODE << "=\""  << stopPoint->getCodeBySources() <<
						"\" " << DATA_STOP_AREA_ID << "=\""        << connPlace->getKey() <<
						"\" " << DATA_STOP_AREA_NAME << "=\""      << connPlace->getName() <<
						"\" " << DATA_CITY_ID << "=\""        << connPlace->getCity()->getKey() <<
						"\" " << DATA_CITY_NAME << "=\""      << connPlace->getCity()->getName() <<
						"\" " << DATA_DIRECTION_ALIAS << "=\""<< connPlace->getName26();
					if(_service.get())
					{
						if(edge->isDepartureAllowed())
						{
							stream << "\" " << DATA_DEPARTURE_TIME << "=\"" << to_simple_string(Service::GetTimeOfDay(_service->getDepartureSchedule(false, edge->getRankInPath())));
						}
						if(edge->isArrivalAllowed())
						{
							stream << "\" " << DATA_ARRIVAL_TIME << "=\"" << to_simple_string(Service::GetTimeOfDay(_service->getArrivalSchedule(false, edge->getRankInPath())));
						}
					}

					stream << "\" />";
				}
				// XML footer
				stream <<
					"</route>";
		}	}



		bool PTRouteDetailFunction::isAuthorized(const Session* session) const
		{
			return true;
		}



		string PTRouteDetailFunction::getOutputMimeType() const
		{
			return _mainPage.get() ? _mainPage->getMimeType() : "text/xml";
		}
}	}
