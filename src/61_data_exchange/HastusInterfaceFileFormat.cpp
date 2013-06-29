
/** HastusInterfaceFileFormat class implementation.
	@file HastusInterfaceFileFormat.cpp

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

#include "HastusInterfaceFileFormat.hpp"

#include "DataSource.h"
#include "DBModule.h"
#include "GraphConstants.h"
#include "Import.hpp"
#include "StopPoint.hpp"
#include "ScheduledServiceTableSync.h"
#include "ContinuousServiceTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "LineStopTableSync.h"
#include "CityTableSync.h"
#include "Service.h"
#include "RollingStock.hpp"
#include "NonConcurrencyRuleTableSync.h"
#include "ReservationContactTableSync.h"
#include "PTUseRule.h"
#include "PTConstants.h"
#include "CoordinatesSystem.hpp"
#include "XmlToolkit.h"
#include "DBTransaction.hpp"
#include "CityAliasTableSync.hpp"
#include "JunctionTableSync.hpp"
#include "RollingStockTableSync.hpp"
#include "RequestException.h"
#include "ImpExModule.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "TransportNetworkTableSync.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <iomanip>
#include <locale>
#include <string>
#include <utility>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;
using namespace boost::filesystem;
using namespace boost::posix_time;
using namespace boost::algorithm;
using namespace geos::geom;

namespace synthese
{
	using namespace data_exchange;
	using namespace geography;
	using namespace util::XmlToolkit;
	using namespace util;
	using namespace graph;
	using namespace impex;
	using namespace db;
	using namespace pt;
	using namespace server;
	using namespace calendar;
	using namespace vehicle;	

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat, HastusInterfaceFileFormat>::FACTORY_KEY("HastusInterface");
	}

	namespace data_exchange
	{
		const string HastusInterfaceFileFormat::Importer_::PARAMETER_TRANSPORT_NETWORK_ID = "network_id";
		const string HastusInterfaceFileFormat::Importer_::PARAMETER_FILE_NAME_IS_A_CALENDAR = "file_name_is_a_calendar";


		//////////////////////////////////////////////////////////////////////////
		// CONSTRUCTOR
		HastusInterfaceFileFormat::Importer_::Importer_(
			Env& env,
			const Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		):	Importer(env, import, minLogLevel, logPath, outputStream, pm),
			OneFileTypeImporter<Importer_>(env, import, minLogLevel, logPath, outputStream, pm),
			PTDataCleanerFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			PTFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			CalendarFileFormat(env, import, minLogLevel, logPath, outputStream, pm),
			_fileNameIsACalendar(false),
			_calendars(*import.get<DataSource>(), _env),
			_lines(*import.get<DataSource>(), _env),
			_stops(*import.get<DataSource>(), _env)
		{}



		//////////////////////////////////////////////////////////////////////////
		// REQUESTS HANDLING
		util::ParametersMap HastusInterfaceFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap result(PTDataCleanerFileFormat::_getParametersMap());

			// Transport network
			if(_network.get())
			{
				result.insert(PARAMETER_TRANSPORT_NETWORK_ID, _network->getKey());
			}

			// File name is a calendar
			if(_fileNameIsACalendar)
			{
				result.insert(PARAMETER_FILE_NAME_IS_A_CALENDAR, _fileNameIsACalendar);
			}

			return result;
		}



		void HastusInterfaceFileFormat::Importer_::_setFromParametersMap(const ParametersMap& map)
		{
			PTDataCleanerFileFormat::_setFromParametersMap(map);

			// Transport network
			RegistryKeyType networkId(map.getDefault<RegistryKeyType>(PARAMETER_TRANSPORT_NETWORK_ID, 0));
			if(networkId) try
			{
				_network = TransportNetworkTableSync::GetEditable(
					networkId,
					_env
				);
			}
			catch(ObjectNotFoundException<TransportNetwork>&)
			{
				throw RequestException("No such network");
			}

			// File name as calendar
			_fileNameIsACalendar = map.getDefault<bool>(PARAMETER_FILE_NAME_IS_A_CALENDAR, false);
		}



		//////////////////////////////////////////////////////////////////////////
		// INPUT

		bool HastusInterfaceFileFormat::Importer_::_parse(
			const path& filePath
		) const {

			// Missing calendars
			set<string> missingCalendars;

			// File opening
			_openFile(filePath);

			// Datasource
			DataSource& dataSource(*_import.get<DataSource>());

			// File name is a calendar
			optional<Calendar> fileMask;
			if(_fileNameIsACalendar)
			{
				string fileName(filePath.filename());
				CalendarTemplate* fileCalendar(
					_getCalendarTemplate(
						_calendars,
						fileName
				)	);
				if(fileCalendar)
				{
					fileMask = fileCalendar->getResult(_calendar);
				}
				else
				{
					missingCalendars.insert(fileName);
				}
			}

			// Record 1.1 Lines number
			vector<string> lineNumbers(
				_getNextVector(1.1, 19, 22, 5)
			);

			// Record 2 : Lines
			typedef map<string, boost::shared_ptr<RollingStock> > LineTransportModes;
			LineTransportModes lineTransportModes;
			for(size_t lineRank(0); lineRank < lineNumbers.size(); ++lineRank)
			{
				// Jump to record 2
				_loadNextRecord(2);

				// Line code
				string lineCode(_getTextField(4, 4));

				// Transport mode
				string transportModeCode = _getTextField(70, 10);
				string tridentKey;
				if(transportModeCode == "Autobus")
				{
					tridentKey = "Bus";
				}
				boost::shared_ptr<RollingStock> rollingStock;
				RollingStockTableSync::SearchResult rollingStocks(
					RollingStockTableSync::Search(
						_env,
						tridentKey,
						true
				)	);
				if(!rollingStocks.empty())
				{
					lineTransportModes[lineCode] = rollingStocks.front();
				}

				// Jump to record 2.2
				_loadNextRecord(2.2);

				// Line name
				string name(_getTextField(6, 50));

				// EOF is unusual here
				if(_eof())
				{
					return false;
				}

				// Line import
				_createOrUpdateLine(
					_lines,
					lineCode,
					name,
					lineCode,
					optional<RGBColor>(),
					*_network,
					dataSource
				);
			}

			// Record 5 : Main calendar
			_loadNextRecord(5);
			string mainCalendarCode(_getTextField(4, 9));
			CalendarTemplate* mainCalendar(
				_getCalendarTemplate(
					_calendars,
					mainCalendarCode
			)	);
			Calendar mainMask;
			if(mainCalendar)
			{
				mainMask = mainCalendar->getResult(_calendar);
				if(fileMask)
				{
					mainMask &= *fileMask;
				}
			}
			else
			{
				missingCalendars.insert(mainCalendarCode);
			}

			// Records 13 : Services
			vector<TemporaryService> services;
			typedef map<string, set<StopPoint*> > StopCodes;
			StopCodes stopCodes;
			for(_loadNextRecord(12.1); !_eof(); _loadNextRecord(12.1))
			{
				// Declarations
				TemporaryService service;

				// Jump dead runs
				if(_getTextField(7,2) != "0")
				{
					_loadNextRecord();
					continue;
				}

				// Schedules on record 12.1
				vector<string> schedulesStr(_getNextVector(12.1, 37, 41, 5));
				BOOST_FOREACH(const string& scheduleStr, schedulesStr)
				{
					service.schedules.push_back(
						minutes(lexical_cast<long>(scheduleStr))
					);
				}

				// Line number on record 12.1
				service.lineCode = _getTextField(29,5);

				// Read scheduled stops in record 12.2
				service.scheduledStops = _getNextVector(12.2, 37, 41, 6);
				BOOST_FOREACH(const string& stopCode, service.scheduledStops)
				{
					stopCodes.insert(
						make_pair(
							stopCode,
							StopCodes::mapped_type()
					)	);
				}

				// Distances in record 12.5
				vector<string> distancesStr(_getNextVector(12.5, 37, 41, 6));
				BOOST_FOREACH(const string& distanceStr, distancesStr)
				{
					service.scheduledStopsDistances.push_back(
						lexical_cast<MetricOffset>(distanceStr)
					);
				}

				// Jump to record 12.7
				_loadNextRecord(12.7);

				// Service Properties in record 12.7
				service.calendar = _getTextField(87,7);
				service.code = _getTextField(40,10);
				service.routeCode = _getTextField(124,4);
				service.wayBack = (lexical_cast<int>(_getTextField(51, 4)) % 2 == 1);

				// All schedules on record 12.8
				vector<string> allSchedulesStr(_getNextVector(12.8, 37, 41, 5));
				BOOST_FOREACH(const string& allScheduleStr, allSchedulesStr)
				{
					service.allSchedules.push_back(
						minutes(lexical_cast<long>(allScheduleStr))
					);
				}

				// Stops on record 12.9
				service.stops = _getNextVector(12.9, 37, 41, 9);
				BOOST_FOREACH(const string& stopCode, service.stops)
				{
					stopCodes.insert(
						make_pair(
							stopCode,
							StopCodes::mapped_type()
					)	);
				}

				services.push_back(service);
			}

			// File closing
			_file.close();

			// Loop on stops
			PTFileFormat::ImportableStopPoints nonLinkedStopPoints;
			bool success(true);
			BOOST_FOREACH(StopCodes::value_type& stopCode, stopCodes)
			{
				// Stop
				stopCode.second = _getStopPoints(
					_stops,
					stopCode.first,
					optional<const string&>(),
					true
				);
				if(stopCode.second.empty())
				{
					PTFileFormat::ImportableStopPoint isp;
					isp.operatorCode = stopCode.first;
					isp.name = stopCode.first;
					nonLinkedStopPoints.push_back(isp);
					success = false;
				}
			}
			if(!success)
			{
				_exportStopPoints(
					nonLinkedStopPoints
				);

				_logError(
					"Au moins un arrêt non trouvé : import interrompu"
				);
				return false;
			}

			// Loop on temporary services
			BOOST_FOREACH(const TemporaryService& service, services)
			{
				// Line
				CommercialLine* line(
					_getLine(
						_lines,
						service.lineCode,
						dataSource
				)	);
				if(line == NULL)
				{
					_logWarning(
						"Inconsistent line number "+ service.lineCode +" in service "+ service.code
					);
					continue;
				}

				// Served stops
				JourneyPattern::StopsWithDepartureArrivalAuthorization servedStops;
				size_t scheduledStopsI(0);
				size_t lastScheduledStop(0);
				MetricOffset metricOffset(0);
				for(size_t allStopsI(0); allStopsI < service.stops.size(); ++allStopsI)
				{
					// Declarations
					const std::set<StopPoint*>& possibleStops(
						stopCodes[service.stops[allStopsI]]
					);

					// Is a scheduled stop ? Schedules must match and the stop must be the same
					bool isScheduledStop(false);
					if(service.allSchedules[allStopsI] == service.schedules[scheduledStopsI])
					{
						BOOST_FOREACH(StopPoint* tstop, stopCodes[service.scheduledStops[scheduledStopsI]])
						{
							if(possibleStops.find(tstop) != possibleStops.end())
							{
								isScheduledStop = true;
								break;
							}
						}
					}

					if(isScheduledStop)
					{
						metricOffset += service.scheduledStopsDistances[scheduledStopsI];
						++scheduledStopsI;
					}
					servedStops.push_back(
						JourneyPattern::StopWithDepartureArrivalAuthorization(
							possibleStops,
							metricOffset,
							true,
							true,
							isScheduledStop
					)	);

					// Distances pseudo-interpolation
					if(isScheduledStop && allStopsI>0)
					{
						// Total length
						MetricOffset totalLength(*servedStops[allStopsI]._metricOffset - *servedStops[lastScheduledStop]._metricOffset);

						size_t rank(1);
						for(size_t unScheduledStopI(lastScheduledStop+1); unScheduledStopI<allStopsI; ++unScheduledStopI)
						{
							servedStops[unScheduledStopI]._metricOffset =
								*servedStops[lastScheduledStop]._metricOffset +
								floor(
									(double(rank) / double(allStopsI-lastScheduledStop)) *
									double(totalLength)
								)
							;
							++rank;
						}

						// Record the last scheduled stop
						lastScheduledStop = allStopsI;
					}
				}
				if(scheduledStopsI < service.scheduledStops.size())
				{
					_logWarning(
						"A scheduled stop did not match with the full stop list : "+ service.scheduledStops[scheduledStopsI] +" after "+ service.stops[lastScheduledStop] +". Service is ignored."
					);
					continue;
				}

				// Route
				JourneyPattern* route(
					_createOrUpdateRoute(
						*line,
						service.routeCode,
						optional<const string&>(),
						optional<const string&>(),
						optional<Destination*>(),
						optional<const RuleUser::Rules&>(),
						service.wayBack,
						lineTransportModes[service.lineCode].get(),
						servedStops,
						dataSource,
						true, // Remove old codes
						false // Don't update metric offsets on update because default metric offset are approximations
				)	);
				if(route == NULL)
				{
					_logWarning(
						"Route "+ service.routeCode +" was not built in service "+ service.code
					);
					continue;
				}

				// Service
				ScheduledService* sservice(
					_createOrUpdateService(
						*route,
						service.schedules,
						service.schedules,
						service.code,
						dataSource
				)	);

				// Calendar
				CalendarTemplate* calendar(
					_getCalendarTemplate(
						_calendars,
						service.calendar
				)	);
				if(calendar && !mainMask.empty())
				{
					*sservice |= calendar->getResult(mainMask);
				}
				else
				{
					missingCalendars.insert(service.calendar);
				}
			}

			// Abort if at least one missing calendar
			if(!missingCalendars.empty())
			{
				stringstream os;
				os << "At least a calendar is missing. Details :<ul>";
				BOOST_FOREACH(const string& code, missingCalendars)
				{
					os << "<li>" << code << "</li>";
				}
				os << "</ul>";
				_logError(os.str());
				return false;
			}

			return true;
		}



		DBTransaction HastusInterfaceFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;

			// Add remove queries generated by _selectObjectsToRemove
			_addRemoveQueries(transaction);

			// Save each created or altered objects
			BOOST_FOREACH(Registry<CommercialLine>::value_type cline, _env.getRegistry<CommercialLine>())
			{
				CommercialLineTableSync::Save(cline.second.get(), transaction);
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
			return transaction;
		}



		//////////////////////////////////////////////////////////////////////////
		// HELPERS
		void HastusInterfaceFileFormat::Importer_::_loadNextRecord(
			double recordNumber
		) const {

			while(!_eof() && (!_record || _record->recordNumber != recordNumber))
			{
				_loadNextRecord();
			}
		}



		void HastusInterfaceFileFormat::Importer_::_loadNextRecord() const
		{
			string line;
			while(line.size() < 5 || line.substr(0,5) == "     ")
			{
				if(!getline(_file, line))
				{
					Record record;
					record.recordNumber = 0;
					_record = record;
					return;
				}
			}

			string recordNumber;
			for(size_t pos(1); pos<line.size(); ++pos)
			{
				if(line[pos] == ' ')
				{
					recordNumber = line.substr(1, pos-1);
					break;
				}
			}
			double lineRecordNumber(lexical_cast<double>(recordNumber));

			Record record;
			record.recordNumber = lineRecordNumber;
			record.content = line;
			_record = record;
			return;
		}



		bool HastusInterfaceFileFormat::Importer_::_eof() const
		{
			return _record && _record->recordNumber == 0;
		}



		void HastusInterfaceFileFormat::Importer_::_openFile(
			const boost::filesystem::path& filePath
		) const	{
			_file.clear();
			_file.open(filePath.file_string().c_str());
			_record.reset();
			if(!_file)
			{
				throw Exception("Could no open the file " + filePath.file_string());
			}
		}



		string HastusInterfaceFileFormat::Importer_::_getTextField(
			size_t start,
			size_t length
		) const	{
			// Empty record
			if(!_record)
			{
				return string();
			}

			// Fix of the position
			--start;

			// Current record is too short to match
			if(_record->content.size() < start)
			{
				return string();
			}

			// Declarations
			IConv converter(_import.get<DataSource>()->get<Charset>(), "UTF-8");

			// Read the current record
			if(_record->content.size() < start+length)
			{
				return converter.convert(
					trim_copy(_record->content.substr(start))
				);
			}
			return converter.convert(
				trim_copy(_record->content.substr(start, length))
			);
		}



		vector<string> HastusInterfaceFileFormat::Importer_::_getNextVector(
			double recordNumber,
			size_t numberPosition,
			size_t start,
			size_t length
		) const	{

			// Declarations
			IConv converter(_import.get<DataSource>()->get<Charset>(), "UTF-8");

			// Fix of the positions
			--numberPosition;
			--start;

			// Search of the section
			_loadNextRecord(recordNumber);

			// EOF check
			if(_eof())
			{
				return vector<string>();
			}

			// Reading of the vector size
			size_t vectorSize(
				lexical_cast<size_t>(
					trim_copy(
						_record->content.substr(numberPosition, start-numberPosition)
			)	)	);

			// Loading of the first row
			vector<string> result;
			if(_record->content.size() < start)
			{
				return result;
			}
			size_t position(start);
			for(size_t loadedRecords(0); loadedRecords < vectorSize;)
			{
				while(_record->content.size() >= position && loadedRecords < vectorSize)
				{
					if(_record->content.size() < position+length)
					{
						string value(
							converter.convert(
								trim_copy(_record->content.substr(position))
						)	);
						if(!value.empty())
						{
							result.push_back(value);
							++loadedRecords;
						}
						break;
					}
					else
					{
						string value(
							converter.convert(
								trim_copy(_record->content.substr(position, length))
						)	);
						if(!value.empty())
						{
							result.push_back(value);
							++loadedRecords;
						}
					}
					position += length;
				}

				if(loadedRecords < vectorSize)
				{
					// Load of the next line of the file
					if(!getline(_file, _record->content))
					{
						_record->recordNumber = 0;
						_record->content.clear();
						return result;
					}

					position = start;
				}
			}
			return result;
		}
}	}
