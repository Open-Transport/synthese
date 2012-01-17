////////////////////////////////////////////////////////////////////////////////
/// StopAreasListFunction class implementation.
///	@file StopAreasListFunction.cpp
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
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA	02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "RequestException.h"
#include "StopAreasListFunction.hpp"
#include "TransportNetwork.h"
#include "CommercialLine.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "RollingStock.hpp"
#include "Request.h"
#include "JourneyPattern.hpp"
#include "Path.h"
#include "Edge.h"
#include "City.h"
#include "Webpage.h"
#include "CommercialLineTableSync.h"
#include "CityTableSync.h"

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
	using namespace geography;

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
		const string StopAreasListFunction::PARAMETER_OUTPUT_FORMAT = "output_format";
		
		const string StopAreasListFunction::DATA_LINE = "line";
		const string StopAreasListFunction::DATA_LINES = "lines";
		const string StopAreasListFunction::DATA_STOP_RANK("stop_rank");
		const string StopAreasListFunction::DATA_STOP_AREA("stopArea");
		const string StopAreasListFunction::DATA_STOP_AREAS("stopAreas");

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
			if(!_outputFormat.empty())
			{
				result.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}
			if(_city.get())
			{
				result.insert(Request::PARAMETER_OBJECT_ID, _city->getKey());
			}
			return result;
		}



		void StopAreasListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			RegistryKeyType roid(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0));
			if(roid)
			{
				// Line filter
				if(decodeTableId(roid) == CommercialLineTableSync::TABLE.ID) try
				{
					_commercialLine = Env::GetOfficialEnv().getRegistry<CommercialLine>().get(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
				}
				catch (ObjectNotFoundException<CommercialLine>&)
				{
					throw RequestException("No such Commercial Line");
				}
				// City filter
				else if(decodeTableId(roid) == CityTableSync::TABLE.ID) try
				{
					_city = Env::GetOfficialEnv().get<City>(roid);
				}
				catch (ObjectNotFoundException<City>&)
				{
					throw RequestException("No such city");
				}
			}

			// Output lines
			_outputLines = map.isTrue(PARAMETER_OUTPUT_LINES);

			// SRID
			CoordinatesSystem::SRID srid(
				map.getDefault<CoordinatesSystem::SRID>(PARAMETER_SRID, CoordinatesSystem::GetInstanceCoordinatesSystem().getSRID())
			);
			_coordinatesSystem = &CoordinatesSystem::GetCoordinatesSystem(srid);

			// BBox filter
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

			// Output format
			_outputFormat = map.getDefault<string>(PARAMETER_OUTPUT_FORMAT);
		}



		StopAreasListFunction::StopAreasListFunction():
			_coordinatesSystem(NULL),
			_outputLines(true)
		{}



		struct StopAreasComparator
		{
		bool operator()(const StopArea* s1, const StopArea* s2) const
			{
				if(s1->getName() == s2->getName())
				{
					return s1 < s2;
				}
				return s1->getName() < s2->getName();
			}
		};



		util::ParametersMap StopAreasListFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			typedef set<const StopArea *, StopAreasComparator> StopSet;
			StopSet stopSet;

			// Populate stopSet
			if(_commercialLine.get())
			{
				BOOST_FOREACH(const Path* path, _commercialLine->getPaths())
				{
					const JourneyPattern* journey = dynamic_cast<const JourneyPattern*>(path);
					BOOST_FOREACH(const Edge* edge,journey->getEdges())
					{
						const StopPoint * stopPoint(static_cast<const StopPoint *>(edge->getFromVertex()));
						const StopArea * connPlace(stopPoint->getConnectionPlace());

						stopSet.insert(connPlace);
					}
				}
			}
			else if(_city.get())
			{
				BOOST_FOREACH(const City::PlacesMatcher::Map::value_type& itStopArea, _city->getLexicalMatcher(FactorableTemplate<NamedPlace,StopArea>::FACTORY_KEY).entries())
				{
					stopSet.insert(dynamic_cast<StopArea*>(itStopArea.second.get()));
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

					stopSet.insert(stopArea.second.get());
				}
			}

			// Stops loop
			ParametersMap pm;
			BOOST_FOREACH(StopSet::value_type it, stopSet)
			{
				shared_ptr<ParametersMap> stopPm(new ParametersMap);

				it->toParametersMap(*stopPm, _coordinatesSystem);

				// Lines calling at the stop
				if(_outputLines)
				{
					BOOST_FOREACH(const StopArea::Lines::value_type& itLine, it->getLines(false))
					{
						// For CMS output
						shared_ptr<ParametersMap> pmLine(new ParametersMap);

						itLine->toParametersMap(*pmLine);
						
						// Rolling stock
						set<RollingStock *> rollingStocks;
						BOOST_FOREACH(Path* path, itLine->getPaths())
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
							shared_ptr<ParametersMap> transportModePM(new ParametersMap);
							rs->toParametersMap(*transportModePM);
							pmLine->insert("transportMode", transportModePM);
						}	

						stopPm->insert(DATA_LINE, pmLine);
					}
				}

				pm.insert(DATA_STOP_AREA, stopPm);
			}

			// Informations about the request
			if(_commercialLine.get())
			{
				pm.insert("lineName", _commercialLine->getName());
				pm.insert("lineImage", _commercialLine->getImage());
				pm.insert("lineShortName", _commercialLine->getShortName());
				pm.insert("lineStyle", _commercialLine->getStyle());
			}


			// JSON output
			if(_outputFormat == "json")
			{
				pm.outputJSON(stream, DATA_STOP_AREAS);
			}
			else if(_stopPage.get()) // CMS output
			{
				size_t stopRank(0);

				BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmStop, pm.getSubMaps(DATA_STOP_AREA))
				{
					// CMS template parameters
					pmStop->merge(getTemplateParameters());

					// Rank
					pmStop->insert(DATA_STOP_RANK, stopRank++);

					// Lines
					if(_linePage.get() && _outputLines && pmStop->hasSubMaps(DATA_LINE))
					{
						stringstream lineStream;
						BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmLine, pmStop->getSubMaps(DATA_LINE))
						{
							// CMS template parameters
							pmLine->merge(getTemplateParameters());

							// Display
							_linePage->display(lineStream, request, *pmLine);
						}
						pmStop->insert(DATA_LINES, lineStream.str());
					}

					// Display
					_stopPage->display(stream, request, *pmStop);
				}
			}
			else // XML output
			{
				pm.outputXML(stream, DATA_STOP_AREAS, true, "http://synthese.rcsmobility.com/include/35_pt/StopAreasListFunction.xsd");
			}

			return pm;
		}



		bool StopAreasListFunction::isAuthorized(
			const Session*
		) const {
			return true;
		}

		std::string StopAreasListFunction::getOutputMimeType() const
		{
			if(_outputFormat == "json")
			{
				return "application/json";
			}
			else if(_stopPage.get())
			{
				return _stopPage->getMimeType();
			}
			return "text/xml";
		}
	}
}
