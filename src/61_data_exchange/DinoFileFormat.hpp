
/** DinoFileFormat class header.
	@file DinoFileFormat.hpp

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

#ifndef SYNTHESE_DinoFileFormat_H__
#define SYNTHESE_DinoFileFormat_H__

#include "Calendar.h"
#include "FileFormatTemplate.h"
#include "MultipleFileTypesImporter.hpp"
#include "OneFileExporter.hpp"
#include "PTFileFormat.hpp"
#include "ImportableTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "TransportNetworkTableSync.h"
#include "CommercialLineTableSync.h"
#include "StopAreaTableSync.hpp"
#include "PTDataCleanerFileFormat.hpp"
#include "LineStopTableSync.h"
#include "Path.h"
#include "SchedulesBasedService.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <list>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace synthese
{
	namespace util
	{
		class Registrable;
	}

	namespace geography
	{
		class City;
	}

	namespace pt
	{
		class JourneyPattern;
		class CommercialLine;
		class ScheduledService;
		class TransportNetwork;
		class PTUseRule;
	}

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// Dino file format.
		/// See attached files at extranet.rcsmobility.com/issues/47687
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m61File refFile
		class DinoFileFormat:
			public impex::FileFormatTemplate<DinoFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::MultipleFileTypesImporter<DinoFileFormat>,
				public PTDataCleanerFileFormat,
				public PTFileFormat
			{
			public:
				static const std::string FILE_STOP_AREAS;
				static const std::string FILE_STOPS;
				static const std::string FILE_STOPPING_POINTS;
				static const std::string FILE_BRANCH;
				static const std::string FILE_LINES;
				static const std::string FILE_STOP_TIMES;
				static const std::string FILE_JOURNEY;
				static const std::string FILE_SERVICE_RESTRICTION;
				static const std::string FILE_DAY_TYPE_TO_ATTRIBUTE;
				static const std::string FILE_CALENDAR;
				static const std::string FILE_CALENDAR_DATES;
				static const std::string FILE_TRIPS;

				static const std::string PARAMETER_IMPORT_TRANSPORT_NETWORK;
				static const std::string PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION;
				static const std::string PARAMETER_DISPLAY_LINKED_STOPS;
				static const std::string PARAMETER_USE_RULE_BLOCK_ID_MASK;
				static const std::string PARAMETER_IGNORE_SERVICE_NUMBER;

			private:
				static const std::string SEP;

				bool _interactive;
				bool _displayLinkedStops;
				boost::posix_time::time_duration _stopAreaDefaultTransferDuration;
				bool _ignoreServiceNumber;
				bool _createNetworks;

				typedef std::map<std::string, std::size_t> FieldsMap;
				mutable FieldsMap _fieldsMap;

				typedef std::map<std::string, const pt::PTUseRule*> PTUseRuleBlockMasks;
				PTUseRuleBlockMasks _ptUseRuleBlockMasks;
				static std::string _serializePTUseRuleBlockMasks(const PTUseRuleBlockMasks& object);

				mutable std::vector<std::string> _line;

				void _loadFieldsMap(const std::string& line) const;
				std::string _getValue(const std::string& field) const;
				void _loadLine(const std::string& line) const;

				typedef std::map<std::string, calendar::Calendar> Calendars;
				mutable Calendars _calendars;

				mutable impex::ImportableTableSync::ObjectBySource<pt::TransportNetworkTableSync> _networks;
				mutable impex::ImportableTableSync::ObjectBySource<pt::StopPointTableSync> _stopPoints;
				mutable impex::ImportableTableSync::ObjectBySource<pt::CommercialLineTableSync> _lines;
				mutable impex::ImportableTableSync::ObjectBySource<pt::StopAreaTableSync> _stopAreas;

				typedef std::map<std::string, boost::shared_ptr<geography::City> > DinoCitiesMap; /* <city code, city> */
				mutable DinoCitiesMap _cities;

				typedef std::map<std::string, pt::TransportNetwork*> _NetworksMap;
				typedef std::map<std::string, std::string> _MissingNetworksMap;

				mutable _NetworksMap _networksMap;
				mutable _MissingNetworksMap _missingNetworks;



				struct StoppingPoint
				{
					std::string code;
					pt::StopPoint* stoppoint;
					int pos;
					graph::MetricOffset offsetFromPreviousStop;
					std::string getStopPointName() const;
				};

				struct DinoSchedule
				{
					boost::posix_time::time_duration art; /* arrival time */
					boost::posix_time::time_duration wt; /* waiting time before departure */
				};
				typedef std::vector<DinoSchedule> DinoSchedules;
				typedef std::map<std::string /* journeyCode */, DinoSchedules> DinoSchedulesMap;
				mutable DinoSchedulesMap _dinoSchedules;

				void _selectAndLoadDinoSchedules(
						DinoSchedulesMap& dinoSchedules,
						const DinoSchedules& schedules,
						const std::string& journeyCode
				) const;

				struct Journey
				{
					std::string code;
					std::string name;
					pt::CommercialLine* line;
					bool direction;
					typedef std::vector<StoppingPoint> StoppingPoints;
					StoppingPoints stoppingPoints;
				};

				typedef std::map<std::string, Journey> JourneysMap;
				mutable JourneysMap _journeys;

				void _selectAndLoadJourney(
					JourneysMap& journeys,
					const Journey::StoppingPoints& stoppingPoints,
					pt::CommercialLine* line,
					const std::string& name,
					bool direction,
					const std::string& journeyCode
				) const;

				struct DinoCalendar
				{
					std::string code;
//					std::string name;
					calendar::Calendar calendar;
				};
				typedef std::map<std::string, DinoCalendar> DinoCalendars;
				mutable DinoCalendars _restrictions;  /* RESTRICTION code as key */
				mutable DinoCalendars _dayAttributes; /* DAY ATTRIBUTE NR as key */
				typedef std::map<int, std::set<int> > DayTypes;
				mutable DayTypes _dayTypes;

				struct Trip
				{
					std::string code;
					Journey* journey;
					boost::posix_time::time_duration startTime;
					calendar::Calendar calendar;
					const DinoSchedules* schedules;
				};
				typedef std::map<std::string, Trip> TripsMap;
				mutable TripsMap _trips;

				void _selectAndLoadTrip(TripsMap& trips,
					Journey &journey,
					const std::string& tripCode,
					const calendar::Calendar& calendar,
					const boost::posix_time::time_duration& startTime,
					const DinoSchedules& schedules
				) const;


				std::string _hexToBinString(const std::string& s) const;
				void _fillCalendar(calendar::Calendar& c, const std::string& bitsetMonthStr, int year, int month) const;


				void _logLoadDetail(
					const std::string& table,
					const std::string& localId,
					const std::string& locaName,
					const util::RegistryKeyType syntheseId,
					const std::string& syntheseName,
					const std::string& oldValue,
					const std::string& newValue,
					const std::string& remarks
				) const;

				void _logWarningDetail(
					const std::string& table,
					const std::string& localId,
					const std::string& locaName,
					const util::RegistryKeyType syntheseId,
					const std::string& syntheseName,
					const std::string& oldValue,
					const std::string& newValue,
					const std::string& remarks
				) const;

				void _logDebugDetail(
					const std::string& table,
					const std::string& localId,
					const std::string& locaName,
					const util::RegistryKeyType syntheseId,
					const std::string& syntheseName,
					const std::string& oldValue,
					const std::string& newValue,
					const std::string& remarks
				) const;

			protected:

				virtual bool _checkPathsMap() const;

				virtual bool _parse(
					const boost::filesystem::path& filePath,
					const std::string& key
				) const;


			public:
				Importer_(
					util::Env& env,
					const impex::Import& import,
					impex::ImportLogLevel minLogLevel,
					const std::string& logPath,
					boost::optional<std::ostream&> outputStream,
					util::ParametersMap& pm
				);



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from attributes to generic parameter maps.
				/// @return Generated parameters map
				/// @author Thomas Puigt
				/// @date 2015
				/// @since 3.8.0
				virtual util::ParametersMap _getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author Thomas Puigt
				/// @date 2015
				/// @since 3.8.0
				virtual void _setFromParametersMap(const util::ParametersMap& map);



				virtual db::DBTransaction _save() const;
			};

			class Exporter_:
				public impex::OneFileExporter<DinoFileFormat>
			{
			private:
				mutable util::Env _env;

				util::RegistryKeyType _key(util::RegistryKeyType key,
					util::RegistryKeyType suffix = 0
				) const;

				std::string _Str(std::string str) const;

				std::string _SubLine(std::string str) const;

				static const std::string LABEL_TAD;
				static const std::string LABEL_NO_EXPORT_DINO;
				static const int WGS84_SRID;

				static std::map<std::string,util::RegistryKeyType> shapeId;

			public:
				Exporter_(const impex::Export& export_);

				virtual util::ParametersMap getParametersMap() const;

				virtual void setFromParametersMap(const util::ParametersMap& map);

				virtual void build(std::ostream& os) const;

				virtual std::string getOutputMimeType() const { return "application/zip"; }
				virtual std::string getFileName() const { return "DINO.zip"; }

			};
		};
}	}

#endif
