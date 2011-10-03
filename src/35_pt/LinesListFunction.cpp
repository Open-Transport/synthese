////////////////////////////////////////////////////////////////////////////////
/// LinesListFunction class implementation.
///	@file LinesListFunction.cpp
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
#include "LinesListFunction.h"
#include "TransportNetwork.h"
#include "CommercialLine.h"
#include "LineMarkerInterfacePage.h"
#include "Webpage.h"
#include "RollingStock.hpp"
#include "Path.h"
#include "JourneyPattern.hpp"
#include "CommercialLineTableSync.h"
#include "ImportableTableSync.hpp"
#include "PTUseRule.h"
#include "Vertex.h"
#include "StopArea.hpp"

#include <geos/geom/LineString.h>
#include <geos/geom/GeometryCollection.h>
#include <geos/io/WKTWriter.h>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace geos::io;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace pt;
	using namespace security;
	using namespace cms;
	using namespace graph;
	using namespace impex;

	template<> const string util::FactorableTemplate<server::Function,pt::LinesListFunction>::FACTORY_KEY(
		"LinesListFunction2"
	);

	namespace pt
	{
		const string LinesListFunction::PARAMETER_OUTPUT_FORMAT("of");
		const string LinesListFunction::PARAMETER_NETWORK_ID("ni");
		const string LinesListFunction::PARAMETER_PAGE_ID("pi");
		const string LinesListFunction::PARAMETER_SRID("srid");
		const string LinesListFunction::PARAMETER_OUTPUT_STOPS("os");
		const string LinesListFunction::PARAMETER_OUTPUT_GEOMETRY("og");
		const string LinesListFunction::PARAMETER_IGNORE_TIMETABLE_EXCLUDED_LINES("ittd");
		const string LinesListFunction::PARAMETER_IGNORE_JOURNEY_PLANNER_EXCLUDED_LINES("ijpd");
		const string LinesListFunction::PARAMETER_IGNORE_DEPARTURES_BOARD_EXCLUDED_LINES("idbd");

		const string LinesListFunction::FORMAT_WKT("wkt");
		const string LinesListFunction::FORMAT_JSON("json");
		const string LinesListFunction::FORMAT_XML("xml");

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
			if (_network.get() != NULL)
			{
				result.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}
			if(_coordinatesSystem)
			{
				result.insert(PARAMETER_SRID, static_cast<int>(_coordinatesSystem->getSRID()));
			}
			result.insert(PARAMETER_OUTPUT_GEOMETRY, _outputGeometry);
			result.insert(PARAMETER_OUTPUT_STOPS, _outputStops);
			if(!_outputFormat.empty())
			{
				result.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}
			result.insert(PARAMETER_IGNORE_DEPARTURES_BOARD_EXCLUDED_LINES, _ignoreDeparturesBoardExcludedLines);
			result.insert(PARAMETER_IGNORE_JOURNEY_PLANNER_EXCLUDED_LINES, _ignoreJourneyPlannerExcludedLines);
			result.insert(PARAMETER_IGNORE_TIMETABLE_EXCLUDED_LINES, _ignoreTimetableExcludedLines);
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

			// Output
			optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_PAGE_ID));
			if(id)
			try
			{
				_page = Env::GetOfficialEnv().get<Webpage>(*id);
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such page");
			}
			_outputFormat = map.getDefault<string>(PARAMETER_OUTPUT_FORMAT);
			_outputGeometry = map.getDefault<string>(PARAMETER_OUTPUT_GEOMETRY);
			_outputStops = map.getDefault<bool>(PARAMETER_OUTPUT_STOPS, false);
			CoordinatesSystem::SRID srid(
				map.getDefault<CoordinatesSystem::SRID>(PARAMETER_SRID, CoordinatesSystem::GetInstanceCoordinatesSystem().getSRID())
			);
			_coordinatesSystem = &CoordinatesSystem::GetCoordinatesSystem(srid);

			// Parameters
			_ignoreDeparturesBoardExcludedLines = map.getDefault<bool>(PARAMETER_IGNORE_DEPARTURES_BOARD_EXCLUDED_LINES, false);
			_ignoreJourneyPlannerExcludedLines = map.getDefault<bool>(PARAMETER_IGNORE_JOURNEY_PLANNER_EXCLUDED_LINES, false);
			_ignoreTimetableExcludedLines = map.getDefault<bool>(PARAMETER_IGNORE_TIMETABLE_EXCLUDED_LINES, false);

			// Saved parameters cleaning if output is a fixed format
			if(!_page.get())
			{
				_savedParameters.clear();
			}
		}

		// Class used for trim:
		class sortableNumber
		{
		private:
			string _value;
			string _begin; // = _value except for 12s it is 12
			string _end;   // =  ""    except for 12s it is  s
			long int _numericalValue; // = 12 for 12 and 12s, = -1 for A

			typedef enum {
				isAnInteger,   //example : 12
				beginIsInteger,   //example : 12s
				beginIsNotInteger //example : A, or T1
			} numberType;
			numberType _numberType;

		public:
			sortableNumber(string str)
			{
				_numericalValue = -1;
				_value = str;

				char * buffer = strdup(_value.c_str());
				char * pEnd;

				errno = 0;

				_numericalValue = strtol (buffer, &pEnd, 10);
				if (errno != 0 || pEnd == buffer) // Is A form
				{
					_numberType = beginIsNotInteger;
					_begin = _value;
					_end = "";
				}
				else
				{
					if(*pEnd != 0 ) // Is 12s form so pEnd is "s";
					{
						_numberType = beginIsInteger;
						_end = string(pEnd);
						*pEnd = '\0';
						_begin = string(buffer);
					}
					else // Is 12 form
					{
						_numberType = isAnInteger;
						_begin = _value;
						_end = "";
					}
				}
			}

			bool operator<(sortableNumber const &otherNumber) const
			{
				if((_numberType != beginIsInteger)
						&& (otherNumber._numberType != beginIsInteger)) // No number have form "12S"
				{
					if((_numberType == beginIsNotInteger)
							&& (otherNumber._numberType == beginIsNotInteger)) // They have both form "A"
					{
						return _value < otherNumber._value;
					}
					else if((_numberType == beginIsNotInteger)
							|| (otherNumber._numberType == beginIsNotInteger)) // One is "A" form, other is "23" form
					{
						return _value > otherNumber._value; // Force "T1" to appear before "23"
					}
					else // The two numbers have form 12
					{
						return _numericalValue < otherNumber._numericalValue;
					}
				}
				else if(_begin != otherNumber._begin) // At least one number have form 12S, and the other one have a different begin (eg. 13)
				{
					if((_numberType == isAnInteger)
							|| (otherNumber._numberType == isAnInteger)) // here is a "13" type and an "12s" form
					{
						return _numericalValue < otherNumber._numericalValue;
					}
					else // There is a "A" type and an "12s" form
					{
						return _value > otherNumber._value;
					}
				}
				else // Case 12/12s or 12s/12k : sort based on end
				{
					return _end < otherNumber._end;
				}
			}
		};

		void LinesListFunction::run( std::ostream& stream, const Request& request ) const
		{
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

			typedef map<sortableNumber,shared_ptr<const CommercialLine> > linesMapType;
			linesMapType linesMap;

			// Get CommercialLine Global Registry
			if(_network.get())
			{
				vector<shared_ptr<CommercialLine> > lines(
					CommercialLineTableSync::Search(Env::GetOfficialEnv(), _network->getKey())
				);
				BOOST_FOREACH(shared_ptr<CommercialLine> line, lines)
				{
					const UseRule& useRule(line->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET));
					if(	dynamic_cast<const PTUseRule*>(&useRule) &&
						(	_ignoreJourneyPlannerExcludedLines && static_cast<const PTUseRule&>(useRule).getForbiddenInJourneyPlanning() ||
							_ignoreTimetableExcludedLines && static_cast<const PTUseRule&>(useRule).getForbiddenInTimetables() ||
							_ignoreDeparturesBoardExcludedLines && static_cast<const PTUseRule&>(useRule).getForbiddenInDepartureBoards()
					)	){
						continue;
					}

					// Insert respecting order described up there
					linesMap[sortableNumber(line->getShortName())] = const_pointer_cast<const CommercialLine>(line);
				}
			}
			else if(_line.get())
			{
				linesMap[sortableNumber(_line->getShortName())] = _line;
			}

			// Populating the parameters map
			ParametersMap pm(request.getFunction()->getSavedParameters());
			BOOST_FOREACH(linesMapType::value_type it, linesMap)
			{
				shared_ptr<const CommercialLine> line = it.second;
				shared_ptr<ParametersMap> linePM(new ParametersMap(request.getFunction()->getSavedParameters()));
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
					shared_ptr<ParametersMap> rsPM(new ParametersMap(request.getFunction()->getSavedParameters()));
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
					shared_ptr<ParametersMap> stopAreasPM(new ParametersMap(request.getFunction()->getSavedParameters()));
					BOOST_FOREACH(const StopArea* stopArea, stopAreas)
					{
						shared_ptr<ParametersMap> stopAreaPM(new ParametersMap(request.getFunction()->getSavedParameters()));
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

					shared_ptr<ParametersMap> geometryPM(new ParametersMap(request.getFunction()->getSavedParameters()));
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
							shared_ptr<ParametersMap> edgePM(new ParametersMap(request.getFunction()->getSavedParameters()));
							shared_ptr<Geometry> prGeom(
								_coordinatesSystem->convertGeometry(*it.second)
							);
							for(size_t i(0); i<prGeom->getNumPoints(); ++i)
							{
								const Coordinate& pt(prGeom->getCoordinates()->getAt(i));
								shared_ptr<ParametersMap> pointPM(new ParametersMap(request.getFunction()->getSavedParameters()));
								pointPM->insert(DATA_X, pt.x);
								pointPM->insert(DATA_Y, pt.y);
								edgePM->insert(DATA_POINT, pointPM);
							}
							geometryPM->insert(DATA_EDGE, edgePM);
						}
					}
					linePM->insert(DATA_GEOMETRY, geometryPM);
				}
				pm.insert(DATA_LINE, linePM);
			}

			if(_page.get()) // CMS output
			{
				size_t rank(0);
				BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmLine, pm.getSubMaps(DATA_LINE))
				{
					pmLine->insert(DATA_RANK, rank++);
					_page->display(stream, request, *pmLine);
				}
			}
			else if(_outputFormat == FORMAT_XML) // XML output
			{
				pm.outputXML(
					stream,
					DATA_LINES,
					true,
					"http://synthese.rcsmobility.com/include/35_pt/LinesListFunction.xsd"
				);
			}
			else if(_outputFormat == FORMAT_JSON) // JSON output
			{
				pm.outputJSON(
					stream,
					DATA_LINES
				);
			}
			else // CSV format
			{
				BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmLine, pm.getSubMaps(DATA_LINE))
				{
					stream << pmLine->get<string>(CommercialLine::DATA_LINE_ID) << ";" << pmLine->get<string>(CommercialLine::DATA_LINE_SHORT_NAME) << "\n";
				}
			}
		}



		bool LinesListFunction::isAuthorized(
			const Session*
		) const {
			return true;
		}



		std::string LinesListFunction::getOutputMimeType() const
		{
			std::string mimeType;
			if(_page.get())
			{
				mimeType = _page->getMimeType();
			}
			else if(_outputFormat == FORMAT_XML)
			{
				mimeType = "text/xml";
			}
			else // default case : csv outputFormat
			{
				mimeType = "text/csv";
			}
			return mimeType;
		}



		LinesListFunction::LinesListFunction():
			_outputStops(false),
			_ignoreTimetableExcludedLines(false),
			_ignoreJourneyPlannerExcludedLines(false),
			_ignoreDeparturesBoardExcludedLines(false)
		{
		}
	}
}
