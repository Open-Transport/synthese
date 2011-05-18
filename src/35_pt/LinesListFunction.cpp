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
#include "RollingStock.h"
#include "Path.h"
#include "JourneyPattern.hpp"
#include "CommercialLineTableSync.h"
#include "ImportableTableSync.hpp"
#include "PTUseRule.h"
#include "PTObjectsCMSExporters.hpp"
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
		const string LinesListFunction::FORMAT_XML("xml");

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
		}

		//Class used for trim:
		class sortableNumber
		{
		private:
			string _value;
			string _begin;// = _value except for 12s it is 12
			string _end;  // =  ""    except for 12s it is  s
			long int _numericalValue;// = 12 for 12 and 12s, = -1 for A

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
				if (errno != 0 || pEnd == buffer)//Is A form
				{
					_numberType = beginIsNotInteger;
					_begin = _value;
					_end = "";
				}
				else
				{
					if(*pEnd != 0 )//Is 12s form so pEnd is "s";
					{
						_numberType = beginIsInteger;
						_end = string(pEnd);
						*pEnd = '\0';
						_begin = string(buffer);
					}
					else//Is 12 form
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
						&& (otherNumber._numberType != beginIsInteger))//No number have form "12S"
				{
					if((_numberType == beginIsNotInteger)
							|| (otherNumber._numberType == beginIsNotInteger))//At least one number have form "A"
					{
						return _value < otherNumber._value;
					}
					else//The two numbers have form 12
					{
						return _numericalValue < otherNumber._numericalValue;
					}
				}
				else if(_begin != otherNumber._begin)//At least one number have form 12S, and the other one have a different begin (eg. 13)
				{
					if((_numberType == isAnInteger)
							|| (otherNumber._numberType == isAnInteger))//The second number have 13 form
					{
						return _numericalValue < otherNumber._numericalValue;
					}
					else//The second number have A form
					{
						return _value < otherNumber._value;
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
			//Sortering is made on numerical order
			//   (except for line number which doesn't begin by a number)
			//   then alphabetic order.
			//
			//So, linesMap is like that:
			//
			// [1]  -> ligne 1 : XXX - XXX
			// [2]  -> ligne 2 : XXX - XXX
			// [2S] -> ligne 2S: XXX - XXX
			// [A]  -> ligne A : XXX - XXX
			// [A1] -> ligne A1: XXX - XXX
			//

			typedef map<sortableNumber,shared_ptr<const CommercialLine> > linesMapType;
			linesMapType linesMap;

			//Get CommercialLine Global Registry
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

					//Insert respecting order described up there
					linesMap[sortableNumber(line->getShortName())] = const_pointer_cast<const CommercialLine>(line);
				}
			}
			else if(_line.get())
			{
				linesMap[sortableNumber(_line->getShortName())] = _line;
			}

			if((!_page.get())&&(_outputFormat == FORMAT_XML))
			{
				// XML header
				stream <<
					"<?xml version=\"1.0\" encoding=\"UTF-8\"?>" <<
					"<lines xsi:noNamespaceSchemaLocation=\"http://synthese.rcsmobility.com/include/35_pt/LinesListFunction.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">"
				;
			}
			size_t rank(0);
			BOOST_FOREACH(linesMapType::value_type it, linesMap)
			{
				shared_ptr<const CommercialLine> line = it.second;
				if(_page.get())
				{
					LineMarkerInterfacePage::Display(
						stream,
						_page,
						request,
						*line,
						rank++
					);
				}
				else if(_outputFormat == FORMAT_XML)
				{
					stream <<"<line id=\""<< line->getKey() <<
						"\" creatorId=\"" << (line->getDataSourceLinks().size() == 1 ? lexical_cast<string>(line->getDataSourceLinks().begin()->second) : ImportableTableSync::SerializeDataSourceLinks(line->getDataSourceLinks())) <<
						"\" name=\""      << line->getName() <<
						"\" shortName=\"" << line->getShortName() <<
						"\" longName=\""  << line->getLongName() <<
						"\" color=\""     << line->getColor() <<
						"\" style=\""     << line->getStyle() <<
						"\" image=\""     << line->getImage() <<
						"\" >";

					set<RollingStock *> rollingStocks;
					BOOST_FOREACH(Path* path, line->getPaths())
					{
						if(
							!static_cast<const JourneyPattern*>(path)->getRollingStock()
						){
							continue;
						}
						rollingStocks.insert(
							static_cast<const JourneyPattern*>(path)->getRollingStock()
						);
					}
					BOOST_FOREACH(RollingStock * rs, rollingStocks)
					{
						stream <<"<transportMode id=\""<< rs->getKey() <<
							"\" name=\""    << rs->getName() <<
							"\" article=\"" << rs->getArticle()<<
							"\" />";
					}
					if(_outputStops)
					{
						set<const StopArea*> stopAreas;
						BOOST_FOREACH(Path* path, line->getPaths())
						{
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
						stream << "<stopAreas>";
						BOOST_FOREACH(const StopArea* stopArea, stopAreas)
						{
							PTObjectsCMSExporters::ExportStopArea(stream, *stopArea, _coordinatesSystem);
						}
						stream << "</stopAreas>";
					}
					if(!_outputGeometry.empty())
					{
						typedef map<pair<Vertex*, Vertex*>, shared_ptr<Geometry> > VertexPairs;
						VertexPairs geometries;
						BOOST_FOREACH(Path* path, line->getPaths())
						{
							BOOST_FOREACH(Edge* edge, path->getEdges())
							{
								if(!edge->getNext())
								{
									break;
								}
								VertexPairs::key_type od(make_pair(edge->getFromVertex(), edge->getNext()->getFromVertex()));
								if(geometries.find(od) == geometries.end())
								{
									geometries.insert(make_pair(od, static_pointer_cast<Geometry,LineString>(edge->getRealGeometry())));
								}
							}
						}
						stream << "<geometry>";
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
							stream << WKTWriter().write(mls.get());
						}
						else
						{
							BOOST_FOREACH(const VertexPairs::value_type& it, geometries)
							{
								shared_ptr<Geometry> prGeom(
									_coordinatesSystem->convertGeometry(*it.second)
								);
								stream << "<edge>";
								for(size_t i(0); i<prGeom->getNumPoints(); ++i)
								{
									const Coordinate& pt(prGeom->getCoordinates()->getAt(i));

									stream << "<point x=\"" << std::fixed << pt.x << "\" y=\"" << std::fixed << pt.y << "\" />";
								}
								stream << "</edge>";
							}
						}
						stream << "</geometry>";
					}
					stream <<"</line>";
				}
				else//default case : csv outputFormat
				{
					stream << line->getKey() << ";" << line->getShortName() << "\n";
				}
			}
			if((!_page.get())&&(_outputFormat =="xml"))
			{
				// XML footer
				stream << "</lines>";
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
			else//default case : csv outputFormat
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
