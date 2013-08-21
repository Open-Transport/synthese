
/** PTRoutesListFunction class implementation.
	@file PTRoutesListFunction.cpp
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
#include "PTRoutesListFunction.hpp"
#include "CommercialLine.h"
#include "JourneyPattern.hpp"
#include "Env.h"
#include "Webpage.h"
#include "StopPoint.hpp"
#include "City.h"
#include "StopArea.hpp"
#include "LinePhysicalStop.hpp"
#include "Destination.hpp"
#include "CalendarTemplate.h"
#include "JourneyPatternCopy.hpp"
#include "MimeTypes.hpp"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace pt;
	using namespace cms;
	using namespace graph;
	using namespace calendar;

	template<> const string FactorableTemplate<Function, PTRoutesListFunction>::FACTORY_KEY("PTRoutesListFunction");

	namespace pt
	{
		const string PTRoutesListFunction::PARAMETER_PAGE_ID("p");
		const string PTRoutesListFunction::PARAMETER_MERGE_INCLUDING_ROUTES("mir");
		const string PTRoutesListFunction::PARAMETER_MERGE_SAME_ROUTES("msr");
		const string PTRoutesListFunction::PARAMETER_DATE("date");
		const string PTRoutesListFunction::PARAMETER_CALENDAR_ID("calendar_id");
		const string PTRoutesListFunction::PARAMETER_FILTER_MAIN_ROUTES("fmr");
		const string PTRoutesListFunction::PARAMETER_STOP_ID = "stop_id";

		const std::string PTRoutesListFunction::DATA_NAME("name");
		const std::string PTRoutesListFunction::DATA_LENGTH("length");
		const std::string PTRoutesListFunction::DATA_STOPS_NUMBER("stops_number");
		const std::string PTRoutesListFunction::DATA_DIRECTION("direction");
		const std::string PTRoutesListFunction::DATA_ORIGIN_CITY_NAME("origin_city_name");
		const std::string PTRoutesListFunction::DATA_ORIGIN_STOP_NAME("origin_stop_name");
		const std::string PTRoutesListFunction::DATA_DESTINATION_CITY_NAME("destination_city_name");
		const std::string PTRoutesListFunction::DATA_DESTINATION_STOP_NAME("destination_stop_name");
		const std::string PTRoutesListFunction::DATA_RANK("rank");
		const std::string PTRoutesListFunction::DATA_RANK_IS_ODD("rank_is_odd");
		const std::string PTRoutesListFunction::DATA_IS_MAIN("is_main");
		const std::string PTRoutesListFunction::DATA_ROUTES("routes");



		ParametersMap PTRoutesListFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			else
			{
				map.insert(PARAMETER_OUTPUT_FORMAT_COMPAT, _outputFormat);
			}
			map.insert(PARAMETER_MERGE_INCLUDING_ROUTES, _mergeIncludingRoutes);
			map.insert(PARAMETER_MERGE_SAME_ROUTES, _mergeSameRoutes);
			if(_line.get())
			{
				map.insert(Request::PARAMETER_OBJECT_ID, _line->getKey());
			}
			if(_filterMainRoutes)
			{
				map.insert(PARAMETER_FILTER_MAIN_ROUTES, *_filterMainRoutes);
			}
			if(_calendar)
			{
				map.insert(PARAMETER_CALENDAR_ID, _calendar->getKey());
			}
			return map;
		}



		void PTRoutesListFunction::_setFromParametersMap(const ParametersMap& map)
		{
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
				setOutputFormatFromMap(map, MimeTypes::XML);
			}
			_mergeIncludingRoutes = map.isTrue(PARAMETER_MERGE_INCLUDING_ROUTES);

			_mergeSameRoutes = map.isTrue(PARAMETER_MERGE_SAME_ROUTES);

			// Filter on the "main route" attribute
			_filterMainRoutes = map.getOptional<bool>(PARAMETER_FILTER_MAIN_ROUTES);

			RegistryKeyType lineId(
				map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0)
			);
			if(lineId) try
			{
				_line = Env::GetOfficialEnv().getRegistry<CommercialLine>().get(
					lineId
				);
			}
			catch (ObjectNotFoundException<CommercialLine>&)
			{
				throw RequestException("No such line");
			}

			RegistryKeyType stopId(
				map.getDefault<RegistryKeyType>(PARAMETER_STOP_ID, 0)
			);
			if(stopId) try
			{
				_stop = Env::GetOfficialEnv().getRegistry<StopPoint>().get(
					stopId
				);
			}
			catch (ObjectNotFoundException<StopPoint>&)
			{
				throw RequestException("No such stop point");
			}

			// Date
			string dateStr(map.getDefault<string>(PARAMETER_DATE));
			if(!dateStr.empty())
			{
				if(dateStr.size() == 10)
				{
					_date = ptime(
						from_string(dateStr)
					);
				}
				else
				{
					_date = time_from_string(dateStr);
				}
			}

			// Calendar
			if(map.getDefault<RegistryKeyType>(PARAMETER_CALENDAR_ID, 0))
			{
				try
				{
					_calendar = Env::GetOfficialEnv().get<CalendarTemplate>(map.get<RegistryKeyType>(PARAMETER_CALENDAR_ID));
				}
				catch(ObjectNotFoundException<CalendarTemplate>&)
				{
					throw RequestException("No such calendar template");
				}

				// Checks if the calendar template is usable
				if(!_calendar->isLimited())
				{
					throw RequestException("The calendar template must define a limited period.");
				}
			}
		}



		util::ParametersMap PTRoutesListFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			set<const JourneyPattern*> routes;
			// Selection of routes by same comparison
			Calendar calendarFilter;
			if(_calendar.get())
			{
				calendarFilter = _calendar->getResult();
			}
			set<const Path*> paths;
			if(_line.get())
			{
				BOOST_FOREACH(const Path* path, _line->getPaths())
				{
					paths.insert(path);
				}
			}
			else if(_stop.get())
			{
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, _stop->getDepartureEdges())
				{
					paths.insert(edge.first);
				}
			}
			BOOST_FOREACH(const Path* path, paths)
			{
				const JourneyPattern* thisRoute = dynamic_cast<const JourneyPattern*>(path);
				if(!thisRoute)
				{
					continue;
				}

				// Date filter
				if(_date && !thisRoute->isActive(_date->date()))
				{
					continue;
				}

				// Calendar filter
				if(_calendar.get())
				{
					bool result(thisRoute->getCalendarCache().hasAtLeastOneCommonDateWith(calendarFilter));
					if(!result)
					{
						BOOST_FOREACH(JourneyPatternCopy* subline, thisRoute->getSubLines())
						{
							if(subline->getCalendarCache().hasAtLeastOneCommonDateWith(calendarFilter))
							{
								result = true;
								break;
							}
						}
					}
					if(!result)
					{
						continue;
					}
				}

				// Main route filter
				if(_filterMainRoutes && thisRoute->getMain() != *_filterMainRoutes)
				{
					continue;
				}


				// Route merging
				if(_mergeSameRoutes)
				{
					bool toInsert(true);
					BOOST_FOREACH(const JourneyPattern* existingRoute, routes)
					{
						if(existingRoute->sameContent(*path, false, false))
						{
							toInsert = false;
							break;
						}
					}
					if(!toInsert)
					{
						continue;
					}
				}

				routes.insert(thisRoute);
			}

			// Selection of routes by inclusion
			if(_mergeIncludingRoutes)
			{
				vector<const JourneyPattern*> routesToRemove;
				BOOST_FOREACH(const JourneyPattern* route, routes)
				{
					BOOST_FOREACH(const JourneyPattern* otherRoute, routes)
					{
						if(otherRoute == route)
						{
							continue;
						}
						if(otherRoute->includes(*route, false))
						{
							routesToRemove.push_back(route);
							break;
						}
					}
				}
				BOOST_FOREACH(const JourneyPattern* route, routesToRemove)
				{
					routes.erase(route);
				}
			}


			bool isOutputXML = !_page.get() && _outputFormat == MimeTypes::XML;
			if(isOutputXML)
			{
				// XML header
				stream <<
					"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
					"<directions xsi:noNamespaceSchemaLocation=\"http://synthese.rcsmobility.com/include/35_pt/PTRoutesListFunction.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""<<
					" lineName=\"" << _line->getName() <<
					"\">";
			}

			ParametersMap result;

			size_t rank(0);
			BOOST_FOREACH(const JourneyPattern* route, routes)
			{
				if(!isOutputXML)
				{
					boost::shared_ptr<ParametersMap> pm(new ParametersMap());

					pm->insert(Request::PARAMETER_OBJECT_ID, route->getKey());
					pm->insert(DATA_NAME, route->getName());
					pm->insert(DATA_LENGTH, route->getLastEdge() ? route->getLastEdge()->getMetricOffset() : double(0));
					pm->insert(DATA_STOPS_NUMBER, route->getEdges().size());
					pm->insert(DATA_IS_MAIN, route->getMain());
					pm->insert(DATA_DIRECTION,
						route->getDirection().empty() && route->getDirectionObj() ?
						route->getDirectionObj()->getDisplayedText() :
						route->getDirection()
					);
					{
						const LinePhysicalStop* lineStop(dynamic_cast<const LinePhysicalStop*>(*route->getAllEdges().begin()));
						if(lineStop)
						{
							pm->insert(DATA_ORIGIN_CITY_NAME, lineStop->getPhysicalStop()->getConnectionPlace()->getCity()->getName());
							pm->insert(DATA_ORIGIN_STOP_NAME, lineStop->getPhysicalStop()->getConnectionPlace()->getName());
						}
					}
					{
						const LinePhysicalStop* lineStop(dynamic_cast<const LinePhysicalStop*>(*route->getAllEdges().rbegin()));
						if(lineStop)
						{
							pm->insert(DATA_DESTINATION_CITY_NAME, lineStop->getPhysicalStop()->getConnectionPlace()->getCity()->getName());
							pm->insert(DATA_DESTINATION_STOP_NAME, lineStop->getPhysicalStop()->getConnectionPlace()->getName());
						}
					}
					pm->insert(DATA_RANK, rank);
					pm->insert(DATA_RANK_IS_ODD, rank % 2);

					if(_page.get())
					{
						_page->display(stream, request, *pm);
					}
					else
					{
						result.insert(DATA_ROUTES, pm);
					}
				}
				else // XML
				{
					// Ignore auto-generated routes
					if(route->getKey()==0 || route->getEdges().empty())
						continue;

					stream << "<direction id=\""<< route->getKey() <<
						"\" name=\""            << route->getName() <<
						"\" " << DATA_IS_MAIN << "=\"" << route->getMain() <<
						"\" directionText=\""   << (
							route->getDirection().empty() && route->getDirectionObj() ?
							route->getDirectionObj()->getDisplayedText() :
							route->getDirection()
						) << "\" >";

					const StopArea & origin(
						*route->getOrigin()->getConnectionPlace()
					);
					stream << "<origin id=\""  << origin.getKey() <<
						"\" name=\""           << origin.getName() <<
						"\" cityId=\""         << origin.getCity()->getKey() <<
						"\" cityName=\""       << origin.getCity()->getName() <<
						"\" directionAlias=\"" << origin.getName26() <<
						"\" />";

					const StopArea & destination(
						*route->getDestination()->getConnectionPlace()
					);
					stream << "<destination id=\"" << destination.getKey() <<
						"\" name=\""           << destination.getName() <<
						"\" cityId=\""         << destination.getCity()->getKey() <<
						"\" cityName=\""       << destination.getCity()->getName() <<
						"\" directionAlias=\"" << destination.getName26() <<
						"\" />";

					stream <<"</direction>";
				}
			}

			if(isOutputXML)
			{
				// XML footer
				stream << "</directions>";
			}
			else if(!_page.get())
			{
				outputParametersMap(
					result,
					stream,
					DATA_ROUTES,
					"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/35_pt/PTRoutesListFunction.xsd"
				);
			}

			return result;
		}



		bool PTRoutesListFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string PTRoutesListFunction::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : getOutputMimeTypeFromOutputFormat(MimeTypes::XML);
		}
	}
}
