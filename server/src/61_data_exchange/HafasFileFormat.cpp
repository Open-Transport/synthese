
/** HafasFileFormat class implementation.
	@file HafasFileFormat.cpp

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

#include "HafasFileFormat.hpp"

#include "DataSource.h"
#include "Import.hpp"
#include "StopPoint.hpp"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"
#include "City.h"
#include "CityTableSync.h"
#include "DBTransaction.hpp"
#include "JourneyPatternTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "CommercialLineTableSync.h"
#include "LineStopTableSync.h"
#include "Calendar.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "TransportNetworkTableSync.h"
#include "RollingStockTableSync.hpp"
#include "ContinuousServiceTableSync.h"
#include "OneFileExporter.hpp"
#include "RequestException.h"
#include "ZipWriter.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <map>
#include <fstream>
#include <cstdlib>
#include <string>
#include <iostream>
#include <cstdio>
#include <memory>
#include <geos/geom/Point.h>
#include <geos/opDistance.h>

using namespace std;
using namespace boost;
using namespace boost::filesystem;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace geos::geom;

namespace synthese
{
	using namespace data_exchange;
	using namespace util;
	using namespace vehicle;
	using namespace impex;
	using namespace pt;
	using namespace road;
	using namespace admin;
	using namespace geography;
	using namespace db;
	using namespace graph;
	using namespace calendar;
	using namespace server;
	using namespace html;



	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,HafasFileFormat>::FACTORY_KEY("Hafas");
	}

	namespace data_exchange
	{
		const string HafasFileFormat::Importer_::FILE_KOORD = "koord";
		const string HafasFileFormat::Importer_::FILE_BAHNOF = "bahnof";
		const string HafasFileFormat::Importer_::FILE_BITFELD = "bitfeld";
		const string HafasFileFormat::Importer_::FILE_ECKDATEN = "eckdaten";
		const string HafasFileFormat::Importer_::FILE_ZUGDAT = "zugdat";
		const string HafasFileFormat::Importer_::FILE_GLEIS = "gleis";
		const string HafasFileFormat::Importer_::FILE_UMSTEIGB = "umsteigb";
		const string HafasFileFormat::Importer_::FILE_UMSTEIGZ = "umsteigz";
		const string HafasFileFormat::Importer_::FILE_METABHF = "metabhf";

		const string HafasFileFormat::Importer_::PARAMETER_SHOW_STOPS_ONLY = "show_stops_only";
		const string HafasFileFormat::Importer_::PARAMETER_WAYBACK_BIT_POSITION = "wayback_bit_position";
		const string HafasFileFormat::Importer_::PARAMETER_IMPORT_FULL_SERVICES = "import_full_services";
		const string HafasFileFormat::Importer_::PARAMETER_IMPORT_STOPS = "import_stops";
		const string HafasFileFormat::Importer_::PARAMETER_LINES_FILTER = "lines_filter";
		const string HafasFileFormat::Importer_::PARAMETER_GLEIS_HAS_ONE_STOP_PER_LINE = "gleis_has_one_stop_per_line";
		const string HafasFileFormat::Importer_::PARAMETER_COMPLETE_EMPTY_STOP_AREA_NAME = "complete_empty_stop_area_name";
		const string HafasFileFormat::Importer_::PARAMETER_NO_GLEIS_FILE = "no_gleis_file";
		const string HafasFileFormat::Importer_::PARAMETER_TRY_TO_READ_LINE_SHORT_NAME = "try_to_read_line_short_name";
		const string HafasFileFormat::Importer_::PARAMETER_CONCATENATE_TRANSPORT_SHORT_NAME = "concatenate_transport_short_name";
		const string HafasFileFormat::Importer_::PARAMETER_READ_WAYBACK = "read_way_back";
		const string HafasFileFormat::Importer_::PARAMETER_CALENDAR_DEFAULT_CODE = "calendar_default_code";
		const string HafasFileFormat::Importer_::PARAMETER_2015_CARPOSTAL_FORMAT = "format_carpostal_2015";
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<HafasFileFormat>::Files MultipleFileTypesImporter<HafasFileFormat>::FILES(
			HafasFileFormat::Importer_::FILE_KOORD.c_str(),
			HafasFileFormat::Importer_::FILE_BAHNOF.c_str(),
			HafasFileFormat::Importer_::FILE_GLEIS.c_str(),
			HafasFileFormat::Importer_::FILE_UMSTEIGB.c_str(),
			HafasFileFormat::Importer_::FILE_UMSTEIGZ.c_str(),
			HafasFileFormat::Importer_::FILE_METABHF.c_str(),
			HafasFileFormat::Importer_::FILE_ECKDATEN.c_str(),
			HafasFileFormat::Importer_::FILE_BITFELD.c_str(),
			HafasFileFormat::Importer_::FILE_ZUGDAT.c_str(),
		"");
	}

	namespace data_exchange
	{
		const string HafasFileFormat::LineFilter::SEP_MAIN = ",";
		const string HafasFileFormat::LineFilter::SEP_FIELD = ":";
		const string HafasFileFormat::LineFilter::JOCKER = "*";
		const string HafasFileFormat::LineFilter::VALUE_LINES_BY_STOPS_PAIRS = "SP";



		HafasFileFormat::LineFilter::LineFilter():
			linesByStopsPair(false)
		{}



		HafasFileFormat::LinesFilter HafasFileFormat::Importer_::getLinesFilter( const std::string& s )
		{
			LinesFilter result;
			vector<string> filters;
			split(filters, s, is_any_of(LineFilter::SEP_MAIN));
			BOOST_FOREACH(const string& filter, filters)
			{
				vector<string> items;
				split(items, filter, is_any_of(LineFilter::SEP_FIELD));
				LineFilter lineFilter;
				string pattern;

				// Pattern
				if(items.size() > 0)
				{
					pattern = items[0];
				}
				else
				{
					pattern = LineFilter::JOCKER;
				}

				// Network
				ImportableTableSync::ObjectBySource<TransportNetworkTableSync>::Set networks(
					_networks.get(pattern)
				);
				if(networks.empty())
				{
					string errorString = "Error in line filter " + s + " : network " + pattern + " not found";
					_logError(errorString);
					continue;
				}
				lineFilter.network = *networks.begin();

				// Lines by stops pairs
				if(items.size() > 1 && items[1]==LineFilter::VALUE_LINES_BY_STOPS_PAIRS)
				{
					lineFilter.linesByStopsPair = true;
				}
				else // Lines by service number pattern matching
				{
					// Line number start
					if(items.size() > 1 && !items[0].empty())
					{
						try
						{
							lineFilter.lineNumberStart = lexical_cast<size_t>(items[1]);
						}
						catch(bad_lexical_cast&)
						{
						}
					}

					// Line number end
					if(items.size() > 2 && !items[1].empty())
					{
						try
						{
							lineFilter.lineNumberEnd = lexical_cast<size_t>(items[2]);
						}
						catch(bad_lexical_cast&)
						{
						}
					}
				}

				// Insertion
				result.insert(
					make_pair(
						pattern,
						lineFilter
				)	);
			}
			return result;
		}



		std::string HafasFileFormat::Importer_::LinesFilterToString(const LinesFilter& value)
		{
			stringstream s;
			bool first(true);
			BOOST_FOREACH(const LinesFilter::value_type& filter, value)
			{
				// Filters separator
				if(first)
				{
					first = false;
				}
				else
				{
					s << LineFilter::SEP_MAIN;
				}

				// Pattern
				s << filter.first << LineFilter::SEP_FIELD;

				// Lines by stops pairs
				if(filter.second.linesByStopsPair)
				{
					s << LineFilter::VALUE_LINES_BY_STOPS_PAIRS << LineFilter::SEP_FIELD;
				}
				else
				{
					// Line number start
					if(filter.second.lineNumberStart)
					{
						s << *filter.second.lineNumberStart;
					}
					s << LineFilter::SEP_FIELD;

					// Line number end
					if(filter.second.lineNumberStart)
					{
						s << *filter.second.lineNumberEnd;
					}
				}
			}
			return s.str();
		}



		bool HafasFileFormat::Importer_::_checkPathsMap() const
		{
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_ECKDATEN));
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_BITFELD);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_KOORD);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_ZUGDAT);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			return true;
		}



		DBTransaction HafasFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;

			// Add remove queries generated by _selectObjectsToRemove
			PTDataCleanerFileFormat::_addRemoveQueries(transaction);

			if(_importStops)
			{
				BOOST_FOREACH(Registry<City>::value_type city, _env.getRegistry<City>())
				{
					CityTableSync::Save(city.second.get(), transaction);
				}
				BOOST_FOREACH(Registry<StopArea>::value_type stopArea, _env.getRegistry<StopArea>())
				{
					StopAreaTableSync::Save(stopArea.second.get(), transaction);
				}
				BOOST_FOREACH(Registry<StopPoint>::value_type stop, _env.getRegistry<StopPoint>())
				{
					StopPointTableSync::Save(stop.second.get(), transaction);
				}
			}
			BOOST_FOREACH(Registry<CommercialLine>::value_type line, _env.getRegistry<CommercialLine>())
			{
				CommercialLineTableSync::Save(line.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<JourneyPattern>::value_type line, _env.getRegistry<JourneyPattern>())
			{
				JourneyPatternTableSync::Save(line.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<LineStop>::value_type lineStop, _env.getRegistry<LineStop>())
			{
				LineStopTableSync::Save(lineStop.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<ScheduledService>::value_type& service, _env.getRegistry<ScheduledService>())
			{
				ScheduledServiceTableSync::Save(service.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<ContinuousService>::value_type& service, _env.getRegistry<ContinuousService>())
			{
				ContinuousServiceTableSync::Save(service.second.get(), transaction);
			}
			return transaction;
		}


		bool HafasFileFormat::Importer_::_parse(
			const path& filePath,
			const std::string& key
		) const {
			if(!_openFile(filePath))
			{
				// It is possible to have no GLEIS file, but it will lead to a different parsing
				// File bahnof is optional
				if (key != FILE_BAHNOF && (!(key == FILE_GLEIS && _noGleisFile)))
					throw Exception("Could no open the file " + filePath.string());
			}

			DataSource& dataSource(*_import.get<DataSource>());

			// 0 : Coordinates
			if(key == FILE_KOORD)
			{
				util::Log::GetInstance().debug("HafasFileFormat : lecture du fichier KOORD");
				while(_loadLine())
				{
					// Declaration
					Bahnhof bahnhof;

					// Operator code
					bahnhof.operatorCode = _getField(0, 7);

					// Point
					std::string strX = _getField(10, 7);
					std::string strY = _getField(20, 7); 
					double x(1000 * lexical_cast<double>(strX));
					double y(1000 * lexical_cast<double>(strY));
					if(x && y)
					{
						bahnhof.point = CoordinatesSystem::GetInstanceCoordinatesSystem().convertPoint(
							*dataSource.get<CoordinatesSystem>()->createPoint(x, y)
						);
					}

					// Names
					vector<string> cols;
					std::string nameStr(_getField(34));
					if (nameStr.substr(0,2) == "% ")
						nameStr = nameStr.substr(2,nameStr.size()-2);
					boost::algorithm::split(cols, nameStr, boost::algorithm::is_any_of(","));
					bahnhof.cityName = cols[0];
					if(cols.size() == 1)
					{
						bahnhof.main = true;
						if (!_complete_empty_stop_area_name.empty())
							bahnhof.name = cols[0] + _complete_empty_stop_area_name;
					}
					else
					{
						bahnhof.name = cols[1];
					}

					// Insertion of the stop
					_bahnhofs.insert(
						make_pair(
							bahnhof.operatorCode,
							bahnhof
					)	);
				}
			}
			else if (key == FILE_BAHNOF)
			{
				util::Log::GetInstance().debug("HafasFileFormat : lecture du fichier BAHNOF");
				while(_loadLine())
				{
					// operator code
					string operatorCode = _getField(0, 7);
					// Search of the bahnhof
					Bahnhofs::iterator itBahnhof(_bahnhofs.find(operatorCode));
					if(itBahnhof == _bahnhofs.end())
					{
						string warnString = "Bahnhof " + operatorCode + " referenced in BAHNHOF file but not declared in KOORD file";
						_logWarning(warnString);
						continue;
					}
					
					// Names
					vector<string> cols;
					std::string nameStr(_getField(12));
					boost::algorithm::split(cols, nameStr, boost::algorithm::is_any_of(","));
					itBahnhof->second.cityName = cols[0];
					if(cols.size() == 1)
					{
						itBahnhof->second.main = true;
						if (!_complete_empty_stop_area_name.empty())
							itBahnhof->second.name = cols[0] + _complete_empty_stop_area_name;
					}
					else
					{
						itBahnhof->second.name = cols[1];
					}
				}
			}
			else if(key == FILE_GLEIS && !_noGleisFile)
			{
				util::Log::GetInstance().debug("HafasFileFormat : lecture du fichier GLEIS");
				GleisMap::iterator itService(_gleisMap.end());

				while(_loadLine())
				{
					if (!_gleisHasOneStopPerLine)
					{
						// New service
						if(_getField(0,1) == "@")
						{
							itService = _gleisMap.insert(
								make_pair(
									boost::make_tuple(
										_getField(3, 5),
										_getField(9, 6),
										lexical_cast<size_t>(_getField(16, 2))
									),
									GleisMap::mapped_type()
							)	).first;
						}
						else if(itService != _gleisMap.end())
						{
							// Fields
							string stopCode(_getField(0, 7));
							string gleisCode(_getField(10, 6));

							// Search of the bahnhof
							Bahnhofs::iterator itBahnhof(_bahnhofs.find(stopCode));
							if(itBahnhof == _bahnhofs.end())
							{
								continue;
							}

							// Record of the gleis in the bahnhof
							itBahnhof->second.gleisSet.insert(gleisCode);

							// Record of the gleis in the service
							itService->second.insert(
								make_pair(
									stopCode,
									gleisCode
							)	);
						}
					}
					else
					{
						string serviceCode(_getField(8, 5));
						string lineNumber(_getField(14, 6));
						string stopCode(_getField(0, 7));
						string gleisCode(_getField(21, 6));
						itService = _gleisMap.find(boost::make_tuple(serviceCode, lineNumber, 0));
						if (itService == _gleisMap.end())
						{
							// We create the service
							itService = _gleisMap.insert(
								make_pair(
									boost::make_tuple(serviceCode, lineNumber, 0),
									GleisMap::mapped_type()
							)	).first;
						}
						
						// Search of the bahnhof
						Bahnhofs::iterator itBahnhof(_bahnhofs.find(stopCode));
						if(itBahnhof == _bahnhofs.end())
						{
							continue;
						}
						
						// Record of the gleis in the bahnhof
						itBahnhof->second.gleisSet.insert(gleisCode);

						// Record of the gleis in the service
						itService->second.insert(
							make_pair(
								stopCode,
								gleisCode
						)	);
					}
				}
			}
			else if(key == FILE_UMSTEIGB)
			{
				util::Log::GetInstance().debug("HafasFileFormat : lecture du fichier UMSTEIGB");
				while(_loadLine())
				{
					// Fields
					string stopCode(_getField(0, 7));

					// Search of the bahnhof
					Bahnhofs::iterator itBahnhof(_bahnhofs.find(stopCode));
					if(itBahnhof == _bahnhofs.end())
					{
						string warnString = "Bahnhof " + stopCode + " referenced in UMSTEIBG file but not declared in KOORD file";
						_logWarning(warnString);
						continue;
					}

					// Storage
					try
					{
						itBahnhof->second.defaultTransferDuration = minutes(lexical_cast<long>(_getField(8, 2)));
					}
					catch(bad_lexical_cast&)
					{
					}
				}
			}
			else if(key == FILE_UMSTEIGZ)
			{
				util::Log::GetInstance().debug("HafasFileFormat : lecture du fichier UMSTEIGZ");
				while(_loadLine())
				{
					try
					{
						_interServiceTransferDurationMap.insert(
							make_pair(
								boost::make_tuple(
									_getField(0, 7), // Stop code
									make_pair(
										_getField(8, 5), // Service code
										_getField(14, 6) // Line code
									),
									make_pair(
										_getField(21, 5), // Service code
										_getField(27, 6) // Line code
								)	),
								minutes(lexical_cast<long>(_getField(34, 2))) // Transfer time
						)	);
					}
					catch(bad_lexical_cast&)
					{
						string warnString = "Cannot parse this line in UMSTEIGZ file : " + _line;
						_logWarning(warnString);
					}
				}
			}
			else if(key == FILE_METABHF)
			{
				util::Log::GetInstance().debug("HafasFileFormat : lecture du fichier METABHF");
				while(_loadLine())
				{
					// Commented lines
					if(_getField(0, 1) == "*")
					{
						continue;
					}
					// Inter stop duration
					if(_getField(7,1) != ":")
					{
						try
						{
							_interStopTransferDurationMap.insert(
								make_pair(
									make_pair(
										_getField(0, 7), // From stop
										_getField(9, 7) // To stop
									),
									minutes(lexical_cast<long>(_getField(18, 2))) // Transfer time
							)	);
						}
						catch(bad_lexical_cast&)
						{
							string warnString = "Cannot parse this line in METABHF file : " + _line;
							_logWarning(warnString);
						}
					}
					else // Stop area mapping
					{
						StopAreaMappingMap::mapped_type mapping;
						string mappedStopsStr(_getField(9));
						vector<string> cols;
						boost::algorithm::split(cols, mappedStopsStr, boost::algorithm::is_any_of(" "));
						BOOST_FOREACH(const string& col, cols)
						{
							mapping.insert(col);
						}
						if(!mapping.empty())
						{
							_stopAreaMappingMap.insert(
								make_pair(
									_getField(0, 7),
									mapping
							)	);
						}
					}
				}
			} // 1 : Time period
			else if(key == FILE_ECKDATEN)
			{
				// This file should already loaded at parameters reading
			} // 2 : Nodes
			else if(key == FILE_BITFELD)
			{
				util::Log::GetInstance().debug("HafasFileFormat : lecture du fichier BITFELD");
				while(_loadLine())
				{
					int id(lexical_cast<int>(_getField(0,6)));
					string calendarString(_getField(7));

					date curDate(_fileFirstDate);
					bool first(true);

					Calendar calendar;
					date endDate(_calendar.getLastActiveDate());

					BOOST_FOREACH(char c, calendarString)
					{
						int bits(0);
						if((c >= '0') && (c <= '9')) bits = (c - '0');
						else if((c >= 'A') && (c <= 'F')) bits = (c - 'A' + 10);
						else if((c >= 'a') && (c <= 'f')) bits = (c - 'a' + 10);

						for(int i(0); i<4; ++i)
						{
							if(!first || i>=2)
							{
								if(curDate > endDate)
								{
									break;
								}
								if(	(bits & 8) &&
									(curDate >= _calendar.getFirstActiveDate())
								){
									calendar.setActive(curDate);
								}
								curDate += days(1);
							}
							bits = bits << 1;
						}
						if(curDate > endDate)
						{
							break;
						}
						first = false;
					}

					_calendarMap[id] = calendar;
				}
			} // 3 : Services
			else if (key == FILE_ZUGDAT)
			{
				util::Log::GetInstance().debug("HafasFileFormat : lecture du fichier ZUGDAT");
				// Declarations
				Zugs::iterator itZug(_zugs.end());
				string zugNumber;
				bool loadCurrentZug(true);

				// File loop
				while(_loadLine())
				{
					if(_getField(0, 2) == "*Z") // New zug
					{
						// Check of the last zug
						if(itZug != _zugs.end())
						{
							// Line number if determinated by stops pair
							if(	itZug->lineFilter->linesByStopsPair &&
								itZug->stops.size() > 1
							){
								if(itZug->stops.begin()->stopCode < itZug->stops.rbegin()->stopCode)
								{
									itZug->lineNumber = itZug->transportModeCode +"-"+ itZug->stops.begin()->stopCode +"-"+ itZug->stops.rbegin()->stopCode;
								}
								else
								{
									itZug->lineNumber = itZug->transportModeCode +"-"+ itZug->stops.rbegin()->stopCode +"-"+ itZug->stops.begin()->stopCode;
								}
							}

							// Check of schedules
							bool hasDeparture(false);
							Zug::Stops::const_iterator itStop(itZug->stops.begin());
							for(;itStop != itZug->stops.end(); ++itStop)
							{
								if(!itStop->departureTime.is_not_a_date_time())
								{
									hasDeparture = true;
									break;
								}
							}
							bool hasArrival(false);
							if(hasDeparture)
							{
								for(++itStop; itStop != itZug->stops.end(); ++itStop)
								{
									if(!itStop->arrivalTime.is_not_a_date_time())
									{
										hasArrival = true;
										break;
									}
								}
							}
							if(!hasDeparture || !hasArrival)
							{
								string warnString = "Zug " + itZug->number + "/" + itZug->lineNumber + " has no departure nor arrival";
								_logWarning(warnString);
								_zugs.pop_back();
								itZug = _zugs.end();
							}
						}

						// Line number filter
						string lineNumber(_getField(9,6));
						if (_formatCarpostal2015) {
							lineNumber = _getField(14,3) + _getField(4,3);
						}
						const LineFilter* lineFilter(
							_lineIsIncluded(lineNumber)
						);
						if(	!lineFilter && !_formatCarpostal2015
						){
							loadCurrentZug = false;
							continue;
						}
						loadCurrentZug = true;

						// Load current zug
						itZug = _zugs.insert(_zugs.end(), Zug());
						itZug->lineFilter = lineFilter;
						itZug->number = _getField(3,5);
						if (_formatCarpostal2015) {
							itZug->number = _getField(7,6);
						}
						try {
							itZug->version = lexical_cast<size_t>(_getField(16, 2));
						}
						catch (bad_lexical_cast&)
						{
							itZug->version = 0;
						}

						zugNumber = itZug->number +"/"+ lineNumber;
						if(lineFilter->lineNumberStart && lineFilter->lineNumberEnd)
						{
							itZug->lineNumber = _getField(
								*lineFilter->lineNumberStart + 3,
								*lineFilter->lineNumberEnd - *lineFilter->lineNumberStart + 1
							);
						}
						else
						{
							itZug->lineNumber = lineNumber;
						}
						
						itZug->readWayback = false;

						// Continuous service
						if(!_getField(21,4).empty() && !_formatCarpostal2015)
						{
							itZug->continuousServiceRange = minutes(lexical_cast<long>(_getField(21, 4)));
							itZug->continuousServiceWaitingTime = minutes(lexical_cast<long>(_getField(26, 3)));
						}
					}
					else if(!loadCurrentZug)
					{
						continue;
					}
					else if(_getField(0, 5) == "*A VE") // Calendar
					{
						if(itZug != _zugs.end())
						{
							Zug::CalendarUse calendarUse;
							try {
								calendarUse.calendarNumber = lexical_cast<size_t>(_getField(22, 6));
							}
							catch (bad_lexical_cast&)
							{
								calendarUse.calendarNumber = _defaultCalendarCode;
							}
							calendarUse.startStopCode = _getField(6, 7);
							calendarUse.endStopCode = _getField(14, 7);
							itZug->calendars.push_back(calendarUse);
						}
					}
					else if(_getField(0, 2) == "*G") // Transport mode
					{
						if(itZug != _zugs.end())
						{
							itZug->transportModeCode = _getField(3, 3);
						}
					}
					else if(_getField(0, 2) == "*L" && _tryToReadShortName) // Short name of the line
					{
						if(itZug != _zugs.end())
						{
							itZug->lineShortName = _getField(3, 8);
						}
					}
					else if (_getField(0,2) == "*R" && _readWayback)
					{
						if (itZug != _zugs.end())
						{
							itZug->readWayback = (_getField(3, 1) == "R");
						}
					}
					else if(_getField(0, 1) != "*") // Stop
					{
						// Stop code
						string stopCode = _getField(0, 7);
						Bahnhofs::iterator itBahnhof(_bahnhofs.find(stopCode));
						if(itBahnhof == _bahnhofs.end())
						{
							_logWarning(
								"Inconsistent service "+ zugNumber +" : stop "+ stopCode +" is not present in koord file. Service is ignored"
							);
							if(itZug != _zugs.end())
							{
								_zugs.pop_back();
								itZug = _zugs.end();
							}
						}
						else
						{
							itBahnhof->second.used = true;
						}

						if(itZug == _zugs.end())
						{
							continue;
						}

						// Departure times
						string departureTimeStr;
						string arrivalTimeStr;
						if (!_gleisHasOneStopPerLine && !_formatCarpostal2015)
						{
						    departureTimeStr = _getField(34, 4);
						    arrivalTimeStr = _getField(29, 4);
						} else {
							// Format is different when gleis file has one stop per line
							// If it is found a set of file where gleis file is by section
							// and departure time is on #38 and arrival on 31
							// OR a set of file where gleis file has one stop per line and
							// and departure time is on #34 and arrival on 29
							// then it will be needed another parameter to manage it independantly
							departureTimeStr = _getField(38, 4);
							arrivalTimeStr = _getField(31, 4);
						}
						
						bool isDeparture(departureTimeStr != "9999" && !departureTimeStr.empty());
						bool isArrival(arrivalTimeStr != "9999" && !arrivalTimeStr.empty());
						if(!isArrival && !isDeparture)
						{
							continue;
						}

						Zug::Stops::iterator itStop = itZug->stops.insert(itZug->stops.end(), Zug::Stop());
						itStop->stopCode = stopCode;
						if(isDeparture)
						{
							try {
								itStop->departureTime =
									hours(lexical_cast<int>(departureTimeStr.substr(0,2))) +
									minutes(lexical_cast<int>(departureTimeStr.substr(2,2)))
								;
							}
							catch (bad_lexical_cast&)
							{
								itStop->departureTime = time_duration(0,0,0);
							}
						}
						if(isArrival)
						{
							try {
								itStop->arrivalTime =
									hours(lexical_cast<int>(arrivalTimeStr.substr(0,2))) +
									minutes(lexical_cast<int>(arrivalTimeStr.substr(2,2)))
								;
							}
							catch (bad_lexical_cast&)
							{
								itStop->departureTime = time_duration(0,0,0);
							}
						}

						// Gleis
						GleisMap::const_iterator itGleis(
							_gleisMap.find(
								boost::make_tuple(itZug->number, itZug->lineNumber, itZug->version)
						)	);
						if(itGleis == _gleisMap.end())
						{
							itGleis = _gleisMap.find(
								boost::make_tuple(itZug->number, itZug->lineNumber, 0)
							);
						}
						if(itGleis != _gleisMap.end())
						{
							GleisMap::mapped_type::const_iterator itStopArea(
								itGleis->second.find(
									stopCode
							)	);
							if(itStopArea != itGleis->second.end())
							{
								itStop->gleisCode = itStopArea->second;
							}
						}
						else if (_noGleisFile)
						{
							// Creation of the _gleis here
							_gleisMap.insert(
								make_pair(
									boost::make_tuple(
										itZug->number,
										itZug->lineNumber,
										0
									),
									GleisMap::mapped_type()
							)	);
						}
					}
				}

				return _importObjects();
			}

			return true;
		}



		util::ParametersMap HafasFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap pm(PTDataCleanerFileFormat::_getParametersMap());

			// Show stops only
			pm.insert(PARAMETER_SHOW_STOPS_ONLY, _showStopsOnly);

			// Wayback bit position
			pm.insert(PARAMETER_WAYBACK_BIT_POSITION, _wayBackBitPosition);

			// Import full services
			if(_importFullServices)
			{
				pm.insert(PARAMETER_IMPORT_FULL_SERVICES, _importFullServices);
			}

			// Import stops
			if(_importStops)
			{
				pm.insert(PARAMETER_IMPORT_STOPS, _importStops);
			}

			// Lines filter
			pm.insert(PARAMETER_LINES_FILTER, LinesFilterToString(_linesFilter));

			// Gleis file format
			pm.insert(PARAMETER_GLEIS_HAS_ONE_STOP_PER_LINE, _gleisHasOneStopPerLine);

			// String to complete empty stop area name
			pm.insert(PARAMETER_COMPLETE_EMPTY_STOP_AREA_NAME, _complete_empty_stop_area_name);

			// No gleis file
			pm.insert(PARAMETER_NO_GLEIS_FILE, _noGleisFile);

			// try to read lin short name
			pm.insert(PARAMETER_TRY_TO_READ_LINE_SHORT_NAME, _tryToReadShortName);

			// concatenate transport and short name
			pm.insert(PARAMETER_CONCATENATE_TRANSPORT_SHORT_NAME, _concatenateTransportShortName);
			
			// Read wayback
			pm.insert(PARAMETER_READ_WAYBACK, _readWayback);

			// Calendar default code
			pm.insert(PARAMETER_CALENDAR_DEFAULT_CODE, _defaultCalendarCode);
			
			// Carpostal 2015 format
			pm.insert(PARAMETER_2015_CARPOSTAL_FORMAT, _formatCarpostal2015);

			return pm;
		}



		void HafasFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& pm )
		{
			PTDataCleanerFileFormat::_setFromParametersMap(pm);

			// Show stops only
			_showStopsOnly = pm.getDefault<bool>(PARAMETER_SHOW_STOPS_ONLY, false);

			// Wayback bit position
			_wayBackBitPosition = pm.getDefault<size_t>(PARAMETER_WAYBACK_BIT_POSITION, 0);

			// Import full services
			_importFullServices = pm.getDefault<bool>(PARAMETER_IMPORT_FULL_SERVICES, false);

			// Import stops
			_importStops = pm.getDefault<bool>(PARAMETER_IMPORT_STOPS, false);

			// Import dates
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_ECKDATEN));
			if(it != _pathsMap.end())
			{
				if(!_openFile(it->second))
				{
					throw Exception("Could not open the calendar file.");
				}

				date now(day_clock::local_day());

				// Start date
				if(!_loadLine())
				{
					throw Exception("Inconsistent Eckdaten file");
				}
				_fileFirstDate = date(
					lexical_cast<int>(_getField(6,4)),
					lexical_cast<int>(_getField(3,2)),
					lexical_cast<int>(_getField(0,2))
				);

				// End date
				if(!_loadLine())
				{
					throw Exception("Inconsistent Eckdaten file");
				}
				date endDate(
					lexical_cast<int>(_getField(6,4)),
					lexical_cast<int>(_getField(3,2)),
					lexical_cast<int>(_getField(0,2))
				);

				// Import mask
				_calendar = Calendar(
					(_fileFirstDate < now && _fromToday) ? now : _fileFirstDate,
					endDate
				);
			}

			// Lines filter
			_linesFilter = getLinesFilter(pm.getDefault<string>(PARAMETER_LINES_FILTER));

			// Gleis file format
			_gleisHasOneStopPerLine = pm.getDefault<bool>(PARAMETER_GLEIS_HAS_ONE_STOP_PER_LINE, false);

			// String to complete empty stop area name
			_complete_empty_stop_area_name = pm.getDefault<string>(PARAMETER_COMPLETE_EMPTY_STOP_AREA_NAME);

			// No gleis file
			_noGleisFile = pm.getDefault<bool>(PARAMETER_NO_GLEIS_FILE, false);
			
			// Line short name
			_tryToReadShortName = pm.getDefault<bool>(PARAMETER_TRY_TO_READ_LINE_SHORT_NAME, false);

			// Concatneate transport and short name
			_concatenateTransportShortName = pm.getDefault<bool>(PARAMETER_CONCATENATE_TRANSPORT_SHORT_NAME, false);

			// Read wayback
			_readWayback = pm.getDefault<bool>(PARAMETER_READ_WAYBACK, false);

			// Calendar default code
			_defaultCalendarCode = pm.getDefault<size_t>(PARAMETER_CALENDAR_DEFAULT_CODE, 0);
			
			// Carpostal format 2015
			_formatCarpostal2015 = pm.getDefault<bool>(PARAMETER_2015_CARPOSTAL_FORMAT, false);
		}



		bool HafasFileFormat::Importer_::_openFile(
			const boost::filesystem::path& filePath
		) const	{

			// Closing last opened file
			_file.close();
			_file.clear();

			// Opening the new file
			_file.open(filePath.string().c_str());

			// Check of the file path
			if(!_file)
			{
				return false;
			}

			return true;
		}



		bool HafasFileFormat::Importer_::_loadLine() const
		{
			do
			{
				if(!getline(_file, _line))
				{
					return false;
				}
			} while(_line.empty() || _line[0]=='%' || _line[0] == ' ');

			return true;
		}



		std::string HafasFileFormat::Importer_::_getField(
			std::size_t start,
			std::size_t len
		) const	{
			if(start >= _line.size())
			{
				return string();
			}
			if(start + len >= _line.size())
			{
				return trim_copy(
					_iconv.convert(
						_line.substr(start)
				)	);
			}
			return trim_copy(
				_iconv.convert(
					_line.substr(start, len)
			)	);
		}



		std::string HafasFileFormat::Importer_::_getField(
			std::size_t start
		) const	{
			if(start >= _line.size())
			{
				return string();
			}
			return trim_copy(
				_iconv.convert(
					_line.substr(start)
			)	);
		}



		bool HafasFileFormat::Importer_::_importObjects(
		) const	{

			DataSource& dataSource(*_import.get<DataSource>());

			// Linked objects loading
			ImportableTableSync::ObjectBySource<StopPointTableSync> stopPoints(dataSource, _env);
			ImportableTableSync::ObjectBySource<StopAreaTableSync> stopAreas(dataSource, _env);
			map<pair<string, string>, set<StopPoint*> > gleisStopPoint;
			typedef map<string, City*> CitiesByName;
			CitiesByName citiesByName;

			// Stops
			if(_importStops)
			{
				// Cities
				CityTableSync::SearchResult cities(
					CityTableSync::Search(
						_env
				)	);
				BOOST_FOREACH(const boost::shared_ptr<City>& city, cities)
				{
					citiesByName.insert(make_pair(city->getName(), city.get()));
				}

				stringstream logStream;
				logStream << "Parsed " << _bahnhofs.size() << " bahnhofs";
				_logInfo(logStream.str());

				// Stop areas
				BOOST_FOREACH(const Bahnhofs::value_type& itBahnhof, _bahnhofs)
				{
					// Declaration
					const Bahnhof& bahnhof(itBahnhof.second);

					// Avoid unused stops
					if(!bahnhof.used)
					{
						string infoString = "Bahnhof " + bahnhof.operatorCode + " is unused, do not import";
						_logInfo(infoString);
						continue;
					}

					// Jump over existing stop areas
					if(stopPoints.contains(bahnhof.operatorCode))
					{
						continue;
					}

					// City
					City* city(NULL);
					if(stopAreas.get(bahnhof.operatorCode).empty())
					{
						CitiesByName::const_iterator itCity(citiesByName.find(bahnhof.cityName));
						if(itCity != citiesByName.end())
						{
							city = itCity->second;
						}
						else
						{
							city = new City(
								CityTableSync::getId(),
								bahnhof.cityName
							);
							_env.getEditableRegistry<City>().add(boost::shared_ptr<City>(city));
							citiesByName.insert(make_pair(city->getName(), city));
						}
					}

					// Search if a new stop area must be created
					set<StopArea*> stopAreasSet(
						_getStopAreas(
							stopAreas,
							bahnhof.operatorCode,
							optional<const string&>(),
							false
					)	);
					// Associated stop areas
					StopAreaMappingMap::const_iterator itMappedStopAreas(
						_stopAreaMappingMap.find(bahnhof.operatorCode)
					);
					if(stopAreasSet.empty())
					{
						// Search if a linked stop already exists
						if(itMappedStopAreas != _stopAreaMappingMap.end())
						{
							BOOST_FOREACH(const string& otherCode, itMappedStopAreas->second)
							{
								stopAreasSet = _getStopAreas(
									stopAreas,
									otherCode,
									optional<const string&>(),
									false
								);
								if(!stopAreasSet.empty())
								{
									StopArea* linkedStopArea(*stopAreasSet.begin());
									linkedStopArea->addCodeBySource(dataSource, bahnhof.operatorCode);
									stopAreas.add(*linkedStopArea);
									break;
								}
							}
						}
					}

					// A new stop area must be created ?
					if(stopAreasSet.empty())
					{
						// Main stop : search in associated areas
						bool mainStop(bahnhof.main);
						if(itMappedStopAreas != _stopAreaMappingMap.end())
						{
							BOOST_FOREACH(const string& otherCode, itMappedStopAreas->second)
							{
								mainStop |= _bahnhofs[otherCode].main;
							}
						}

						// Name : concatenate stop names
						stringstream name;
						if(itMappedStopAreas != _stopAreaMappingMap.end())
						{
							bool first(true);
							BOOST_FOREACH(const string& otherCode, itMappedStopAreas->second)
							{
								if(_bahnhofs[otherCode].name.empty())
								{
									continue;
								}
								if(first)
								{
									first = false;
								}
								else
								{
									name << " - ";
								}
								name  << _bahnhofs[otherCode].name;
							}
						}
						else
						{
							name << bahnhof.name;
						}
						if(name.str().empty())
						{
							name << "Arrêt";
						}

						// Creation
						StopArea* newStopArea(
							_createStopArea(
								stopAreas,
								bahnhof.operatorCode,
								name.str(),
								*city,
								bahnhof.defaultTransferDuration,
								mainStop,
								dataSource
						)	);

						// Links
						Importable::DataSourceLinks links;
						if(itMappedStopAreas != _stopAreaMappingMap.end())
						{
							BOOST_FOREACH(const string& otherCode, itMappedStopAreas->second)
							{
								links.insert(make_pair(&dataSource, otherCode));
							}
						}
						else
						{
							links.insert(make_pair(&dataSource, bahnhof.operatorCode));
						}
						newStopArea->setDataSourceLinksWithoutRegistration(links);
						stopAreas.add(*newStopArea);
					}
				}

				// Stops
				BOOST_FOREACH(const Bahnhofs::value_type& itBahnhof, _bahnhofs)
				{
					// Declaration
					const Bahnhof& bahnhof(itBahnhof.second);

					// Avoid unused stops
					if(!bahnhof.used)
					{
						continue;
					}

					// Stop area
					StopArea* stopArea(NULL);
					set<StopPoint*> stopPointsSet(
						stopPoints.get(bahnhof.operatorCode)
					);
					if(!stopPointsSet.empty())
					{
						stopArea = const_cast<StopArea*>((*stopPointsSet.begin())->getConnectionPlace());
					}
					else
					{
						set<StopArea*> stopAreasSet(
							stopAreas.get(bahnhof.operatorCode)
						);
						if(stopAreasSet.size() > 1)
						{
							_logWarning(
								"Multiple stop areas with code "+ bahnhof.operatorCode
							);
						}
						assert(!stopAreasSet.empty());
						stopArea = *stopAreasSet.begin();
					}

					// Gleis or not
					if(bahnhof.gleisSet.empty())
					{
						// One stop for the bahnhof
						bahnhof.stops = _createOrUpdateStop(
							stopPoints,
							bahnhof.operatorCode,
							bahnhof.name,
							boost::optional<const graph::RuleUser::Rules&>(),
							stopArea,
							bahnhof.point.get() ? optional<const Point*>(bahnhof.point.get()) : optional<const Point*>(),
							dataSource,
							true
    					);
					}
					else
					{
						set<StopPoint*> stopsSet(
							_getStopPoints(
								stopPoints,
								bahnhof.operatorCode,
								optional<const string&>(),
								false
						)	);
						BOOST_FOREACH(const string& gleis, bahnhof.gleisSet)
						{
							// Search for an existing stop with the code of the gleis
							set<StopPoint*> linkedStops;
							BOOST_FOREACH(StopPoint* stop, stopsSet)
							{
								if(stop->hasCodeBySource(dataSource, gleis))
								{
									linkedStops.insert(stop);
								}
							}

							// If not found new stop creation
							if(linkedStops.empty())
							{
								// Creation
								StopPoint* newStop(
									_createStop(
										stopPoints,
										bahnhof.operatorCode,
										gleis,
										*stopArea,
										dataSource
								)	);

								// Source links with gleis number
								newStop->addCodeBySource(dataSource, gleis);
								stopPoints.add(*newStop);

								// Position
								if(bahnhof.point.get())
								{
									newStop->setGeometry(bahnhof.point);
								}

								// Insertion in the list
								linkedStops.insert(newStop);
							}
							else // update coordinates if necessary and possible
							{
								BOOST_FOREACH(StopPoint* stop, linkedStops)
								{
									if(!stop->hasGeometry() && bahnhof.point.get())
									{
										stop->setGeometry(bahnhof.point);
									}
								}
							}

							// Registration in gleis map
							gleisStopPoint[make_pair(bahnhof.operatorCode, gleis)] = linkedStops;
						}
					}
				}
			}
			else
			{
				// TODO implement manual stop creation (call screen if possible, else log the missing links)
				assert(false);
				return false;
			}

			// Services
			ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(dataSource, _env);
			ImportableTableSync::ObjectBySource<RollingStockTableSync> transportModes(dataSource, _env);

			stringstream logStream;
			logStream << "Parsed " << _zugs.size() << " zugs";
			_logInfo(logStream.str());

			BOOST_FOREACH(const Zug& zug, _zugs)
			{
				// Line
				CommercialLine* line(NULL);
				line = _createOrUpdateLine(
					lines,
					zug.lineNumber,
					optional<const string&>(),
					optional<const string&>(),
					optional<RGBColor>(),
					*zug.lineFilter->network,
					dataSource,
					zug.lineFilter->linesByStopsPair || (zug.lineFilter->lineNumberStart && zug.lineFilter->lineNumberEnd)
				);
				if(!line)
				{
					logStream.str("");
					logStream << "Failed to create or update line for zug #" << zug.lineNumber;
					_logError(logStream.str());
					continue;
				}

				// Update of the name with the code or with the stops pair if nothing else is defined
				if(line->getName().empty())
				{
					if(zug.lineFilter->linesByStopsPair)
					{
						Bahnhofs::const_iterator it1(
							_bahnhofs.find(zug.stops.begin()->stopCode)
						);
						Bahnhofs::const_iterator it2(
							_bahnhofs.find(zug.stops.rbegin()->stopCode)
						);
						if(	it1 != _bahnhofs.end() &&
							it2 != _bahnhofs.end()
						){
							line->setName(
								it1->second.cityName +" "+ it1->second.name +
								" - "+
								it2->second.cityName +" "+ it2->second.name
							);
						}
						line->setShortName(zug.transportModeCode);
					}
					else
					{
						line->setName(zug.lineNumber);
					}
				}

				// Update the short name if possible
				if (_tryToReadShortName && !zug.lineShortName.empty())
				{
					if (_concatenateTransportShortName)
					{
						line->setShortName(zug.transportModeCode + zug.lineShortName);
					}
					else
					{
						line->setShortName(zug.lineShortName);
					}
				}

				// Wayback
				int numericServiceNumber(0);
				try
				{
					numericServiceNumber = lexical_cast<int>(zug.number.substr(zug.number.size() - 1 - _wayBackBitPosition, 1));
				}
				catch(bad_lexical_cast&)
				{
				}
				bool wayBack = (numericServiceNumber % 2 == 1);
				
				// wayBack is overwritten if read in file
				if (_readWayback)
				{
					wayBack = zug.readWayback;
				}

				// Transport mode (can be NULL)
				RollingStock* transportMode(
					_getTransportMode(
						transportModes,
						zug.transportModeCode
				)	);

				// Calendars
				for(size_t i(zug.calendars.size()); i>0; --i)
				{
					for(size_t n(0); n<zug.calendars.size()-i+1; ++n)
					{
						Calendar mergedCalendar;
						for(size_t m(n); m<n+i; ++m)
						{
							if(m == n)
							{
								mergedCalendar = _calendarMap[zug.calendars[m].calendarNumber];
							}
							else
							{
								mergedCalendar &= _calendarMap[zug.calendars[m].calendarNumber];
							}
						}
						if(n>0)
						{
							mergedCalendar -= _calendarMap[zug.calendars[n-1].calendarNumber];
						}
						if(n+i < zug.calendars.size())
						{
							mergedCalendar -= _calendarMap[zug.calendars[n+i].calendarNumber]; 
						}
						mergedCalendar &= _calendar;

						// Jump over useless combinations
						if(mergedCalendar.empty() && !_importEvenIfNoDate)
						{
							continue;
						}

						// Route bounds
						string startStopCode = zug.calendars[n].startStopCode;
						string endStopCode = zug.calendars[n+i-1].endStopCode;

						// Stops and schedules
						ScheduledService::Schedules departures;
						ScheduledService::Schedules arrivals;
						JourneyPattern::StopsWithDepartureArrivalAuthorization stops;
						bool ignoreService(false);
						bool inStop(false);
						bool firstStop(true);
						BOOST_FOREACH(const Zug::Stop& zugStop, zug.stops)
						{
							// Jump over stops before the first stop
							if(zugStop.stopCode == startStopCode)
							{
								inStop = true;
							}
							if(!inStop)
							{
								continue;
							}

							// Stop link
							JourneyPattern::StopWithDepartureArrivalAuthorization stop(
								zugStop.gleisCode.empty() ? stopPoints.get(zugStop.stopCode) : gleisStopPoint[make_pair(zugStop.stopCode, zugStop.gleisCode)],
								boost::optional<graph::MetricOffset>(),
								!zugStop.departureTime.is_not_a_date_time(),
								!zugStop.arrivalTime.is_not_a_date_time()
							);

							// Check if at least a stop was found
							if(stop._stop.empty())
							{
								_logWarning(
									"The stop "+ zugStop.stopCode +"/"+ zugStop.gleisCode +" was not found : the service "+ zug.number +"/"+ zug.lineNumber +" is ignored"
								);
								ignoreService = true;
								break;
							}

							// Storage
							stops.push_back(stop);

							// Schedules

							departures.push_back(zugStop.departureTime.is_not_a_date_time() ? zugStop.arrivalTime : zugStop.departureTime);
							arrivals.push_back(zugStop.arrivalTime.is_not_a_date_time() ? zugStop.departureTime : zugStop.arrivalTime);

							// End of the route
							if(!firstStop && inStop && zugStop.stopCode == endStopCode)
							{
								break;
							}

							firstStop = false;
						}
						if(ignoreService)
						{
							if(_importFullServices)
							{
								continue;
							}
							else
							{
								return false;
							}
						}

						// Journey pattern
						JourneyPattern* route(
							_createOrUpdateRoute(
								*line,
								optional<const string&>(),
								optional<const string&>(),
								optional<const string&>(),
								optional<Destination*>(),
								optional<const RuleUser::Rules&>(),
								wayBack,
								transportMode,
								stops,
								dataSource,
								true,
								true,
								true,
								true
						)	);

						// Service
						SchedulesBasedService* service(NULL);
						if(	zug.continuousServiceRange.is_not_a_date_time() ||
							zug.continuousServiceRange.total_seconds() == 0
						){
							service = _createOrUpdateService(
								*route,
								departures,
								arrivals,
								zug.number,
								dataSource,
								optional<const string&>(),
								optional<const RuleUser::Rules&>(),
								optional<const JourneyPattern::StopsWithDepartureArrivalAuthorization&>(stops),
								zug.number
							);
						}
						else
						{
							service = _createOrUpdateContinuousService(
								*route,
								departures,
								arrivals,
								zug.number,
								zug.continuousServiceRange,
								zug.continuousServiceWaitingTime,
								dataSource
							);
						}

						// Calendar
						if(service)
						{
							*service |= mergedCalendar;
						}
					}
				}
			}

			return true;
		}



		const HafasFileFormat::LineFilter* HafasFileFormat::Importer_::_lineIsIncluded(
			const std::string& lineNumber
		) const {

			// No filter = import nothing
			if(_linesFilter.empty())
			{
				string errorString = "Line filter is empty";
				_logError(errorString);
				return NULL;
			}

			// Exact matching
			LinesFilter::const_iterator it(
				_linesFilter.find(lineNumber)
			);
			if( it != _linesFilter.end())
			{
				return &it->second;
			}

			// Pattern matching
			BOOST_FOREACH(const LinesFilter::value_type& item, _linesFilter)
			{
				// Pattern
				const string& pattern(item.first);

				// Filter analysis
				if(	pattern[pattern.size()-1] == LineFilter::JOCKER[0] &&
					(	pattern.size() == 1 ||
						(	lineNumber.size() >= pattern.size()-1 &&
							pattern.substr(0, pattern.size()-1) == lineNumber.substr(0, pattern.size()-1)
				)	)	){
					return &item.second;
				}
			}

			string infoString = "Zug " + lineNumber + " does not match line filters";
			_logInfo(infoString);

			return NULL;
		}



		HafasFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	impex::Importer(env, import, minLogLevel, logPath, outputStream, pm),
			impex::MultipleFileTypesImporter<HafasFileFormat>(env, import, minLogLevel, logPath, outputStream, pm),
			PTDataCleanerFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			PTFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			_networks(*import.get<DataSource>(), env),
			_showStopsOnly(false),
			_wayBackBitPosition(0),
			_importFullServices(false),
			_importStops(false),
			_iconv(
				import.get<DataSource>()->get<Charset>().empty() ?
					string("UTF-8") :
					import.get<DataSource>()->get<Charset>(),
				"UTF-8"
			)
		{}

		//////////////////////////////////////////////////////////////////////////

		// **** HAFAS EXPORTER ****

		const string HafasFileFormat::Exporter_::FILE_BAHNHOF = "BAHNHOF";
		const string HafasFileFormat::Exporter_::FILE_KOORD = "BFKOORD";
		const string HafasFileFormat::Exporter_::FILE_ZUGDAT = "FPLAN";
		const string HafasFileFormat::Exporter_::FILE_BITFIELD = "BITFELD";
		const string HafasFileFormat::Exporter_::FILE_ECKDATEN = "ECKDATEN";
		const string HafasFileFormat::Exporter_::FILE_ZUGART = "ZUGART";
		const string HafasFileFormat::Exporter_::FILE_UMSTEIGB = "UMSTEIGB";

		const string HafasFileFormat::Exporter_::PARAMETER_DEBUG = "debug";
		const string HafasFileFormat::Exporter_::PARAMETER_NETWORK_NAME = "network";
		const string HafasFileFormat::Exporter_::PARAMETER_MAIN_DATA_SOURCE = "main_ds";
		const string HafasFileFormat::Exporter_::PARAMETER_STOPS_DATA_SOURCE = "stops_ds";
		const string HafasFileFormat::Exporter_::PARAMETER_BITFIELD_START_DATE = "bitfield_start_date";
		const string HafasFileFormat::Exporter_::PARAMETER_BITFIELD_END_DATE = "bitfield_end_date";
		const string HafasFileFormat::Exporter_::PARAMETER_TIMETABLE_NAME = "timetable_name";
		const string HafasFileFormat::Exporter_::PARAMETER_FTP_HOST = "ftp_host";
		const string HafasFileFormat::Exporter_::PARAMETER_FTP_PORT = "ftp_port";
		const string HafasFileFormat::Exporter_::PARAMETER_FTP_USER = "ftp_user";
		const string HafasFileFormat::Exporter_::PARAMETER_FTP_PASS = "ftp_pass";
		const string HafasFileFormat::Exporter_::PARAMETER_FTP_PATH = "ftp_path";

		const string HafasFileFormat::Exporter_::OUTWARD_TRIP_CODE = "H";
		const string HafasFileFormat::Exporter_::RETURN_TRIP_CODE = "R";
		const string HafasFileFormat::Exporter_::DAILY_SERVICE_CODE = "000000";
		const string HafasFileFormat::Exporter_::NO_DIDOK = "NODIDOK";
		const unsigned int HafasFileFormat::Exporter_::MAX_BITFIELD_SIZE = 48;
		// 21781 = Swiss format (CH1903), 4326 = GPS format (WGS84)
		const unsigned int HafasFileFormat::Exporter_::COORDINATES_SYSTEM = 4326;


		HafasFileFormat::Exporter_::Exporter_(const impex::Export& export_): OneFileExporter<HafasFileFormat>(export_), _debug(false), _ftpPort(21) {
		}

		void HafasFileFormat::Exporter_::setFromParametersMap(const ParametersMap& map)
		{
			_debug = map.getDefault<bool>(PARAMETER_DEBUG, false);

			_networkName = getMandatoryString(map, PARAMETER_NETWORK_NAME);

			_mainDataSource = getMandatoryString(map, PARAMETER_MAIN_DATA_SOURCE);
			_stopsDataSource = getMandatoryString(map, PARAMETER_STOPS_DATA_SOURCE);

			_ftpHost = getMandatoryString(map, PARAMETER_FTP_HOST);
			_ftpPort = map.getDefault<int>(PARAMETER_FTP_PORT, 21);
			_ftpUser = getMandatoryString(map, PARAMETER_FTP_USER);
			_ftpPass = getMandatoryString(map, PARAMETER_FTP_PASS);
			_ftpPath = getMandatoryString(map, PARAMETER_FTP_PATH);

			_bitfieldStartDate = getMandatoryString(map, PARAMETER_BITFIELD_START_DATE);
			_bitfieldEndDate = getMandatoryString(map, PARAMETER_BITFIELD_END_DATE);

			_timetableName = getMandatoryString(map, PARAMETER_TIMETABLE_NAME);

		}

		void HafasFileFormat::Exporter_::build(ostream& os) const {

			// TODO : Only store HTML in the stream when _debug = true to improve performances!
			stringstream html;
			html << "<html><body>\n";

			const CoordinatesSystem& coordinatesSytem = CoordinatesSystem::GetCoordinatesSystem(COORDINATES_SYSTEM);

			// Force loading of some entities in order to be able to access their fields
			DataSourceTableSync::Search(_env);

			// We validate the dates
			boost::gregorian::date startDate(from_simple_string(_bitfieldStartDate));
			boost::gregorian::date endDate(from_simple_string(_bitfieldEndDate));
			if (startDate > endDate) {
				os << "bitfield_start_date cannot be more recent than bitfield_end_date!" << flush;
				return;
			}
			int maxDaysPerBitfield = (MAX_BITFIELD_SIZE * 2 - 1) * 4;
			if ((endDate - startDate).days() > maxDaysPerBitfield) {
				os << (boost::format("Cannot store more than %1% days in a bitfield!") % maxDaysPerBitfield).str() << flush;
				return;
			}

			// We store the data we crawled in those objects
			Bahnhofs _bahnhofs;
			Zugs _zugs;
			BitFields _bitfields;
			Zugarts _zugarts;

			// ** Rolling stocks **
			RollingStockTableSync::SearchResult rsRes = RollingStockTableSync::Search(_env);
			html << "<h1>Modes de transport</h1>\n";
			html << "<ul>\n";
			BOOST_FOREACH(const boost::shared_ptr<RollingStock>& rs, rsRes) {
				// Note : We only list the rolling stock who have a code for the data source to avoid unused cluterring !
				string transportModeCode = getCodesForDataSource(rs.get(), _mainDataSource, string(), false);
				if (!transportModeCode.empty()) {
					html << "<li>" << transportModeCode << "</li>\n";
					_zugarts.insert(transportModeCode);
				}
			}
			html << "</ul>\n";

			// ** Stop area **
			html << "<h1>Lieux</h1>\n";
			html << "<ul>\n";
			StopPointTableSync::Search(_env); // lazy-loading
			StopAreaTableSync::SearchResult stopsRes = StopAreaTableSync::Search(_env);
			BOOST_FOREACH(const boost::shared_ptr<StopArea>& stopArea, stopsRes) {
				string didokCode = getCodesForDataSource(stopArea.get(), _stopsDataSource, NO_DIDOK);
				html << "<li>" << stopArea->getFullName() << " <small>(DIDOK: " + didokCode + ", key: " << stopArea->getKey() << ")</small></li>\n";
				html << "<ul>\n";

				// ** Stop points for this stop area **
				map<util::RegistryKeyType,const pt::StopPoint*> stopPoints = stopArea->getPhysicalStops();

				double minX = numeric_limits<double>::max(), minY = numeric_limits<double>::max();
				double maxX = numeric_limits<double>::min(), maxY = numeric_limits<double>::min();
				for (map<util::RegistryKeyType,const pt::StopPoint*>::const_iterator it = stopPoints.begin(); it != stopPoints.end(); ++it)
				{
					const StopPoint* stopPoint = it->second;
					ParametersMap pm;
					stopPoint->toParametersMap(pm, false, coordinatesSytem);
					if (pm.isDefined("x") && pm.isDefined("y")) {
						string xStr = pm.getValue("x");
						string yStr = pm.getValue("y");
						double x = atof(xStr.c_str());
						double y = atof(yStr.c_str());
						if (x > maxX) {
							maxX = x;
						}
						if (x < minX) {
							minX = x;
						}
						if (y > maxY) {
							maxY = y;
						}
						if (y < minY) {
							minY = y;
						}
						// TODO : Support for Z
						html << "<li>(Est,Nord,Z) = (" << xStr << "," << yStr << ",0)</li>\n";
					}
				}

				// TODO : Document that if no coordinates, (0,0,0) will be set.
				double x = 0;
				double y = 0;
				if (minX != numeric_limits<double>::max() &&
					minY != numeric_limits<double>::max() &&
					maxX != numeric_limits<double>::min() &&
					maxY != numeric_limits<double>::min()) {
					x = (maxX - minX)/2 + minX;
					y = (maxY - minY)/2 + minY;
				}

				html << "<li><b>Point central </b> : (Est,Nord,Z) = (" << x << "," << y << ",0)</li>\n";
				html << "</ul>\n";

				Bahnhof bahnhof;
				bahnhof.operatorCode = didokCode;
				bahnhof.point = coordinatesSytem.createPoint(x, y);
				// Note : Name contains the city name too! To only get the stop name, use getName() instead.
				bahnhof.name = stopArea->getFullName();
				bahnhof.cityName = stopArea->getCity()->getName();
				_bahnhofs.insert(make_pair(
									didokCode,
									bahnhof
								));
			}
			html << "</ul>\n";

			// ** Network **
			TransportNetworkTableSync::SearchResult networksRes =
					TransportNetworkTableSync::Search(_env, _networkName, "", 0, boost::optional<size_t>(),
							true, true, util::UP_LINKS_LOAD_LEVEL);
			if(networksRes.empty()) {
				os << "No such network with name " << _networkName << flush;
				return;
			}
			TransportNetwork* network = (*(networksRes.begin())).get();

			// We obtain the code of this network
			string networkCode = getCodesForDataSource(network, _mainDataSource);
			if (networkCode.empty()) {
				os << "Network " << _networkName << " doesn't have a code for data source " << _mainDataSource << flush;
				return;
			}

			html << "<h1>Réseau " << network->getName() << " <small>(code:" << networkCode << ", key: " << network->getKey() << ")</small></h1>\n";

			// ** Lines for this network **
			int trainNumber = 1;
			CommercialLineTableSync::SearchResult linesRes =
					CommercialLineTableSync::Search(_env, network->getKey());
			BOOST_FOREACH(const boost::shared_ptr<CommercialLine>& line, linesRes) {
				string lineShortName = line->getShortName();
				html << "<h2>Ligne " << lineShortName << " : " << line->getName()
						<< " <small>(key: " << line->getKey() << ")</small></h2>\n";

				// ** Journeys for this line **
				JourneyPatternTableSync::SearchResult journeysRes = JourneyPatternTableSync::Search(_env, line->getKey());
				BOOST_FOREACH(const boost::shared_ptr<JourneyPattern>& journey, journeysRes) {

					// We get the code of this transport mode
					// TODO : Mention in the documentation that the rolling stock must have a code for the data source !!!
					RollingStock* rs = journey->getRollingStock();
					string transportModeCode = getCodesForDataSource(rs, _mainDataSource);

					ScheduledServiceTableSync::SearchResult schServiceRes =
							ScheduledServiceTableSync::Search(_env, journey->getKey(), line->getKey());
					ContinuousServiceTableSync::SearchResult conServiceRes =
							ContinuousServiceTableSync::Search(_env, journey->getKey(), line->getKey());
					LineStopTableSync::SearchResult lineStopRes = LineStopTableSync::Search(_env, journey->getKey());

					const StopPoint* from = journey->getOrigin();
					const StopPoint* to = journey->getDestination();

					// We read the DIDOK code of the StopArea containing this StopPoint
					string fromCode = getCodesForDataSource(from->getConnectionPlace(), _stopsDataSource, NO_DIDOK);
					string toCode = getCodesForDataSource(to->getConnectionPlace(), _stopsDataSource, NO_DIDOK);

					bool wayBack = journey->getWayBack();
					html << "<h3>Parcours " << (wayBack ? "retour" : "aller")
							<< " \"" << journey->getName() << "\" :"
							<< " Origine " << (from != 0 ? from->getName() + " (DIDOK: " + fromCode + ")" : "?") << ", "
							<< " Destination " << (to != 0 ? to->getName() + " (DIDOK: " + toCode + ")": "?") << ", "
							<< " Mode de transport: " << transportModeCode
							<< " <small>(key: " << journey->getKey() << ")</small></h3>\n";

					// ** Routes for this journey **
					html << "<h4>Desserte</h4>\n";
					html << "<ul>\n";
					vector<pair<string, string> > lineStops;
					BOOST_FOREACH(const boost::shared_ptr<LineStop>& lineStop, lineStopRes) {
						BOOST_FOREACH(const boost::shared_ptr<synthese::pt::LinePhysicalStop>& ls, lineStop->getGeneratedLineStops()) {
							StopPoint* stop = ls->getPhysicalStop();
							string didok = getCodesForDataSource(stop->getConnectionPlace(), _stopsDataSource, NO_DIDOK);
							html << "<li>" << stop->getName() << " <small>(didok: " << didok << ", key: " << stop->getKey() << ")</small></li>\n";
							lineStops.push_back(make_pair(stop->getConnectionPlace()->getFullName(), didok));
						}
					}
					html << "</ul>\n";

					// ** Calendar **
					html << "<h4>Calendrier</h4>\n";
					calendar::Calendar& calendar = journey->getCalendarCache();
					Calendar::DatesVector activeDates = calendar.getActiveDates();
					html << "<p>\n";
					BOOST_FOREACH(const boost::gregorian::date date, activeDates) {
						html << date << " ";
					}
					html << "</p>\n";

					// We compute the active days in the year we're interested in
					vector<Zug::CalendarUse> calendars;
					boost::gregorian::date_duration dd(1);
					size_t bitfieldDaysCounter = 0;
					date currentDate = startDate;
					date lastDate = endDate;
					while (currentDate != lastDate) {
						// If this day is one of the active days...
						if (find(activeDates.begin(), activeDates.end(), currentDate) != activeDates.end()) {
							// We add it
							Zug::CalendarUse calendarUse;
							calendarUse.calendarNumber = bitfieldDaysCounter;
							calendars.push_back(calendarUse);
						}
						// We increment the day and the counter
						currentDate += dd;
						bitfieldDaysCounter++;
					}

					// ** Scheduled service for this journey **
					bool title = false;
					BOOST_FOREACH(const boost::shared_ptr<ScheduledService>& schService, schServiceRes) {

						// We compute the train number
						char trainNumberBuffer[5];
						sprintf(trainNumberBuffer, "%05d", trainNumber++);
						string trainNumberStr(trainNumberBuffer);

						if (!title) {
							html << "<h4>Services à horaire</h4>\n";
							html << "<ul>\n";
							title = true;
						}
						html << "<li>" << schService->getDepartureSchedule(false, 0) << " - "
								<< schService->getLastArrivalSchedule(false) << "</li>\n";

						// We grab the schedule for each stops
						Zug::Stops stops;
						html << "<ul>\n";
						for (unsigned int i=0; i<lineStops.size(); i++) {
							Zug::Stop stop;
							stop.stopCode = lineStops[i].second;
							stop.stopName = lineStops[i].first;
							// true --> real time (generated) schedule!
							if (i > 0) {
								stop.arrivalTime = schService->getArrivalSchedule(true, i);
							}
							if (i < lineStops.size() - 1) {
								stop.departureTime = schService->getDepartureSchedule(true, i);
							}
							stops.push_back(stop);

							html << "<li>" << lineStops[i].first << " <small>(didok: " << stop.stopCode << ")</small> : "
									<< stop.arrivalTime << " -> "
									<< stop.departureTime << "</li>\n";
						}
						html << "</ul>\n";


						// We set the fields
						Zug zug;
						zug.number = trainNumberStr;
						zug.lineNumber = networkCode;
						zug.transportModeCode = transportModeCode;
						zug.stops = stops;
						zug.lineShortName = lineShortName;
						zug.readWayback = wayBack;
						zug.calendars = calendars;
						zug.bitfieldCode = getBitfieldCode(calendars, bitfieldDaysCounter, _bitfields);

						_zugs.push_back(zug);

					}
					if (title) {
						html << "</ul>\n";
					}

					// ** Continuous services for this journey (not currently supported by export!) **
					title = false;
					BOOST_FOREACH(const boost::shared_ptr<ContinuousService>& conService, conServiceRes) {
						if (!title) {
							html << "<h4>Services continus</h4>\n";
							html << "<ul>\n";
							title = true;
						}
						html << "<li>" << conService->getDepartureSchedule(false, 0) << " - "
								<< conService->getLastArrivalSchedule(false) << "</li>\n";
					}
					if (title) {
						html << "</ul>\n";
					}

				}

			}

			html << "</body></html>\n" << flush;

			if (_debug) {

				// In debug mode, we don't export anything
				os << html.str();

			} else {

				// In production mode, we run the actual export
				ExportConfiguration config;
				config.zugs = _zugs;
				config.bahnhofs = _bahnhofs;
				config.bitfields = _bitfields;
				config.zugarts = _zugarts;
				config.startDate = from_simple_string(_bitfieldStartDate);
				config.endDate = from_simple_string(_bitfieldEndDate);
				config.timetableName = _timetableName;

				try {

					// We generate the HAFAS files
					path hafasExportFolder = exportToHafasFormat(config);

					// We zip them
					path hafasZip = zipFolder(hafasExportFolder);

					// We delete the temporary folder
					remove_all(hafasExportFolder);

					// We send archive to FTP server
					ftpUpload(_ftpHost, _ftpPort, _ftpUser, _ftpPass, _ftpPath, hafasZip);

					// We delete the archive
					remove(hafasZip);

					os << "OK" << flush;

				} catch (std::exception& e) {

					os << e.what() << flush;
					return;

				}

			}

		}

		path HafasFileFormat::Exporter_::exportToHafasFormat(ExportConfiguration config) const
		{
			path dir = createRandomFolder();
			exportToBahnhofFile(dir, FILE_BAHNHOF, config.bahnhofs);
			exportToKoordFile(dir, FILE_KOORD, config.bahnhofs); // a.k.a. BFKOORD
			exportToZugdatFile(dir, FILE_ZUGDAT, config.zugs); // a.k.a. FPLAN
			exportToBitfieldFile(dir, FILE_BITFIELD, config.bitfields); // a.k.a. BITFELD
			exportToEckdatenFile(dir, FILE_ECKDATEN, config.startDate, config.endDate, config.timetableName);
			exportToZugartFile(dir, FILE_ZUGART, config.zugarts);
			exportToUmsteigbFile(dir, FILE_UMSTEIGB);
			return dir;
		}

		void HafasFileFormat::Exporter_::exportToBahnhofFile(path dir, string file, Bahnhofs _bahnhofs) const
		{
			ofstream fileStream;
			createFile(fileStream, dir, file);
			int pos = 1;
			BOOST_FOREACH(Bahnhofs::value_type &bahnhof, _bahnhofs) {
				// "Die Nummer der Haltestelle" (DIDOK Code)
				printColumn(fileStream, pos, bahnhof.first, 1, 7);
				// "Haltestellenname"
				printColumn(fileStream, pos, bahnhof.second.name, 13, 513);
				newLine(fileStream, pos);
			}
			fileStream.close();
		}

		void HafasFileFormat::Exporter_::exportToKoordFile(path dir, string file, Bahnhofs _bahnhofs) const
		{
			ofstream fileStream;
			createFile(fileStream, dir, file);
			int pos = 1;
			BOOST_FOREACH(Bahnhofs::value_type &bahnhof, _bahnhofs) {
				// "Die Nummer der Haltestelle" (DIDOK Code)
				printColumn(fileStream, pos, bahnhof.first, 1, 7);
				// "X-Koordinate"
				printColumn(fileStream, pos, (boost::format("%10.6f") % bahnhof.second.point->getX()).str(), 9, 18);
				// "Y-Koordinate"
				printColumn(fileStream, pos, (boost::format("%10.6f") % bahnhof.second.point->getY()).str(), 20, 29);
				// TODO : "Z-Koordinate"
				printColumn(fileStream, pos, "0", 31, 36);
				// "Haltestellenname"
				printColumn(fileStream, pos, "% " + bahnhof.second.name, 38);
				newLine(fileStream, pos);
			}
			fileStream.close();
		}

		void HafasFileFormat::Exporter_::exportToZugdatFile(path dir, string file, Zugs _zugs) const
		{
			ofstream fileStream;
			createFile(fileStream, dir, file);
			int pos = 1;

			BOOST_FOREACH(Zug &zug, _zugs) {

				int commentLine = 1;
				stringstream comment;
				comment << zug.number << " " << zug.lineNumber;

				// *** *Z row ***
				printColumn(fileStream, pos, "*Z", 1, 2);
				// "Fahrtnummer"
				printColumn(fileStream, pos, zug.number, 4, 8);
				// "Verwaltung"
				printColumn(fileStream, pos, zug.lineNumber, 10, 15);
				// "Leer"
				printColumn(fileStream, pos, "", 17, 21);
				// "(optional) Taktanzhalt"
				printColumn(fileStream, pos, "", 23, 25);
				// "(optional) Taktzeit in Minuten"
				printColumn(fileStream, pos, "", 27, 29);
				// "Kommentar"
				printZugdatComment(fileStream, pos, commentLine, comment.str());
				newLine(fileStream, pos);

				//  *** *G row ***
				printColumn(fileStream, pos, "*G", 1, 2);
				// "Verkehrsmittel bzw. Gattung"
				printColumn(fileStream, pos, zug.transportModeCode, 4, 6);
				// "(optional) Laufwegsindex oder Haltestellennummber, ab der die Gattung gilt."
				printColumn(fileStream, pos, zug.stops.empty() ? string() : zug.stops.front().stopCode, 8, 14);
				// "(optional) Laufwegsindex oder Haltestellennummber, bis zu der die Gattung gilt."
				printColumn(fileStream, pos, zug.stops.empty() ? string() : zug.stops.back().stopCode, 16, 22);
				// "(optional) Index für das x. Auftreten oder Abfahrtszeitpunkt."
				printColumn(fileStream, pos, zug.stops.empty() ? string() : formatTime(zug.stops.front().departureTime), 24, 29);
				// "(optional) Index für das x. Auftreten oder Ankunftszeitpunkt."
				printColumn(fileStream, pos, zug.stops.empty() ? string() : formatTime(zug.stops.back().arrivalTime), 31, 36);
				// "Kommentar"
				printZugdatComment(fileStream, pos, commentLine, comment.str());
				newLine(fileStream, pos);

				// *** *A VE row ***
				printColumn(fileStream, pos, "*A", 1, 2);
				printColumn(fileStream, pos, "VE", 4, 5);
				// "(optional) Laufwegsindex oder Haltestellennummer, ab der die Verkehrstage im Laufweg gelten."
				printColumn(fileStream, pos, zug.stops.empty() ? string() : zug.stops.front().stopCode, 7, 13);
				// "(optional) Laufwegsindex oder Haltestellennummer, bis zu der die Verkehrstage im Laufweg gelten."
				printColumn(fileStream, pos, zug.stops.empty() ? string() : zug.stops.back().stopCode, 15, 21);
				// "(optional) Verkehrstagenummer für die Tage, an denen die Fahrt stattfindet. Fehlt diese Angabe, so verkehrt diese Fahrt täglich (entspricht dann 000000)."
				printColumn(fileStream, pos, zug.bitfieldCode, 23, 28);
				// "(optional) Index für das x. Auftreten oder Abfahrtszeitpunkt."
				printColumn(fileStream, pos, zug.stops.empty() ? string() : formatTime(zug.stops.front().departureTime), 30, 35);
				// "(optional) Index für das x. Auftreten oder Ankunftszeitpunkt."
				printColumn(fileStream, pos, zug.stops.empty() ? string() : formatTime(zug.stops.back().arrivalTime), 37, 42);
				// "Kommentar"
				printZugdatComment(fileStream, pos, commentLine, comment.str());
				newLine(fileStream, pos);

				// *** *L row ***
				printColumn(fileStream, pos, "*L", 1, 2);
				// "Liniennummer"
				printColumn(fileStream, pos, zug.lineShortName, 4, 11);
				// "(optional) Laufwegsindex oder Haltestellennummer, ab der die Liniennummer gilt."
				printColumn(fileStream, pos, zug.stops.empty() ? string() : zug.stops.front().stopCode, 13, 19);
				// "(optional) Laufwegsindex oder Haltestellennummer, bis zu der die Liniennummer gilt."
				printColumn(fileStream, pos, zug.stops.empty() ? string() : zug.stops.back().stopCode, 21, 27);
				// "(optional) Index für das x. Auftreten oder Abfahrtszeitpunkt."
				printColumn(fileStream, pos, zug.stops.empty() ? string() : formatTime(zug.stops.front().departureTime), 29, 34);
				// "(optional) Index für das x. Auftreten oder Ankunftszeitpunkt."
				printColumn(fileStream, pos, zug.stops.empty() ? string() : formatTime(zug.stops.back().arrivalTime), 36, 41);
				// "Kommentar"
				printZugdatComment(fileStream, pos, commentLine, comment.str());
				newLine(fileStream, pos);

				// *** *R row ***
				printColumn(fileStream, pos, "*R", 1, 2);
				// (optional) Kennung für Richtung (0 = Hin, 1= Rück). Diese Kennung wird für zusätzliche Angaben wie z.B. linien- und richtungsbezogene Umsteigezeiten benutzt.
				printColumn(fileStream, pos, zug.readWayback ? RETURN_TRIP_CODE : OUTWARD_TRIP_CODE, 4, 4);
				// (optional) Richtungscode. Wird kein Code vermerkt, so wird der Bahnhofsname als Richtungscode verwendet.
				printColumn(fileStream, pos, "", 6, 12);
				// (optional) Laufwegsindex oder Haltestellennummer, ab der die Richtungsangabe im Laufweg gilt.
				printColumn(fileStream, pos, zug.stops.empty() ? string() : zug.stops.front().stopCode, 14, 20);
				// (optional) Laufwegsindex oder Haltestellennummer, bis zu der die Richtungsangabe im Laufweg gilt.
				printColumn(fileStream, pos, zug.stops.empty() ? string() : zug.stops.back().stopCode, 22, 28);
				// (optional) Index für das x. Auftreten oder Abfahrtszeitpunkt.
				printColumn(fileStream, pos, zug.stops.empty() ? string() : formatTime(zug.stops.front().departureTime), 30, 35);
				// (optional) Index für das x. Auftreten oder Ankunftszeitpunkt.
				printColumn(fileStream, pos, zug.stops.empty() ? string() : formatTime(zug.stops.back().arrivalTime), 37, 42);
				// "Kommentar"
				printZugdatComment(fileStream, pos, commentLine, comment.str());
				newLine(fileStream, pos);

				// *** Timetable rows ***
				BOOST_FOREACH(Zug::Stop &stop, zug.stops) {
					// "Haltestellennummer"
					printColumn(fileStream, pos, stop.stopCode, 1, 7);
					// "(optional) Haltestellenname"
					printColumn(fileStream, pos, stop.stopName, 9, 29);
					// "Ankunftszeit an der Haltestelle (lt. Ortszeit der Haltestelle)."
					printColumn(fileStream, pos, formatTime(stop.arrivalTime), 30, 35);
					// "Abfahrtszeit an der Haltestelle (lt. Ortszeit der Haltestelle)."
					printColumn(fileStream, pos, formatTime(stop.departureTime), 37, 42);
					// "(optional) Ab dem Halt gültige Fahrtnummer."
					printColumn(fileStream, pos, "", 44, 48);
					// "(optional) Ab dem Halt gültige Verwaltung."
					printColumn(fileStream, pos, "", 50, 55);
					// "(optional) „X“, falls diese Haltestelle auf dem Laufschild der Fahrt aufgeführt wird."
					printColumn(fileStream, pos, "", 57, 57);
					// "Kommentar"
					printZugdatComment(fileStream, pos, commentLine, comment.str());
					newLine(fileStream, pos);
				}
			}

			fileStream.close();
		}

		void HafasFileFormat::Exporter_::exportToBitfieldFile(path dir, string file, BitFields _bitfields) const
		{
			ofstream fileStream;
			createFile(fileStream, dir, file);
			int pos = 1;
			BOOST_FOREACH(BitFields::value_type &row, _bitfields) {
				// Bit field number
				printColumn(fileStream, pos, row.first, 1, 6);
				// Bit field
				printColumn(fileStream, pos, row.second, 8/*, 103*/);
				newLine(fileStream, pos);
			}
			fileStream.close();
		}

		void HafasFileFormat::Exporter_::exportToEckdatenFile(path dir, string file, date _startDate, date _endDate, string _timetableName) const
		{
			ofstream fileStream;
			createFile(fileStream, dir, file);
			int pos = 1;
			printColumn(fileStream, pos, formatDate(_startDate), 1);
			newLine(fileStream, pos);
			printColumn(fileStream, pos, formatDate(_endDate), 1);
			newLine(fileStream, pos);
			printColumn(fileStream, pos, _timetableName, 1);
			newLine(fileStream, pos);
			fileStream.close();
		}

		void HafasFileFormat::Exporter_::exportToZugartFile(path dir, string file, Zugarts _zugarts) const
		{
			ofstream fileStream;
			createFile(fileStream, dir, file);
			int pos = 1;
			BOOST_FOREACH(const string &zugart, _zugarts) {
				printColumn(fileStream, pos, zugart, 1);
				newLine(fileStream, pos);
			}
			fileStream.close();
		}

		void HafasFileFormat::Exporter_::exportToUmsteigbFile(path dir, string file) const
		{
			ofstream fileStream;
			createFile(fileStream, dir, file);
			// TODO : Generate content of UMSTEIGB file
			/*
			int pos = 1;
			{
				// Haltestellennummer 1.
				printColumn(fileStream, pos, "", 1, 7);
				// Umsteigezeit (max. 60 Minuten)
				printColumn(fileStream, pos, "", 9, 10);
				// Umsteigezeit
				printColumn(fileStream, pos, "", 12, 13);
				// Haltestellenname im Klartext
				printColumn(fileStream, pos, "", 15);
				newLine(fileStream, pos);
			}
			*/
			fileStream.close();

		}

		util::ParametersMap HafasFileFormat::Exporter_::getParametersMap() const
		{
			ParametersMap result;
			// TODO : If it's really needed, fill this up (seems that it's not used.)
			return result;
		}

		//////////////////////////////////////////////////////////////////////////
		// HELPERS

		void HafasFileFormat::Exporter_::printColumn(ofstream& fileStream, int& pos, string value, int firstColumn, int lastColumn) {
			if (firstColumn < pos) {
				stringstream error;
				error << "Impossible to print value \"" << value << "\" at position " << firstColumn << " because we're already at position " << pos << "!";
				throw runtime_error(error.str());
			} else if (firstColumn > pos) {
				// We need to add some padding...
				fileStream << string(firstColumn - pos, ' ');
			}
			int maxLength = lastColumn - firstColumn + 1;
			string toPrint = (lastColumn > 0 && (int) (strlenUtf8(value)) > maxLength) ? firstChars(value, maxLength) : value;
			fileStream << toPrint;
			pos = firstColumn + strlenUtf8(toPrint);

		}

		void HafasFileFormat::Exporter_::newLine(ofstream& fileStream, int& pos) {
			fileStream << endl;
			pos = 1;
		}

		void HafasFileFormat::Exporter_::printZugdatComment(ofstream& fileStream, int& pos, int& commentLine, string value) {
			stringstream ss;
			ss << "% " << value << " (" << (boost::format("%03d") % commentLine++) << ")";
			printColumn(fileStream, pos, ss.str(), 59);
		}

		string HafasFileFormat::Exporter_::getGuid() {
			stringstream ss;
			ss << boost::uuids::random_generator()();
			return ss.str();
		}

		path HafasFileFormat::Exporter_::createRandomFolder() {
			path dir(getGuid() + "/");
			if (create_directory(dir)) {
				return dir;
			} else {
				throw runtime_error("Unable to create folder " + dir.native() + " !");
			}
		}

		path HafasFileFormat::Exporter_::zipFolder(path sourceFolder) {
			// Folder /a/b/c/ would become file /a/b/c.zip

			if (!is_directory(sourceFolder)) {
				throw runtime_error(sourceFolder.native() + " is not a folder!");
			}

			path zipFile = sourceFolder.parent_path().replace_extension(".zip");

			stringstream info;
			info << "Zipping folder " << sourceFolder.native() << " to archive " << zipFile.native();
			util::Log::GetInstance().info(info.str());

			// We prepare the zip file...
			ofstream zipStream;
			zipStream.open(zipFile.c_str());
			ZipWriter * zip = new ZipWriter(zipStream);

			// We iterate through all the files and zip them
			directory_iterator it(sourceFolder), eod;
			BOOST_FOREACH(path const &file, make_pair(it, eod))
			{
				ifstream fileStream;
				fileStream.open(file.c_str());
				stringstream fileContent;
				fileContent << fileStream.rdbuf();
				fileStream.close();
				zip->Write(file.filename().c_str(), fileContent);
			}

			// We finalize the zip
			zip->WriteDirectory();
			zipStream.close();

			return zipFile;
		}

		void HafasFileFormat::Exporter_::createFile(ofstream& fileStream, path dir, string file) {
			fileStream.open((dir.native() + file).c_str());
			if (!fileStream.is_open()) {
				throw runtime_error((boost::format("Unable to open stream to file %1% !") % (dir.native() + file)).str());
			}
		}

		void HafasFileFormat::Exporter_::ftpUpload(string host, int port, string user, string pass, string target, path file) {

			stringstream command;
			path temporaryLogFile("ftp_log_" + getGuid());
			command << "curl -T " << file.filename().c_str() << " -sS ftp://" << user << ":" << pass << "@" << host << ":" << port << target << " 2> " << temporaryLogFile.native();
			if (!std::system(NULL)) {
				throw runtime_error("You don't have the rights to execute this command!");
			}

			stringstream info;
			info << "Uploading file " << file.native() << " to FTP server " << host << ":" << port << " using account " << user;
			util::Log::GetInstance().info(info.str());

			// We run the command
			int returnCode = std::system(command.str().c_str());

			// We load the log of the command and delete it
			ifstream ifs(temporaryLogFile.c_str());
			string output((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
			ifs.close();
			remove(temporaryLogFile);

			// If there was an error, we throw the log
			if (returnCode != 0) {
				throw runtime_error("FTP Upload error : " + output);
			}
		}

		string HafasFileFormat::Exporter_::getMandatoryString(const ParametersMap& map, string parameterName) {
			string result = map.getDefault<string>(parameterName, "");
			if (result.empty()) {
				throw RequestException("Missing parameter " + parameterName);
			}
			return result;
		}

		string HafasFileFormat::Exporter_::getCodesForDataSource(const impex::Importable* object, string dataSourceName, string defaultValue, bool warnOnNotFound) {

			if (!object) {
				return defaultValue;
			}

			stringstream codes;

			multimap<const synthese::impex::DataSource*, basic_string<char> > dataSourceLinks = object->getDataSourceLinks();

			for (multimap<const synthese::impex::DataSource*, basic_string<char> >::iterator it=dataSourceLinks.begin(); it!=dataSourceLinks.end(); ++it) {

				if (((*it).first)->get<Name>() == dataSourceName) {
					if (!codes.str().empty()) {
						codes << ",";
					}
					codes << (*it).second;
				}

			}

			if (codes.str().empty()) {
				if (warnOnNotFound) {
					stringstream warn;
					warn << "Could't find code in data source " << dataSourceName << " for object with key " << object->getKey()  << " (table: " << object->getTableName() << ") !";
					util::Log::GetInstance().warn(warn.str());
				}
				codes << defaultValue;
			}

			return codes.str();

		}

		string HafasFileFormat::Exporter_::formatTime(boost::posix_time::time_duration time) {
			if (time == boost::posix_time::not_a_date_time) {
				return string();
			}
			// We want 01:02 to be printed " 00102"
			return (boost::format(" 0%02d%02d") % time.hours() % time.minutes()).str();
		}

		string HafasFileFormat::Exporter_::formatDate(date date) {
			// We want 2015-01-01 to be printed 01.01.2015
			date::ymd_type ymd = date.year_month_day();
			return (boost::format("%02d.%02d.%04d") % ymd.day % ymd.month.as_number() % ymd.year).str();
		}

		// http://stackoverflow.com/a/4063229
		unsigned int HafasFileFormat::Exporter_::strlenUtf8(string str) {
			const char* s = str.c_str();
			unsigned int len = 0;
			while (*s) len += (*s++ & 0xc0) != 0x80;
			return len;
		}

		string HafasFileFormat::Exporter_::firstChars(string str, unsigned int maxLen) {
			const char* s = str.c_str();
			stringstream ss;
			unsigned int len = 0;
			while (*s) {
				len += (*s & 0xc0) != 0x80;
				if (len > maxLen) {
					return ss.str();
				}
				ss << *s++;
			}
			return ss.str();
		}

		string HafasFileFormat::Exporter_::getBitfieldCode(vector<Zug::CalendarUse> calendars, unsigned int bitfieldSize, BitFields& _bitfields) {
			// If this service runs daily, we return the special code 000000
			// TODO : What if the service runs every day of the interval, but not daily?
			if (calendars.size() == bitfieldSize) {
				return DAILY_SERVICE_CODE;
			}

			string bitfield = computeBitfield(calendars, bitfieldSize);

			// If this bitfield was already in the map, we reuse its code
			BOOST_FOREACH(BitFields::value_type &row, _bitfields) {
				if (row.second == bitfield) {
					return row.first;
				}
			}

			// If not, we compute its code, insert the bitfield into the map and return the code
			stringstream ss;
			ss << boost::format("%06d") % (_bitfields.size() + 1);
			_bitfields.insert(make_pair(ss.str(), bitfield));
			return ss.str();
		}

		string HafasFileFormat::Exporter_::computeBitfield(vector<Zug::CalendarUse> calendars, unsigned int bitfieldSize) {

			// Beginning and end bits are ones
			unsigned short bitfield [MAX_BITFIELD_SIZE] = {};
			setBit(bitfield, 0);
			setBit(bitfield, 1);
			setBit(bitfield, bitfieldSize + 2);
			setBit(bitfield, bitfieldSize + 3);

			// We mark the bits of the active days
			BOOST_FOREACH(Zug::CalendarUse& entry, calendars) {
				setBit(bitfield, entry.calendarNumber + 2);
			}

			// We print the bitfield
			stringstream test;
			for (unsigned int i=0; i<MAX_BITFIELD_SIZE; i++) {
				test << boost::format("%02X") % bitfield[i];
			}
			return test.str();
		}

		void HafasFileFormat::Exporter_::setBit(unsigned short bitfield [], int bit) {

			// bit  |  bit / 8  | 7 - bit % 8 | 1u << (7 - bit % 8)
			// -----+-----------+-------------+--------------------
			// 0    |  0        | 7           | 10000000
			// 1    |  0        | 6           | 01000000
			// 2    |  0        | 5           | 00100000
			// 3    |  0        | 4           | 00010000
			// 4    |  0        | 3           | 00001000
			// 5    |  0        | 2           | 00000100
			// 6    |  0        | 1           | 00000010
			// 7    |  0        | 0           | 00000001
			// 8    |  1        | 7           | 10000000
			// 9    |  1        | 6           | 01000000
			// 10   |  1        | 5           | 00100000

			bitfield[bit / 8] |= (1u << (7 - bit % 8));
		}

		// **** /HAFAS EXPORTER ****

}	}
