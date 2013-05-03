
/** HeuresFileFormat class header.
	@file HeuresFileFormat.hpp

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

#ifndef SYNTHESE_HeuresFileFormat_H__
#define SYNTHESE_HeuresFileFormat_H__

#include "FileFormatTemplate.h"
#include "Calendar.h"
#include "MultipleFileTypesImporter.hpp"
#include "OneFileExporter.hpp"
#include "PTDataCleanerFileFormat.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplate;
	}

	namespace util
	{
		class Registrable;
	}

	namespace geography
	{
		class City;
	}

	namespace tree
	{
		class TreeFolder;
	}

	namespace pt
	{
		class Destination;
		class JourneyPattern;
		class CommercialLine;
		class ScheduledService;
		class TransportNetwork;
	}

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// Heures file format.
		/// @ingroup m61File refFile
		//////////////////////////////////////////////////////////////////////////
		/// The Heures schedules import uses 3 files :
		///	<ul>
		///		<li>itinerai.tmp : Routes</li>
		///		<li>troncons.tmp : Schedules</li>
		///		<li>services.tmp : Calendars</li>
		///	</ul>
		///
		class HeuresFileFormat:
			public impex::FileFormatTemplate<HeuresFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::MultipleFileTypesImporter<HeuresFileFormat>,
				public PTDataCleanerFileFormat
			{

			public:
				static const std::string FILE_POINTSARRETS;
				static const std::string FILE_ITINERAI;
				static const std::string FILE_TRONCONS;
				static const std::string FILE_SERVICES;

				static const std::string PARAMETER_DISPLAY_LINKED_STOPS;
				static const std::string PARAMETER_NETWORK_ID;
				static const std::string PARAMETER_DAY7_CALENDAR_ID;
				static const std::string PARAMETER_STOPS_DATASOURCE_ID;

			private:

				bool _displayLinkedStops;
				boost::shared_ptr<pt::TransportNetwork> _network;
				boost::shared_ptr<const calendar::CalendarTemplate> _day7CalendarTemplate;
				boost::shared_ptr<const impex::DataSource> _stopsDataSource;

				typedef std::map<std::pair<int, std::string>, pt::JourneyPattern*> RoutesMap;
				mutable RoutesMap _routes;
				typedef std::set<std::pair<int, std::string> > IgnoredRoutes;
				mutable IgnoredRoutes _technicalRoutes;

				typedef std::map<std::pair<int, int>, std::vector<pt::ScheduledService*> > ServicesMap;
				mutable ServicesMap _services;

				typedef std::map<int, pt::Destination*> DestinationsMap;
				mutable DestinationsMap _destinations;

			protected:

				virtual bool _checkPathsMap() const;

				virtual bool _parse(
					const boost::filesystem::path& filePath,
					const std::string& key,
					boost::optional<const server::Request&> adminRequest
				) const;



			public:
				Importer_(
					util::Env& env,
					const impex::Import& import,
					const impex::ImportLogger& logger
				):	impex::Importer(env, import, logger),
					impex::MultipleFileTypesImporter<HeuresFileFormat>(env, import, logger),
					PTDataCleanerFileFormat(env, import, logger)
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

				virtual db::DBTransaction _save() const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from attributes to generic parameter maps.
				/// @return Generated parameters map
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.2.1
				virtual util::ParametersMap _getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.2.1
				virtual void _setFromParametersMap(const util::ParametersMap& map);
			};

			class Exporter_:
				public impex::OneFileExporter<HeuresFileFormat>
			{
			private:
				boost::shared_ptr<const pt::TransportNetwork> _network;
				boost::shared_ptr<tree::TreeFolder> _folder;
				boost::shared_ptr<const impex::DataSource> _dataSource;

			public:
				static const std::string PARAMETER_NETWORK_ID;
				static const std::string PARAMETER_DATASOURCE_ID;

				Exporter_(){}

				virtual util::ParametersMap getParametersMap() const;

				virtual void setFromParametersMap(const util::ParametersMap& map);

				virtual void build(std::ostream& os) const;

				static void _writeHour(std::ostream& os, const boost::posix_time::time_duration& duration);
				static void _writeTextAndSpaces(
					std::ostream& os,
					const std::string& value,
					size_t width,
					bool spacesAtRight = true,
					char spaceChar = ' '
				);

				virtual std::string getOutputMimeType() const { return "application/zip"; }
				virtual std::string getFileName() const { return "Heures.zip"; }
			};
		};
	}
}

#endif
