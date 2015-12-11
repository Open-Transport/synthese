
/** TridentFileFormat class implementation.
	@file TridentFileFormat.cpp

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

#include "TridentFileFormat.h"

#include "DataSource.h"
#include "DBModule.h"
#include "GraphConstants.h"
#include "Import.hpp"
#include "ScheduledServiceTableSync.h"
#include "ContinuousServiceTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "LineStopTableSync.h"
#include "DRTArea.hpp"
#include "CityTableSync.h"
#include "NonConcurrencyRuleTableSync.h"
#include "ReservationContactTableSync.h"
#include "PTModule.h"
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
#include "CalendarTemplateElementTableSync.h"
#include "CalendarLinkTableSync.hpp"

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
		template<> const string FactorableTemplate<FileFormat, TridentFileFormat>::FACTORY_KEY("Trident");
	}

	namespace data_exchange
	{
		const string TridentFileFormat::Importer_::PARAMETER_IMPORT_STOPS("impstp");
		const string TridentFileFormat::Importer_::PARAMETER_IMPORT_JUNCTIONS("impjun");
		const string TridentFileFormat::Importer_::PARAMETER_DEFAULT_TRANSFER_DURATION("dtd");
		const string TridentFileFormat::Importer_::PARAMETER_AUTOGENERATE_STOP_AREAS("asa");
		const string TridentFileFormat::Importer_::PARAMETER_TREAT_ALL_STOP_AREA_AS_QUAY("sasp");
		const string TridentFileFormat::Importer_::PARAMETER_IMPORT_TIMETABLES_AS_TEMPLATES("itt");
		const string TridentFileFormat::Importer_::PARAMETER_USE_CALENDAR_LINKS("use_calendar_links");
		const string TridentFileFormat::Importer_::PARAMETER_THROW_WARNING_SERVICE_ON_TWO_DAYS("throw_warning_in_case_service_on_two_days");
		const string TridentFileFormat::Importer_::PARAMETER_MERGE_ROUTES("mr");
		const string TridentFileFormat::Exporter_::PARAMETER_LINE_ID("li");
		const string TridentFileFormat::Exporter_::PARAMETER_WITH_TISSEO_EXTENSION("wt");
		const string TridentFileFormat::Exporter_::PARAMETER_WITH_OLD_DATES("wod");
		const string TridentFileFormat::Exporter_::PARAMETER_WRITE_ONLY_DEPARTURE_TIMES("write_only_departure_times");
		const string TridentFileFormat::Exporter_::PARAMETER_EXCLUDE_HLP("exclude_hlp");

		TridentFileFormat::SRIDConversionMap TridentFileFormat::_SRIDConversionMap;

		string ToXsdDaysDuration (date_duration daysDelay);
		string ToXsdDuration(posix_time::time_duration duration);
		string ToXsdTime (const time_duration& time);
		time_duration FromXsdDuration(const std::string& text);



		//////////////////////////////////////////////////////////////////////////
		// CONSTRUCTOR
		TridentFileFormat::Importer_::Importer_(
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
			_importStops(false),
			_autoGenerateStopAreas(false),
			_importJunctions(false),
			_mergeRoutes(true),
			_defaultTransferDuration(minutes(8)),
			_importTimetablesAsTemplates(false),
			_useCalendarLinks(false),
			_throwWarnInCaseScheduleUp24(false),
			_calendarTemplates(*import.get<DataSource>(), env),
			_stopAreas(*import.get<DataSource>(), env),
			_stops(*import.get<DataSource>(), env),
			_networks(*import.get<DataSource>(), env),
			_lines(*import.get<DataSource>(), env)
		{}



		//////////////////////////////////////////////////////////////////////////
		// REQUESTS HANDLING
		util::ParametersMap TridentFileFormat::Importer_::_getParametersMap() const
		{
			ParametersMap result(PTDataCleanerFileFormat::_getParametersMap());
			result.insert(PARAMETER_IMPORT_STOPS, _importStops);
			result.insert(PARAMETER_IMPORT_JUNCTIONS, _importJunctions);
			result.insert(PARAMETER_AUTOGENERATE_STOP_AREAS, _autoGenerateStopAreas);
			if(!_defaultTransferDuration.is_not_a_date_time())
			{
				result.insert(PARAMETER_DEFAULT_TRANSFER_DURATION, _defaultTransferDuration.total_seconds() / 60);
			}
			result.insert(PARAMETER_TREAT_ALL_STOP_AREA_AS_QUAY, _treatAllStopAreaAsQuay);
			result.insert(PARAMETER_IMPORT_TIMETABLES_AS_TEMPLATES, _importTimetablesAsTemplates);
			result.insert(PARAMETER_USE_CALENDAR_LINKS, _useCalendarLinks);
			result.insert(PARAMETER_THROW_WARNING_SERVICE_ON_TWO_DAYS, _throwWarnInCaseScheduleUp24);
			result.insert(PARAMETER_MERGE_ROUTES, _mergeRoutes);
			return result;
		}



		util::ParametersMap TridentFileFormat::Exporter_::getParametersMap() const
		{
			ParametersMap result;
			if(_startDate < day_clock::local_day())
			{
				date_duration du(day_clock::local_day() - _startDate);
				result.insert(PARAMETER_WITH_OLD_DATES, static_cast<int>(du.days()));
			}
			if(_line.get())
			{
				result.insert(PARAMETER_LINE_ID, _line->getKey());
			}
			result.insert(PARAMETER_WITH_TISSEO_EXTENSION, _withTisseoExtension);

			result.insert(PARAMETER_WRITE_ONLY_DEPARTURE_TIMES, _writeOnlyDepartureTimes);
			result.insert(PARAMETER_EXCLUDE_HLP, _excludeHLP);
			return result;
		}



		void TridentFileFormat::Importer_::_setFromParametersMap(const ParametersMap& map)
		{
			PTDataCleanerFileFormat::_setFromParametersMap(map);
			_importStops = map.getDefault<bool>(PARAMETER_IMPORT_STOPS, false);
			_autoGenerateStopAreas = map.getDefault<bool>(PARAMETER_AUTOGENERATE_STOP_AREAS, false);
			_importJunctions = map.getDefault<bool>(PARAMETER_IMPORT_JUNCTIONS, false);
			if(map.getDefault<int>(PARAMETER_DEFAULT_TRANSFER_DURATION, 0))
			{
				_defaultTransferDuration = minutes(map.get<int>(PARAMETER_DEFAULT_TRANSFER_DURATION));
			}
			_treatAllStopAreaAsQuay = map.getDefault<bool>(PARAMETER_TREAT_ALL_STOP_AREA_AS_QUAY, false);
			_importTimetablesAsTemplates = map.getDefault<bool>(PARAMETER_IMPORT_TIMETABLES_AS_TEMPLATES, false);
			_useCalendarLinks = map.getDefault<bool>(PARAMETER_USE_CALENDAR_LINKS, false);
			_throwWarnInCaseScheduleUp24 = map.getDefault<bool>(PARAMETER_THROW_WARNING_SERVICE_ON_TWO_DAYS, false);
			_mergeRoutes = map.getDefault<bool>(PARAMETER_MERGE_ROUTES, true);
		}



		void TridentFileFormat::Exporter_::setFromParametersMap(const ParametersMap& map)
		{
			_startDate = day_clock::local_day();
			_startDate -= days(map.getDefault<int>(PARAMETER_WITH_OLD_DATES, 0));

			RegistryKeyType id(map.get<RegistryKeyType>(PARAMETER_LINE_ID));
			if (id == 0)
				throw RequestException("JourneyPattern id must be specified");

			try
			{
				_line = CommercialLineTableSync::Get(id, _env);
			}
			catch (...)
			{
				throw RequestException("No such line");
			}

			_withTisseoExtension = map.getDefault<bool>(PARAMETER_WITH_TISSEO_EXTENSION, false);

			_writeOnlyDepartureTimes = map.getDefault<bool>(PARAMETER_WRITE_ONLY_DEPARTURE_TIMES, false);
			_excludeHLP = map.getDefault<bool>(PARAMETER_EXCLUDE_HLP, false);
		}


		//////////////////////////////////////////////////////////////////////////
		// OUTPUT

		void TridentFileFormat::Exporter_::build(
			ostream& os
		) const {
			static const string peerid ("SYNTHESE");

			// Collect all data related to selected commercial line
			JourneyPatternTableSync::Search(
				_env,
				_line->getKey(),
				0,
				optional<size_t>(),
				true, true, UP_LINKS_LOAD_LEVEL
			);
			NonConcurrencyRuleTableSync::Search(
				_env,
				_line->getKey(),
				_line->getKey(),
				false
			);

			bool resaIsCompulsory=false;
            BOOST_FOREACH(Registry<PTUseRule>::value_type r, _env.getRegistry<PTUseRule>())
            {
				const PTUseRule& rule(*r.second);

				if (rule.getReservationType() == PTUseRule::RESERVATION_RULE_FORBIDDEN || (rule.getMinDelayDays().days() == 0 && rule.getMinDelayMinutes().total_seconds() == 0))       continue;
				resaIsCompulsory = true;
				break;
			}

			// Lines
			const RollingStock* rollingStock(NULL);
			BOOST_FOREACH(Registry<JourneyPattern>::value_type itline, _env.getRegistry<JourneyPattern>())
			{
				const JourneyPattern& line(*itline.second);
				if (line.getRollingStock())
					rollingStock = line.getRollingStock();
				LineStopTableSync::Search(
					_env,
					line.getKey(),
					optional<RegistryKeyType>(),
					0,
					optional<size_t>(),
					true, true,
					UP_LINKS_LOAD_LEVEL
				);
			}

			// Link DRTAreas physical stops
			BOOST_FOREACH(const LineStop::Registry::value_type& itls, _env.getRegistry<LineStop>())
			{
				itls.second->link(_env); // Todo : useful ?
			}

			BOOST_FOREACH(Registry<JourneyPattern>::value_type itline, _env.getRegistry<JourneyPattern>())
			{
				const JourneyPattern& line(*itline.second);
				ScheduledServiceTableSync::Search(
					_env,
					line.getKey(),
					optional<RegistryKeyType>(),
					optional<RegistryKeyType>(),
					optional<string>(),
					false,
					0,
					optional<size_t>(),
					true, true,
					UP_DOWN_LINKS_LOAD_LEVEL
				);
				ContinuousServiceTableSync::Search(
					_env,
					line.getKey(),
					optional<RegistryKeyType>(),
					0,
					optional<size_t>(),
					true, true,
					UP_DOWN_LINKS_LOAD_LEVEL
				);
			}


			// Writing of the header
			os << "<?xml version='1.0' encoding='UTF-8'?>" << "\n" << "\n";
		    if (_withTisseoExtension)
				os << "<TisseoPTNetwork xmlns='http://www.trident.org/schema/trident' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.trident.org/schema/trident  https://extranet.rcsmobility.com/projects/synthese/repository/raw/doc/include/35_pt/trident2-tisseo/tisseo-chouette-extension.xsd'>" << "\n";
			else
				os << "<ChouettePTNetwork xmlns='http://www.trident.org/schema/trident' xmlns:xsi='http://www.w3.org/2001/XMLSchema-instance' xsi:schemaLocation='http://www.trident.org/schema/trident  https://extranet.rcsmobility.com/projects/synthese/repository/raw/doc/include/35_pt/chouette/Chouette.xsd'>" << "\n";

			// --------------------------------------------------- PTNetwork
			const TransportNetwork* tn(_line->getNetwork());
			os << "<PTNetwork>" << "\n";
			os << "<objectId>" << TridentId (peerid, "PTNetwork", *tn) << "</objectId>" << "\n";
			os << "<versionDate>" << to_iso_extended_string(day_clock::local_day()) << "</versionDate>" << "\n";
			os << "<name>" << tn->getName () << "</name>" << "\n";
			os << "<registration>" << "\n";
			os << "<registrationNumber>" << tn->getKey() << "</registrationNumber>" << "\n";
			os << "</registration>" << "\n";
			os << "<lineId>" << TridentId (peerid, "Line", *_line) << "</lineId>" << "\n";
			if(!_withTisseoExtension && resaIsCompulsory)
				os << "<comment>TAD : compulsory reservation</comment>" << "\n";
			else
				os << "<comment />" << "\n";
			os << "</PTNetwork>" << "\n";

			// --------------------------------------------------- GroupOfLine


			// --------------------------------------------------- Company
			os << "<Company>" << "\n";
			os << "<objectId>SYNTHESE:Company:1</objectId>" << "\n";
			os << "<name>" << tn->getName() << "</name>" << "\n";
			os << "<shortName>" << tn->getName() << "</shortName>" << "\n";
			os << "<organisationalUnit></organisationalUnit>" << "\n";
			os << "<operatingDepartmentName></operatingDepartmentName>" << "\n";
			os << "<code>-</code>" << "\n";
			os << "<phone>-</phone>" << "\n";
			os << "<fax></fax>" << "\n";
			os << "<email></email>" << "\n";
			os << "<registration><registrationNumber>1</registrationNumber></registration>" << "\n";
			os << "</Company>" << "\n";

			os << "<ChouetteArea>" << "\n";

			// --------------------------------------------------- StopArea (type = BoardingPosition)
			// BoardingPosition corresponds to a very accurate location along a quay for instance.
			// Not implemented right now.

			// --------------------------------------------------- StopArea (type = Quay) <=> StopPoint
			BOOST_FOREACH(Registry<StopPoint>::value_type itps, _env.getRegistry<StopPoint>())
			{
				const StopPoint* ps(itps.second.get());
				util::Env fakeEnv;
				LineStopTableSync::SearchResult lineStops(
					LineStopTableSync::Search(fakeEnv, boost::optional<RegistryKeyType>(), ps->getKey())
				);

				if (lineStops.empty())
					continue;

				os << "<StopArea>" << "\n";

				os << "<objectId>" << TridentId (peerid, "StopArea", *ps) << "</objectId>" << "\n";
				os << "<creatorId>" << ps->getCodeBySources() << "</creatorId>" << "\n";

				os << "<name>";
				if(ps->getName().empty())
					os << ps->getConnectionPlace ()->getName ();
				else
					os << ps->getName ();
				os << "</name>" << "\n";

				BOOST_FOREACH(const LineStopTableSync::SearchResult::value_type& ls, lineStops)
				{
					os << "<contains>" << TridentId(peerid, "StopPoint", *ls)  << "</contains>" << "\n";
				}

				os << "<centroidOfArea>" << TridentId (peerid, "AreaCentroid", *ps) << "</centroidOfArea>" << "\n";
				os << "<StopAreaExtension>" << "\n";
				os << "<areaType>" << "Quay" << "</areaType>" << "\n";
				string rn = ps->getCodeBySources();
				if (rn.empty ()) rn = "0";
				os << "<registration><registrationNumber>" << rn << "</registrationNumber></registration>" << "\n";
				os << "</StopAreaExtension>" << "\n";
				os << "</StopArea>" << "\n";

			}

			// --------------------------------------------------- StopArea (type = CommercialStopPoint)
			BOOST_FOREACH(
				Registry<StopArea>::value_type itcp,
				_env.getRegistry<StopArea>()
			){
				const StopArea* cp(itcp.second.get());
				const StopArea::PhysicalStops& stops(cp->getPhysicalStops());

				// Don't export empty StopArea, not valid.
				// When NonConcurrency rules are loaded above, StopAreas are added to the environnement without any StopPoints associated.
				// It breaks the XML file validation.
				if(stops.size() > 0)
				{
					os << "<StopArea>" << "\n";
					os << "<objectId>" << TridentId (peerid, "StopArea", *cp) << "</objectId>" << "\n";
					os << "<name>" << cp->getName () << "</name>" << "\n";

					// Contained physical stops

					for(StopArea::PhysicalStops::const_iterator it(stops.begin()); it != stops.end(); ++it)
					{
						os << "<contains>" << TridentId (peerid, "StopArea", *it->second)  << "</contains>" << "\n";
					}

					// Decide what to take for centroidOfArea of a connectionPlace. Only regarding physical stops coordinates
					// or also regarding addresses coordinates, or fixed manually ?
					// os << "<centroidOfArea>" << TridentId (peerid, "AreaCentroid", cp->getKey ()) << "</centroidOfArea>" << "\n";

					os << "<StopAreaExtension>" << "\n";
					os << "<areaType>" << "CommercialStopPoint" << "</areaType>" << "\n";
					os << "</StopAreaExtension>" << "\n";
					os << "</StopArea>" << "\n";
				}
			}


			// --------------------------------------------------- StopArea (type = ITL)
			// "Interdiction de traffic local"
			// Not mapped right now.



			// --------------------------------------------------- AreaCentroid
			BOOST_FOREACH(Registry<StopPoint>::value_type itps, _env.getRegistry<StopPoint>())
			{
				const StopPoint& ps(*itps.second);

				boost::shared_ptr<Point> wgs84ps;
				if(ps.hasGeometry())
				{
					wgs84ps = CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(*ps.getGeometry());
				}
				os << fixed;
				os << "<AreaCentroid>" << "\n";
				os << "<objectId>" << TridentId (peerid, "AreaCentroid", ps) << "</objectId>" << "\n";

				os << "<longitude>" << (ps.hasGeometry() ? wgs84ps->getX() : 0) << "</longitude>" << "\n";
				os << "<latitude>" << (ps.hasGeometry() ? wgs84ps->getY() : 0) << "</latitude>" << "\n";
				os << "<longLatType>" << (ps.hasGeometry() ? _getTridentFromSRID(wgs84ps->getSRID()) : "WGS84") << "</longLatType>" << "\n";

				// we do not provide full addresses right now.
				os << "<address><countryCode>" << ps.getConnectionPlace()->getCity()->get<Code>() << "</countryCode></address>";

				if(ps.hasGeometry())
				{
					os << "<projectedPoint>" << "\n";
					os << "<X>" << ps.getGeometry()->getX() << "</X>" << "\n";
					os << "<Y>" << ps.getGeometry()->getY() << "</Y>" << "\n";
					os << "<projectionType>" << _getTridentFromSRID(ps.getGeometry()->getSRID()) << "</projectionType>" << "\n";
					os << "</projectedPoint>" << "\n";
				}

				os << "<containedIn>" << TridentId (peerid, "StopArea", ps.getKey ()) << "</containedIn>" << "\n";
				os << "<name>" << ps.getKey() << "</name>" << "\n";

				os << "</AreaCentroid>" << "\n";
			}

			os << "</ChouetteArea>" << "\n";

			// --------------------------------------------------- ConnectionLink
			BOOST_FOREACH(
				Registry<StopArea>::value_type itcp,
				_env.getRegistry<StopArea>()
			){
				const StopArea& cp(*itcp.second);
				if(!cp.isConnectionPossible()) continue;

				// Contained physical stops
				const StopArea::PhysicalStops& stops(cp.getPhysicalStops());
				BOOST_FOREACH(const StopArea::PhysicalStops::value_type& it1, stops)
				{
					BOOST_FOREACH(const StopArea::PhysicalStops::value_type& it2, stops)
					{
						if(!cp.isConnectionAllowed(*it1.second, *it2.second))
							continue;

						os << "<ConnectionLink>" << "\n";
						stringstream clkey;
						clkey << it1.second->getKey () << "t" << it2.second->getKey ();
						os << "<objectId>" << TridentId (peerid, "ConnectionLink", clkey.str ()) << "</objectId>" << "\n";
						os << "<startOfLink>" << TridentId (peerid, "StopArea", it1.first) << "</startOfLink>" << "\n";
						os << "<endOfLink>" << TridentId (peerid, "StopArea", it2.first) << "</endOfLink>" << "\n";
						os << "<defaultDuration>" << ToXsdDuration (cp.getTransferDelay (*it1.second, *it2.second)) << "</defaultDuration>" << "\n";
						os << "</ConnectionLink>" << "\n";
					}
				}
			}

			// --------------------------------------------------- Timetable
			// One timetable per service
			BOOST_FOREACH(Registry<ScheduledService>::value_type itsrv, _env.getRegistry<ScheduledService>())
			{
				const ScheduledService* srv(itsrv.second.get());
				if (_excludeHLP && _isSServiceHLP(srv))
				{
						continue;
					}

				os << "<Timetable>" << "\n";
				os << "<objectId>" << TridentId (peerid, "Timetable", *srv) << "</objectId>" << "\n";

				BOOST_FOREACH(const date& d, srv->getActiveDates())
				{
					if(d < _startDate)
					{
						continue;
					}
					os << "<calendarDay>" << to_iso_extended_string(d) << "</calendarDay>" << "\n";
				}
				os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", *srv) << "</vehicleJourneyId>" << "\n";

				os << "</Timetable>" << "\n";
			}
			BOOST_FOREACH(Registry<ContinuousService>::value_type itsrv, _env.getRegistry<ContinuousService>())
			{
				const ContinuousService* srv(itsrv.second.get());
				if (_excludeHLP && _isCServiceHLP(srv))
				{
						// Exclude this service
						continue;
					}
				os << "<Timetable>" << "\n";
				os << "<objectId>" << TridentId (peerid, "Timetable", *srv) << "</objectId>" << "\n";

				BOOST_FOREACH(const date& d, srv->getActiveDates())
				{
					if(d < _startDate)
					{
						continue;
					}
					os << "<calendarDay>" << to_iso_extended_string(d) << "</calendarDay>" << "\n";
				}
				os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", *srv) << "</vehicleJourneyId>" << "\n";

				os << "</Timetable>" << "\n";
			}


			// --------------------------------------------------- TimeSlot
			BOOST_FOREACH(Registry<ContinuousService>::value_type itsrv, _env.getRegistry<ContinuousService>())
			{
				const ContinuousService* csrv(itsrv.second.get());
				if (_excludeHLP && _isCServiceHLP(csrv))
				{
						// Exclude this service
						continue;
					}
				string timeSlotId;
				timeSlotId = TridentId(peerid, "TimeSlot", *csrv);

				os << "<TimeSlot>" << "\n";
				os << "<objectId>" << timeSlotId << "</objectId>" << "\n";
				os << "<beginningSlotTime>" << ToXsdTime(Service::GetTimeOfDay(csrv->getDepartureBeginScheduleToIndex(false, 0))) << "</beginningSlotTime>" << "\n";
				os << "<endSlotTime>" << ToXsdTime(Service::GetTimeOfDay(csrv->getDepartureEndScheduleToIndex(false, 0))) << "</endSlotTime>" << "\n";
				os << "<firstDepartureTimeInSlot>" << ToXsdTime(Service::GetTimeOfDay(csrv->getDepartureBeginScheduleToIndex(false, 0))) << "</firstDepartureTimeInSlot>" << "\n";
				os << "<lastDepartureTimeInSlot>" << ToXsdTime(Service::GetTimeOfDay(csrv->getDepartureEndScheduleToIndex(false, 0))) << "</lastDepartureTimeInSlot>" << "\n";
				os << "</TimeSlot>" << "\n";
			}


			// --------------------------------------------------- ChouetteLineDescription
			{
			os << "<ChouetteLineDescription>" << "\n";

			// --------------------------------------------------- Line
			{
				os << "<Line>" << "\n";
				os << "<objectId>" << TridentId (peerid, "Line", *_line) << "</objectId>" << "\n";
				os << "<name>" << _line->getName () << "</name>" << "\n";
				os << "<number>" << _line->getShortName () << "</number>" << "\n";
				os << "<publishedName>" << _line->getLongName () << "</publishedName>" << "\n";

				os <<
					"<transportModeName>" <<
					((rollingStock == NULL || rollingStock->get<TridentKey>().empty()) ? "Other" : rollingStock->get<TridentKey>()) <<
					"</transportModeName>" <<
				"\n";

				BOOST_FOREACH(Registry<JourneyPattern>::value_type line, _env.getRegistry<JourneyPattern>())
				{
					os << "<routeId>" << TridentId (peerid, "ChouetteRoute", *line.second) << "</routeId>" << "\n";
				}
				os << "<registration>" << "\n";
				os << "<registrationNumber>" << _line->getKey() << "</registrationNumber>" << "\n";
				os << "</registration>" << "\n";

				os << "</Line>" << "\n";
			}

			// --------------------------------------------------- ChouetteRoute
			BOOST_FOREACH(Registry<JourneyPattern>::value_type itline, _env.getRegistry<JourneyPattern>())
			{
				const JourneyPattern* line(itline.second.get());

				if (_excludeHLP && _hasJPOnlyHLP(line))
				{
						continue;
					}

				os << "<ChouetteRoute>" << "\n";
				os << "<objectId>" << TridentId (peerid, "ChouetteRoute", line->getKey ()) << "</objectId>" << "\n";
				os << "<name>" << line->getName () << "</name>" << "\n";

				os << "<publishedName>";
				{
					const StopPoint* ps(static_cast<const StopPoint*>(line->getOrigin()));
					if (ps && ps->getConnectionPlace () && ps->getConnectionPlace ()->getCity ())
						os << ps->getConnectionPlace ()->getCity ()->getName () << " " << ps->getConnectionPlace ()->getName ();
				}
				os << " -&gt; ";
				{
					const StopPoint* ps(static_cast<const StopPoint*>(line->getDestination()));
					if (ps && ps->getConnectionPlace () && ps->getConnectionPlace ()->getCity ())
						os << ps->getConnectionPlace ()->getCity ()->getName () << " " << ps->getConnectionPlace ()->getName ();
				}
				os << "</publishedName>" << "\n";

				const LinePhysicalStop* from(NULL);
				BOOST_FOREACH(const Edge* to, line->getEdges())
				{
					if (from != NULL)
					{
						os << "<ptLinkId>" << TridentId (peerid, "PtLink", *from) << "</ptLinkId>" << "\n";
					}
					from = static_cast<const LinePhysicalStop*>(to);
				}


				os << "<journeyPatternId>" << TridentId (peerid, "JourneyPattern", *line) << "</journeyPatternId>" << "\n";

				// Wayback
				int wayback(line->getWayBack() ? 1 : 0);
				if (_withTisseoExtension)
					++wayback;

				os << "<RouteExtension><wayBack>";
				if (!logic::indeterminate(line->getWayBack()))
				{
					os << wayback;
				}
				os << "</wayBack></RouteExtension>" << "\n";
				os << "</ChouetteRoute>" << "\n";
			}

			// --------------------------------------------------- StopPoint
			BOOST_FOREACH(Registry<LineStop>::value_type lineStop, _env.getRegistry<LineStop>())
			{
				StopPoint* ps(
					dynamic_cast<StopPoint*>(
						&*lineStop.second->get<LineNode>()
				)	);
				if(!ps)
				{
					continue;
				}

				boost::shared_ptr<Point> wgs84ps;
				if(ps->hasGeometry())
				{
					wgs84ps = CoordinatesSystem::GetCoordinatesSystem(4326).convertPoint(*ps->getGeometry());
				}

				os << fixed;
				os << "<StopPoint" << (_withTisseoExtension ? " xsi:type=\"TisseoStopPointType\"" : "") << ">" << "\n";
				os << "<objectId>" << TridentId (peerid, "StopPoint", *lineStop.second) << "</objectId>" << "\n";
				os << "<creatorId>" << ps->getCodeBySources() << "</creatorId>" << "\n";
				os << "<longitude>" << (ps->hasGeometry() ? wgs84ps->getX() :0) << "</longitude>" << "\n";
				os << "<latitude>" << (ps->hasGeometry() ? wgs84ps->getY() : 0) << "</latitude>" << "\n";
				os << "<longLatType>" << (ps->hasGeometry() ? _getTridentFromSRID(wgs84ps->getSRID()) : "WGS84") << "</longLatType>" << "\n";

				os << "<address><countryCode>" << ps->getConnectionPlace()->getCity()->get<Code>() << "</countryCode></address>";

				if(ps->hasGeometry())
				{
					os << "<projectedPoint>" << "\n";
					os << "<X>" << ps->getGeometry()->getX() << "</X>" << "\n";
					os << "<Y>" << ps->getGeometry()->getY() << "</Y>" << "\n";
					os << "<projectionType>" << _getTridentFromSRID(ps->getGeometry()->getSRID()) << "</projectionType>" << "\n";
					os << "</projectedPoint>" << "\n";
				}


				os << "<containedIn>" << TridentId (peerid, "StopArea", *ps) << "</containedIn>" << "\n";
				os << "<name>";
				if (ps->getName ().empty ())
					os << ps->getConnectionPlace ()->getName ();
				else
					os << ps->getName ();
				os << "</name>" << "\n";

				os << "<lineIdShortcut>" << TridentId (peerid, "Line", *_line) << "</lineIdShortcut>" << "\n";
				os << "<ptNetworkIdShortcut>" << TridentId (peerid, "PTNetwork", *tn) << "</ptNetworkIdShortcut>" << "\n";

				if (_withTisseoExtension)
				{
					os << "<mobilityRestrictedSuitability>0</mobilityRestrictedSuitability>" << "\n";
				}
				os << "</StopPoint>" << "\n";
			}

			// --------------------------------------------------- ITL
			// Not implemented
			{

			}


			// --------------------------------------------------- PtLink
			BOOST_FOREACH(Registry<JourneyPattern>::value_type line, _env.getRegistry<JourneyPattern>())
			{
				if (_excludeHLP && _hasJPOnlyHLP(line.second.get()))
				{
						continue;
					}
				const LinePhysicalStop* from(NULL);
				BOOST_FOREACH(const Edge* toEdge, line.second->getEdges())
				{
					const LinePhysicalStop* to = static_cast<const LinePhysicalStop*>(toEdge);					if (from != NULL)
					{
						os << "<PtLink>" << "\n";
						os << "<objectId>" << TridentId (peerid, "PtLink", *from) << "</objectId>" << "\n";
						os << "<startOfLink>" << TridentId (peerid, "StopPoint", *from) << "</startOfLink>" << "\n";
						os << "<endOfLink>" << TridentId (peerid, "StopPoint", *to) << "</endOfLink>" << "\n";
						os << "<linkDistance>" << (to->getMetricOffset() - from->getMetricOffset()) << "</linkDistance>" << "\n";   // in meters!
						os << "</PtLink>" << "\n";
					}
					from = to;
				}
			}

			// --------------------------------------------------- JourneyPattern
			// One per route
			BOOST_FOREACH(Registry<JourneyPattern>::value_type itline, _env.getRegistry<JourneyPattern>())
			{
				const JourneyPattern* line(itline.second.get());
				if (line->getEdges().empty())
					continue;

				if (_excludeHLP && _hasJPOnlyHLP(line))
				{
						continue;
					}

				os << "<JourneyPattern>" << "\n";
				os << "<objectId>" << TridentId (peerid, "JourneyPattern", *line) << "</objectId>" << "\n";
				os << "<routeId>" << TridentId (peerid, "ChouetteRoute", *line) << "</routeId>" << "\n";

				const vector<Edge*>& edges = line->getEdges ();
				const LinePhysicalStop* origin = static_cast<const LinePhysicalStop*>(edges.at(0));
				const LinePhysicalStop* destination = static_cast<const LinePhysicalStop*>(edges.at(edges.size()-1));
				os << "<origin>" << TridentId (peerid, "StopPoint", *origin) << "</origin>" << "\n";
				os << "<destination>" << TridentId (peerid, "StopPoint", *destination) << "</destination>" << "\n";

				for (vector<Edge*>::const_iterator itedge = edges.begin ();
					 itedge != edges.end (); ++itedge)
				{
					const DesignatedLinePhysicalStop* lineStop = dynamic_cast<const DesignatedLinePhysicalStop*> (*itedge);
					if(!lineStop)
					{
						continue;
					}
					os << "<stopPointList>" << TridentId (peerid, "StopPoint", *lineStop) << "</stopPointList>" << "\n";
				}

				os << "<lineIdShortcut>" << TridentId (peerid, "Line", *_line) << "</lineIdShortcut>" << "\n";
				os << "</JourneyPattern>" << "\n";
			}

			// --------------------------------------------------- VehicleJourney
			BOOST_FOREACH(Registry<ScheduledService>::value_type itsrv, _env.getRegistry<ScheduledService>())
			{
				const ScheduledService* srv(itsrv.second.get());
				if (_excludeHLP && _isSServiceHLP(srv))
				{
						// Exclude this service
						continue;
					}
				bool isDRT(
					dynamic_cast<const PTUseRule*>(&srv->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET)) != NULL &&
					static_cast<const PTUseRule&>(srv->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET)).getReservationType() != PTUseRule::RESERVATION_RULE_FORBIDDEN
				);

				os << "<VehicleJourney";
				if (_withTisseoExtension)
				{
					os << " xsi:type=\"" << (isDRT ? "DRTVehicleJourneyType" : "TisseoVehicleJourneyType" ) << "\"";
				}
				os << ">" << "\n";
				os << "<objectId>" << TridentId (peerid, "VehicleJourney", *srv) << "</objectId>" << "\n";
				os << "<creatorId>" << srv->getServiceNumber() << "</creatorId>" << "\n";
				os << "<routeId>" << TridentId (peerid, "ChouetteRoute", srv->getPath()->getKey()) << "</routeId>" << "\n";
				os << "<journeyPatternId>" << TridentId (peerid, "JourneyPattern", srv->getPath()->getKey()) << "</journeyPatternId>" << "\n";
				os << "<lineIdShortcut>" << TridentId (peerid, "Line", *_line) << "</lineIdShortcut>" << "\n";
				os << "<routeIdShortcut>" << TridentId (peerid, "ChouetteRoute", srv->getPath()->getKey()) << "</routeIdShortcut>" << "\n";
				if (!srv->getServiceNumber().empty())
				{
					os << "<number>" << srv->getServiceNumber() << "</number>" << "\n";
				}

				// --------------------------------------------------- VehicleJourneyAtStop

				LineStopTableSync::SearchResult linestops(
					LineStopTableSync::Search(_env, srv->getPath()->getKey())
				);
				BOOST_FOREACH(const boost::shared_ptr<LineStop>& ls, linestops)
				{
					os << "<vehicleJourneyAtStop>" << "\n";
					os << "<stopPointId>" << TridentId (peerid, "StopPoint", *ls) << "</stopPointId>" << "\n";
					os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", *srv) << "</vehicleJourneyId>" << "\n";

					if (ls->get<RankInPath>() > 0 && ls->get<IsArrival>())
						os << "<arrivalTime>" << ToXsdTime (Service::GetTimeOfDay(srv->getArrivalBeginScheduleToIndex(false, ls->get<RankInPath>())))
						<< "</arrivalTime>" << "\n";

					os	<< "<departureTime>";
					if (ls->get<RankInPath>()+1 != linestops.size() && ls->get<IsDeparture>() && !_writeOnlyDepartureTimes)
					{
						os << ToXsdTime (Service::GetTimeOfDay(srv->getDepartureBeginScheduleToIndex(false, ls->get<RankInPath>())));
					}
					else
					{
						os << ToXsdTime (Service::GetTimeOfDay(srv->getArrivalBeginScheduleToIndex(false, ls->get<RankInPath>())));
					}
					os	<< "</departureTime>" << "\n";

					if(!ls->get<IsDeparture>() && !ls->get<IsArrival>())
					{
						os << "<boardingAlightingPossibility>NeitherBoardOrAlight</boardingAlightingPossibility>\n";
					}
					else if(!ls->get<IsDeparture>())
					{
						os << "<boardingAlightingPossibility>AlightOnly</boardingAlightingPossibility>\n";
					}
					else if(!ls->get<IsArrival>())
					{
						os << "<boardingAlightingPossibility>BoardOnly</boardingAlightingPossibility>\n";
					}

					os << "</vehicleJourneyAtStop>" << "\n";
				}

				if (_withTisseoExtension)
				{
					const UseRule& hRule(srv->getUseRule(USER_HANDICAPPED - USER_CLASS_CODE_OFFSET));
					os <<
						"<mobilityRestrictedSuitability>" <<
						(!hRule.getAccessCapacity() || *hRule.getAccessCapacity()) <<
						"</mobilityRestrictedSuitability>" <<
						"\n"
					;
					if(	dynamic_cast<const PTUseRule*>(&hRule) != NULL &&
						static_cast<const PTUseRule&>(hRule).getReservationType() != PTUseRule::RESERVATION_RULE_FORBIDDEN
					){
						os << "<mobilityRestrictedReservationRule>" <<
							TridentId(
								peerid,
								"ReservationRule",
								static_cast<const PTUseRule&>(hRule).getKey()
							) <<
							"</mobilityRestrictedReservationRule>" <<
							"\n"
						;
					}

					const UseRule& bRule(srv->getUseRule(USER_BIKE - USER_CLASS_CODE_OFFSET));
					os <<
						"<bikeSuitability>" <<
						(!bRule.getAccessCapacity() || *bRule.getAccessCapacity()) <<
						"</bikeSuitability>" <<
						"\n"
					;
					if (dynamic_cast<const PTUseRule*>(&bRule) != NULL &&
						static_cast<const PTUseRule&>(bRule).getReservationType() != PTUseRule::RESERVATION_RULE_FORBIDDEN
					){
						os <<
							"<bikeReservationRule>" <<
							TridentId(
								peerid,
								"ReservationRule",
								static_cast<const PTUseRule&>(bRule).getKey()
							) <<
							"</bikeReservationRule>" <<
							"\n"
						;
					}
					if (isDRT)
					{
						const PTUseRule& pRule(static_cast<const PTUseRule&>(srv->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET)));
						os << "<reservationRule>" << TridentId(peerid, "ReservationRule", pRule.getKey()) << "</reservationRule>" << "\n";
					}
				}
				else //!_withTisseoExtension
					if(resaIsCompulsory)
                                                os << "<comment>TAD : compulsory reservation</comment>"  << "\n";
				os << "</VehicleJourney>" << "\n";
			}

			BOOST_FOREACH(Registry<ContinuousService>::value_type itsrv, _env.getRegistry<ContinuousService>())
			{
				const ContinuousService* srv(itsrv.second.get());
				if (_excludeHLP && _isCServiceHLP(srv))
				{
						// Exclude this service
						continue;
					}
				bool isDRT(
					dynamic_cast<const PTUseRule*>(&srv->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET)) &&
					static_cast<const PTUseRule&>(srv->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET)).getReservationType() != PTUseRule::RESERVATION_RULE_FORBIDDEN
				);

				os << "<VehicleJourney";
				if (_withTisseoExtension)
				{
					os << " xsi:type=\"" << (isDRT ? "DRTVehicleJourneyType" : "TisseoVehicleJourneyType" ) << "\"";
				}
				os << ">" << "\n";
				os << "<objectId>" << TridentId (peerid, "VehicleJourney", *srv) << "</objectId>" << "\n";
				os << "<creatorId>" << srv->getServiceNumber() << "</creatorId>" << "\n";
				os << "<routeId>" << TridentId (peerid, "ChouetteRoute", srv->getPath()->getKey()) << "</routeId>" << "\n";
				os << "<journeyPatternId>" << TridentId (peerid, "JourneyPattern", srv->getPath()->getKey()) << "</journeyPatternId>" << "\n";
				os << "<lineIdShortcut>" << TridentId (peerid, "Line", *_line) << "</lineIdShortcut>" << "\n";
				os << "<routeIdShortcut>" << TridentId (peerid, "ChouetteRoute", srv->getPath()->getKey()) << "</routeIdShortcut>" << "\n";
				if (!srv->getServiceNumber().empty())
				{
					os << "<number>" << srv->getServiceNumber() << "</number>" << "\n";
				}

				// --------------------------------------------------- VehicleJourneyAtStop
				{
					LineStopTableSync::SearchResult linestops(
						LineStopTableSync::Search(_env, srv->getPath()->getKey())
					);
					BOOST_FOREACH(const boost::shared_ptr<LineStop>& ls, linestops)
					{
						os << "<vehicleJourneyAtStop>" << "\n";
						os << "<stopPointId>" << TridentId (peerid, "StopPoint", *ls) << "</stopPointId>" << "\n";
						os << "<vehicleJourneyId>" << TridentId (peerid, "VehicleJourney", *srv) << "</vehicleJourneyId>" << "\n";

						const time_duration& schedule((ls->get<RankInPath>() > 0 && ls->get<IsArrival>()) ? srv->getArrivalBeginScheduleToIndex(false, ls->get<RankInPath>()) : srv->getDepartureBeginScheduleToIndex(false, ls->get<RankInPath>()));
						os << "<elapseDuration>" << ToXsdDuration(schedule - srv->getDepartureBeginScheduleToIndex(false, 0)) << "</elapseDuration>" << "\n";
						os << "<headwayFrequency>" << ToXsdDuration(srv->getMaxWaitingTime()) << "</headwayFrequency>" << "\n";

						os << "</vehicleJourneyAtStop>" << "\n";
					}
				}
				os << "<timeSlotId>" << TridentId(peerid, "TimeSlot", *srv) << "</timeSlotId>" << "\n";

				if (_withTisseoExtension)
				{
					const UseRule& hRule(srv->getUseRule(USER_HANDICAPPED - USER_CLASS_CODE_OFFSET));
					os <<
						"<mobilityRestrictedSuitability>" <<
						(!hRule.getAccessCapacity() || *hRule.getAccessCapacity()) <<
						"</mobilityRestrictedSuitability>" <<
						"\n"
					;
					if(	dynamic_cast<const PTUseRule*>(&hRule) != NULL &&
						static_cast<const PTUseRule&>(hRule).getReservationType() != PTUseRule::RESERVATION_RULE_FORBIDDEN
					){
						os << "<mobilityRestrictedReservationRule>" <<
							TridentId(
								peerid,
								"ReservationRule",
								static_cast<const PTUseRule&>(hRule).getKey()
							) <<
							"</mobilityRestrictedReservationRule>" <<
							"\n"
						;
					}

					const UseRule& bRule(srv->getUseRule(USER_BIKE - USER_CLASS_CODE_OFFSET));
					os <<
						"<bikeSuitability>" <<
						(!bRule.getAccessCapacity() || *bRule.getAccessCapacity()) <<
						"</bikeSuitability>" <<
						"\n"
					;
					if (dynamic_cast<const PTUseRule*>(&bRule) != NULL &&
						static_cast<const PTUseRule&>(bRule).getReservationType() != PTUseRule::RESERVATION_RULE_FORBIDDEN
					){
						os <<
							"<bikeReservationRule>" <<
							TridentId(
								peerid,
								"ReservationRule",
								static_cast<const PTUseRule&>(bRule).getKey()
							) <<
							"</bikeReservationRule>" <<
							"\n"
						;
					}
					if (isDRT)
					{
						const PTUseRule& pRule(static_cast<const PTUseRule&>(srv->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET)));
						os << "<reservationRule>" << TridentId(peerid, "ReservationRule", pRule.getKey()) << "</reservationRule>" << "\n";
					}
				}
				os << "</VehicleJourney>" << "\n";
			}


			os << "</ChouetteLineDescription>" << "\n";
			}


			if (_withTisseoExtension)
			{
				// Reservation Rules -----------------------------------------------------------------------

 				BOOST_FOREACH(Registry<PTUseRule>::value_type r, _env.getRegistry<PTUseRule>())
 				{
 					const PTUseRule& rule(*r.second);

					if (rule.getReservationType() == PTUseRule::RESERVATION_RULE_FORBIDDEN || (rule.getMinDelayDays().days() == 0 && rule.getMinDelayMinutes().total_seconds() == 0))	continue;

 					os << "<ReservationRule>" << "\n";
 					os << "<objectId>" << TridentId (peerid, "ReservationRule", rule.getKey ()) << "</objectId>" << "\n";
					os << "<ReservationCompulsory>" << ((rule.getReservationType() == PTUseRule::RESERVATION_RULE_COMPULSORY) ? "compulsory" : "optional") << "</ReservationCompulsory>" << "\n";
 					os << "<deadLineIsTheCustomerDeparture>" << !rule.getOriginIsReference() << "</deadLineIsTheCustomerDeparture>" << "\n";
 					if (rule.getMinDelayMinutes().total_seconds() > 0)
 					{
						os << "<minMinutesDurationBeforeDeadline>" << ToXsdDuration(rule.getMinDelayMinutes()) << "</minMinutesDurationBeforeDeadline>" << "\n";
					}
 					if (rule.getMinDelayDays().days() > 0)
 					{
						os << "<minDaysDurationBeforeDeadline>" << ToXsdDaysDuration(rule.getMinDelayDays()) << "</minDaysDurationBeforeDeadline>" << "\n";
 					}
 					if (!rule.getHourDeadLine().is_not_a_date_time())
 					{
 						os << "<yesterdayBookingMaxTime>" << ToXsdTime(rule.getHourDeadLine()) << "</yesterdayBookingMaxTime>" << "\n";
 					}
 					if (rule.getMaxDelayDays())
 					{
 						os << "<maxDaysDurationBeforeDeadline>" << ToXsdDaysDuration(*rule.getMaxDelayDays()) << "</maxDaysDurationBeforeDeadline>" << "\n";
 					}
// 					if (!rule.getPhoneExchangeNumber().empty())
 //					{
 //						os << "<phoneNumber>" << rule.getPhoneExchangeNumber() << "</phoneNumber>" << "\n";
 //						os << "<callcenterOpeningPeriod>" << rule.getPhoneExchangeOpeningHours() << "</callcenterOpeningPeriod>" << "\n";
 //					}
// 					if (!rule.getWebSiteUrl().empty())
// 					{
// 						os << "<bookingWebsiteURL>" << rule.getWebSiteUrl() << "</bookingWebsiteURL>" << "\n";
// 					}
 					os << "</ReservationRule>" << "\n";
 				}

				// Non concurrency -----------------------------------------------------------------------
				BOOST_FOREACH(Registry<NonConcurrencyRule>::value_type itrule, _env.getRegistry<NonConcurrencyRule>())
				{
					const NonConcurrencyRule& rule(*itrule.second);
					os << "<LineConflict>" << "\n";
					os << "<objectId>" << TridentId (peerid, "LineConflict", rule) << "</objectId>" << "\n";
					os << "<forbiddenLine>" << TridentId (peerid, "Line", rule.get<HiddenLine>()->getKey()) << "</forbiddenLine>" << "\n";
					os << "<usedLine>" << TridentId (peerid, "Line", rule.get<PriorityLine>()->getKey()) << "</usedLine>" << "\n";
					os << "<conflictDelay>" << ToXsdDuration(rule.get<Delay>()) << "</conflictDelay>" << "\n";
					os << "</LineConflict>" << "\n";
				}


				// CityMainStops ---------------------------------------------------
				BOOST_FOREACH(Registry<City>::value_type itcity, _env.getRegistry<City>())
				{
					const City* city(itcity.second.get());
					vector<string> containedStopAreas;

					// Contained connection places
					Env senv;
					StopAreaTableSync::SearchResult places(
						StopAreaTableSync::Search(senv, city->getKey(), true)
					);
					BOOST_FOREACH(const boost::shared_ptr<const StopArea>& cp, places)
					{
						// filter physical stops not concerned by this line.
						if(!_env.getRegistry<StopArea>().contains(cp->getKey())) continue;

						containedStopAreas.push_back (TridentId (peerid, "StopArea", *cp));

					}
					if (containedStopAreas.size () == 0) continue;


					os << "<CityMainStops>" << "\n";
					os << "<objectId>" << TridentId (peerid, "CityMainStops", *city) << "</objectId>" << "\n";
					os << "<name>" << city->getName () << "</name>" << "\n";


					BOOST_FOREACH(string sa, containedStopAreas)
					{
						os << "<contains>" << sa  << "</contains>" << "\n";
					}

					os << "</CityMainStops>" << "\n";
				}
			}


			if (_withTisseoExtension)
				os << "</TisseoPTNetwork>" <<  "\n" << flush;
			else
				os << "</ChouettePTNetwork>" <<  "\n" << flush;
		}


		//////////////////////////////////////////////////////////////////////////
		// INPUT

		bool TridentFileFormat::Importer_::_parse(
			const path& filePath
		) const {
			bool failure(false);

			DataSource& dataSource(*_import.get<DataSource>());

			string encoding(dataSource.get<Charset>());
			if(encoding.empty())
			{
				ifstream inFile;
				inFile.open(filePath.string().c_str());
				if(!inFile)
				{
					_logError("Could no open the file " + filePath.string());
					throw Exception("Could no open the file " + filePath.string());
				}
				string line;
				if(!getline(inFile, line))
				{
					return false;
				}
				string pattern("encoding=");
				string::const_iterator it(search(line.begin(), line.end(), pattern.begin(), pattern.end()));
				if(it != line.end())
				{
					for(it += pattern.size(); it != line.end() && *it != '\'' && *it != '"'; ++it) ;
					if( it != line.end())
					{
						string::const_iterator it2(it+1);
						for(; it2 != line.end() && *it2 != '\'' && *it2 != '"'; ++it2);
						if(it2 != line.end())
						{
							encoding = line.substr(it-line.begin()+1, it2-it-1);
						}
					}
				}
			}
			if(encoding.empty())
			{
				encoding = "UTF-8";
			}
			IConv charset_converter(encoding, "UTF-8");

			XMLResults pResults;
			XMLNode allNode = XMLNode::parseFile(filePath.string().c_str(), "ChouettePTNetwork", &pResults);
			if (pResults.error != eXMLErrorNone)
			{
				_logError(
					"XML Parsing error "+ lexical_cast<string>(XMLNode::getError(pResults.error)) +
					" inside file "+ filePath.string() +
					" at line "+ lexical_cast<string>(pResults.nLine) +", column "+
					lexical_cast<string>(pResults.nColumn)
				);
				return false;
			}
			if(allNode.isEmpty())
			{
				_logError(
					"File "+ filePath.string() +" is empty."
				);
				return false;
			}

			// Title
			XMLNode chouetteLineDescriptionNode(allNode.getChildNode("ChouetteLineDescription"));
			XMLNode lineNode(chouetteLineDescriptionNode.getChildNode("Line"));
			XMLNode lineKeyNode(lineNode.getChildNode("objectId"));
			XMLNode clineShortNameNode = lineNode.getChildNode("number", 0);
			XMLNode clineNameNode = lineNode.getChildNode("publishedName");
			_logDebug("<h2>Trident import of "+ charset_converter.convert(clineNameNode.isEmpty() ? lineKeyNode.getText() : clineNameNode.getText()) +"</h2>");

			// Network
			XMLNode networkNode =  allNode.getChildNode("PTNetwork", 0);
			XMLNode networkIdNode = networkNode.getChildNode("objectId", 0);
			XMLNode networkNameNode = networkNode.getChildNode("name", 0);

			TransportNetwork* network(
				_createOrUpdateNetwork(
					_networks,
					networkIdNode.getText(),
					charset_converter.convert(networkNameNode.getText()),
					dataSource
			)	);

			std::string longName(charset_converter.convert(clineNameNode.getText()));
			std::string shortName(charset_converter.convert(clineShortNameNode.getText()));

			// Commercial lines
			CommercialLine* cline(
				_createOrUpdateLine(
					_lines,
					lineKeyNode.getText(),
					clineNameNode.isEmpty() || !clineNameNode.nText() ?
						optional<const string&>() :
						longName,
					clineShortNameNode.isEmpty() || !clineShortNameNode.nText() ?
						optional<const string&>() :
						shortName,
					optional<RGBColor>(),
					*network,
					dataSource
			)	);

			// Transport mode
			boost::shared_ptr<RollingStock> rollingStock;
			RollingStockTableSync::SearchResult rollingStocks(
				RollingStockTableSync::Search(
					_env,
					charset_converter.convert(lineNode.getChildNode("transportModeName").getText()),
					true
			)	);
			if(!rollingStocks.empty())
			{
				rollingStock = rollingStocks.front();
			}

			// Places
			map<string, XMLNode> areaCentroids;
			if(_importStops)
			{
				XMLNode chouetteAreaNode(allNode.getChildNode("ChouetteArea"));
				int stopsNumber(chouetteAreaNode.nChildNode("AreaCentroid"));
				for(int stopRank(0); stopRank < stopsNumber; ++stopRank)
				{
					XMLNode stopAreaNode(chouetteAreaNode.getChildNode("AreaCentroid", stopRank));

					areaCentroids.insert(
						make_pair(
							stopAreaNode.getChildNode("objectId", 0).getText(),
							stopAreaNode
					)	);
				}
			}


			// Commercial stops
			map<string, StopArea*> commercialStopsByPhysicalStop;
			if(_importStops && !_treatAllStopAreaAsQuay)
			{
				XMLNode chouetteAreaNode(allNode.getChildNode("ChouetteArea"));
				int stopsNumber(chouetteAreaNode.nChildNode("StopArea"));
				for(int stopRank(0); stopRank < stopsNumber; ++stopRank)
				{
					XMLNode stopAreaNode(chouetteAreaNode.getChildNode("StopArea", stopRank));
					XMLNode extensionNode(stopAreaNode.getChildNode("StopAreaExtension", 0));
					XMLNode areaTypeNode(extensionNode.getChildNode("areaType",0));
					if(to_lower_copy(string(areaTypeNode.getText())) != string("commercialstoppoint")) continue;

					// Stop area key
					XMLNode keyNode(stopAreaNode.getChildNode("objectId", 0));
					string stopKey(keyNode.getText());

					// Stop area name
					string name;
					{
						XMLNode nameNode(stopAreaNode.getChildNode("name"));
						if(!nameNode.isEmpty())
						{
							name = charset_converter.convert(nameNode.getText());
						}
					}

					XMLNode areaNode(stopAreaNode.getChildNode("centroidOfArea", 0));
					string cityCode;
					if(!areaNode.isEmpty())
					{
						// Place
						map<string,XMLNode>::iterator itPlace(areaCentroids.find(areaNode.getText()));
						if(itPlace == areaCentroids.end())
						{
							_logWarning(
								"Area centroid "+ string(areaNode.getText()) +" not found in CommercialStopPoint "+
								lexical_cast<string>(stopKey) +" ("+ name +")"
							);
							failure = true;
							continue;
						}
						XMLNode& areaCentroid(itPlace->second);
						XMLNode addressNode(areaCentroid.getChildNode("address", 0));
						if(!addressNode.isEmpty() && addressNode.getChildNode("countryCode", 0).getText())
						{
							cityCode = addressNode.getChildNode("countryCode", 0).getText();
						}
					}
					else
					{ // case city is on boarding position
						int containsNumber(stopAreaNode.nChildNode("contains"));
						for(int i(0); i<containsNumber; ++i)
						{
							string boardingPositionId(
								stopAreaNode.getChildNode("contains", i).getText()
							);

							for(int j(0); j<stopsNumber; ++j)
							{
								XMLNode stopAreaNode2(chouetteAreaNode.getChildNode("StopArea", j));
								if(!stopAreaNode2.nChildNode("objectId"))
								{
									continue;
								}
								string stopAreaKey(stopAreaNode2.getChildNode("objectId").getText());
								if(stopAreaKey != boardingPositionId)
								{
									continue;
								}
								XMLNode areaNode(stopAreaNode2.getChildNode("centroidOfArea", 0));
								if(areaNode.isEmpty())
								{
									continue;
								}

								// Place
								map<string,XMLNode>::iterator itPlace(areaCentroids.find(areaNode.getText()));
								if(itPlace == areaCentroids.end())
								{
									_logWarning(
										"Area centroid "+ string(areaNode.getText()) +" not found in CommercialStopPoint "+ stopKey +" ("+ name +")"
									);
									failure = true;
									continue;
								}
								XMLNode& areaCentroid(itPlace->second);
								XMLNode addressNode(areaCentroid.getChildNode("address", 0));
								if(!addressNode.isEmpty() && addressNode.getChildNode("countryCode", 0).getText())
								{
									cityCode = addressNode.getChildNode("countryCode", 0).getText();
								}

								if(!cityCode.empty())
								{
									break;
								}
							}

							if(!cityCode.empty())
							{
								break;
							}
						}
					}

					// Search of an existing connection place with the same code
					StopArea* curStop(NULL);
					set<StopArea*> cstops(
						_getStopAreas(
							_stopAreas,
							stopKey,
							optional<const string&>(),
							false
					)	);
					if(cstops.size() > 1)
					{
						_logWarning(
							"More than one stop with key"+ stopKey
						);
					}
					if(!cstops.empty())
					{
						// Load existing commercial stop point
						curStop = *cstops.begin();

						if(curStop->getName() != name)
						{
							_logInfo(
								"Stop area is differently named in source file and in SYNTHESE (source="+ name +", SYNTHESE="+ curStop->getName() +")"
							);
						}
					}
					else if(!cityCode.empty()) // Link by city and name
					{
						// Search of the city
						City* city(NULL);
						CityTableSync::SearchResult cityResult(
							CityTableSync::Search(_env, optional<string>(), optional<string>(), cityCode)
						);
						if(!cityResult.empty())
						{
							city = cityResult.front().get();
						}
						else
						{
							// If no city was found, attempting to find an alias with the right code
							CityAliasTableSync::SearchResult cityAliasResult(
								CityAliasTableSync::Search(_env, optional<RegistryKeyType>(), cityCode)
							);

							if(cityAliasResult.empty())
							{
								_logError("Commercial stop point "+ stopKey +" does not link to a valid city ("+ cityCode +")");
								failure = true;
								continue;
							}

							city = const_cast<City*>(cityAliasResult.front()->getCity());
						}

						// Stop area creation
						curStop = _createStopArea(
							_stopAreas,
							stopKey,
							name,
							*city,
							_defaultTransferDuration,
							false,
							dataSource
						);
					}
					else
					{
						string logText(
							"Commercial stop point "+ stopKey +" cannot be created due to undefined city."
						);
						if(_autoGenerateStopAreas)
						{
							_logWarning(logText);
						}
						else
						{
							_logError(logText);
							failure = true;
						}
						continue;
					}

					curStop->setName(name);

					// Link from physical stops
					int pstopsNumber(stopAreaNode.nChildNode("contains"));
					for(int pstopRank(0); pstopRank < pstopsNumber; ++pstopRank)
					{
						commercialStopsByPhysicalStop[stopAreaNode.getChildNode("contains", pstopRank).getText()] = curStop;
					}
				}
			}

			// Stops
			XMLNode chouetteAreaNode(allNode.getChildNode("ChouetteArea"));
			int stopsNumber(chouetteAreaNode.nChildNode("StopArea"));
			for(int stopRank(0); stopRank < stopsNumber; ++stopRank)
			{
				XMLNode stopAreaNode(chouetteAreaNode.getChildNode("StopArea", stopRank));
				XMLNode extensionNode(stopAreaNode.getChildNode("StopAreaExtension", 0));
				XMLNode areaTypeNode(extensionNode.getChildNode("areaType",0));
				if(	!_treatAllStopAreaAsQuay &&
					to_lower_copy(string(areaTypeNode.getText())) != "boardingposition" &&
					to_lower_copy(string(areaTypeNode.getText())) != "quay"
				){
					continue;
				}
				XMLNode areaCentroidNode(stopAreaNode.getChildNode("centroidOfArea", 0));

				// ID
				string stopKey(stopAreaNode.getChildNode("objectId", 0).getText());

				// Name
				string name(charset_converter.convert(stopAreaNode.getChildNode("name", 0).getText()));

				if(_importStops)
				{
					// Geometry
					boost::shared_ptr<StopPoint::Geometry> geometry;
					boost::shared_ptr<const City> city;
					if(areaCentroidNode.isEmpty())
					{
						_logWarning(
							"Physical stop "+ stopKey +" does not have any area centroid ("+ name +")"
						);
					}
					else
					{
						map<string, XMLNode>::iterator itPlace(areaCentroids.find(areaCentroidNode.getText()));
						if(itPlace == areaCentroids.end())
						{
							_logWarning(
								"Physical stop with key "+ stopKey +" links to a not found area centroid "+ areaCentroidNode.getText() +" ("+ name +")"
							);
						}
						else
						{
							XMLNode& areaCentroid(itPlace->second);
							XMLNode longitudeNode(areaCentroid.getChildNode("longitude", 0));
							XMLNode latitudeNode(areaCentroid.getChildNode("latitude", 0));
							try
							{
								if(!longitudeNode.isEmpty() && !latitudeNode.isEmpty())
								{
									geometry = CoordinatesSystem::GetCoordinatesSystem(
										_getSRIDFromTrident(areaCentroid.getChildNode("longLatType", 0).getText())
									).createPoint(
										lexical_cast<double>(longitudeNode.getText()),
										lexical_cast<double>(latitudeNode.getText())
									);
								}
								else
								{
									XMLNode projectedPointNode(areaCentroid.getChildNode("projectedPoint", 0));
									if(!projectedPointNode.isEmpty())
									{
										geometry = CoordinatesSystem::GetCoordinatesSystem(
											_getSRIDFromTrident(projectedPointNode.getChildNode("projectionType", 0).getText())
										).createPoint(
											lexical_cast<double>(projectedPointNode.getChildNode("X", 0).getText()),
											lexical_cast<double>(projectedPointNode.getChildNode("Y", 0).getText())
										);
									}
								}
							}
							catch(UnknkownSRIDException&)
							{
								_logWarning(
									"Physical stop with key "+ stopKey +" uses an unknown SRID"
								);
							}

							XMLNode addressNode(areaCentroid.getChildNode("address", 0));
							string cityCode(
								(addressNode.isEmpty() || !addressNode.getChildNode("countryCode", 0).getText()) ?
								string() :
								addressNode.getChildNode("countryCode", 0).getText()
							);

							// Search of the city
							if(!cityCode.empty())
							{
								CityTableSync::SearchResult cityResult(
									CityTableSync::Search(_env, optional<string>(), optional<string>(), cityCode)
								);
								if(!cityResult.empty())
								{
									city = cityResult.front();
								}
								else
								{
									// If no city was found, attempting to find an alias with the right code
									CityAliasTableSync::SearchResult cityAliasResult(
										CityAliasTableSync::Search(_env, optional<RegistryKeyType>(), cityCode)
									);

									if(cityAliasResult.empty())
									{
										_logError(
											"Stop point "+ stopKey +" with area centroid "+ areaCentroid.getChildNode("name").getText() +" does not link to a valid city ("+ addressNode.getChildNode("countryCode").getText() +")"
										);
										failure = true;
										continue;
									}

									city = _env.getSPtr(cityAliasResult.front()->getCity());
								}
							}
						}
					}

					// Stop area
					map<string,StopArea*>::const_iterator itcstop(commercialStopsByPhysicalStop.find(stopKey));
					StopArea* curStop(NULL);
					if(itcstop != commercialStopsByPhysicalStop.end())
					{
						curStop = itcstop->second;
					}

					set<StopPoint*> stopPoints;

					if(curStop)
					{
						stopPoints = _createOrUpdateStop(
							_stops,
							stopKey,
							name,
							optional<const RuleUser::Rules&>(),
							curStop,
							geometry.get(),
							dataSource
						);
					}
					else if(_autoGenerateStopAreas && city)
					{
						stopPoints = _createOrUpdateStopWithStopAreaAutocreation(
							_stops,
							stopKey,
							name,
							geometry.get(),
							*city,
							_defaultTransferDuration,
							dataSource,
							boost::optional<const graph::RuleUser::Rules&>()
						);
					}
					if(stopPoints.empty())
					{
						_logError(
							"Stop "+ stopKey +" not found and cannot be created in any commercial stop ("+ name +")"
						);
						failure = true;
						continue;
					}
				}
				else
				{
					if(	_getStopPoints(
							_stops,
							stopKey,
							name
						).empty()
					){
						failure = true;
						continue;
					}
				}
			}


			if(failure)
			{
				_logError(
					"<b>FAILURE : At least a stop is missing : load interrupted</b>"
				);
				return !failure;
			}

			// JourneyPattern stops
			int stopPointsNumber(chouetteLineDescriptionNode.nChildNode("StopPoint"));
			map<string,set<StopPoint*> > stopPoints;
			for(int stopPointRank(0); stopPointRank < stopPointsNumber; ++stopPointRank)
			{
				XMLNode stopPointNode(chouetteLineDescriptionNode.getChildNode("StopPoint", stopPointRank));
				XMLNode spKeyNode(stopPointNode.getChildNode("objectId"));
				XMLNode containedNode(stopPointNode.getChildNode("containedIn"));
				set<StopPoint*> linkableStops(_stops.get(containedNode.getText()));
				if(linkableStops.empty())
				{
					_logError(
						"Stop "+ string(containedNode.getText()) +" not found by stop point "+ 
						string(spKeyNode.getText()) +")"
					);
					failure = true;
					continue;
				}
				stopPoints[spKeyNode.getText()] = linkableStops;
			}

			if(failure)
			{
				_logError(
					"<b>FAILURE : At least a stop point is missing : load interrupted</b>"
				);
				return !failure;
			}

			// Load of existing routes
			JourneyPatternTableSync::SearchResult sroutes(
				JourneyPatternTableSync::Search(_env, cline->getKey())
			);
			BOOST_FOREACH(const boost::shared_ptr<JourneyPattern>& line, sroutes)
			{
				LineStopTableSync::Search(
					_env,
					line->getKey(),
					optional<RegistryKeyType>(),
					0,
					optional<size_t>(),
					true, true,
					UP_LINKS_LOAD_LEVEL
				);
				ScheduledServiceTableSync::Search(
					_env,
					line->getKey(),
					optional<RegistryKeyType>(),
					optional<RegistryKeyType>(),
					optional<string>(),
					false,
					0, optional<size_t>(), true, true,
					UP_LINKS_LOAD_LEVEL
				);
			}

			// Chouette routes
			map<string,string> routeNames;
			map<string,bool> routeWaybacks;
			int croutesNumber(chouetteLineDescriptionNode.nChildNode("ChouetteRoute"));
			for(int crouteRank(0); crouteRank < croutesNumber; ++crouteRank)
			{
				XMLNode routeNode(chouetteLineDescriptionNode.getChildNode("ChouetteRoute",crouteRank));
				XMLNode crouteKeyNode(routeNode.getChildNode("objectId"));
				XMLNode nameNode(routeNode.getChildNode("name"));
				routeNames[crouteKeyNode.getText()] =
					(!nameNode.isEmpty() && nameNode.getText()) ?
					charset_converter.convert(nameNode.getText()) :
					string();

				bool wayBack(false);
				XMLNode extNode(routeNode.getChildNode("RouteExtension"));
				if(!extNode.isEmpty())
				{
					XMLNode waybackNode(extNode.getChildNode("wayBack"));
					if(!waybackNode.isEmpty() && waybackNode.getText())
					{
						wayBack = (
							waybackNode.getText() == string("R") ||
							waybackNode.getText() == string("1")
						);
					}
				}
				routeWaybacks[crouteKeyNode.getText()] = wayBack;
			}


			// Routes
			map<string, Route> routes;
			int routesNumber(chouetteLineDescriptionNode.nChildNode("JourneyPattern"));
			for(int routeRank(0); routeRank < routesNumber; ++routeRank)
			{
				Route route;

				XMLNode routeNode(chouetteLineDescriptionNode.getChildNode("JourneyPattern",routeRank));
				route.objectId = routeNode.getChildNode("objectId").getText();
				string routeId(routeNode.getChildNode("routeId").getText());
				route.name = routeNames[routeId];
				route.wayBack = routeWaybacks[routeId];

				// Reading stops list
				int lineStopsNumber(routeNode.nChildNode("stopPointList"));
				string lastStopPoint;
				for(int lineStopRank(0); lineStopRank < lineStopsNumber; ++lineStopRank)
				{
					string stopPointCode(
						routeNode.getChildNode("stopPointList", lineStopRank).getText()
					);
					if(stopPointCode == lastStopPoint)
					{
						_logWarning(
							"StopPoint "+ lastStopPoint +" is repeated in journey pattern "+ route.objectId
						);
						continue;
					}
					lastStopPoint = stopPointCode;
					route.stops.push_back(
						JourneyPattern::StopWithDepartureArrivalAuthorization(
							stopPoints[stopPointCode]
					)	);
				}

				routes[route.objectId] = route;
			}

			// Services
			map<string, ScheduledService*> services;
			int servicesNumber(chouetteLineDescriptionNode.nChildNode("VehicleJourney"));
			for(int serviceRank(0); serviceRank < servicesNumber; ++serviceRank)
			{
				XMLNode serviceNode(chouetteLineDescriptionNode.getChildNode("VehicleJourney",serviceRank));
				XMLNode keyNode(serviceNode.getChildNode("objectId"));
				XMLNode jpKeyNode(serviceNode.getChildNode("journeyPatternId"));
				XMLNode numberNode(serviceNode.getChildNode("publishedJourneyName"));
				string serviceNumber((numberNode.getText() == NULL) ? string() : charset_converter.convert(numberNode.getText()));

				// Creation of the service

				map<string,Route>::const_iterator itLine(routes.find(jpKeyNode.getText()));
				if(itLine == routes.end())
				{
					_logWarning(
						"Service "+ serviceNumber +" / "+ keyNode.getText() +" ignored because journey pattern "+ jpKeyNode.getText() +" was not found."
					);
					continue;
				}
				Route route(itLine->second);
				size_t stopsNumber(serviceNode.nChildNode("VehicleJourneyAtStop"));
				if(stopsNumber != route.stops.size())
				{
					_logWarning(
						"Service "+ serviceNumber +" / "+ keyNode.getText() +" ignored due to bad stops number"
					);
					continue;
				}
				bool updatedRoute(false);

				ScheduledService::Schedules deps;
				ScheduledService::Schedules arrs;
				time_duration lastDep(0,0,0);
				time_duration lastArr(0,0,0);
				size_t ignoredStops(0);
				for(size_t stopRank(0); stopRank < stopsNumber; ++stopRank)
				{
					XMLNode vjsNode(serviceNode.getChildNode("VehicleJourneyAtStop", static_cast<int>(stopRank)));
					XMLNode depNode(vjsNode.getChildNode("departureTime"));
					XMLNode arrNode(vjsNode.getChildNode("arrivalTime"));

					// Jump over non served stops
					if(depNode.isEmpty() && arrNode.isEmpty())
					{
						route.stops.erase(route.stops.begin() + (stopRank - ignoredStops));
						++ignoredStops;
						continue;
					}

					// Route boarding and alight possibility
					if(	vjsNode.nChildNode("boardingAlightingPossibility") &&
						stopRank > 0 &&
						stopRank + 1 < stopsNumber
					){
						string bap(vjsNode.getChildNode("boardingAlightingPossibility").getText());
						if(bap == "BoardOnly" || bap == "NeitherBoardOrAlight")
						{
							route.stops[stopRank - ignoredStops]._arrival = false;
							updatedRoute = true;
						}
						else if(bap == "AlightOnly" || bap == "NeitherBoardOrAlight")
						{
							route.stops[stopRank - ignoredStops]._departure = false;
							updatedRoute = true;
						}
					}

					// Schedules
					time_duration depHour(duration_from_string(depNode.isEmpty() ? arrNode.getText() : depNode.getText()));
					time_duration arrHour(duration_from_string(arrNode.isEmpty() ? depNode.getText() : arrNode.getText()));
					time_duration depSchedule(depHour + hours(24 * (lastDep.hours() / 24 + (depHour < Service::GetTimeOfDay(lastDep) ? 1 : 0))));
					time_duration arrSchedule(arrHour + hours(24 * (lastArr.hours() / 24 + (arrHour < Service::GetTimeOfDay(lastArr) ? 1 : 0))));
					if ((depHour < Service::GetTimeOfDay(lastDep) || arrHour < Service::GetTimeOfDay(lastArr)) &&
						_throwWarnInCaseScheduleUp24)
					{
						_logWarning(
							"Service "+ serviceNumber +" / "+ keyNode.getText() +" is detected on more than one day."
						);
					}
					lastDep = depSchedule;
					lastArr = arrSchedule;
					deps.push_back(depSchedule);
					arrs.push_back(arrSchedule);
				}

				JourneyPattern* journeyPattern(NULL);
				if(!updatedRoute && route.journeyPattern)
				{
					journeyPattern = route.journeyPattern;
				}
				else
				{
					// Route creation
					journeyPattern = _createOrUpdateRoute(
						*cline,
						(_mergeRoutes || updatedRoute) ? optional<const string&>() : optional<const string&>(route.objectId),
						optional<const string&>(route.name),
						optional<const string&>(),
						optional<Destination*>(),
						optional<const RuleUser::Rules&>(),
						route.wayBack,
						rollingStock.get(),
						route.stops,
						dataSource,
						true,
						true,
						true,
						true
					);
					if(!updatedRoute)
					{
						routes[route.objectId].journeyPattern = journeyPattern;
					}
				}

				// Service creation
				ScheduledService* service(
					_createOrUpdateService(
						*journeyPattern,
						deps,
						arrs,
						serviceNumber,
						dataSource
				)	);
				if(service)
				{
					services[keyNode.getText()] = service;
				}
			}

			// Calendars
			int calendarNumber(allNode.nChildNode("Timetable"));
			date now(gregorian::day_clock::local_day());
			for(int calendarRank(0); calendarRank < calendarNumber; ++calendarRank)
			{
				XMLNode calendarNode(allNode.getChildNode("Timetable", calendarRank));

				int daysNumber(calendarNode.nChildNode("calendarDay"));
				int periodsNumber(calendarNode.nChildNode("period"));
				int dayTypesNumber(calendarNode.nChildNode("dayType"));
				int servicesNumber(calendarNode.nChildNode("vehicleJourneyId"));
				CalendarTemplate* ct(NULL);

				if(_importTimetablesAsTemplates)
				{
					string calendarId(calendarNode.getChildNode("objectId").getText());
					ImportableTableSync::ObjectBySource<CalendarTemplateTableSync>::Set cts(_calendarTemplates.get(calendarId));
					bool calendarToImport(false);
					if(cts.empty())
					{
						ct = new CalendarTemplate(CalendarTemplateTableSync::getId());
						ct->addCodeBySource(dataSource, calendarId);
						_env.getEditableRegistry<CalendarTemplate>().add(boost::shared_ptr<CalendarTemplate>(ct));
						_calendarTemplates.add(*ct);
						calendarToImport = true;
					}
					else
					{
						ct = *cts.begin();
						CalendarTemplateElementTableSync::SearchResult elements(
							CalendarTemplateElementTableSync::Search(_env, ct->getKey())
						);
						if(!ct->isLimited())
						{
							calendarToImport = true;
						}
						else
						{
							Calendar c;
							for(int dayRank(0); dayRank < daysNumber; ++dayRank)
							{
								XMLNode dayNode(calendarNode.getChildNode("calendarDay", dayRank));
								c.setActive(from_string(dayNode.getText()));
							}
							if(c != ct->getResult())
							{
								calendarToImport = true;
							}
						}
						if(calendarToImport)
						{
							BOOST_FOREACH(const boost::shared_ptr<CalendarTemplateElement>& element, elements)
							{
								_calendarElementsToRemove.insert(element);
								_env.getEditableRegistry<CalendarTemplateElement>().remove(element->getKey());
								ct->removeElement(*element);
							}
						}
					}
					if(calendarNode.nChildNode("comment"))
					{
						ct->setName(
							charset_converter.convert(calendarNode.getChildNode("comment").getText())
						);
					}
					if(calendarToImport)
					{
						size_t rank(0);
						for(int dayRank(0); dayRank < daysNumber; ++dayRank)
						{
							XMLNode dayNode(calendarNode.getChildNode("calendarDay", dayRank));
							date d(from_string(dayNode.getText()));
							boost::shared_ptr<CalendarTemplateElement> cte(new CalendarTemplateElement(CalendarTemplateElementTableSync::getId()));
							cte->setCalendar(ct);
							cte->setMinDate(d);
							cte->setMaxDate(d);
							cte->setRank(rank++);
							ct->addElement(*cte);
							_env.getEditableRegistry<CalendarTemplateElement>().add(cte);
						}
					}
				}

				vector<ScheduledService*> calendarServices;
				for(int serviceRank(0); serviceRank < servicesNumber; ++serviceRank)
				{
					XMLNode serviceNode(calendarNode.getChildNode("vehicleJourneyId", serviceRank));
					map<string, ScheduledService*>::const_iterator its(services.find(serviceNode.getText()));
					if(its == services.end() || its->second == NULL)
					{
						continue;
					}
					calendarServices.push_back(its->second);
				}

				// Result
				vector<date> calendarDates;

				if(daysNumber)
				{	// Definition by dates list
					for(int dayRank(0); dayRank < daysNumber; ++dayRank)
					{
						XMLNode dayNode(calendarNode.getChildNode("calendarDay", dayRank));
						date d(from_string(dayNode.getText()));
						if(_fromToday && d < now)
						{
							continue;
						}
						calendarDates.push_back(d);
					}
				}
				else if(periodsNumber)
				{ // Definition by period / template

					// Periods
					vector<date_period> periods;
					for(int periodRank(0); periodRank < periodsNumber; ++periodRank)
					{
						XMLNode periodNode(calendarNode.getChildNode("period", periodRank));
						if(!periodNode.nChildNode("startOfPeriod") || !periodNode.nChildNode("endOfPeriod"))
						{
							continue;
						}

						date_period period(
							from_string(periodNode.getChildNode("startOfPeriod").getText()),
							from_string(periodNode.getChildNode("endOfPeriod").getText())
						);
						
						periods.push_back(period);
					}

					// Templates
					vector<CalendarTemplate*> calendarTemplates;
					for(int dayTypeRank(0); dayTypeRank < dayTypesNumber; ++dayTypeRank)
					{
						string dayTypeKey(calendarNode.getChildNode("dayType", dayTypeRank).getText());
						
						if(!_calendarTemplates.contains(dayTypeKey))
						{
							_logWarning(
								"Calendar template "+ dayTypeKey +" was not found."
							);
							continue;
						}

						ImportableTableSync::ObjectBySource<CalendarTemplateTableSync>::Set calendarTemplatesSet(
							_calendarTemplates.get(dayTypeKey)
						);

						calendarTemplates.push_back(*calendarTemplatesSet.begin());
					}

					// Transformation into calendar dates
					Calendar dates;
					BOOST_FOREACH(const date_period& period, periods)
					{
						Calendar periodMask(
							period.begin(),
							period.end()
						);

						BOOST_FOREACH(CalendarTemplate* calendarTemplate, calendarTemplates)
						{
							dates |= calendarTemplate->getResult(periodMask);
						}
					}
					calendarDates = dates.getActiveDates();
				}

				if (!_useCalendarLinks)
				{
					// Update of the calendar of the services
					if(!calendarDates.empty())
					{
						BOOST_FOREACH(const date& d, calendarDates)
						{
							BOOST_FOREACH(ScheduledService* service, calendarServices)
							{
								service->setActive(d);
							}
						}
					}
				}
				else
				{
					BOOST_FOREACH(ScheduledService* service, calendarServices)
					{
						boost::shared_ptr<CalendarLink> serviceCalendarLink;

						// Search for existing CalendarLink
						CalendarLinkTableSync::SearchResult serviceCalendarLinks(
							CalendarLinkTableSync::Search(
								_env,
								service->getKey(),
								0
						)	);
						if(!serviceCalendarLinks.empty())
						{
							BOOST_FOREACH(const boost::shared_ptr<CalendarLink>& scl, serviceCalendarLinks)
							{
								if(ct && scl->getCalendarTemplate() == ct)
								{
									serviceCalendarLink = scl;
									_logLoad(
										"Use of calendar_link "+ lexical_cast<string>(scl->getKey()) +" for service "+ lexical_cast<string>(service->getKey()) + " and calendar " + ct->getName()
									);
								}
							}
						}

						if(!serviceCalendarLink)
						{
							serviceCalendarLink = boost::shared_ptr<CalendarLink>(new CalendarLink(CalendarLinkTableSync::getId()));

							if(ct)
							{
								serviceCalendarLink->setCalendarTemplate(ct);
							}
							else
							{
								_logWarning(
									"Calendar not found"
								);
							}

							serviceCalendarLink->setCalendar(service);

							service->addCalendarLink(*serviceCalendarLink,true);

							_logCreation(
								"Creation of calendar_link "+ lexical_cast<string>(serviceCalendarLink->getKey()) +" for service "+ lexical_cast<string>(service->getKey()) + " and calendar " + ct->getName()
							);

							_env.getEditableRegistry<CalendarLink>().add(boost::shared_ptr<CalendarLink>(serviceCalendarLink));
						}
					}
				}
			}

			// ConnectionLink / Junction
			if(_importJunctions)
			{
				int connectionsNumber(allNode.nChildNode("ConnectionLink"));
				for(int connectionRank(0); connectionRank < connectionsNumber; ++connectionRank)
				{
					// Connection node
					XMLNode connectionNode(allNode.getChildNode("ConnectionLink", connectionRank));
					if(	!connectionNode.nChildNode("objectId") ||
						!connectionNode.nChildNode("startOfLink") ||
						!connectionNode.nChildNode("endOfLink") ||
						!connectionNode.nChildNode("linkDistance") ||
						!connectionNode.nChildNode("defaultDuration")
					){
						_logWarning(
							"Connection link at rank "+ lexical_cast<string>(connectionRank) +" is malformed."
						);
						continue;
					}

					// Connection properties
					XMLNode key(connectionNode.getChildNode("objectId", 0));
					XMLNode startNode(connectionNode.getChildNode("startOfLink", 0));
					XMLNode endNode(connectionNode.getChildNode("endOfLink", 0));
					XMLNode distanceNode(connectionNode.getChildNode("linkDistance", 0));
					XMLNode durationNode(connectionNode.getChildNode("defaultDuration", 0));

					// Fetching the stops
					StopPointTableSync::SearchResult startStops(
						StopPointTableSync::Search(
							_env, optional<RegistryKeyType>(), string(startNode.getText()), false, true,  0, 1
					)	);
					StopPointTableSync::SearchResult endStops(
						StopPointTableSync::Search(
							_env, optional<RegistryKeyType>(), string(endNode.getText()), false, true, 0, 1
					)	);
					if(startStops.empty() || endStops.empty())
					{
						_logWarning(
							"Connection link "+ string(key.getText()) +" rejected because of inexistent stop ("+
							string(startNode.getText()) +"/"+ string(endNode.getText()) +")"
						);
						continue;
					}
					boost::shared_ptr<StopPoint> startStop = startStops.front();
					boost::shared_ptr<StopPoint> endStop = endStops.front();

					// Internal or external connection
					if(startStop->getConnectionPlace() == endStop->getConnectionPlace())
					{
						// Internal transfer delay, updated only if the import handles the stops
						if(!_importStops)
						{
							continue;
						}

						StopArea::TransferDelaysMap value(
							startStop->getConnectionPlace()->getTransferDelays()
						);
						StopArea::_addTransferDelay(
							value,
							startStop->getKey(),
							endStop->getKey(),
							FromXsdDuration(durationNode.getText())
						);
						const_cast<StopArea*>(startStop->getConnectionPlace())->setTransferDelaysMatrix(value);
					}
					else
					{
						// Junction
						JunctionTableSync::SearchResult junctions(
							JunctionTableSync::Search(
								_env, startStop->getKey(), endStop->getKey()
						)	);

						boost::shared_ptr<Junction> junction;
						if(!junctions.empty())
						{
							junction = junctions.front();
							_logLoad(
								"Load of junction "+ string(key.getText())
							);
						}
						else
						{
							junction.reset(new Junction);
							junction->setKey(JunctionTableSync::getId());
							_logCreation(
								"Creation of junction "+ string(key.getText())
							);
						}


						junction->setStops(
							startStop.get(),
							endStop.get(),
							lexical_cast<double>(distanceNode.getText()),
							FromXsdDuration(durationNode.getText()),
							false
						);
					}
				}
			}

			_logDebug("<b>SUCCESS : Data loaded</b>");
			return !failure;
		}



		DBTransaction TridentFileFormat::Importer_::_save() const
		{
			DBTransaction transaction;

			// Add remove queries generated by _selectObjectsToRemove
			_addRemoveQueries(transaction);

			// Saving of each created or altered objects
			if(_importStops)
			{
				BOOST_FOREACH(Registry<StopArea>::value_type cstop, _env.getRegistry<StopArea>())
				{
					StopAreaTableSync::Save(cstop.second.get(), transaction);
				}
				BOOST_FOREACH(Registry<StopPoint>::value_type stop, _env.getRegistry<StopPoint>())
				{
					StopPointTableSync::Save(stop.second.get(), transaction);
				}
			}
			BOOST_FOREACH(Registry<TransportNetwork>::value_type network, _env.getRegistry<TransportNetwork>())
			{
				TransportNetworkTableSync::Save(network.second.get(), transaction);
			}
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
			BOOST_FOREACH(const Registry<Junction>::value_type& junction, _env.getRegistry<Junction>())
			{
				JunctionTableSync::Save(junction.second.get(), transaction);
			}
			if(_importTimetablesAsTemplates)
			{
				BOOST_FOREACH(const boost::shared_ptr<CalendarTemplateElement>& element, _calendarElementsToRemove)
				{
					DBModule::GetDB()->deleteStmt(element->getKey(), transaction);
				}
				BOOST_FOREACH(const Registry<CalendarTemplate>::value_type& calendarTemplate, _env.getRegistry<CalendarTemplate>())
				{
					CalendarTemplateTableSync::Save(calendarTemplate.second.get(), transaction);
				}
				BOOST_FOREACH(const Registry<CalendarTemplateElement>::value_type& calendarTemplateElement, _env.getRegistry<CalendarTemplateElement>())
				{
					CalendarTemplateElementTableSync::Save(calendarTemplateElement.second.get(), transaction);
				}
				if (_useCalendarLinks)
				{
					BOOST_FOREACH(Registry<CalendarLink>::value_type link, _env.getRegistry<CalendarLink>())
					{
						CalendarLinkTableSync::Save(link.second.get(), transaction);
					}
				}
			}
			return transaction;
		}



		//////////////////////////////////////////////////////////////////////////
		// HELPERS

		string TridentFileFormat::Exporter_::TridentId(
			const string& peer,
			const string clazz,
			const util::RegistryKeyType& id
		){
			stringstream ss;
			ss << peer << ":" << clazz << ":" << id;
			return ss.str ();
		}


		string TridentFileFormat::Exporter_::TridentId(
			const string& peer,
			const string clazz,
			const string& s
		){
			stringstream ss;
			ss << peer << ":" << clazz << ":" << s;
			return ss.str ();
		}


		string TridentFileFormat::Exporter_::TridentId(
			const string& peer,
			const string clazz,
			const Registrable& obj
		){
			stringstream ss;
			ss << peer << ":" << clazz << ":" << obj.getKey();
			return ss.str ();
		}


		string TridentFileFormat::Exporter_::TridentId(
			const string& peer,
			const string clazz,
			const LinePhysicalStop& linePhysicalStop
		){
			stringstream ss;
			pt::LineStop* lineStop = linePhysicalStop.getLineStop();
			ss << peer << ":" << clazz << ":" << ((NULL != lineStop) ? lineStop->getKey() : 0);
			return ss.str ();
		}


		TridentFileFormat::Exporter_::Exporter_(
			const impex::Export& export_
		):	OneFileExporter<TridentFileFormat>(export_),
			_withTisseoExtension(false)
		{}



		string ToXsdDaysDuration (date_duration daysDelay)
		{
			stringstream s;
			s << "P" << daysDelay.days() << "D";
			return s.str();
		}


		string ToXsdDuration(posix_time::time_duration duration)
		{
			stringstream s;
			s << "PT";
			if(duration.hours() > 0)
				s << duration.hours() << "H";
			s << duration.minutes() << "M";
			return s.str();
		}


		time_duration FromXsdDuration(const std::string& text)
		{
			string::const_iterator t(find(text.begin(), text.end(), 'T'));
			if(t == text.end())
			{
				return time_duration(not_a_date_time);
			}
			time_duration result(minutes(0));
			string::const_iterator h(find(t, text.end(), 'H'));
			if(h != text.end())
			{
				result += hours(lexical_cast<int>(text.substr((t-text.begin())+1, (h-t)-1)));
				t = h;
			}
			string::const_iterator m(find(t, text.end(), 'M'));
			if(m != text.end())
			{
				result += minutes(lexical_cast<int>(text.substr((t-text.begin())+1, (m-t)-1)));
			}
			return result;
		}


		string ToXsdTime (const time_duration& time)
		{
			stringstream ss;
			ss << setw( 2 ) << setfill ( '0' )
				<< time.hours() << ":"
				<< setw( 2 ) << setfill ( '0' )
				<< time.minutes () << ":00";
			return ss.str ();
		}



		std::string TridentFileFormat::Importer_::GetCoordinate( const double value )
		{
			return
				(value > 0) ?
				lexical_cast<string>(value) :
				string();
		}



		void TridentFileFormat::Importer_::setImportStops( bool value )
		{
			_importStops = value;
		}



		bool TridentFileFormat::Importer_::getImportStops() const
		{
			return _importStops;
		}



		void TridentFileFormat::_populateSRIDTridentConversionMap()
		{
			if(_SRIDConversionMap.left.empty())
			{
				_SRIDConversionMap.left.insert(SRIDConversionMap::left_value_type(4326, "WGS84"));
				_SRIDConversionMap.left.insert(SRIDConversionMap::left_value_type(27572, "LAMBERT II ETENDU"));
				_SRIDConversionMap.left.insert(SRIDConversionMap::left_value_type(2154, "LAMBERT 93"));
			}
		}



		CoordinatesSystem::SRID TridentFileFormat::_getSRIDFromTrident( const std::string& value )
		{
			_populateSRIDTridentConversionMap();
			SRIDConversionMap::right_const_iterator it(_SRIDConversionMap.right.find(to_upper_copy(value)));
			if(it == _SRIDConversionMap.right.end())
			{
				throw Exception("Trident SRID not found");
			}
			return it->second;
		}



		const std::string& TridentFileFormat::_getTridentFromSRID( const CoordinatesSystem::SRID value )
		{
			_populateSRIDTridentConversionMap();
			SRIDConversionMap::left_const_iterator it(_SRIDConversionMap.left.find(value));
			if(it == _SRIDConversionMap.left.end())
			{
				throw Exception("Trident SRID not found");
			}
			return it->second;
		}

		bool TridentFileFormat::Exporter_::_isSServiceHLP(const pt::ScheduledService* srv) const
		{
			const PTUseRule* pedestrianUseRule = dynamic_cast<const PTUseRule*>(
				&srv->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET)
			);
			if (pedestrianUseRule &&
				pedestrianUseRule->getForbiddenInTimetables() &&
				pedestrianUseRule->getForbiddenInDepartureBoards() &&
				pedestrianUseRule->getForbiddenInJourneyPlanning())
			{
				// Exclude this service
				return true;
			}
			return false;
		}

		bool TridentFileFormat::Exporter_::_isCServiceHLP(const pt::ContinuousService* srv) const
		{
			const PTUseRule* pedestrianUseRule = dynamic_cast<const PTUseRule*>(
				&srv->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET)
			);
			if (pedestrianUseRule &&
				pedestrianUseRule->getForbiddenInTimetables() &&
				pedestrianUseRule->getForbiddenInDepartureBoards() &&
				pedestrianUseRule->getForbiddenInJourneyPlanning())
			{
				// Exclude this service
				return true;
			}
			return false;
		}

		bool TridentFileFormat::Exporter_::_hasJPOnlyHLP(const pt::JourneyPattern* jp) const
		{
			bool displayAtLeastAService(false);
			ScheduledServiceTableSync::SearchResult services(
				ScheduledServiceTableSync::Search(
					_env,
					jp->getKey()
			)	);
			BOOST_FOREACH(const boost::shared_ptr<ScheduledService>& service, services)
			{
				const PTUseRule* pedestrianUseRule = dynamic_cast<const PTUseRule*>(
					&service->getUseRule(USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET)
				);
				if (!pedestrianUseRule ||
					!pedestrianUseRule->getForbiddenInTimetables() ||
					!pedestrianUseRule->getForbiddenInDepartureBoards() ||
					!pedestrianUseRule->getForbiddenInJourneyPlanning())
				{
					displayAtLeastAService = true;
					break;
				}
			}

			if (!displayAtLeastAService)
			{
				return true;
			}

			return false;
		}
}	}
