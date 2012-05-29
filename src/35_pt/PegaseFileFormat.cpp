
/** PegaseFileFormat class implementation.
	@file PegaseFileFormat.cpp

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

#include "PegaseFileFormat.hpp"
#include "PTFileFormat.hpp"
#include "DataSourceAdmin.h"
#include "AdminFunctionRequest.hpp"
#include "PropertiesHTMLTable.h"
#include "City.h"
#include "CityTableSync.h"
#include "StopArea.hpp"
#include "StopAreaTableSync.hpp"
#include "JourneyPatternTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "RequestException.h"
#include "LineStopTableSync.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "CalendarTemplateTableSync.h"
#include "IConv.hpp"
#include "PTUseRuleTableSync.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/unordered_map.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace admin;
	using namespace calendar;
	using namespace geography;
	using namespace graph;
	using namespace impex;
	using namespace db;
	using namespace html;
	using namespace server;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat, pt::PegaseFileFormat>::FACTORY_KEY("Pégase");
	}



	namespace pt
	{
		void PegaseFileFormat::SQLDumpParser::setTableToParse(const string& tableToParse)
		{
			_rows.clear();
			_inFile.clear();
			_inFile.seekg(0, ios::beg);
			_tableToParse = tableToParse;
		}


		bool PegaseFileFormat::SQLDumpParser::getRow()
		{
			if(!_rows.empty())
			{
				_row = _rows.front();
				_rows.pop_front();
				return true;
			}
			while(getline(_inFile, _line))
			{
				_ptr = _line.begin();

				if(!_hasNext("insert into "))
				{
					continue;
				}
				_parseInsert();
				if(!_rows.empty())
				{
					return getRow();
				}
			}
			return false;
		}



		const string& PegaseFileFormat::SQLDumpParser::getCell(const string& column)
		{
			if(_columnMap.find(column) == _columnMap.end())
			{
				throw Exception("Invalid column");
			}
			return _row[_columnMap[column]];
		}



		int PegaseFileFormat::SQLDumpParser::getCellInt(const string& column)
		{
			const string& value = getCell(column);
			try
			{
				return lexical_cast<int>(value);
			}
			catch(bad_lexical_cast)
			{
				return -1;
			}
		}



		void PegaseFileFormat::SQLDumpParser::_skipWhitespace()
		{
			while(_hasNext(' '))
				/* nop */;
		}



		string PegaseFileFormat::SQLDumpParser::_getToken()
		{
			_skipWhitespace();
			string token;

			if(_hasNext('`'))
			{
				string::const_iterator start = _ptr;
				while(_ptr != _line.end() && *_ptr != '`')
					_ptr++;

				token = string(start, _ptr);
				_expect('`');
			}
			else if(_hasNext('\''))
			{
				while(true)
				{
					if (_ptr != _line.end() && *_ptr == '\'')
					{
						// double single quotes escaped as a single quote.
						if ((_ptr + 1) != _line.end() && *(_ptr + 1) == '\'')
						{
							_ptr++;
						}
						else // end of string single quote.
						{
							break;
						}
					}

					if(_ptr == _line.end())
					{
						if(!getline(_inFile, _line))
							throw Exception("Unexpected EOF");
						_ptr = _line.begin();
						token.push_back('\n');
						continue;
					}
					// backslash escape
					if(*_ptr == '\\')
					{
						_ptr++;
					}
					token.push_back(*_ptr);
					_ptr++;
				}
				_expect('\'');
			}
			else
			{
				while(_ptr != _line.end() && *_ptr != ' ' && *_ptr != ',' && *_ptr != ')')
				{
					token.push_back(*_ptr);
					_ptr++;
				}
				// Normalize NULL values
				if(token == "null")
				{
					token = "NULL";
				}
			}
			return token;
		}



		PegaseFileFormat::SQLDumpParser::Row PegaseFileFormat::SQLDumpParser::_parseTokenList()
		{
			Row cells;
			_skipWhitespace();
			if(_ptr == _line.end() || *_ptr != '(')
			{
				return Row();
			}
			_ptr++;

			while(true)
			{
				_skipWhitespace();
				string token = _getToken();
				cells.push_back(trim_copy(token));
				_skipWhitespace();

				if(_hasNext(')'))
				{
					return cells;
				}
				_expect(',');
			}
			throw Exception("Parse error");
		}



		void PegaseFileFormat::SQLDumpParser::_parseInsert()
		{
			string tableName = _getToken();
			if(tableName != _tableToParse)
			{
				return;
			}

			// columns
			Row columnNames = _parseTokenList();
			_columnMap.clear();
			size_t index(0);
			BOOST_FOREACH(const Row::value_type& columnName, columnNames)
			{
				_columnMap[columnName] = index;
				++index;
			}

			_skipWhitespace();
			_expect("values");

			// values
			while(true)
			{
				_rows.push_back(_parseTokenList());
				_skipWhitespace();
				if(_hasNext(';'))
				{
					break;
				}
				_expect(',');
			}
		}



		void PegaseFileFormat::SQLDumpParser::_expect(char c)
		{
			if(_ptr == _line.end() || *_ptr != c)
			{
				throw Exception("Syntax error. Expected: '" + string(1, c) +
					"' Got '" + _line.substr(_ptr - _line.begin(), 30) + "'");
			}
			_ptr++;
		}



		void PegaseFileFormat::SQLDumpParser::_expect(const string& match)
		{
			if(!_hasNext(match))
			{
				throw Exception("Syntax error. Expected: '" + match +
					"' Got '" + _line.substr(_ptr - _line.begin(), 30) + "'");
			}
		}



		bool PegaseFileFormat::SQLDumpParser::_hasNext(char c)
		{
			if(_ptr != _line.end() && *_ptr == c)
			{
				_ptr++;
				return true;
			}
			return false;
		}



		bool PegaseFileFormat::SQLDumpParser::_hasNext(const string& match)
		{
			int matchSize = match.size();
			string::size_type ptrOffset = _ptr - _line.begin();
			if(ptrOffset + matchSize > _line.size() ||
				!istarts_with(string(_ptr, _ptr + matchSize), match))
			{
				return false;
			}
			_ptr += matchSize;
			return true;
		}



		const std::string PegaseFileFormat::Importer_::PARAMETER_NETWORK_ID("net");
		const std::string PegaseFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_CITY("sadc");
		const std::string PegaseFileFormat::Importer_::PARAMETER_LINE_FILTER_MODE("line_filter_mode");
		const std::string PegaseFileFormat::Importer_::FILTER_MODE1("mode1");
		const std::string PegaseFileFormat::Importer_::FILTER_MODE2("mode2");
		const std::string PegaseFileFormat::Importer_::FILTER_MODE3("mode3");
		const std::string PegaseFileFormat::Importer_::FILTER_MODE4("mode4");
		const std::string PegaseFileFormat::Importer_::PARAMETER_RESERVATION_RULE_ID("reservation_rule_id");



		// Objects and methods used by _parse().
		namespace {
			struct JourneyPatternKey
			{
				string name;
				string startDate;
				JourneyPatternKey(const string& name_, const string& startDate_):
					name(name_),
					startDate(startDate_)
				{
				}
			};
			bool operator==(const JourneyPatternKey& jp1, const JourneyPatternKey& jp2)
			{
				return jp1.name == jp2.name && jp1.startDate == jp2.startDate;
			}
			ostream& operator<<(ostream& os, const JourneyPatternKey& jp)
			{
				os << "journeyPatternName: " << jp.name << " startDate: " << jp.startDate;
				return os;
			}
			size_t hash_value(const JourneyPatternKey& jp)
			{
				size_t seed = 0;
				hash_combine(seed, jp.name);
				hash_combine(seed, jp.startDate);
				return seed;
			}

			struct JourneyPatternInfo
			{
				string longName;
				string shortName;
				bool wayBack;
			};

			typedef unordered_map<JourneyPatternKey, JourneyPatternInfo> JourneyPatternInfoMap;

			typedef int CalendarId;
			typedef unordered_map<CalendarId, Calendar> CalendarMap;
			struct ServiceKey;
			typedef unordered_map<ServiceKey, CalendarId> ServiceToCalendarIdMap;
			typedef unordered_map<ServiceKey, Calendar> ServiceToCalendarMap;

			struct ServiceKey
			{
				JourneyPatternKey journeyPatternId;
				int serviceId;

				ServiceKey(const string& journeyPatternName_, const string& startDate_, int serviceId_):
					journeyPatternId(journeyPatternName_, startDate_),
					serviceId(serviceId_)
				{
				}
			};
			bool operator==(const ServiceKey& s1, const ServiceKey& s2)
			{
				return s1.journeyPatternId == s2.journeyPatternId && s1.serviceId == s2.serviceId;
			}
			ostream& operator<<(ostream& os, const ServiceKey& s)
			{
				os << "journeyPatternId: " << s.journeyPatternId << " serviceId: " << s.serviceId;
				return os;
			}
			size_t hash_value(const ServiceKey& s)
			{
				size_t seed = 0;
				hash_combine(seed, s.journeyPatternId);
				hash_combine(seed, s.serviceId);
				return seed;
			}

			struct ScheduleInfo
			{
				string stopPointId;
				time_duration arrivalTime;
				time_duration departureTime;
				bool departure;
				bool arrival;
			};

			typedef map<int, ScheduleInfo> ServiceInfo;

			typedef unordered_map<ServiceKey, ServiceInfo> ServiceInfoMap;


			struct LineInfo
			{
				bool ignored;
				bool needReservation;
				string lineName;
				string routeName;

				LineInfo() : ignored(false), needReservation(false)
				{
				}
			};
			ostream& operator<<(ostream& os, const LineInfo& li)
			{
				os << "ignored: " << li.ignored << "needResa " << li.needReservation <<
					" lineName: " << li.lineName << " routeName: " << li.routeName;
				return os;
			}

			void _parseSuffix(const string& jpName, LineInfo& lineInfo)
			{
				size_t len = jpName.size();

				// Format {lineName}[AR]nnHn
				if(len > 5 &&
					jpName[len - 2] == 'H' &&
					(jpName[len - 5] == 'A' || jpName[len - 5] == 'R'))
				{
					lineInfo.lineName = jpName.substr(0, len - 5);
					lineInfo.routeName = jpName.substr(len - 4, 4);
					return;
				}

				// Format {lineName}[AR]nn
				if(len > 3 &&
					(jpName[len - 3] == 'A' || jpName[len - 3] == 'R'))
				{
					lineInfo.lineName = jpName.substr(0, len - 3);
					lineInfo.routeName = jpName.substr(len - 2, 2);
					return;
				}

				// Format {lineName}[AR]n
				if(len > 2 &&
					(jpName[len - 2] == 'A' || jpName[len - 2] == 'R'))
				{
					lineInfo.lineName = jpName.substr(0, len - 2);
					lineInfo.routeName = jpName.substr(len - 1, 1);
					return;
				}

				lineInfo.lineName = jpName;
				lineInfo.routeName = "";
			}

			LineInfo getLineInfo(const string& mode, const string& jpName)
			{
				LineInfo lineInfo;
				_parseSuffix(jpName, lineInfo);
				size_t len = lineInfo.lineName.length();

				if(mode == PegaseFileFormat::Importer_::FILTER_MODE1)
				{
					// "LR{nnn}" || "LSR{nnn}"

					if(!starts_with(lineInfo.lineName, "LR") && !starts_with(lineInfo.lineName, "LSR"))
					{
						lineInfo.ignored = true;
					}
					return lineInfo;
				}

				if(mode == PegaseFileFormat::Importer_::FILTER_MODE2)
				{
					// FIXME: unused for now.
					// "S{digit string}

					if(lineInfo.lineName.length() <= 1 ||
						!starts_with(lineInfo.lineName, "S") ||
						!isdigit(lineInfo.lineName[1]))
					{
						lineInfo.ignored = true;
						return lineInfo;
					}
					lineInfo.lineName = lineInfo.lineName.substr(1, len - 1);
					return lineInfo;
				}

				if(mode == PegaseFileFormat::Importer_::FILTER_MODE3)
				{
					// LR-{string}-{n} or SR-{string}-{nn}

					// LR-{string}-{n}
					if(starts_with(lineInfo.lineName, "LR-") &&
						len >= 6 &&
						lineInfo.lineName[len - 2] == '-')
					{
						lineInfo.needReservation = false;
						lineInfo.lineName = lineInfo.lineName.substr(3, len - (3 + 2));
						return lineInfo;
					}

					// SR-{string}-{nn}
					if(starts_with(lineInfo.lineName, "SR-") &&
						len >= 7 &&
						lineInfo.lineName[len - 3] == '-')
					{
						lineInfo.needReservation = true;
						string previousLineName(lineInfo.lineName);
						lineInfo.lineName = previousLineName.substr(3, len - (3 + 3));
						lineInfo.routeName = previousLineName.substr(len - 2, 2);
						return lineInfo;
					}

					// Not found
					lineInfo.ignored = true;
					return lineInfo;
				}

				if(mode == PegaseFileFormat::Importer_::FILTER_MODE4)
				{
					// FIXME: unused for now.
					// !LR-{string}-{n}

					if(!starts_with(lineInfo.lineName, "LR-") ||
						len <= 4 ||
						lineInfo.lineName[len - 4] != '-')
					{
						return lineInfo;
					}
					else
					{
						lineInfo.ignored = true;
						return lineInfo;
					}
				}

				throw Exception("Invalid line filter mode: " + mode);
			}
		}



		bool PegaseFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			std::ostream& os,
			boost::optional<const admin::AdminRequest&> adminRequest
		) const {
			ifstream inFile;
			inFile.open(filePath.file_string().c_str());
			if(!inFile)
			{
				throw Exception("Could no open the file " + filePath.file_string());
			}

			SQLDumpParser parser(inFile);

			os << "INFO : loading POINT_D_ARRET<br />";
			parser.setTableToParse("POINT_D_ARRET");
			while(parser.getRow())
			{
				string cityCode = parser.getCell("PA_COD_COMMU");
				string stopCode = parser.getCell("PA_CODE_ARRET");

				shared_ptr<const City> cityForStopAreaAutoGeneration;
				CityTableSync::SearchResult cities(
					CityTableSync::Search(_env, optional<string>(), optional<string>(), cityCode, 0, 1)
				);
				if(cities.empty())
				{
					os << "WARN : City with code: " << cityCode << " not found (stop code: " << stopCode << ")<br />";
					if(_defaultCity)
					{
						cityForStopAreaAutoGeneration = _defaultCity;
					}
					else
					{
						os << "ERR : no default city. Data might be inconsistent<br />";
						continue;
					}
				}
				else
				{
					cityForStopAreaAutoGeneration = *cities.begin();
				}

				shared_ptr<geos::geom::Point> point;

				string x = parser.getCell("PA_ABSCISSE");
				string y = parser.getCell("PA_ORDONNEE");
				if(x != "NULL" && y != "NULL")
				{
					point = _dataSource.getActualCoordinateSystem().createPoint(
						lexical_cast<double>(x),
						lexical_cast<double>(y)
					);
					if(point->isEmpty())
					{
						point.reset();
					}
				}

				IConv iconv(_dataSource.getCharset(), "UTF-8");
				string shortName(trim_copy(parser.getCell("PA_NOM_COURT")));
				// Somtimes, the short name is truncated. This tries to look
				// at the long name (which also contains the city as a prefix)
				// to see if it can complete a potentially truncated short name.
				string longName(trim_copy(parser.getCell("PA_NOM_LONG")));
				size_t shortPosInLong = longName.find(shortName);
				if(shortPosInLong != string::npos)
				{
					shortName = longName.substr(shortPosInLong);
				}

				PTFileFormat::CreateOrUpdateStopWithStopAreaAutocreation(
					_stopPoints,
					stopCode,
					iconv.convert(shortName),
					point.get(),
					*cityForStopAreaAutoGeneration.get(),
					optional<time_duration>(),
					_dataSource,
					_env,
					os,
					boost::optional<const graph::RuleUser::Rules&>()
				);
			}

			ServiceInfoMap serviceInfos;

			os << "INFO : loading ARR_SER<br />";
			parser.setTableToParse("ARR_SER");
			while(parser.getRow())
			{
				ServiceKey serviceKey(
					parser.getCell("AS_COD_ITINERAIR"),
					parser.getCell("AS_DAT_DEBVALID"),
					parser.getCellInt("AS_COD_SERVICE"));
				if(serviceInfos.find(serviceKey) == serviceInfos.end())
				{
					serviceInfos[serviceKey] = ServiceInfo();
				}
				ServiceInfo& serviceInfo = serviceInfos[serviceKey];

				int rank = parser.getCellInt("AS_NO_ORDRE");
				if(serviceInfo.find(rank) != serviceInfo.end())
				{
					os << "ERR : duplicate ranks (" << rank << ") found for service key: " <<
						serviceKey << "<br />";
					continue;
				}

				ScheduleInfo scheduleInfo;
				scheduleInfo.stopPointId = parser.getCell("AS_COD_ARRET");
				if(!_stopPoints.contains(scheduleInfo.stopPointId))
				{
					os << "ERR : Found a ARR_SER entry without a matching stop point. Service key: " <<
						serviceKey << " stop code: " << scheduleInfo.stopPointId << "<br />";
					continue;
				}

				int arrivalTimeMinutes = parser.getCellInt("AS_HEURE_PASSAGE");
				// FIXME: in the data dump, an arrival time of 0 means that the stop is not serviced.
				// How can we distinguish when the stop is service at midnight?
				if(arrivalTimeMinutes <= 0)
				{
					continue;
				}
				scheduleInfo.arrivalTime = minutes(arrivalTimeMinutes);

				scheduleInfo.departureTime = scheduleInfo.arrivalTime;
				int waitingMinutes = parser.getCellInt("AS_TMP_ATTENTE");
				if(waitingMinutes > 0)
				{
					scheduleInfo.departureTime += minutes(waitingMinutes);
				}

				scheduleInfo.departure = parser.getCellInt("AS_MONT_INTERDIT") == 0;
				scheduleInfo.arrival = parser.getCellInt("AS_DESC_INTERDIT") == 0;

				serviceInfo[rank] = scheduleInfo;
			}

			JourneyPatternInfoMap journeyPatternInfos;

			os << "INFO : loading ITINERAIRE<br />";
			parser.setTableToParse("ITINERAIRE");
			while(parser.getRow())
			{
				JourneyPatternKey journeyPatternKey(
					parser.getCell("ITI_COD_ITINERAIR"), parser.getCell("ITI_DAT_DEBVALID"));
				if(journeyPatternInfos.find(journeyPatternKey) == journeyPatternInfos.end())
				{
					journeyPatternInfos[journeyPatternKey] = JourneyPatternInfo();
				}
				JourneyPatternInfo& journeyPatternInfo = journeyPatternInfos[journeyPatternKey];

				journeyPatternInfo.longName = parser.getCell("ITI_NOM_LONG");
				journeyPatternInfo.shortName = parser.getCell("ITI_NOM_COURT");
				journeyPatternInfo.wayBack = parser.getCell("ITI_SENS") == "R";
			}

			CalendarMap calendars;

			os << "INFO : loading CAL_DAT<br />";
			parser.setTableToParse("CAL_DAT");
			date now(day_clock::local_day());
			while(parser.getRow())
			{
				CalendarId calendarId = parser.getCellInt("CAD_COD_CAL");

				string dateStr = parser.getCell("CAD_DATE");
				if(dateStr.size() != 8)
				{
					os << "WARN : incorrect calendar date: " << dateStr << "<br />";
					continue;
				}
				date activeDate(
					lexical_cast<int>(dateStr.substr(0, 4)),
					lexical_cast<int>(dateStr.substr(4, 2)),
					lexical_cast<int>(dateStr.substr(6, 2))
				);
				if(_fromToday && activeDate < now)
				{
					continue;
				}

				if(calendars.find(calendarId) == calendars.end())
				{
					calendars[calendarId] = Calendar();
				}
				Calendar& calendar = calendars[calendarId];

				calendar.setActive(activeDate);
			}

			ServiceToCalendarIdMap serviceToCalendarId;
			os << "INFO : loading CAL_SER<br />";
			parser.setTableToParse("CAL_SER");
			while(parser.getRow())
			{
				ServiceKey serviceKey(
					parser.getCell("CS_COD_ITINERAIR"), parser.getCell("CS_DAT_DEBVALID"),
					parser.getCellInt("CS_COD_SERVICE"));
				serviceToCalendarId[serviceKey] = parser.getCellInt("CS_CALENDRIER");
			}

			ServiceToCalendarMap serviceToCalendar;
			os << "INFO : loading SERVICE<br />";
			parser.setTableToParse("SERVICE");
			while(parser.getRow())
			{
				ServiceKey serviceKey(
					parser.getCell("SER_COD_ITINERAIR"), parser.getCell("SER_DAT_ITINERAIR"),
					parser.getCellInt("SER_COD_SERVICE"));

				if(serviceToCalendarId.find(serviceKey) == serviceToCalendarId.end())
				{
					continue;
				}
				CalendarId calendarId = serviceToCalendarId[serviceKey];
				if(calendars.find(calendarId) == calendars.end())
				{
					continue;
				}

				Calendar cal = calendars[calendarId];

				// Week starts on Monday
				string runningDaysString = parser.getCell("SER_JOU_FONCT");
				// Week starts on Sunday
				bool runningDays[7];

				for(int i = 0; i <= 5; i++)
				{
					runningDays[i + 1] = runningDaysString[i] != '-';
				}
				runningDays[0] = runningDaysString[6] != '-';

				if(cal.empty())
				{
					os << "WARN : calendar id: " << calendarId << " from service " << serviceKey << " is empty<br />";
					continue;
				}

				if(cal.getFirstActiveDate() > cal.getLastActiveDate())
				{
					os << "WARN : calendar id: " << calendarId << " from service " << serviceKey <<
						" has a last active date (" << cal.getLastActiveDate() <<
						") lower than its first active date (" << cal.getFirstActiveDate() <<
						")<br />";
					continue;
				}
				assert(cal.getFirstActiveDate() <= cal.getLastActiveDate());

				date lastActiveDate(cal.getLastActiveDate());
				for(date d(cal.getFirstActiveDate()); d <= lastActiveDate; d += days(1))
				{
					if(!runningDays[d.day_of_week()])
					{
						cal.setInactive(d);
					}
				}
				serviceToCalendar[serviceKey] = cal;
			}

			// Extract the JourneyPatterns

			BOOST_FOREACH(const ServiceInfoMap::value_type& serviceKeyInfo, serviceInfos)
			{
				ServiceKey serviceKey = serviceKeyInfo.first;
				ServiceInfo serviceInfo = serviceKeyInfo.second;

				if(journeyPatternInfos.find(serviceKey.journeyPatternId) == journeyPatternInfos.end())
				{
					os << "WARN : unable to find journey pattern with name: " <<
						serviceKey.journeyPatternId.name <<
						" and start date: " << serviceKey.journeyPatternId.startDate << "<br />";
					continue;
				}
				JourneyPatternInfo& journeyPatternInfo = journeyPatternInfos[serviceKey.journeyPatternId];


				LineInfo lineInfo = getLineInfo(_lineFilterMode, serviceKey.journeyPatternId.name);
				if (lineInfo.ignored)
				{
					continue;
				}

				// CommercialLine

				CommercialLine* commercialLine(
					PTFileFormat::CreateOrUpdateLine(
						_lines,
						lineInfo.lineName,
						journeyPatternInfo.longName,
						// TODO: Use shortName somewhere?
						lineInfo.lineName,
						optional<RGBColor>(),
						*_network,
						_dataSource,
						_env,
						os
					)
				);
				assert(commercialLine);

				if(serviceInfo.empty())
				{
					os << "ERR : no schedules found for service: " << serviceKey << "<br />";
					continue;
				}

				// JourneyPattern

				JourneyPattern::StopsWithDepartureArrivalAuthorization stops;
				ScheduledService::Schedules departures;
				ScheduledService::Schedules arrivals;

				BOOST_FOREACH(const ServiceInfo::value_type& serviceRankSchedule, serviceInfo)
				{
					ScheduleInfo scheduleInfo = serviceRankSchedule.second;

					if(!_stopPoints.contains(scheduleInfo.stopPointId))
					{
						os << "ERR : Missing stop point with id: " << scheduleInfo.stopPointId << "<br />";
						continue;
					}
					set<StopPoint*> stopPoints = _stopPoints.get(scheduleInfo.stopPointId);

					JourneyPattern::StopWithDepartureArrivalAuthorization stop(
						stopPoints,
						optional<MetricOffset>(),
						scheduleInfo.departure,
						scheduleInfo.arrival
					);
					stops.push_back(stop);

					departures.push_back(scheduleInfo.departureTime);
					arrivals.push_back(scheduleInfo.arrivalTime);
				}

				if(stops.size() < 2)
				{
					os << "WARN : journey pattern has less than 2 stops. It is ignored. service key: " << serviceKey << "<br />";
					continue;
				}

				// Use rules
				RuleUser::Rules rules(RuleUser::GetEmptyRules());
				if (lineInfo.needReservation && _reservationUseRule.get())
				{
					rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = _reservationUseRule.get();
				}

				JourneyPattern* journeyPattern(
					PTFileFormat::CreateOrUpdateRoute(
						*commercialLine,
						optional<const string&>(), // id
						lineInfo.routeName + ":" + (lineInfo.needReservation ? "resa:" : "") +
							journeyPatternInfo.longName, // name
						optional<const string&>(), // destination
						optional<Destination*>(),
						rules,
						journeyPatternInfo.wayBack,
						NULL,
						stops,
						_dataSource,
						_env,
						os,
						true,
						true
					)
				);
				assert(journeyPattern);

				// ScheduledService

				ScheduledService* service(
					PTFileFormat::CreateOrUpdateService(
						*journeyPattern,
						departures,
						arrivals,
						lexical_cast<string>(serviceKey.serviceId),
						_dataSource,
						_env,
						os
					)
				);
				if(service)
				{
					if(serviceToCalendar.find(serviceKey) == serviceToCalendar.end())
					{
						os << "WARN : service has no calendar. service key: " << serviceKey << "<br />";
						continue;
					}
					*service |= serviceToCalendar[serviceKey];
				}
			}

			return true;
		}



		PegaseFileFormat::Importer_::Importer_(util::Env& env, const impex::DataSource& dataSource):
			OneFileTypeImporter<PegaseFileFormat>(env, dataSource),
			Importer(env, dataSource),
			PTDataCleanerFileFormat(env, dataSource),
			_stopPoints(_dataSource, env),
			_lines(_dataSource, env)
		{}



		void PegaseFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const admin::AdminRequest& request
		) const {
				AdminFunctionRequest<DataSourceAdmin> reloadRequest(request);
				PropertiesHTMLTable t(reloadRequest.getHTMLForm());
				stream << t.open();
				stream << t.title("Mode");
				stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
				stream << t.title("Fichier");
				stream << t.cell("Fichier", t.getForm().getTextInput(PARAMETER_PATH, _pathsSet.empty() ? string() : _pathsSet.begin()->file_string()));
				stream << t.title("Paramètres");
				stream << t.cell("Effacer données anciennes", t.getForm().getOuiNonRadioInput(PTDataCleanerFileFormat::PARAMETER_CLEAN_OLD_DATA, _cleanOldData));
				stream << t.cell("Effacer arrêts inutilisés", t.getForm().getOuiNonRadioInput(PTDataCleanerFileFormat::PARAMETER_CLEAN_UNUSED_STOPS, _cleanUnusedStops));
				stream << t.cell("Réseau (ID)", t.getForm().getTextInput(PARAMETER_NETWORK_ID, _network.get() ? lexical_cast<string>(_network->getKey()) : string()));
				stream << t.cell("Commune par défaut (ID)", t.getForm().getTextInput(PARAMETER_STOP_AREA_DEFAULT_CITY, _defaultCity.get() ? lexical_cast<string>(_defaultCity->getKey()) : string()));
				stream << t.cell("Ne pas importer données anciennes", t.getForm().getOuiNonRadioInput(PTDataCleanerFileFormat::PARAMETER_FROM_TODAY, _fromToday));

				vector<pair<optional<string>, string> > filterModes;
				filterModes.push_back(make_pair(optional<string>(FILTER_MODE1), "LR{nnn} || LSR{nnn}"));
				filterModes.push_back(make_pair(optional<string>(FILTER_MODE2), "S{nombre chaine}"));
				filterModes.push_back(make_pair(optional<string>(FILTER_MODE3), "LR-{chaine}-{n} ou SR-{chaine}-{nn}"));
				filterModes.push_back(make_pair(optional<string>(FILTER_MODE4), "!LR-{chaine}-{n}"));
				stream << t.cell("Mode de filtrage des lignes", t.getForm().getSelectInput(PARAMETER_LINE_FILTER_MODE, filterModes, optional<string>(_lineFilterMode)));
				stream << t.cell("Condition d'utilisation pour les réservations (ID)", t.getForm().getTextInput(PARAMETER_RESERVATION_RULE_ID, _reservationUseRule.get() ? lexical_cast<string>(_reservationUseRule->getKey()) : string()));

				stream << t.close();
		}



		util::ParametersMap PegaseFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map(PTDataCleanerFileFormat::_getParametersMap());

			if(_network.get())
			{
				map.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}

			if(_defaultCity.get())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_CITY, _defaultCity->getKey());
			}

			if(!_lineFilterMode.empty())
			{
				map.insert(PARAMETER_LINE_FILTER_MODE, _lineFilterMode);
			}

			if(_reservationUseRule.get())
			{
				map.insert(PARAMETER_RESERVATION_RULE_ID, _reservationUseRule->getKey());
			}

			return map;
		}



		void PegaseFileFormat::Importer_::_setFromParametersMap(const util::ParametersMap& map)
		{
			PTDataCleanerFileFormat::_setFromParametersMap(map);

			if(map.isDefined(PARAMETER_NETWORK_ID)) try
			{
				_network = TransportNetworkTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID), _env);
			}
			catch (ObjectNotFoundException<TransportNetwork>&)
			{
			}

			if(map.getDefault<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY, 0))
			{
				_defaultCity = CityTableSync::Get(map.get<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY), _env);
			}

			_lineFilterMode = map.getDefault<string>(PARAMETER_LINE_FILTER_MODE, FILTER_MODE1);

			if(map.getDefault<RegistryKeyType>(PARAMETER_RESERVATION_RULE_ID, 0))
			{
				_reservationUseRule = PTUseRuleTableSync::Get(
					map.get<RegistryKeyType>(PARAMETER_RESERVATION_RULE_ID),
					_env
				);
			}
		}



		db::DBTransaction PegaseFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;

			// Add remove queries generated by _selectObjectsToRemove
			PTDataCleanerFileFormat::_addRemoveQueries(transaction);

			BOOST_FOREACH(Registry<StopArea>::value_type stopArea, _env.getRegistry<StopArea>())
			{
				StopAreaTableSync::Save(stopArea.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<StopPoint>::value_type stop, _env.getRegistry<StopPoint>())
			{
				StopPointTableSync::Save(stop.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<CommercialLine>::value_type cline, _env.getRegistry<CommercialLine>())
			{
				CommercialLineTableSync::Save(cline.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<JourneyPattern>::value_type line, _env.getRegistry<JourneyPattern>())
			{
				JourneyPatternTableSync::Save(line.second.get(), transaction);
			}
			BOOST_FOREACH(Registry<DesignatedLinePhysicalStop>::value_type lineStop, _env.getRegistry<DesignatedLinePhysicalStop>())
			{
				LineStopTableSync::Save(lineStop.second.get(), transaction);
			}
			BOOST_FOREACH(const Registry<ScheduledService>::value_type& service, _env.getRegistry<ScheduledService>())
			{
				ScheduledServiceTableSync::Save(service.second.get(), transaction);
			}

			return transaction;
		}
	}
}
