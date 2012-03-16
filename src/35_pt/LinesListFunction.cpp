////////////////////////////////////////////////////////////////////////////////
/// LinesListFunction class implementation.
///	@file LinesListFunction.cpp
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
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "RequestException.h"
#include "LinesListFunction.h"
#include "TransportNetwork.h"
#include "CommercialLine.h"
#include "Webpage.h"
#include "RollingStock.hpp"
#include "RollingStockFilter.h"
#include "SortableLineNumber.hpp"
#include "Path.h"
#include "JourneyPattern.hpp"
#include "CommercialLineTableSync.h"
#include "ImportableTableSync.hpp"
#include "PTUseRule.h"
#include "Vertex.h"
#include "StopArea.hpp"
#include "GetMessagesFunction.hpp"
#include "MimeTypes.hpp"

#include <geos/geom/LineString.h>
#include <geos/geom/GeometryCollection.h>
#include <geos/io/WKTWriter.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace geos::io;
using namespace boost::algorithm;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace pt;
	using namespace security;
	using namespace cms;
	using namespace graph;
	using namespace impex;
	using namespace messages;
	using namespace pt_website;

	template<> const string util::FactorableTemplate<server::Function,pt::LinesListFunction>::FACTORY_KEY(
		"LinesListFunction2"
	);

	namespace pt
	{
		const string LinesListFunction::PARAMETER_NETWORK_ID("ni");
		const string LinesListFunction::PARAMETER_PAGE_ID("pi");
		const string LinesListFunction::PARAMETER_SRID("srid");
		const string LinesListFunction::PARAMETER_OUTPUT_STOPS("os");
		const string LinesListFunction::PARAMETER_OUTPUT_GEOMETRY("og");
		const string LinesListFunction::PARAMETER_IGNORE_TIMETABLE_EXCLUDED_LINES = "ittd";
		const string LinesListFunction::PARAMETER_IGNORE_JOURNEY_PLANNER_EXCLUDED_LINES = "ijpd";
		const string LinesListFunction::PARAMETER_IGNORE_DEPARTURES_BOARD_EXCLUDED_LINES = "idbd";
		const string LinesListFunction::PARAMETER_LETTERS_BEFORE_NUMBERS = "letters_before_numbers";
		const string LinesListFunction::PARAMETER_ROLLING_STOCK_FILTER_ID = "tm";
		const string LinesListFunction::PARAMETER_SORT_BY_TRANSPORT_MODE = "sort_by_transport_mode";
		const string LinesListFunction::PARAMETER_OUTPUT_MESSAGES = "output_messages";
		const string LinesListFunction::PARAMETER_RIGHT_CLASS = "right_class";
		const string LinesListFunction::PARAMETER_RIGHT_LEVEL = "right_level";

		const string LinesListFunction::FORMAT_WKT("wkt");

		const string LinesListFunction::DATA_LINE("line");
		const string LinesListFunction::DATA_LINES("lines");
		const string LinesListFunction::DATA_STOP_AREAS("stopAreas");
		const string LinesListFunction::DATA_STOP_AREA("stopArea");
		const string LinesListFunction::DATA_TRANSPORT_MODE("transportMode");
		const string LinesListFunction::DATA_GEOMETRY("geometry");
		const string LinesListFunction::DATA_WKT("wkt");
		const string LinesListFunction::DATA_EDGE("edge");
		const string LinesListFunction::DATA_POINT("point");
		const string LinesListFunction::DATA_RANK("rank");
		const string LinesListFunction::DATA_X("x");
		const string LinesListFunction::DATA_Y("y");



		ParametersMap LinesListFunction::_getParametersMap() const
		{
			ParametersMap result;

			// Network
			if (_network.get() != NULL)
			{
				result.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}

			// SRID
			if(_coordinatesSystem)
			{
				result.insert(PARAMETER_SRID, static_cast<int>(_coordinatesSystem->getSRID()));
			}

			// Letters before numbers
			result.insert(PARAMETER_LETTERS_BEFORE_NUMBERS, _lettersBeforeNumbers);

			// Output geometry
			result.insert(PARAMETER_OUTPUT_GEOMETRY, _outputGeometry);

			// Output stops
			result.insert(PARAMETER_OUTPUT_STOPS, _outputStops);

			// Output messages
			result.insert(PARAMETER_OUTPUT_MESSAGES, _outputMessages);

			// Page or output format
			if(_page.get())
			{
				result.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			else
			{
				result.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}

			// Ignore excluded lines
			result.insert(PARAMETER_IGNORE_DEPARTURES_BOARD_EXCLUDED_LINES, _ignoreDeparturesBoardExcludedLines);
			result.insert(PARAMETER_IGNORE_JOURNEY_PLANNER_EXCLUDED_LINES, _ignoreJourneyPlannerExcludedLines);
			result.insert(PARAMETER_IGNORE_TIMETABLE_EXCLUDED_LINES, _ignoreTimetableExcludedLines);

			// Rolling stock filter
			if(_rollingStockFilter.get() != NULL)
			{
				result.insert(PARAMETER_ROLLING_STOCK_FILTER_ID, _rollingStockFilter->getKey());
			}

			// Transport mode sorting
			if(_sortByTransportMode.size() > 1)
			{
				stringstream s;
				bool first(true);
				BOOST_FOREACH(const shared_ptr<const RollingStock>& tm, _sortByTransportMode)
				{
					if(!tm.get())
					{
						continue;
					}
					if(first)
					{
						first = false;
					}
					else
					{
						s << ",";
					}
					s << tm->getKey();
				}
				result.insert(PARAMETER_SORT_BY_TRANSPORT_MODE, s.str());
			}

			// Right class
			if(!_rightClass.empty())
			{
				result.insert(PARAMETER_RIGHT_CLASS, _rightClass);
			}

			// Right level
			if(_rightLevel)
			{
				result.insert(PARAMETER_RIGHT_LEVEL, static_cast<int>(*_rightLevel));
			}

			return result;
		}



		void LinesListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			{ // Object(s) selection
				RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_NETWORK_ID));
				if(id) try
				{
					_network = Env::GetOfficialEnv().get<TransportNetwork>(id);
				}
				catch(ObjectNotFoundException<TransportNetwork>&)
				{
					throw RequestException("Transport network " + lexical_cast<string>(id) + " not found");
				}
				else
				{
					id = map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID);
					try
					{
						_line = Env::GetOfficialEnv().get<CommercialLine>(id);
					}
					catch(ObjectNotFoundException<CommercialLine>&)
					{
						throw RequestException("Line " + lexical_cast<string>(id) + " not found");
					}
				}
			}

			// Letters before numbers
			_lettersBeforeNumbers = map.isTrue(PARAMETER_LETTERS_BEFORE_NUMBERS);

			// Right class
			_rightClass = map.getDefault<string>(PARAMETER_RIGHT_CLASS);
			if(	!_rightClass.empty() &&
				!Factory<Right>::contains(_rightClass)
			){
				throw RequestException("No such right class");
			}

			// Right level
			if(map.getOptional<int>(PARAMETER_RIGHT_LEVEL))
			{
				_rightLevel = static_cast<RightLevel>(map.get<int>(PARAMETER_RIGHT_LEVEL));
			}

			// Output messages
			_outputMessages = map.isTrue(PARAMETER_OUTPUT_MESSAGES);

			// Output
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
				setOutputFormatFromMap(map, MimeTypes::CSV);
			}
			_outputGeometry = map.getDefault<string>(PARAMETER_OUTPUT_GEOMETRY);
			_outputStops = map.isTrue(PARAMETER_OUTPUT_STOPS);
			CoordinatesSystem::SRID srid(
				map.getDefault<CoordinatesSystem::SRID>(PARAMETER_SRID, CoordinatesSystem::GetInstanceCoordinatesSystem().getSRID())
			);
			_coordinatesSystem = &CoordinatesSystem::GetCoordinatesSystem(srid);

			// Parameters
			_ignoreDeparturesBoardExcludedLines = map.isTrue(PARAMETER_IGNORE_DEPARTURES_BOARD_EXCLUDED_LINES);
			_ignoreJourneyPlannerExcludedLines = map.isTrue(PARAMETER_IGNORE_JOURNEY_PLANNER_EXCLUDED_LINES);
			_ignoreTimetableExcludedLines = map.isTrue(PARAMETER_IGNORE_TIMETABLE_EXCLUDED_LINES);
			
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

			// Transport mode sorting
			_sortByTransportMode.clear();
			string strSBTM(map.getDefault<string>(PARAMETER_SORT_BY_TRANSPORT_MODE));
			if(!strSBTM.empty())
			{
				vector<string> tms;
				split(tms, strSBTM, is_any_of(","));
				BOOST_FOREACH(const string& tmstr, tms)
				{
					try
					{
						shared_ptr<const RollingStock> tm(
							Env::GetOfficialEnv().get<RollingStock>(lexical_cast<RegistryKeyType>(tmstr))
						);
						_sortByTransportMode.push_back(tm);
					}
					catch (ObjectNotFoundException<RollingStock>&)
					{
						throw RequestException("No rolling stock");
					}
				}
			}
			_sortByTransportMode.push_back(shared_ptr<RollingStock>()); // NULL pointer at end
		}



		util::ParametersMap LinesListFunction::run(
			std::ostream& stream,
			const Request& request
		) const	{
			// Sorting is made on numerical order
			//   (except for line number which doesn't begin by a number)
			//   then alphabetic order.
			//
			// So, linesMap is like that:
			//
			// [1]  -> ligne 1 : XXX - XXX
			// [2]  -> ligne 2 : XXX - XXX
			// [2S] -> ligne 2S: XXX - XXX
			// [A]  -> ligne A : XXX - XXX
			// [A1] -> ligne A1: XXX - XXX
			//

			typedef std::map<
				const RollingStock*,
				map<SortableLineNumber, shared_ptr<const CommercialLine> >
			> LinesMapType;
			LinesMapType linesMap;

			// Get CommercialLine Global Registry
			if(_network.get())
			{
				CommercialLineTableSync::SearchResult lines(
					CommercialLineTableSync::Search(
						Env::GetOfficialEnv(),
						_network->getKey(),
						optional<string>(),
						optional<string>(),
						0,
						optional<size_t>(),
						true,
						false,
						true,
						UP_LINKS_LOAD_LEVEL,
						((_rightClass.empty() && _rightLevel) || !request.getUser()) ?
							optional<const RightsOfSameClassMap&>() :
							request.getUser()->getProfile()->getRights(_rightClass),
						_rightLevel ? *_rightLevel : FORBIDDEN
				)	);
				set<CommercialLine*> alreadyShownLines;
				BOOST_FOREACH(const shared_ptr<const RollingStock>& tm, _sortByTransportMode)
				{
					BOOST_FOREACH(const shared_ptr<CommercialLine>& line, lines)
					{
						// Avoid to return a line twice
						if(alreadyShownLines.find(line.get()) != alreadyShownLines.end())
						{
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
								if(line->usesTransportMode(*rollingStock))
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

						// Use rule tests
						const UseRule& useRule(line->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET));
						if(	dynamic_cast<const PTUseRule*>(&useRule) &&
							(	_ignoreJourneyPlannerExcludedLines && static_cast<const PTUseRule&>(useRule).getForbiddenInJourneyPlanning() ||
								_ignoreTimetableExcludedLines && static_cast<const PTUseRule&>(useRule).getForbiddenInTimetables() ||
								_ignoreDeparturesBoardExcludedLines && static_cast<const PTUseRule&>(useRule).getForbiddenInDepartureBoards()
						)	){
							continue;
						}

						// Transport mode check
						if(!tm.get() || line->usesTransportMode(*tm))
						{
							// Insert respecting order described up there
							linesMap[tm.get()][SortableLineNumber(line->getShortName(), _lettersBeforeNumbers)] = const_pointer_cast<const CommercialLine>(line);
							alreadyShownLines.insert(line.get());
						}
				}	}
			}
			else if(_line.get())
			{
				linesMap[NULL][SortableLineNumber(_line->getShortName(), _lettersBeforeNumbers)] = _line;
			}

			// Populating the parameters map
			ParametersMap pm;
			BOOST_FOREACH(const shared_ptr<const RollingStock>& tm, _sortByTransportMode)
			{
				BOOST_FOREACH(const LinesMapType::mapped_type::value_type& it, linesMap[tm.get()])
				{
					shared_ptr<const CommercialLine> line = it.second;
					shared_ptr<ParametersMap> linePM(new ParametersMap);
					line->toParametersMap(*linePM);

					// Rolling stock
					set<RollingStock *> rollingStocks;
					BOOST_FOREACH(Path* path, line->getPaths())
					{
						if(!dynamic_cast<const JourneyPattern*>(path))
						{
							continue;
						}

						if(	!static_cast<const JourneyPattern*>(path)->getRollingStock()
						){
							continue;
						}
						rollingStocks.insert(
							static_cast<const JourneyPattern*>(path)->getRollingStock()
						);
					}
					BOOST_FOREACH(RollingStock * rs, rollingStocks)
					{
						shared_ptr<ParametersMap> rsPM(new ParametersMap);
						rs->toParametersMap(*rsPM);
						linePM->insert(DATA_TRANSPORT_MODE, rsPM);
					}

					if(_outputStops)
					{
						set<const StopArea*> stopAreas;
						BOOST_FOREACH(Path* path, line->getPaths())
						{
							if(!dynamic_cast<const JourneyPattern*>(path))
							{
								continue;
							}

							BOOST_FOREACH(Edge* edge, path->getEdges())
							{
								if(!edge->getFromVertex())
								{
									break;
								}
								const StopArea* stopArea(
									dynamic_cast<const StopArea*>(edge->getFromVertex()->getHub())
								);
								if(stopArea)
								{
									stopAreas.insert(stopArea);
								}
						}	}
						shared_ptr<ParametersMap> stopAreasPM(new ParametersMap);
						BOOST_FOREACH(const StopArea* stopArea, stopAreas)
						{
							shared_ptr<ParametersMap> stopAreaPM(new ParametersMap);
							stopArea->toParametersMap(*stopAreaPM, _coordinatesSystem);
							stopAreasPM->insert(DATA_STOP_AREA, stopAreaPM);
						}
						linePM->insert(DATA_STOP_AREAS, stopAreasPM);
					}
					if(!_outputGeometry.empty())
					{
						typedef map<pair<Vertex*, Vertex*>, shared_ptr<Geometry> > VertexPairs;
						VertexPairs geometries;
						BOOST_FOREACH(Path* path, line->getPaths())
						{
							if(!dynamic_cast<const JourneyPattern*>(path))
							{
								continue;
							}

							BOOST_FOREACH(Edge* edge, path->getEdges())
							{
								if(!edge->getNext())
								{
									break;
								}
								VertexPairs::key_type od(make_pair(edge->getFromVertex(), edge->getNext()->getFromVertex()));
								if(geometries.find(od) == geometries.end())
								{
									shared_ptr<LineString> lineGeometry = edge->getRealGeometry();
									if (lineGeometry)
										geometries.insert(make_pair(od, lineGeometry));
								}
							}
						}

						shared_ptr<ParametersMap> geometryPM(new ParametersMap);
						if(_outputGeometry == FORMAT_WKT)
						{
							vector<shared_ptr<Geometry> > vec;
							vector<Geometry*> vecd;
							BOOST_FOREACH(const VertexPairs::value_type& it, geometries)
							{
								shared_ptr<Geometry> prGeom(
									_coordinatesSystem->convertGeometry(*it.second)
								);
								vec.push_back(prGeom);
								vecd.push_back(prGeom.get());
							}
							shared_ptr<GeometryCollection> mls(
								_coordinatesSystem->getGeometryFactory().createGeometryCollection(vecd)
							);
							geometryPM->insert(DATA_WKT, WKTWriter().write(mls.get()));
						}
						else
						{
							BOOST_FOREACH(const VertexPairs::value_type& it, geometries)
							{
								shared_ptr<ParametersMap> edgePM(new ParametersMap);
								shared_ptr<Geometry> prGeom(
									_coordinatesSystem->convertGeometry(*it.second)
								);
								for(size_t i(0); i<prGeom->getNumPoints(); ++i)
								{
									const Coordinate& pt(prGeom->getCoordinates()->getAt(i));
									shared_ptr<ParametersMap> pointPM(new ParametersMap);
									pointPM->insert(DATA_X, pt.x);
									pointPM->insert(DATA_Y, pt.y);
									edgePM->insert(DATA_POINT, pointPM);
								}
								geometryPM->insert(DATA_EDGE, edgePM);
							}
						}
						linePM->insert(DATA_GEOMETRY, geometryPM);
					}

					// Messages output
					if(_outputMessages)
					{
						GetMessagesFunction::GetMessages(
							*linePM,
							line->getKey(),
							optional<size_t>(),
							true,
							true,
							second_clock::local_time(),
							second_clock::local_time()
						);
					}

					pm.insert(DATA_LINE, linePM);
			}	}

			if(_page.get()) // CMS output
			{
				size_t rank(0);
				BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmLine, pm.getSubMaps(DATA_LINE))
				{
					// Template parameters
					pmLine->merge(getTemplateParameters());

					// Rank
					pmLine->insert(DATA_RANK, rank++);

					//Add Rolling stock information with a prefix
					if(pmLine->hasSubMaps(DATA_TRANSPORT_MODE))
					{
						pmLine->merge(*pmLine->getSubMaps(DATA_TRANSPORT_MODE)[0].get(),string("rs_"));
					}

					// Display
					_page->display(stream, request, *pmLine);
				}
			}
			else if(_outputFormat == MimeTypes::CSV)
			{
				// Hand made formatting for CSV.
				BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmLine, pm.getSubMaps(DATA_LINE))
				{
					stream << pmLine->get<string>(CommercialLine::DATA_LINE_ID) << ";" << pmLine->get<string>(CommercialLine::DATA_LINE_SHORT_NAME) << "\n";
				}
			}
			else
			{
				outputParametersMap(
					pm,
					stream,
					DATA_LINES,
					"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/35_pt/LinesListFunction.xsd"
				);
			}
			return pm;
		}



		bool LinesListFunction::isAuthorized(
			const Session*
		) const {
			return true;
		}



		std::string LinesListFunction::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : getOutputMimeTypeFromOutputFormat();
		}



		LinesListFunction::LinesListFunction():
			_outputStops(false),
			_ignoreTimetableExcludedLines(false),
			_ignoreJourneyPlannerExcludedLines(false),
			_ignoreDeparturesBoardExcludedLines(false),
			_outputMessages(false),
			_lettersBeforeNumbers(true)
		{}
}	}
