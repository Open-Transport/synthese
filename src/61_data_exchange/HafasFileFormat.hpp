
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
#include "ImportableTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "IConv.hpp"

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
		class RollingStock;
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
			class Importer_:
				public impex::MultipleFileTypesImporter<HafasFileFormat>,
				public PTDataCleanerFileFormat
			{

			public:
				static const std::string FILE_ECKDATEN;
				static const std::string FILE_BITFELD;
				static const std::string FILE_ZUGDAT;
				static const std::string FILE_KOORD;
				static const std::string FILE_GLEIS;
				static const std::string FILE_METABHF;
				static const std::string FILE_UMSTEIGB;
				static const std::string FILE_UMSTEIGZ;

				static const std::string PARAMETER_SHOW_STOPS_ONLY;
				static const std::string PARAMETER_NETWORK_ID;
				static const std::string PARAMETER_WAYBACK_BIT_POSITION;
				static const std::string PARAMETER_IMPORT_FULL_SERVICES;
				static const std::string PARAMETER_IMPORT_STOPS;

			private:
				//! @name Parameters
				//@{
					boost::shared_ptr<pt::TransportNetwork> _network;
					bool _showStopsOnly;
					std::size_t _wayBackBitPosition;
					bool _importFullServices;
					bool _importStops;
				//@}

				typedef std::map<int, calendar::Calendar> CalendarMap;
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
				mutable Bahnhofs _bahnhofs;

				struct Zug
				{
					struct CalendarUse
					{
						std::size_t calendarNumber;
						std::string startStopCode;
						std::string endStopCode;
					};

					std::string number;
					std::string lineNumber;
					std::size_t version;
					std::vector<CalendarUse> calendars;
					std::string transportModeCode;
					boost::posix_time::time_duration continuousServiceRange;
					boost::posix_time::time_duration continuousServiceWaitingTime;

					// Served stops
					struct Stop
					{
						std::string stopCode;
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
				mutable Zugs _zugs;

				mutable Bahnhofs _nonLinkedBahnhofs;
				mutable Bahnhofs _linkedBahnhofs;
				const util::IConv _iconv;

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

				void _showBahnhofScreen(
					std::ostream& os,
					boost::optional<const server::Request&> adminRequest
				) const;

				bool _importObjects(std::ostream& os) const;

			protected:

				virtual bool _checkPathsMap() const;

				virtual bool _parse(
					const boost::filesystem::path& filePath,
					std::ostream& os,
					const std::string& key,
					boost::optional<const server::Request&> adminRequest
				) const;



			public:
				Importer_(
					util::Env& env,
					const impex::DataSource& dataSource
				):	impex::MultipleFileTypesImporter<HafasFileFormat>(env, dataSource),
					PTDataCleanerFileFormat(env, dataSource),
					impex::Importer(env, dataSource),
					_showStopsOnly(false),
					_wayBackBitPosition(0),
					_importFullServices(false),
					_importStops(false),
					_iconv(dataSource.getCharset().empty() ? string("UTF-8") : dataSource.getCharset(), "UTF-8")
				{}

				//////////////////////////////////////////////////////////////////////////
				/// Import screen to include in the administration console.
				/// @param os stream to write the result on
				/// @param request request for display of the administration console
				/// @since 3.2.0
				/// @date 2010
				virtual void displayAdmin(
					std::ostream& os,
					const server::Request& request
				) const;




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

			typedef impex::NoExportPolicy<HafasFileFormat> Exporter_;
		};
	}
}

#endif
