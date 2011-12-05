
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

#include "DataSource.h"
#include "DBModule.h"
#include "HastusInterfaceFileFormat.hpp"
#include "GraphConstants.h"
#include "StopPoint.hpp"
#include "StopPointTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "ContinuousService.h"
#include "ContinuousServiceTableSync.h"
#include "JourneyPattern.hpp"
#include "JourneyPatternTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "LineStopTableSync.h"
#include "City.h"
#include "CityTableSync.h"
#include "Service.h"
#include "RollingStock.hpp"
#include "NonConcurrencyRule.h"
#include "NonConcurrencyRuleTableSync.h"
#include "ReservationContact.h"
#include "ReservationContactTableSync.h"
#include "PTUseRule.h"
#include "PTConstants.h"
#include "CoordinatesSystem.hpp"
#include "Conversion.h"
#include "XmlToolkit.h"
#include "DBTransaction.hpp"
#include "CityAliasTableSync.hpp"
#include "JunctionTableSync.hpp"
#include "RollingStockTableSync.hpp"
#include "ImportFunction.h"
#include "PropertiesHTMLTable.h"
#include "RequestException.h"
#include "AdminFunctionRequest.hpp"
#include "DataSourceAdmin.h"
#include "PTFileFormat.hpp"
#include "ImpExModule.h"
#include "DesignatedLinePhysicalStop.hpp"
#include "CommercialLineTableSync.h"
#include "TransportNetworkTableSync.h"
#include "CalendarTemplateTableSync.h"
#include "CalendarFileFormat.hpp"

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
	using namespace geography;
	using namespace util::XmlToolkit;
	using namespace util;
	using namespace graph;
	using namespace impex;
	using namespace db;
	using namespace pt;
	using namespace server;
	using namespace admin;
	using namespace html;
	using namespace calendar;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,pt::HastusInterfaceFileFormat>::FACTORY_KEY("HastusInterface");
	}

	namespace pt
	{
		const string HastusInterfaceFileFormat::Importer_::PARAMETER_TRANSPORT_NETWORK_ID = "network_id";


		//////////////////////////////////////////////////////////////////////////
		// CONSTRUCTOR
		HastusInterfaceFileFormat::Importer_::Importer_(
			Env& env,
			const DataSource& dataSource
		):	OneFileTypeImporter<Importer_>(env, dataSource),
			Importer(env, dataSource),
			PTDataCleanerFileFormat(env, dataSource)
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
		}



		//////////////////////////////////////////////////////////////////////////
		// INPUT

		bool HastusInterfaceFileFormat::Importer_::_parse(
			const path& filePath,
			ostream& os,
			boost::optional<const admin::AdminRequest&> adminRequest
		) const {

			// Load object linked to the datasource
			impex::ImportableTableSync::ObjectBySource<CalendarTemplateTableSync> calendars(_dataSource, _env);

			_file.open(filePath.file_string().c_str());
			if(!_file)
			{
				throw Exception("Could no open the file " + filePath.file_string());
			}

			// Record 1.1 Lines number
			vector<string> lineNumbers(
				_getNextVector(1.1, 19, 22, 5)
			);

			// Record 2 : Lines
			ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(_dataSource, _env);
			typedef map<string, shared_ptr<RollingStock> > LineTransportModes;
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
				shared_ptr<RollingStock> rollingStock;
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
				PTFileFormat::CreateOrUpdateLine(
					lines,
					lineCode,
					name,
					lineCode,
					optional<RGBColor>(),
					*_network,
					_dataSource,
					_env,
					os
				);
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
			ImportableTableSync::ObjectBySource<StopPointTableSync> stops(_dataSource, _env);
			PTFileFormat::ImportableStopPoints nonLinkedStopPoints;
			bool success(true);
			BOOST_FOREACH(StopCodes::value_type& stopCode, stopCodes)
			{
				// Stop
				stopCode.second = PTFileFormat::GetStopPoints(
					stops,
					stopCode.first,
					optional<const string&>(),
					os,
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
				PTFileFormat::DisplayStopPointImportScreen(
					nonLinkedStopPoints,
					*adminRequest,
					_env,
					_dataSource,
					os
				);

				os << "ERR  : Au moins un arrêt non trouvé : import interrompu<br />";
				return false;
			}

			// Loop on temporary services
			set<string> missingCalendars;
			BOOST_FOREACH(const TemporaryService& service, services)
			{
				// Line
				CommercialLine* line(
					PTFileFormat::GetLine(
						lines,
						service.lineCode,
						_dataSource,
						_env,
						os
				)	);
				if(line == NULL)
				{
					os << "WARN : Inconsistent line number " << service.lineCode << " in service " << service.code << "<br />";
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
					os << "WARN : A scheduled stop did not match with the full stop list : " << service.scheduledStops[scheduledStopsI] << " after " << service.stops[lastScheduledStop] << ". Service is ignored.<br />";
					continue;
				}

				// Route
				JourneyPattern* route(
					PTFileFormat::CreateOrUpdateRoute(
						*line,
						service.routeCode,
						optional<const string&>(),
						service.routeCode,
						optional<Destination*>(),
						optional<const RuleUser::Rules&>(),
						service.wayBack,
						lineTransportModes[service.lineCode].get(),
						servedStops,
						_dataSource,
						_env,
						os,
						true, // Remove old codes
						false // Don't update metric offsets on update because default metric offset are approximations
				)	);
				if(route == NULL)
				{
					os << "WARN : Route " << service.routeCode << " was not built in service " << service.code << "<br />";
					continue;
				}

				// Service
				ScheduledService* sservice(
					PTFileFormat::CreateOrUpdateService(
						*route,
						service.schedules,
						service.schedules,
						service.code,
						_dataSource,
						_env,
						os
				)	);

				// Calendar
				CalendarTemplate* calendar(
					CalendarFileFormat::GetCalendarTemplate(
						calendars,
						service.calendar,
						os
				)	);
				if(calendar)
				{
					*sservice |= calendar->getResult(_calendar);
				}
				else
				{
					missingCalendars.insert(service.calendar);	
				}
			}

			// Abort if at least one missing calendar
			if(!missingCalendars.empty())
			{
				os << "ERR  : At least a calendar is missing. Details :<ul>";
				BOOST_FOREACH(const string& code, missingCalendars)
				{
					os << "<li>" << code << "</li>";
				}
				os << "</ul>";
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



		std::string synthese::pt::HastusInterfaceFileFormat::Importer_::_getTextField(
			std::size_t start,
			std::size_t length
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
			IConv converter(_dataSource.getCharset(), "UTF-8");

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



		std::vector<std::string> synthese::pt::HastusInterfaceFileFormat::Importer_::_getNextVector(
			double recordNumber,
			std::size_t numberPosition,
			std::size_t start,
			std::size_t length
		) const	{

			// Declarations
			IConv converter(_dataSource.getCharset(), "UTF-8");

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



		void HastusInterfaceFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const admin::AdminRequest& request
		) const	{
			AdminFunctionRequest<DataSourceAdmin> importRequest(request);
			PropertiesHTMLTable t(importRequest.getHTMLForm());
			stream << t.open();
			stream << t.title("Mode");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.cell("Effacer données anciennes", t.getForm().getOuiNonRadioInput(PARAMETER_CLEAN_OLD_DATA, false));
			stream << t.title("Données (remplir un des deux champs)");
			stream << t.cell("Ligne", t.getForm().getTextInput(PARAMETER_PATH, (_pathsSet.size() == 1) ? _pathsSet.begin()->file_string() : string()));
			stream << t.cell("Répertoire", t.getForm().getTextInput(PARAMETER_DIRECTORY, _dirPath.file_string()));
			stream << t.title("Paramètres");
			stream << t.cell("Réseau", t.getForm().getTextInput(PARAMETER_TRANSPORT_NETWORK_ID, _network.get() ? lexical_cast<string>(_network->getKey()) : string()));
			stream << t.cell("Date début", t.getForm().getCalendarInput(PARAMETER_START_DATE, _startDate ? *_startDate : date(not_a_date_time)));
			stream << t.cell("Date fin", t.getForm().getCalendarInput(PARAMETER_END_DATE, _endDate ? *_endDate : date(not_a_date_time)));
			stream << t.close();
		}
}	}
