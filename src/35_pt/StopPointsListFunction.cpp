
/** StopPointsListFunction class implementation.
	@file StopPointsListFunction.cpp
	@author Hugues Romain
	@date 2010

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
#include "ServicePointer.h"
#include "StopPointsListFunction.hpp"
#include "StopArea.hpp"
#include "StopPoint.hpp"
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

		const string StopPointsListFunction::DATA_NAME("name");
		const string StopPointsListFunction::DATA_STOPAREA_NAME("stopAreaName");

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
			CoordinatesSystem::SRID srid(
				map.getDefault<CoordinatesSystem::SRID>(PARAMETER_SRID, CoordinatesSystem::GetInstanceCoordinatesSystem().getSRID())
			);
			_coordinatesSystem = &CoordinatesSystem::GetCoordinatesSystem(srid);

			try
			{
				_stopArea = Env::GetOfficialEnv().getRegistry<StopArea>().get(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch (ObjectNotFoundException<StopArea>&)
			{
				throw RequestException("No such stop area");
			}
			_commercialLineID = map.getOptional<RegistryKeyType>(PARAMETER_LINE_ID);
			if(_commercialLineID)
			{
				// Date is usefull only if a line was given
				if(!map.getDefault<string>(PARAMETER_DATE).empty() && map.getDefault<string>(PARAMETER_DATE) != "A")
				{
					_date = time_from_string(map.get<string>(PARAMETER_DATE));
				}
			}

			if(map.getOptional<RegistryKeyType>(PARAMETER_PAGE_ID)) try
			{
				_page = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_PAGE_ID));
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such page");
			}

			string bbox(map.getDefault<string>(PARAMETER_BBOX));
			if(!bbox.empty())
			{
				CoordinatesSystem::SRID srid(
					map.getDefault<CoordinatesSystem::SRID>(PARAMETER_SRID, CoordinatesSystem::GetInstanceCoordinatesSystem().getSRID())
				);
				_coordinatesSystem = &CoordinatesSystem::GetCoordinatesSystem(srid);

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
					pt1->getY(),
					pt2->getX(),
					pt2->getY()
				);
			}
		}

		void StopPointsListFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			ptime startDateTime,endDateTime;

			ptime date = _date ? *_date : second_clock::local_time();

			//and startDateTime is begin of the day (a day begin at 3:00):
			startDateTime = date - date.time_of_day() + hours(3);
			//and endDateTime is end of the day (a day end at 27:00):
			endDateTime = date - date.time_of_day() + hours(27);

			// XML header
			if(!_page.get())
			{
				if(_commercialLineID)//destination of this line will be displayed
				{
					stream <<
						"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
						"<physicalStops xsi:noNamespaceSchemaLocation=\"http://synthese.rcsmobility.com/include/35_pt/StopPointsListFunction.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance"<<
						"\" " << DATA_STOPAREA_NAME << "=\"" << _stopArea->getName() <<
						"\" lineName=\""     << Env::GetOfficialEnv().getRegistry<CommercialLine>().get(*_commercialLineID)->getName() <<
						"\" lineShortName=\""<< Env::GetOfficialEnv().getRegistry<CommercialLine>().get(*_commercialLineID)->getShortName() <<
						"\" lineStyle=\""    << Env::GetOfficialEnv().getRegistry<CommercialLine>().get(*_commercialLineID)->getStyle() <<
						"\">";
				}
				else
				{
					stream <<
						"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
						"<physicalStops xsi:noNamespaceSchemaLocation=\"http://synthese.rcsmobility.com/include/35_pt/StopPointsListFunction.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance"<<
						"\" " << DATA_STOPAREA_NAME << "=\"" << _stopArea->getName() <<
						"\">";
				}
			}

			const StopArea::PhysicalStops& stops(_stopArea->getPhysicalStops());
			BOOST_FOREACH(const StopArea::PhysicalStops::value_type& it, stops)
			{
				if(_commercialLineID)//only physicalStop used by the commercial line will be displayed
				{
					typedef map<RegistryKeyType, const StopArea * > stopAreaMapType;
					stopAreaMapType stopAreaMap;

					BOOST_FOREACH(const Vertex::Edges::value_type& edge, it.second->getDepartureEdges())
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
							if(it.second->getKey() != servicePointer.getRealTimeDepartureVertex()->getKey())
								continue;

							//only physical stops the commercial line given will be displayed
							const JourneyPattern* journeyPattern = static_cast<const JourneyPattern*>(servicePointer.getService()->getPath());
							const CommercialLine * commercialLine(journeyPattern->getCommercialLine());
							if(commercialLine->getKey()!=_commercialLineID)
								continue;

							const StopArea * destination = journeyPattern->getDestination()->getConnectionPlace();

							stopAreaMap[destination->getKey()] = destination;
						}
					}

					//Generate output only if commercial line use stoppoint
					if(stopAreaMap.empty())
						continue;

					if(_page.get())
					{
						_display(stream, request, *it.second);
					}
					else
					{
						stream << "<physicalStop id=\"" << it.second->getKey() <<
							"\" operatorCode=\""    << it.second->getCodeBySources() <<
							"\">";

						BOOST_FOREACH(const stopAreaMapType::value_type& destination, stopAreaMap)
						{
							stream << "<destination id=\"" << destination.second->getKey() <<
								"\" name=\""    << destination.second->getName() <<
								"\" cityName=\""<< destination.second->getCity()->getName() <<
								"\" />";
						}

						stream << "</physicalStop>";
					}
				}
				else//all physical stop will be displayed, without lines destination information
				{
					if(_page.get())
					{
						_display(stream, request, *it.second);
					}
					else
					{
						stream << "<physicalStop id=\"" << it.second->getKey() <<
							"\" operatorCode=\""    << it.second->getCodeBySources() <<
							"\" />";
					}
				}
			}

			// XML footer
			stream << "</physicalStops>";
		}



		bool StopPointsListFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string StopPointsListFunction::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : "text/xml";
		}



		void StopPointsListFunction::_display(
			std::ostream& stream,
			const server::Request& request,
			const StopPoint& stop
		) const {
			ParametersMap pm;
			pm.insert(Request::PARAMETER_OBJECT_ID, stop.getKey());
			pm.insert(DATA_NAME, stop.getName());
			pm.insert(DATA_STOPAREA_NAME, stop.getConnectionPlace()->getFullName());
			_page->display(stream, request, pm);
		}
	}
}
