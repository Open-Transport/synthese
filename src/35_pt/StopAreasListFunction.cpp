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

#include "StopAreasListFunction.hpp"

#include "Destination.hpp"
#include "RequestException.h"
#include "StopAreaTableSync.hpp"
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
#include "MimeTypes.hpp"

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
	using namespace vehicle;
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
		const string StopAreasListFunction::PARAMETER_TERMINUS_ID = "terminus_id";
		const string StopAreasListFunction::PARAMETER_OUTPUT_STOPS = "output_stops";
		const string StopAreasListFunction::PARAMETER_OUTPUT_LINES_IN_STOPS = "output_lines_in_stops";
		const string StopAreasListFunction::PARAMETER_GROUP_BY_CITIES = "group_by_cities";
		const string StopAreasListFunction::PARAMETER_STOPS_DIRECTIONS = "stops_directions";


		const string StopAreasListFunction::TAG_DIRECTION = "direction";
		const string StopAreasListFunction::TAG_CITY = "city";
		const string StopAreasListFunction::TAG_STOP = "stop";
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
			if(_stopAreaFilter.get())
			{
				result.insert(Request::PARAMETER_OBJECT_ID, _stopAreaFilter->getKey());
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
			else
			{
				result.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}
			if(_linePage.get())
			{
				result.insert(PARAMETER_LINE_PAGE_ID, _linePage->getKey());
			}
			if(_city.get())
			{
				result.insert(Request::PARAMETER_OBJECT_ID, _city->getKey());
			}
			// terminusId
			if(_terminusId)
			{
				result.insert(PARAMETER_TERMINUS_ID, *_terminusId);
			}

			// Output stops ?
			if(_outputStops)
			{
				result.insert(PARAMETER_OUTPUT_STOPS, _outputStops);
			}

			// Group by cities ?
			if(_groupByCities)
			{
				result.insert(PARAMETER_GROUP_BY_CITIES, _groupByCities);
			}

			if(_outputLinesInStops)
			{
				result.insert(PARAMETER_OUTPUT_LINES_IN_STOPS, _outputLinesInStops);
			}
			return result;
		}



		void StopAreasListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			RegistryKeyType roid(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0));
			if(roid)
			{
				// Line filter
				RegistryTableType tableId(decodeTableId(roid));
				if(tableId == CommercialLineTableSync::TABLE.ID) try
				{
					_commercialLine = Env::GetOfficialEnv().getRegistry<CommercialLine>().get(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
				}
				catch (ObjectNotFoundException<CommercialLine>&)
				{
					throw RequestException("No such Commercial Line");
				}
				// City filter
				else if(tableId == CityTableSync::TABLE.ID) try
				{
					_city = Env::GetOfficialEnv().get<City>(roid);
				}
				catch (ObjectNotFoundException<City>&)
				{
					throw RequestException("No such city");
				}
				// Stop area filter
				else if(tableId == StopAreaTableSync::TABLE.ID) try
				{
					_stopAreaFilter = Env::GetOfficialEnv().get<StopArea>(roid);
				}
				catch (ObjectNotFoundException<StopArea>&)
				{
					throw RequestException("No such stop area");
				}
			}

			// Output lines
			_outputLines = map.isTrue(PARAMETER_OUTPUT_LINES);

			// Output stops ?
			_outputStops = map.isTrue(PARAMETER_OUTPUT_STOPS);

			// Output lines in stops ?
			_outputLinesInStops = map.isTrue(PARAMETER_OUTPUT_LINES_IN_STOPS);

			// Group by cities ?
			_groupByCities = map.isTrue(PARAMETER_GROUP_BY_CITIES);

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

				boost::shared_ptr<Point> pt1(
					_coordinatesSystem->createPoint(lexical_cast<double>(parsed_bbox[0]), lexical_cast<double>(parsed_bbox[1]))
				);
				boost::shared_ptr<Point> pt2(
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
			if(!_stopPage.get())
			{
				setOutputFormatFromMap(map, MimeTypes::XML);
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

			if(!map.getDefault<string>(PARAMETER_TERMINUS_ID).empty())
			{
				_terminusId = map.getOptional<RegistryKeyType>(PARAMETER_TERMINUS_ID);
			}

			// Stops directions
			_stopsDirections = map.getDefault<size_t>(PARAMETER_STOPS_DIRECTIONS, 0);
		}



		StopAreasListFunction::StopAreasListFunction():
			_coordinatesSystem(NULL),
			_outputLines(true),
			_outputStops(false),
			_outputLinesInStops(false),
			_groupByCities(false),
			_stopsDirections(0)
		{}



		struct StopAreasComparator
		{
			bool operator()(const StopArea* s1, const StopArea* s2) const
			{
				if(	s1->getCity() != s2->getCity()
				){
					if(s1->getCity()->getName() != s2->getCity()->getName())
					{
						return s1->getCity()->getName() < s2->getCity()->getName();
					}
					else
					{
						return s1->getCity() < s2->getCity();
					}
				}
				if(s1->getName() != s2->getName())
				{
					return s1->getName() < s2->getName();
				}
				return s1 < s2;
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
					
					//get stopArea terminus of line
					bool isAreaOfTerminus= false;
					if(_terminusId)
					{
						const StopArea * stopAreaTerminus = journey->getDestination()->getConnectionPlace();

						if (stopAreaTerminus->getKey() == *_terminusId)
						{
							isAreaOfTerminus = true;
						}
					}

					BOOST_FOREACH(const Edge* edge,journey->getAllEdges())
					{
						const StopPoint * stopPoint(static_cast<const StopPoint *>(edge->getFromVertex()));
						const StopArea * connPlace(stopPoint->getConnectionPlace());
						
						//is Terminus
						if (_terminusId && isAreaOfTerminus)
						{
							stopSet.insert(connPlace);
						}
						else if (!_terminusId)
						{
							stopSet.insert(connPlace);
						}
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
			else if(_stopAreaFilter.get())
			{
				stopSet.insert(_stopAreaFilter.get());
			}
			else
			{
				BOOST_FOREACH(const Registry<StopArea>::value_type& stopArea, Env::GetOfficialEnv().getRegistry<StopArea>())
				{
					if(	!stopArea.second->getPoint().get() ||
						(_bbox && !_bbox->contains(*stopArea.second->getPoint()->getCoordinate())) ||
						!stopArea.second->getCity()
					){
						continue;
					}

					//Terminus
					bool isAreaOfTerminus= false;
					if(_terminusId)
					{
						BOOST_FOREACH(const StopArea::Lines::value_type& itLine, stopArea.second->getLines(false))
						{
							BOOST_FOREACH(Path* path, itLine->getPaths())
							{
								const JourneyPattern* journeyPattern = dynamic_cast<const JourneyPattern*>(path);

								if(!journeyPattern)
									continue;

								//get stopArea terminus of line
								const StopArea * stopAreaTerminus = journeyPattern->getDestination()->getConnectionPlace();

								if (stopAreaTerminus->getKey() == *_terminusId)
								{
									isAreaOfTerminus = true;
								}
							}
						}
					}

					if (_terminusId && isAreaOfTerminus)
					{
						stopSet.insert(stopArea.second.get());
					}
					else if (!_terminusId)
					{
						stopSet.insert(stopArea.second.get());
					}
				}
			}

			// Stops loop
			ParametersMap pm;
			const City* lastCity(NULL);
			boost::shared_ptr<ParametersMap> cityPM;
			BOOST_FOREACH(StopSet::value_type it, stopSet)
			{
				// Group by cities
				if(_groupByCities && it->getCity() != lastCity)
				{
					cityPM.reset(new ParametersMap);
					it->getCity()->toParametersMap(*cityPM, _coordinatesSystem);
					pm.insert(TAG_CITY, cityPM);
					lastCity = it->getCity();
				}

				boost::shared_ptr<ParametersMap> stopPm(new ParametersMap);
				it->toParametersMap(*stopPm, _coordinatesSystem);

				// Output stops
				if(_outputStops)
				{
					BOOST_FOREACH(const StopArea::PhysicalStops::value_type& itStop, it->getPhysicalStops())
					{
						const StopPoint& stop(*itStop.second);

						boost::shared_ptr<ParametersMap> sPm(new ParametersMap);
						stop.toParametersMap(*sPm, false, *_coordinatesSystem);

						// Lines
						if(_outputLinesInStops)
						{
							StopPoint::LinesSet lines(stop.getCommercialLines());
							BOOST_FOREACH(const StopPoint::LinesSet::value_type& line, lines)
							{
								shared_ptr<ParametersMap> linePM(new ParametersMap);
								line->toParametersMap(*linePM, true);
								sPm->insert(DATA_LINE, linePM);
							}
						}

						// Stops directions
						if(_stopsDirections)
						{
							set<string> directions;
							size_t directionsNumber(0);
							BOOST_FOREACH(const Vertex::Edges::value_type& itEdge, stop.getDepartureEdges())
							{
								const Path* path(itEdge.first);
								const JourneyPattern* jp(dynamic_cast<const JourneyPattern*>(path));
								if(!jp || !jp->getMain())
								{
									continue;
								}
								string direction;
								if(jp->getDirectionObj())
								{
									direction = jp->getDirectionObj()->getDisplayedText();
								}
								else if(!jp->getDirection().empty())
								{
									direction = jp->getDirection();
								}
								else if(dynamic_cast<const NamedPlace*>(jp->getDestination()->getHub()))
								{
									direction = dynamic_cast<const NamedPlace*>(jp->getDestination()->getHub())->getFullName();
								}
								if(directions.find(direction) == directions.end())
								{
									directions.insert(direction);
									++directionsNumber;
									if(directionsNumber == _stopsDirections)
									{
										break;
									}
								}
							}
							BOOST_FOREACH(const string& direction, directions)
							{
								boost::shared_ptr<ParametersMap> directionPM(new ParametersMap);
								directionPM->insert(TAG_DIRECTION, direction);
								sPm->insert(TAG_DIRECTION, directionPM);
							}
						}
						stopPm->insert(TAG_STOP, sPm);
					}
				}

				// Lines calling at the stop
				if(_outputLines)
				{
					BOOST_FOREACH(const StopArea::Lines::value_type& itLine, it->getLines(false))
					{
						// For CMS output
						boost::shared_ptr<ParametersMap> pmLine(new ParametersMap);

						itLine->toParametersMap(*pmLine, true);

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
							boost::shared_ptr<ParametersMap> transportModePM(new ParametersMap);
							rs->toParametersMap(*transportModePM, true);
							pmLine->insert("transportMode", transportModePM);
						}

						stopPm->insert(DATA_LINE, pmLine);
					}
				}

				if(_groupByCities)
				{
					cityPM->insert(DATA_STOP_AREA, stopPm);
				}
				else
				{
					pm.insert(DATA_STOP_AREA, stopPm);
				}
			}

			// Informations about the request
			if(_commercialLine.get())
			{
				pm.insert("lineName", _commercialLine->getName());
				pm.insert("lineImage", _commercialLine->getImage());
				pm.insert("lineShortName", _commercialLine->getShortName());
				pm.insert("lineStyle", _commercialLine->getStyle());
			}


			if(_stopPage.get()) // CMS output
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
			else
			{
				outputParametersMap(
					pm,
					stream,
					DATA_STOP_AREAS,
					"http://synthese.rcsmobility.com/include/35_pt/StopAreasListFunction.xsd"
				);
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
			return _stopPage.get() ? _stopPage->getMimeType() : getOutputMimeTypeFromOutputFormat();
		}
	}
}
