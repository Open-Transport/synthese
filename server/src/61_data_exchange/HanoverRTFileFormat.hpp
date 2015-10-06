
//////////////////////////////////////////////////////////////////////////
/// HanoverRT file format class header.
///	@file HanoverRTFileFormat.hpp
///	@author Camille Hue
///	@date 2015
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_HanoverRTFileFormat_H__
#define SYNTHESE_HanoverRTFileFormat_H__

#include "DatabaseReadImporter.hpp"
#include "FactorableTemplate.h"
#include "FileFormatTemplate.h"
#include "HanoverFileFormat.hpp"
#include "PTFileFormat.hpp"
#include "NoExportPolicy.hpp"

#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "CommercialLineTableSync.h"
#include "SchedulesBasedService.h"

namespace synthese
{
	namespace impex
	{
		class DataSource;
	}

	namespace departure_boards
	{
		class DisplayScreen;
	}

	namespace pt_operation
	{
		class Depot;
	}

	namespace pt
	{
		class StopPoint;
		class StopArea;
		class CommercialLine;
		class JourneyPattern;
		class ScheduledService;
		class TransportNetwork;
	}

	namespace geography
	{
		class City;
	}

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// HanoverRT file format.
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m61File refFile
		class HanoverRTFileFormat:
			public impex::FileFormatTemplate<HanoverRTFileFormat>
		{
		public:

			class Importer_:
				public impex::DatabaseReadImporter<HanoverRTFileFormat>,
				public PTFileFormat,
				public HanoverFileFormat
			{
			public:
				static const std::string PARAMETER_DB_CONN_STRING;
				static const std::string PARAMETER_PLANNED_DATASOURCE_ID;
				static const std::string PARAMETER_HYSTERESIS;

			private:
				boost::optional<std::string> _dbConnString;
				boost::shared_ptr<const impex::DataSource> _plannedDataSource;
				boost::posix_time::time_duration _hysteresis;

				mutable std::set<pt::ScheduledService*> _servicesToSave;
				mutable impex::ImportableTableSync::ObjectBySource<pt::StopAreaTableSync> _stopAreas;
				mutable impex::ImportableTableSync::ObjectBySource<pt::StopPointTableSync> _stopPoints;
				mutable impex::ImportableTableSync::ObjectBySource<pt::CommercialLineTableSync> _lines;

				mutable RoutesMap _routes;
				mutable HanoverSchedulesMap _hanoverTheoricalSchedules;
				mutable Calendars _calendars;
				mutable RunsMap _runs;


			protected:
				//////////////////////////////////////////////////////////////////////////
				/// Generates a generic parameters map from the action parameters.
				/// @return The generated parameters map
				util::ParametersMap getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Reads the parameters of the action on a generic parameters map.
				/// @param map Parameters map to interpret
				/// @exception ActionException Occurs when some parameters are missing or incorrect.
				void _setFromParametersMap(const util::ParametersMap& map);

			public:
				Importer_(
					util::Env& env,
					const impex::Import& import,
					impex::ImportLogLevel minLogLevel,
					const std::string& logPath,
					boost::optional<std::ostream&> outputStream,
					util::ParametersMap& pm
				);


				virtual db::DBTransaction _save() const;


				//////////////////////////////////////////////////////////////////////////
				/// The action execution code.
				virtual bool _read(
				) const;
			};


			typedef impex::NoExportPolicy<HanoverRTFileFormat> Exporter_;
		};
}	}

#endif // SYNTHESE_HanoverRTFileFormat_H__

