
/** ObitiFileFormat class implementation.
	@file ObitiFileFormat.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "ObitiFileFormat.hpp"
#include "TransportNetwork.h"
#include "StopArea.hpp"
#include "PTFileFormat.hpp"
#include "JourneyPatternTableSync.hpp"
#include "LineStopTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "ImpExModule.h"
#include "PropertiesHTMLTable.h"
#include "DataSourceAdmin.h"
#include "AdminFunctionRequest.hpp"
#include "PropertiesHTMLTable.h"
#include "DataSourceAdmin.h"
#include "AdminFunctionRequest.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "PTPlaceAdmin.h"
#include "StopPointAdmin.hpp"
#include "StopAreaAddAction.h"
#include "RollingStock.hpp"
#include "RollingStockTableSync.hpp"
#include "StopArea.hpp"
#include "DataSource.h"
#include "CalendarTemplateTableSync.h"
#include "Importer.hpp"
#include "AdminActionFunctionRequest.hpp"
#include "HTMLModule.h"
#include "HTMLForm.h"
#include "DBModule.h"
#include "ObitiFileFormat.hpp"
#include "City.h"
#include "PTFileFormat.hpp"
#include "CityTableSync.h"
#include "DestinationTableSync.hpp"
#include "Junction.hpp"
#include "JunctionTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "PTUseRuleTableSync.h"
#include "IConv.hpp"
#include "ContinuousService.h"
#include "Path.h"
#include "ServiceCalendarLink.hpp"
#include "ServiceCalendarLinkTableSync.hpp"

#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>

#include <geos/geom/Geometry.h>
#include <geos/geom/GeometryCollection.h>
#include <geos/geom/Coordinate.h>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::gregorian;
using namespace boost::posix_time;
using namespace geos::geom;

namespace synthese
{
	using namespace impex;
	using namespace pt;
	using namespace util;
	using namespace db;
	using namespace calendar;
	using namespace graph;
	using namespace html;
	using namespace admin;
	using namespace server;
	using namespace geography;

	namespace util
	{
		template<> const string FactorableTemplate<FileFormat,ObitiFileFormat>::FACTORY_KEY("Obiti");
	}

	namespace pt
	{
		const std::string ObitiFileFormat::Importer_::FILE_ARRET("arrets");
		const std::string ObitiFileFormat::Importer_::FILE_ITINERAIRES("itineraires");
		const std::string ObitiFileFormat::Importer_::FILE_LIGNE("ligne");
		const std::string ObitiFileFormat::Importer_::FILE_HORAIRE("horaire");

		const std::string ObitiFileFormat::Importer_::SEP(";");

		const std::string ObitiFileFormat::Importer_::PARAMETER_LINE_OBITI_ID("line_obiti_id");
		const std::string ObitiFileFormat::Importer_::PARAMETER_NETWORK_ID("network_id");
		const std::string ObitiFileFormat::Importer_::PARAMETER_ROLLING_STOCK_ID("rolling_stock_id");
		const std::string ObitiFileFormat::Importer_::PARAMETER_IMPORT_STOP_AREA("isa");
		const std::string ObitiFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_CITY("sadc");
		const std::string ObitiFileFormat::Importer_::PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION("sadt");
		const std::string ObitiFileFormat::Importer_::PARAMETER_DISPLAY_LINKED_STOPS("display_linked_stops");
		const string ObitiFileFormat::Importer_::PARAMETER_USE_RULE_BLOCK_ID_MASK("use_rule_block_id_mask");
	}

	namespace impex
	{
		template<> const MultipleFileTypesImporter<ObitiFileFormat>::Files MultipleFileTypesImporter<ObitiFileFormat>::FILES(
			ObitiFileFormat::Importer_::FILE_ARRET.c_str(),
			ObitiFileFormat::Importer_::FILE_ITINERAIRES.c_str(),
			ObitiFileFormat::Importer_::FILE_LIGNE.c_str(),
			ObitiFileFormat::Importer_::FILE_HORAIRE.c_str(),
		"");
	}


	namespace pt
	{
		bool ObitiFileFormat::Importer_::_checkPathsMap() const
		{
			FilePathsMap::const_iterator it(_pathsMap.find(FILE_ARRET));
			if(it == _pathsMap.end() || it->second.empty()) return false;
			it = _pathsMap.find(FILE_ITINERAIRES);
			if(it == _pathsMap.end() || it->second.empty()) return false;
			return true;
		}



		ObitiFileFormat::Importer_::Importer_(
			util::Env& env,
			const impex::DataSource& dataSource
		):	MultipleFileTypesImporter<ObitiFileFormat>(env, dataSource),
			Importer(env, dataSource),
			_importStopArea(false),
			_interactive(true),
			_displayLinkedStops(false),
			_lines(_dataSource, env),
			_stopAreas(_dataSource, env),
			_stopPoints(_dataSource, env)
		{}



		bool ObitiFileFormat::Importer_::_parse(
			const boost::filesystem::path& filePath,
			std::ostream& stream,
			const std::string& key,
			boost::optional<const admin::AdminRequest&> request
		) const {
			ifstream inFile;
			inFile.open(filePath.file_string().c_str());
			if(!inFile)
			{
				throw Exception("Could no open the file " + filePath.file_string());
			}
			string line;
			if(!getline(inFile, line))
			{
				return false;
			}
			_loadFieldsMap(line);

			stream << "INFO : Loading file " << filePath << " as " << key << "<br />";

			// 1 : Stop Areas
			if(key == FILE_ARRET)
			{
				PTFileFormat::ImportableStopAreas linkedStopAreas;
				PTFileFormat::ImportableStopAreas nonLinkedStopAreas;
				// Loop
				while(getline(inFile, line))
				{
					_loadLine(line);

					string id(_getValue("IDArret"));
					string name(_getValue("Appellation Commerciale1"));
					string cityName(_getValue("Commune"));

					// City
					City* city(NULL);
					CityTableSync::SearchResult cities(
						CityTableSync::Search(_env, cityName, optional<string>(), optional<string>(), 0, 1)
					);
					if(cities.empty())
					{
						stream << "WARN : City " << cityName << " not found<br />";
					}
					else
					{
						city = cities.begin()->get();
					}

					if(request)
					{
						PTFileFormat::ImportableStopArea isa;
						isa.operatorCode = id;
						isa.name = name;
						isa.linkedStopAreas = _stopAreas.get(id);
						isa.cityName = cityName;

						if(isa.linkedStopAreas.empty())
						{
							nonLinkedStopAreas.push_back(isa);
						}
						else if(_displayLinkedStops)
						{
							linkedStopAreas.push_back(isa);
						}
					}

					PTFileFormat::CreateOrUpdateStopAreas(
						_stopAreas,
						id,
						name,
						city,
						false,
						_stopAreaDefaultTransferDuration,
						_dataSource,
						_env,
						stream
					);
				}

				if(request)
				{
					PTFileFormat::DisplayStopAreaImportScreen(
						nonLinkedStopAreas,
						*request,
						true,
						false,
						_defaultCity,
						_env,
						_dataSource,
						stream
					);
					if(_displayLinkedStops)
					{
						PTFileFormat::DisplayStopAreaImportScreen(
							linkedStopAreas,
							*request,
							true,
							false,
							_defaultCity,
							_env,
							_dataSource,
							stream
							);
					}
				}
			}

			// 2 : Physical Stops
			if(key == FILE_ITINERAIRES)
			{
				// 2.2 : stops
				PTFileFormat::ImportableStopPoints linkedStopPoints;
				PTFileFormat::ImportableStopPoints nonLinkedStopPoints;

				// Loop
				while(getline(inFile, line))
				{
					_loadLine(line);

					string idLigne(_getValue("idLigne"));
					string stopAreaId(_getValue("idarret"));
					string id(_getValue("nom_commune") + " " + _getValue("nom_arret") + " " + idLigne + " " + (_getValue("sens_aller") == "O" ? "A" : "R"));
					string name(_getValue("app_comm_arret"));

					// Stop area
					const StopArea* stopArea(NULL);
					if(_stopAreas.contains(stopAreaId))
					{
						stopArea = *_stopAreas.get(stopAreaId).begin();
					}
					else
					{
						stream << "WARN : inconsistent stop area id "<< stopAreaId <<" in the stop point "<< id <<"<br />";
						continue;
					}

					// Point
					shared_ptr<geos::geom::Point> point;

					PTFileFormat::ImportableStopPoint isp;
					isp.operatorCode = id;
					isp.name = name;
					isp.cityName = _getValue("nom_commune");
					isp.linkedStopPoints = _stopPoints.get(id);
					isp.stopArea = stopArea;
					isp.coords = point;

					if(request && isp.linkedStopPoints.empty())
					{
						nonLinkedStopPoints.push_back(isp);
					}
					else if(request && _displayLinkedStops)
					{
						linkedStopPoints.push_back(isp);
					}

					// Creation or update
					PTFileFormat::CreateOrUpdateStop(
						_stopPoints,
						id,
						name,
						NULL,
						stopArea,
						point.get(),
						_dataSource,
						_env,
						stream
					);
				}

				if(request)
				{
					PTFileFormat::DisplayStopPointImportScreen(
						nonLinkedStopPoints,
						*request,
						_env,
						_dataSource,
						stream
					);
					if(_displayLinkedStops)
					{
						PTFileFormat::DisplayStopPointImportScreen(
							linkedStopPoints,
							*request,
							_env,
							_dataSource,
							stream
						);
					}
				}
			}

			// 3 : lines
			if(key == FILE_LIGNE)
			{
				// Loop
				while(getline(inFile, line))
				{
					_loadLine(line);

					LineObitiElement lineObiti;

					string idLigne = _getValue("IDLigne");
					lineObiti.name = _getValue("Nom");
					lineObiti.shortname = _getValue("Code Commerciale");
					lineObiti.backward = _getValue("Sens") == "Retour" ? true : false;
					_linesMap.insert(make_pair(idLigne, lineObiti));
				}
			}

			// 4 services & schedules
			if(key == FILE_HORAIRE)
			{
				ImportableTableSync::ObjectBySource<CommercialLineTableSync> lines(_dataSource, _env);

				// Line
				string id(_lineObitiID);
				LinesMap::const_iterator it(_linesMap.find(id));
				if(it == _linesMap.end())
				{
					stream << "WARN : Obiti line ID "<< id << " not found<br />";
					return false;
				}
				LineObitiElement lineObiti(it->second);

				// Network
				const TransportNetwork* network;
				if(_network.get())
				{
					network = _network.get();
				}
				else
				{
					stream << "WARN : network not defined";
					return false;
				}

				// RollingStock
				RollingStock* rollingStock;
				if(_rollingStock.get())
				{
					rollingStock = _rollingStock.get();
				}
				else
				{
					stream << "WARN : rollingStock not defined";
					return false;
				}

				// Color
				optional<RGBColor> color;
				CommercialLine* commercialLine = PTFileFormat::CreateOrUpdateLine(
					_lines,
					id,
					lineObiti.name, // route_long_name
					lineObiti.shortname, // route_short_name
					color,
					*network,
					_dataSource,
					_env,
					stream
				);

				// Routes
				string serviceID;
				string daysCalendarName;
				string periodCalendarName;

				ScheduledService::Schedules departureSchedules;
				ScheduledService::Schedules arrivalSchedules;
				time_duration lastTd(minutes(0));

				_firstLine(inFile,line);
				int numService = 1;
				while(_line[numService] != "'-1")
				{
					// Service ID
					serviceID = _line[numService];

					// Period Calendar
					getline(inFile, line);
					_loadLine(line);
					periodCalendarName = _line[numService];
					CalendarTemplate* periodCalendar(NULL);
					CalendarTemplateTableSync::SearchResult calendars(
						CalendarTemplateTableSync::Search(
							_env,
							periodCalendarName,
							optional<RegistryKeyType>(),
							false,
							true,
							0,
							1,
							UP_LINKS_LOAD_LEVEL,
							optional<RegistryKeyType>())
					);
					if(calendars.empty())
					{
						stream << "WARN : Calendar <pre>\"" << periodCalendarName << "\"</pre> not found<br />";
					}
					else
					{
						periodCalendar = calendars.begin()->get();
					}

					// Days Calendar
					getline(inFile, line);
					_loadLine(line);
					daysCalendarName = _line[numService];
					CalendarTemplate* daysCalendar(NULL);
					CalendarTemplateTableSync::SearchResult calendars2(
						CalendarTemplateTableSync::Search(
							_env,
							daysCalendarName,
							optional<RegistryKeyType>(),
							false,
							true,
							0,
							1,
							UP_LINKS_LOAD_LEVEL,
							optional<RegistryKeyType>())
					);
					if(calendars2.empty())
					{
						stream << "WARN : Calendar <pre>\"" << daysCalendarName << "\"</pre> not found<br />";
					}
					else
					{
						daysCalendar = calendars2.begin()->get();
					}

					// move to Schedule
					while(_line[0] != "   Transporteur")
					{
						getline(inFile, line);
						_loadLine(line);
					}

					JourneyPattern::StopsWithDepartureArrivalAuthorization stops;
					departureSchedules.clear();
					arrivalSchedules.clear();
					lastTd = minutes(0);

					// Schedule
					while(getline(inFile, line))
					{
						_loadLine(line);
						string stopPointId(_line[0] + " " + _lineObitiID + " " + (lineObiti.backward ? "R" : "A"));
						string timeStr(_line[numService]);

						const StopPoint* stopPoint(NULL);
						if(_stopPoints.contains(stopPointId))
							stopPoint = *_stopPoints.get(stopPointId).begin();
						else
						{
							stream << "WARN : inconsistent stop id "<< stopPointId <<" in the service "<< numService <<"<br />";
							continue;
						}

						if(timeStr.size() > 4)
						{
							time_duration td(
								lexical_cast<int>(timeStr.substr(0,2)),
								lexical_cast<int>(timeStr.substr(3,2)),
								0
							);
							if(td < lastTd)
							{
								td += hours(24);
							}
							departureSchedules.push_back(td - seconds(td.seconds()));
							arrivalSchedules.push_back(td.seconds() ? td + seconds(60 - td.seconds()) : td);
							lastTd = td;

							JourneyPattern::StopWithDepartureArrivalAuthorization::StopsSet linkedStops(_stopPoints.get(stopPointId));
							JourneyPattern::StopWithDepartureArrivalAuthorization stop(
								linkedStops,
								optional<double>(),
								true,
								true,
								true
							);
							stops.push_back(stop);
						}
					}

					// Route
					JourneyPattern* route(
						PTFileFormat::CreateOrUpdateRoute(
							*commercialLine,
							id,
							id,
							optional<const string&>(),
							optional<Destination*>(),
							optional<const RuleUser::Rules&>(),
							lineObiti.backward,
							rollingStock,
							stops,
							_dataSource,
							_env,
							stream,
							true
					)	);
					if(route == NULL)
					{
						stream << "WARN : failure at route creation ("<< id <<")<br />";
						continue;
					}

					// Service
					ScheduledService* service(
						PTFileFormat::CreateOrUpdateService(
							*route,
							departureSchedules,
							arrivalSchedules,
							serviceID,
							_dataSource,
							_env,
							stream
					)	);

					// Calendars
					if(service)
					{
						boost::shared_ptr<ServiceCalendarLink> link(new ServiceCalendarLink(ServiceCalendarLinkTableSync::getId()));

						if(periodCalendar)
							link->setCalendarTemplate(periodCalendar);
						if(daysCalendar)
							link->setCalendarTemplate2(daysCalendar);

						link->setService(service);

						_env.getEditableRegistry<ServiceCalendarLink>().add(shared_ptr<ServiceCalendarLink>(link));
					}

					// next service
					_firstLine(inFile,line);
					numService++;
				}
			}
			return true;
		}



		void ObitiFileFormat::Importer_::displayAdmin(
			std::ostream& stream,
			const admin::AdminRequest& request
		) const	{
			stream << "<h1>Fichiers</h1>";

			AdminFunctionRequest<DataSourceAdmin> reloadRequest(request);
			PropertiesHTMLTable t(reloadRequest.getHTMLForm());
			stream << t.open();
			stream << t.title("Mode");
			stream << t.cell("Effectuer import", t.getForm().getOuiNonRadioInput(DataSourceAdmin::PARAMETER_DO_IMPORT, false));
			stream << t.title("Import arrêts");
			stream << t.cell("Fichier arrêt", t.getForm().getTextInput(_getFileParameterName(FILE_ARRET), _pathsMap[FILE_ARRET].file_string()));
			stream << t.cell("Fichier itinéraires", t.getForm().getTextInput(_getFileParameterName(FILE_ITINERAIRES), _pathsMap[FILE_ITINERAIRES].file_string()));
			stream << t.title("Import services & horaires");
			stream << t.cell("Fichier ligne", t.getForm().getTextInput(_getFileParameterName(FILE_LIGNE), _pathsMap[FILE_LIGNE].file_string()));
			stream << t.cell("Fichier horaire", t.getForm().getTextInput(_getFileParameterName(FILE_HORAIRE), _pathsMap[FILE_HORAIRE].file_string()));
			stream << t.cell("Ligne concernée (ID Obiti)", t.getForm().getTextInput(PARAMETER_LINE_OBITI_ID, _lineObitiID));
			stream << t.title("Paramètres");
			stream << t.cell("Réseau (ID)", t.getForm().getTextInput(PARAMETER_NETWORK_ID, _network.get() ? lexical_cast<string>(_network->getKey()) : string()));
			stream << t.cell("Mode de transport (ID)",
			t.getForm().getTextInput(PARAMETER_ROLLING_STOCK_ID, _rollingStock.get() ? lexical_cast<string>(_rollingStock->getKey()) : string()));
			stream << t.cell("Affichage arrêts liés", t.getForm().getOuiNonRadioInput(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops));
			stream << t.cell("Import zones d'arrêt", t.getForm().getOuiNonRadioInput(PARAMETER_IMPORT_STOP_AREA, _importStopArea));
			stream << t.cell("Commune par défaut (ID)", t.getForm().getTextInput(PARAMETER_STOP_AREA_DEFAULT_CITY, _defaultCity.get() ? lexical_cast<string>(_defaultCity->getKey()) : string()));
			stream << t.cell("Temps de transfert par défaut (min)", t.getForm().getTextInput(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, lexical_cast<string>(_stopAreaDefaultTransferDuration.total_seconds() / 60)));
			stream << t.cell("Masque règles d'utilisation", t.getForm().getTextInput(PARAMETER_USE_RULE_BLOCK_ID_MASK, _serializePTUseRuleBlockMasks(_ptUseRuleBlockMasks)));
			stream << t.close();
		}



		db::DBTransaction ObitiFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;

			// Saving of each created or altered objects
			BOOST_FOREACH(Registry<StopArea>::value_type cstop, _env.getRegistry<StopArea>())
			{
				StopAreaTableSync::Save(cstop.second.get(), transaction);
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
			BOOST_FOREACH(Registry<ServiceCalendarLink>::value_type link, _env.getRegistry<ServiceCalendarLink>())
			{
				ServiceCalendarLinkTableSync::Save(link.second.get(), transaction);
			}

			return transaction;
		}



		void ObitiFileFormat::Importer_::_loadFieldsMap( const std::string& line ) const
		{
			vector<string> cols;
			split(
				cols,
				line[line.size() - 1] == '\r' ? line.substr(0, line.size() - 1) : line,
				is_any_of(SEP)
			);
			_fieldsMap.clear();
			size_t rank(0);
			BOOST_FOREACH(const string& col, cols)
			{
				_fieldsMap[col] = rank;
				++rank;
			}
		}



		std::string ObitiFileFormat::Importer_::_getValue( const std::string& field ) const
		{
			return trim_copy(_line[_fieldsMap[field]]);
		}



		void ObitiFileFormat::Importer_::_loadLine( const std::string& line ) const
		{
			_line.clear();
			if(!line.empty())
			{
				string utfline(
					line[line.size() - 1] == '\r' ?
					line.substr(0, line.size() - 1) :
					line
				);
				utfline = IConv::IConv(_dataSource.getCharset(), "UTF-8").convert(line);
				split(_line, utfline, is_any_of(SEP));
			}
		}



		void ObitiFileFormat::Importer_::_firstLine(ifstream& inFile, std::string& line) const
		{
			inFile.clear();
			inFile.seekg(0, ios::beg);
			getline(inFile, line);
			_loadLine(line);
		}




		util::ParametersMap ObitiFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_IMPORT_STOP_AREA, _importStopArea);
			map.insert(PARAMETER_DISPLAY_LINKED_STOPS, _displayLinkedStops);
			map.insert(PARAMETER_LINE_OBITI_ID, _lineObitiID);
			if(_defaultCity.get())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_CITY, _defaultCity->getKey());
			}
			if(!_stopAreaDefaultTransferDuration.is_not_a_date_time())
			{
				map.insert(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, _stopAreaDefaultTransferDuration.total_seconds() / 60);
			}
			if(_network.get())
			{
				map.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}
			if(_rollingStock.get())
			{
				map.insert(PARAMETER_ROLLING_STOCK_ID, _rollingStock->getKey());
			}
			map.insert(PARAMETER_USE_RULE_BLOCK_ID_MASK, _serializePTUseRuleBlockMasks(_ptUseRuleBlockMasks));
			return map;
		}



		void ObitiFileFormat::Importer_::_setFromParametersMap( const util::ParametersMap& map )
		{
			_importStopArea = map.getDefault<bool>(PARAMETER_IMPORT_STOP_AREA, false);
			_stopAreaDefaultTransferDuration = minutes(map.getDefault<long>(PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION, 8));
			_displayLinkedStops = map.getDefault<bool>(PARAMETER_DISPLAY_LINKED_STOPS, false);
			_lineObitiID = map.getDefault<string>(PARAMETER_LINE_OBITI_ID, string());

			if(map.getDefault<RegistryKeyType>(PARAMETER_NETWORK_ID, 0))
			{
				_network = TransportNetworkTableSync::Get(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID), _env);
			}

			if(map.getDefault<RegistryKeyType>(PARAMETER_ROLLING_STOCK_ID, 0))
			{
				_rollingStock = RollingStockTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_ROLLING_STOCK_ID), _env);
			}

			if(map.getDefault<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY, 0))
			{
				_defaultCity = CityTableSync::Get(map.get<RegistryKeyType>(PARAMETER_STOP_AREA_DEFAULT_CITY), _env);
			}

			string ptUseRuleBlockMasksStr(map.getDefault<string>(PARAMETER_USE_RULE_BLOCK_ID_MASK));
			if(!ptUseRuleBlockMasksStr.empty())
			{
				vector<string> rules;
				split(rules, ptUseRuleBlockMasksStr, is_any_of(","));
				BOOST_FOREACH(const string& rule, rules)
				{
					vector<string> parts;
					split(parts, rule, is_any_of("="));

					if(parts.size() < 2)
					{
						continue;
					}

					try
					{
						shared_ptr<const PTUseRule> ptUseRule(
							PTUseRuleTableSync::Get(
								lexical_cast<RegistryKeyType>(parts[1]),
								_env
						)	);
						_ptUseRuleBlockMasks.insert(make_pair(parts[0], ptUseRule.get()));
					}
					catch (...)
					{
					}
				}
			}

		}



		std::string ObitiFileFormat::Importer_::_serializePTUseRuleBlockMasks( const PTUseRuleBlockMasks& object )
		{
			bool first(true);
			stringstream serializedPTUseRuleBlockMasks;
			BOOST_FOREACH(const PTUseRuleBlockMasks::value_type& rule, object)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					serializedPTUseRuleBlockMasks << ",";
				}
				serializedPTUseRuleBlockMasks << rule.first << "=" << rule.second->getKey();
			}
			return serializedPTUseRuleBlockMasks.str();
		}
	}
}
