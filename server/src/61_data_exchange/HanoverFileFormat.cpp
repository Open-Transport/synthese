
/** HanoverFileFormat class implementation.
	@file HanoverFileFormat.cpp

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

#include "HanoverFileFormat.hpp"

using namespace boost;
using namespace std;
using namespace boost::posix_time;
using namespace geos::geom;

namespace synthese
{
	namespace data_exchange
	{
		HanoverFileFormat::HanoverFileFormat(
			util::Env& env,
			const impex::Import& import,
			impex::ImportLogLevel minLogLevel,
			const std::string& logPath,
			boost::optional<std::ostream&> outputStream,
			util::ParametersMap& pm
		): Importer(env, import, minLogLevel, logPath, outputStream, pm)
		{}


		void HanoverFileFormat::_selectAndLoadRun(
			RunsMap& runs,
			int runId,
			Route& route,
			const calendar::Calendar& calendar,
			const HanoverSchedules& schedules,
			const std::string& service_number
		) const {

			// Jump over courses with incomplete chainages
			if(schedules.size() > route.links.size() + 1)
			{
				_logWarningDetail(
					"RUN",
					lexical_cast<string>(runId),
					service_number,0,
					lexical_cast<string>(schedules.size()),
					lexical_cast<string>(route.links.size() + 1),
					string(),
					"Bad schedules number compared to Route stops number "
				);
				return;
			}
			else
			{
				// OK let's store the service
				Run& run(
					runs.insert(
						make_pair(
							runId,
							Run()
					)	).first->second
				);
				run.id = runId;
				run.schedules = schedules;
				run.route = &route;
				run.calendar = calendar;
				run.service_number = service_number;

				// Trace
				_logLoadDetail(
					"RUN",lexical_cast<string>(runId),service_number,0,string(),string(), string(),"OK"
				);
			}
		}

		void HanoverFileFormat::_selectAndLoadLink(
			LinksMap& links,
			int id,
			const Link::HanoverLineStops& stops,
			boost::shared_ptr<geos::geom::LineString> lineString
		) const {
			if(stops.size() < 2)
			{
				_logWarningDetail(
					"LINK",lexical_cast<string>(id),string(),stops.size(),string(),string(), string(),"This link has less than 2 stops"
				);
				return;
			}
			Link& link(
				links.insert(
					make_pair(
						id,
						Link()
				)	).first->second
			);

			link.id = id;
			link.stops = stops;
			link.lineString = lineString;
			_logLoadDetail(
				"LINK",lexical_cast<string>(id),string(),0,string(),string(),lexical_cast<string>(stops[1].offsetFromPreviousStop),"OK"
			);
		}



		void HanoverFileFormat::_selectAndLoadRoute(
			RoutesMap& routes,
			const Route::Links& links,
			pt::CommercialLine* line,
			const std::string& name,
			bool direction,
			int id
		) const {
			if(links.size() == 0)
			{
				_logWarningDetail(
					"ROUTE",lexical_cast<string>(id),line->getShortName(),0,string(),string(), string(),"This route has no link"
				);
				return;
			}
			else
			{
				Route& route(
					routes.insert(
						make_pair(
							id,
							Route()
					)	).first->second
				);

				route.id = id;
				route.name = name;
				route.links = links;
				route.line = line;
				route.direction = direction;
				_logLoadDetail(
					"ROUTE",lexical_cast<string>(id),line->getShortName(),0,name,lexical_cast<string>(direction),lexical_cast<string>(links.size()),"OK"
				);
			}
		}

		void HanoverFileFormat::_logLoadDetail(
			const std::string& table,
			const std::string& localId,
			const std::string& locaName,
			const util::RegistryKeyType syntheseId,
			const std::string& syntheseName,
			const std::string& oldValue,
			const std::string& newValue,
			const std::string& remarks
		) const	{

			stringstream content;
			content <<
				table << ";" <<
				localId << ";" <<
				locaName << ";" <<
				(syntheseId ? lexical_cast<string>(syntheseId) : string()) << ";" <<
				syntheseName << ";" <<
				oldValue << ";" <<
				newValue << ";" <<
				remarks
			;
			_logLoad(content.str());
		}



		void HanoverFileFormat::_logWarningDetail(
			const std::string& table,
			const std::string& localId,
			const std::string& locaName,
			const util::RegistryKeyType syntheseId,
			const std::string& syntheseName,
			const std::string& oldValue,
			const std::string& newValue,
			const std::string& remarks
		) const	{

			stringstream content;
			content <<
				table << ";" <<
				localId << ";" <<
				locaName << ";" <<
				(syntheseId ? lexical_cast<string>(syntheseId) : string()) << ";" <<
				syntheseName << ";" <<
				oldValue << ";" <<
				newValue << ";" <<
				remarks
			;
			_logWarning(content.str());
		}



		void HanoverFileFormat::_logDebugDetail(
			const std::string& table,
			const std::string& localId,
			const std::string& locaName,
			const util::RegistryKeyType syntheseId,
			const std::string& syntheseName,
			const std::string& oldValue,
			const std::string& newValue,
			const std::string& remarks
		) const	{

			stringstream content;
			content <<
				table << ";" <<
				localId << ";" <<
				locaName << ";" <<
				(syntheseId ? lexical_cast<string>(syntheseId) : string()) << ";" <<
				syntheseName << ";" <<
				oldValue << ";" <<
				newValue << ";" <<
				remarks
			;
			_logDebug(content.str());
		}



		void HanoverFileFormat::_logTraceDetail(
			const std::string& table,
			const std::string& localId,
			const std::string& locaName,
			const util::RegistryKeyType syntheseId,
			const std::string& syntheseName,
			const std::string& oldValue,
			const std::string& newValue,
			const std::string& remarks
		) const	{

			stringstream content;
			content <<
				table << ";" <<
				localId << ";" <<
				locaName << ";" <<
				(syntheseId ? lexical_cast<string>(syntheseId) : string()) << ";" <<
				syntheseName << ";" <<
				oldValue << ";" <<
				newValue << ";" <<
				remarks
			;
			_logTrace(content.str());
		}

		string HanoverFileFormat::HanoverLineStop::getStopName() const
		{
			return syntheseStop->getName();
		}

}	}
