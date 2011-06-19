////////////////////////////////////////////////////////////////////////////////
/// StopAreasListFunction class implementation.
///	@file StopAreasListFunction.cpp
///	@author Hugues Romain
///	@date 2008
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "RequestException.h"
#include "StopAreasListFunction.hpp"
#include "TransportNetwork.h"
#include "CommercialLine.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "LineMarkerInterfacePage.h"
#include "Request.h"
#include "JourneyPattern.hpp"
#include "Path.h"
#include "Edge.h"
#include "City.h"
#include "Webpage.h"
#include "PTObjectsCMSExporters.hpp"

#include <map>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace geos::geom;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace pt;
	using namespace security;
	using namespace graph;
	using namespace std;
	using namespace cms;

	template<> const string util::FactorableTemplate<server::Function,pt::StopAreasListFunction>::FACTORY_KEY(
		"StopAreasListFunction"
	);

	namespace pt
	{
		const string StopAreasListFunction::PARAMETER_BBOX = "bbox";
		const string StopAreasListFunction::PARAMETER_SRID = "srid";
		const string StopAreasListFunction::PARAMETER_OUTPUT_LINES = "ol";
		const string StopAreasListFunction::PARAMETER_STOP_PAGE_ID = "stop_page_id";
		const string StopAreasListFunction::PARAMETER_LINE_PAGE_ID = "line_page_id";

		const string StopAreasListFunction::DATA_LINES = "lines";

		ParametersMap StopAreasListFunction::_getParametersMap() const
		{
			ParametersMap result;
			if(_commercialLine.get())
			{
				result.insert(Request::PARAMETER_OBJECT_ID, _commercialLine->getKey());
			}
			if(_bbox)
			{
				stringstream s;
				s << _bbox->getMinX() << "," << _bbox->getMinY() << "," <<
					_bbox->getMaxX() << "," << _bbox->getMaxY();
				result.insert(PARAMETER_BBOX, s.str());
			}
			result.insert(PARAMETER_SRID, static_cast<int>(_coordinatesSystem->getSRID()));
			result.insert(PARAMETER_OUTPUT_LINES, _outputLines);
			if(_stopPage.get())
			{
				result.insert(PARAMETER_STOP_PAGE_ID, _stopPage->getKey());
			}
			if(_linePage.get())
			{
				result.insert(PARAMETER_LINE_PAGE_ID, _linePage->getKey());
			}
			return result;
		}

		void StopAreasListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			if(map.isDefined(Request::PARAMETER_OBJECT_ID)) try
			{
				_commercialLine = Env::GetOfficialEnv().getRegistry<CommercialLine>().get(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch (ObjectNotFoundException<CommercialLine>&)
			{
				throw RequestException("No such Commercial Line");
			}

			_outputLines = map.getDefault<bool>(PARAMETER_OUTPUT_LINES, true);
			CoordinatesSystem::SRID srid(
				map.getDefault<CoordinatesSystem::SRID>(PARAMETER_SRID, CoordinatesSystem::GetInstanceCoordinatesSystem().getSRID())
			);
			_coordinatesSystem = &CoordinatesSystem::GetCoordinatesSystem(srid);

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

			// Load of stop page
			if(map.getDefault<RegistryKeyType>(PARAMETER_STOP_PAGE_ID, 0)) try
			{
				_stopPage = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_STOP_PAGE_ID));
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such stop page");
			}

			// Load of line page
			if(map.getDefault<RegistryKeyType>(PARAMETER_LINE_PAGE_ID, 0)) try
			{
				_linePage = Env::GetOfficialEnv().get<Webpage>(map.get<RegistryKeyType>(PARAMETER_LINE_PAGE_ID));
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such line page");
			}
		}

		StopAreasListFunction::StopAreasListFunction():
			_coordinatesSystem(NULL),
			_outputLines(true)
		{
		}

		void StopAreasListFunction::run( std::ostream& stream, const Request& request ) const
		{
			//Key Is stop Area Name
			typedef map<string,const StopArea *> stopMap;
			stopMap stopAreaMap;

			//Populate stopAreaMap
			if(_commercialLine.get())
			{
				BOOST_FOREACH(const Path* path, _commercialLine->getPaths())
				{
					const JourneyPattern* journey = dynamic_cast<const JourneyPattern*>(path);
					BOOST_FOREACH(const Edge* edge,journey->getEdges())
					{
						const StopPoint * stopPoint(static_cast<const StopPoint *>(edge->getFromVertex()));
						const StopArea * connPlace(stopPoint->getConnectionPlace());

						stopAreaMap[connPlace->getName()] = connPlace;
					}
				}
			}
			else
			{
				BOOST_FOREACH(const Registry<StopArea>::value_type& stopArea, Env::GetOfficialEnv().getRegistry<StopArea>())
				{
					if(	!stopArea.second->getPoint().get() ||
						(_bbox && !_bbox->contains(*stopArea.second->getPoint()->getCoordinate()))
					){
						continue;
					}

					stopAreaMap[stopArea.second->getName()] = stopArea.second.get();
				}
			}

			if(!_stopPage.get())
			{
				// XML header
				stream <<
					"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
					"<stopAreas xsi:noNamespaceSchemaLocation=\"http://synthese.rcsmobility.com/include/35_pt/StopAreasListFunction.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance";
				if(_commercialLine.get())
				{
					stream <<
						"\" lineName=\""      << _commercialLine->getName() <<
						"\" lineImage=\""     << _commercialLine->getImage() <<
						"\" lineShortName=\"" << _commercialLine->getShortName() <<
						"\" lineStyle=\""     << _commercialLine->getStyle();
				}
				stream << "\">";
			}

			BOOST_FOREACH(stopMap::value_type& it, stopAreaMap)
			{
				// For CMS output
				ParametersMap pm(request.getFunction()->getSavedParameters());

				if(_stopPage.get())
				{
					PTObjectsCMSExporters::ExportStopArea(pm, *it.second, _coordinatesSystem);
				}
				else
				{
					stream << "<stopArea id=\""   << it.second->getKey() <<
						"\" name=\""          << it.second->getName() <<
						"\" cityId=\""        << it.second->getCity()->getKey() <<
						"\" cityName=\""      << it.second->getCity()->getName() <<
						"\" directionAlias=\""<< it.second->getName26();
					if(it.second->getPoint().get())
					{
						shared_ptr<Point> pts(it.second->getPoint());
						pts = _coordinatesSystem->convertPoint(*pts);
						stream << "\" x=\"" << pts->getX() <<
							"\" y=\"" << pts->getY();
					}
					stream << "\">";
				}
				if(_outputLines)
				{
					// For CMS output
					ParametersMap pmLine(request.getFunction()->getSavedParameters());

					BOOST_FOREACH(const StopArea::Lines::value_type& itLine, it.second->getLines(false))
					{
						if(_linePage.get())
						{
							PTObjectsCMSExporters::ExportLine(pmLine, *itLine);
						}
						else
						{
							stream << "<line id=\"" << itLine->getKey() <<
								"\" lineName=\""      << itLine->getName() <<
								"\" lineImage=\""     << itLine->getImage() <<
								"\" lineShortName=\"" << itLine->getShortName() <<
								"\" lineStyle=\""     << itLine->getStyle() <<
								"\" />";
						}
					}

					if(_linePage.get())
					{
						stringstream lineStream;
						_linePage->display(lineStream, request, pmLine);
						pm.insert(DATA_LINES, lineStream.str());
					}
				}

				if(_stopPage.get())
				{
					_stopPage->display(stream, request, pm);
				}
				else
				{
					stream << "</stopArea>";
				}
			}

			if(!_stopPage.get())
			{
				// XML footer
				stream << "</stopAreas>";
			}
		}



		bool StopAreasListFunction::isAuthorized(
			const Session*
		) const {
			return true;
		}

		std::string StopAreasListFunction::getOutputMimeType() const
		{
			return "text/xml";
		}
	}
}
