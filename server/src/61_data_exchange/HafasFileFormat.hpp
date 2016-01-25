
/** HafasFileFormat class header.
	@file HafasFileFormat.hpp

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

#ifndef SYNTHESE_HafasFileFormat_H__
#define SYNTHESE_HafasFileFormat_H__

#include "FileFormatTemplate.h"
#include "Calendar.h"
#include "MultipleFileTypesImporter.hpp"
#include "NoExportPolicy.hpp"
#include "PTDataCleanerFileFormat.hpp"
#include "PTFileFormat.hpp"
#include "ImportableTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "TransportNetworkTableSync.h"
#include "IConv.hpp"
#include "OneFileExporter.hpp"
#include "Importable.h"
#include "exception"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/tuple/tuple.hpp>

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
		class TransportNetwork;
	}

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// Hafas file format.
		/// @ingroup m61File refFile
		//////////////////////////////////////////////////////////////////////////
		/// The Hafas export reads 4 files :
		///	<ul>
		///		<li>KOORD.DAT : Coordinates of commercial stops</li>
		///		<li>ECKDATEN.DAT : Date range of the data</li>
		///		<li>BITFELD.DAT : Calendars</li>
		///		<li>ZUGUDAT.DAT : Routes and service schedules</li>
		///	</ul>
		///
		/// <h2>Import</h2>
		///
		/// The import is separated into two parts :
		///	<ul>
		///		<li>Import of the stops : there is no automated import, because of the need of merging
		///		the data with other sources. This work can only be done manually. A @ref PTStopsImportWizardAdmin "special admin page"
		///		can read such BAHNHOF.DAT and KOORD.DAT and show differences with SYNTHESE.</li>
		///		<li>Import of the services : the import is automated, and is possible only if all stops
		///		are referenced in the SYNTHESE database, and if the line has been manually created in
		///		the SYNTHESE database and linked to the Hafas database by the operator_code field</li>
		///	</ul>
		///
		///	<h3>Import of the stops</h3>
		///
		/// The physical stops must be linked with the items of BAHNHOF.DAT.
		/// More than one physical stop can be linked with the same Hafas stop. In this case,
		/// the import will select automatically the actual stop regarding the whole itinerary.
		///
		/// <h3>Import of the services</h3>
		///
		/// The import follows these steps :
		///	<ol>
		///		<li>Load of the date ranges from ECKDATEN.DAT</li>
		///		<li>Load of the bits from BITFELD.DAT</li>
		///		<li>Load of the services from ZUGDAT.DAT. For each service :</li>
		///		<ul>
		///			<li>Fetch of the line by its creator code</li>
		///			<li>Load of the route</li>
		///			<li>Comparison with all existing routes</li>
		///			<li>Creation of a route if necessary</li>
		///			<li>Comparison with all existing services</li>
		///			<li>Creation of a service if ncessary</li>
		///		</ul>
		///	</ol>
		class HafasFileFormat:
			public impex::FileFormatTemplate<HafasFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			/*        Shared between Importer and Exporter        */

			struct Bahnhof
			{
				std::string operatorCode;
				std::string cityName;
				std::string name;
				bool main;
				boost::shared_ptr<geos::geom::Point> point;
				mutable std::set<pt::StopPoint*> stops;
				boost::posix_time::time_duration defaultTransferDuration;
				std::set<std::string> gleisSet;
				bool used;

				Bahnhof():
					main(false),
					used(false)
				{}
			};
			typedef std::map<std::string, Bahnhof> Bahnhofs;

			struct LineFilter
			{
				static const std::string SEP_MAIN;
				static const std::string SEP_FIELD;
				static const std::string JOCKER;
				static const std::string VALUE_LINES_BY_STOPS_PAIRS;

				pt::TransportNetwork* network;
				boost::optional<size_t> lineNumberStart;
				boost::optional<size_t> lineNumberEnd;
				bool linesByStopsPair;

				LineFilter();
			};
			typedef std::map<std::string, LineFilter> LinesFilter;

			struct Zug
			{
				struct CalendarUse
				{
					size_t calendarNumber;
					std::string startStopCode;
					std::string endStopCode;
				};

				std::string number;
				std::string lineNumber;
				std::string lineShortName;
				bool readWayback;
				size_t version;
				std::vector<CalendarUse> calendars;
				std::string bitfieldCode;
				std::string transportModeCode;
				boost::posix_time::time_duration continuousServiceRange;
				boost::posix_time::time_duration continuousServiceWaitingTime;
				const LineFilter* lineFilter;

				// Served stops
				struct Stop
				{
					std::string stopCode;
					std::string stopName;
					std::string gleisCode;
					boost::posix_time::time_duration departureTime;
					boost::posix_time::time_duration arrivalTime;

					Stop():
						departureTime(boost::posix_time::not_a_date_time),
						arrivalTime(boost::posix_time::not_a_date_time)
					{}
				};
				typedef std::vector<Stop> Stops;
				Stops stops;
			};
			typedef std::vector<Zug> Zugs;


			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::MultipleFileTypesImporter<HafasFileFormat>,
				public PTDataCleanerFileFormat,
				public PTFileFormat
			{

			public:
				static const std::string FILE_ECKDATEN;
				static const std::string FILE_BITFELD;
				static const std::string FILE_ZUGDAT;
				static const std::string FILE_KOORD;
				static const std::string FILE_BAHNOF;
				static const std::string FILE_GLEIS;
				static const std::string FILE_METABHF;
				static const std::string FILE_UMSTEIGB;
				static const std::string FILE_UMSTEIGZ;

				static const std::string PARAMETER_SHOW_STOPS_ONLY;
				static const std::string PARAMETER_WAYBACK_BIT_POSITION;
				static const std::string PARAMETER_IMPORT_FULL_SERVICES;
				static const std::string PARAMETER_IMPORT_STOPS;
				static const std::string PARAMETER_LINES_FILTER;
				static const std::string PARAMETER_GLEIS_HAS_ONE_STOP_PER_LINE;
				static const std::string PARAMETER_COMPLETE_EMPTY_STOP_AREA_NAME;
				static const std::string PARAMETER_NO_GLEIS_FILE;
				static const std::string PARAMETER_TRY_TO_READ_LINE_SHORT_NAME;
				static const std::string PARAMETER_CONCATENATE_TRANSPORT_SHORT_NAME;
				static const std::string PARAMETER_READ_WAYBACK;
				static const std::string PARAMETER_CALENDAR_DEFAULT_CODE;
				static const std::string PARAMETER_2015_CARPOSTAL_FORMAT;

			private:

				LinesFilter getLinesFilter(const std::string& s);
				static std::string LinesFilterToString(const LinesFilter& value);

				impex::ImportableTableSync::ObjectBySource<pt::TransportNetworkTableSync> _networks;

				//! @name Parameters
				//@{
					bool _showStopsOnly;
					size_t _wayBackBitPosition;
					bool _importFullServices;
					bool _importStops;
					LinesFilter _linesFilter;
					bool _gleisHasOneStopPerLine;
					std::string _complete_empty_stop_area_name;
					bool _noGleisFile;
					bool _tryToReadShortName;
					bool _concatenateTransportShortName;
					bool _readWayback;
					size_t _defaultCalendarCode;
					bool _formatCarpostal2015;
				//@}

				const LineFilter* _lineIsIncluded(const std::string& lineNumber) const;

				typedef std::map<size_t, calendar::Calendar> CalendarMap;

				mutable Bahnhofs _bahnhofs;

				mutable Zugs _zugs;

				mutable Bahnhofs _nonLinkedBahnhofs;
				mutable Bahnhofs _linkedBahnhofs;
				const util::IConv _iconv;

				mutable boost::gregorian::date _fileFirstDate;
				mutable CalendarMap _calendarMap;

				typedef std::pair<
					std::string, // Service code
					std::string // Line code
				> ServiceId;
				typedef std::map<
					boost::tuple<
						std::string, // Stop code
						ServiceId, // From service
						ServiceId // To service
					>,
					boost::posix_time::time_duration // Inter service transfer duration
				> InterServiceTransferDurationMap;
				mutable InterServiceTransferDurationMap _interServiceTransferDurationMap;

				typedef std::map<
					std::pair<
						std::string, // From stop
						std::string // To stop
					>,
					boost::posix_time::time_duration // Inter stop transfer duration
				> InterStopTransferDurationMap;
				mutable InterStopTransferDurationMap _interStopTransferDurationMap;

				typedef std::map<
					std::string, // Stop code
					std::set<std::string> // Linked stops code
				> StopAreaMappingMap;
				mutable StopAreaMappingMap _stopAreaMappingMap;

				typedef std::map<
					boost::tuple<std::string, std::string, std::size_t>, // Number, Line, Version
					std::map<std::string, std::string> // StopArea, StopPoint
				> GleisMap;
				mutable GleisMap _gleisMap;


				//! @name File lines handling
				//@{
					mutable std::string _line;
					mutable std::ifstream _file;
					bool _openFile(const boost::filesystem::path& path) const;
					bool _loadLine() const;
					std::string _getField(std::size_t start, std::size_t len) const;
					std::string _getField(std::size_t start) const;
				//@}

				bool _importObjects() const;

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
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.3.0
				virtual util::ParametersMap _getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.3.0
				virtual void _setFromParametersMap(const util::ParametersMap& map);



				virtual db::DBTransaction _save() const;
			};

			//////////////////////////////////////////////////////////////////////////

			// **** HAFAS EXPORTER ****

			class Exporter_: public impex::OneFileExporter<HafasFileFormat> {

				public:
					static const std::string PARAMETER_DEBUG;
					static const std::string PARAMETER_NETWORK_NAME;
					static const std::string PARAMETER_FTP_HOST;
					static const std::string PARAMETER_FTP_PORT;
					static const std::string PARAMETER_FTP_USER;
					static const std::string PARAMETER_FTP_PASS;
					static const std::string PARAMETER_BITFIELD_START_DATE;
					static const std::string PARAMETER_BITFIELD_END_DATE;

				private:
					bool _debug;
					std::string _networkName;
					std::string _ftpHost;
					int _ftpPort;
					std::string _ftpUser;
					std::string _ftpPass;
					std::string _bitfieldStartDate;
					std::string _bitfieldEndDate;

					mutable util::Env _env;

					// TODO : Turn these into export parameters !
					static const std::string DIDOK_DATA_SOURCE_NAME;
					static const std::string TL_DATA_SOURCE_NAME;

					static const std::string OUTWARD_TRIP_CODE;
					static const std::string RETURN_TRIP_CODE;
					static const std::string DAILY_SERVICE_CODE;
					static const int DAYS_PER_BITFIELD;
					static const unsigned int BITFIELD_SIZE;

					static const unsigned int COORDINATES_SYSTEM;

					// File generation helpers
					static boost::filesystem::path createRandomFolder();
					static void createFile(std::ofstream& fileStream, boost::filesystem::path dir, string file);
					static void printColumn(std::ofstream& fileStream, int& pos, std::string value, int firstColumn, int lastColumn = -1);
					static void newLine(std::ofstream& fileStream, int& pos);
					static void printZugdatComment(std::ofstream& fileStream, int& pos, int& commentLine, std::string value);

					// General helpers
					static std::string getMandatoryString(const util::ParametersMap& map, std::string parameterName);
					static std::string getCodesForDataSource(const impex::Importable* object, std::string dataSourceName, std::string defaultValue = std::string());
					static std::string formatTime(boost::posix_time::time_duration time);
					static unsigned int strlenUtf8(std::string str);
					static std::string firstChars(std::string str, unsigned int maxLen);

					// Bit field helpers
					typedef std::map<std::string, std::string> BitFields;
					static std::string getBitfieldCode(std::vector<Zug::CalendarUse> calendars, unsigned int bitfieldSize, BitFields& _bitfields);
					static std::string computeBitfield(std::vector<Zug::CalendarUse> calendars, unsigned int bitfieldSize);
					static void setBit(unsigned short bitfield [], int bit);


				public:
					Exporter_(const impex::Export& export_);

					virtual void build(std::ostream& os) const;
					virtual util::ParametersMap getParametersMap() const;
					virtual void setFromParametersMap(const util::ParametersMap& map);
					virtual std::string getOutputMimeType() const { return "text/html"; }
					boost::filesystem::path exportToHafasFormat(Bahnhofs _bahnhofs, Zugs _zugs, BitFields _bitfields) const;
					virtual void exportToBahnhofFile(boost::filesystem::path dir, string file, Bahnhofs _bahnhofs) const;
					virtual void exportToKoordFile(boost::filesystem::path dir, string file, Bahnhofs _bahnhofs) const;
					virtual void exportToZugdatFile(boost::filesystem::path dir, string file, Zugs _zugs) const;
					virtual void exportToBitfieldFile(boost::filesystem::path dir, string file, BitFields _bitfields) const;
			};

			// **** /HAFAS EXPORTER ****

		};
	}
}

#endif
