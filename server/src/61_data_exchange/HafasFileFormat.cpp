
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

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <map>
#include <fstream>
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
		const string HafasFileFormat::Importer_::LineFilter::SEP_MAIN = ",";
		const string HafasFileFormat::Importer_::LineFilter::SEP_FIELD = ":";
		const string HafasFileFormat::Importer_::LineFilter::JOCKER = "*";
		const string HafasFileFormat::Importer_::LineFilter::VALUE_LINES_BY_STOPS_PAIRS = "SP";



		HafasFileFormat::Importer_::LineFilter::LineFilter():
			linesByStopsPair(false)
		{}



		HafasFileFormat::Importer_::LinesFilter HafasFileFormat::Importer_::getLinesFilter( const std::string& s )
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



		const HafasFileFormat::Importer_::LineFilter* HafasFileFormat::Importer_::_lineIsIncluded(
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
}	}
