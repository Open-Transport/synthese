
//////////////////////////////////////////////////////////////////////////
///	ServicesCSVFileFormat class header.
///	@file ServicesCSVFileFormat.hpp
///	@author Gael Sauvanet
///	@date 2012
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

#ifndef SYNTHESE_ServicesCSVFileFormat_hpp__
#define SYNTHESE_ServicesCSVFileFormat_hpp__

#include "FileFormatTemplate.h"

#include "CommercialLineTableSync.h"
#include "ImportableTableSync.hpp"
#include "JourneyPatternTableSync.hpp"
#include "MultipleFileTypesImporter.hpp"
#include "NoExportPolicy.hpp"
#include "PTDataCleanerFileFormat.hpp"
#include "ScheduledServiceTableSync.h"
#include "StopPointTableSync.hpp"
#include "StopAreaTableSync.hpp"

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

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
		class ServiceCalendarLink;

		//////////////////////////////////////////////////////////////////////////
		///	ServicesCSV file format.
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m35File refFile
		class ServicesCSVFileFormat:
			public impex::FileFormatTemplate<ServicesCSVFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::MultipleFileTypesImporter<ServicesCSVFileFormat>,
				public PTDataCleanerFileFormat
			{
			public:
				static const std::string PATH_SERVICES;

				static const std::string PARAMETER_ROLLING_STOCK_ID;
				static const std::string PARAMETER_NUMBER_OF_LINES_TO_IGNORE;
				static const std::string PARAMETER_USE_RULE_BLOCK_ID_MASK;
				static const std::string PARAMETER_STOP_AREA_DEFAULT_CITY;

				static const std::string PARAMETER_FIELD_SERVICE_NUMBER;
				static const std::string PARAMETER_FIELD_STOP_CODE;
				static const std::string PARAMETER_FIELD_STOP_NAME;
				static const std::string PARAMETER_FIELD_TIME;

			private:
				static const std::string SEP;

				boost::shared_ptr<RollingStock> _rollingStock;
				boost::shared_ptr<const geography::City> _defaultCity;
				boost::optional<std::size_t> _serviceNumberField;
				boost::optional<std::size_t> _stopCodeField;
				boost::optional<std::size_t> _stopNameField;
				boost::optional<std::size_t> _timeField;

				int _numberOfLinesToIgnore;

				bool _interactive;

				typedef std::map<std::string, std::size_t> FieldsMap;
				mutable FieldsMap _fieldsMap;

				mutable std::vector<std::string> _line;

				struct ServiceDetail
				{
					std::string serviceNumber;
					ScheduledService::Schedules arrivalSchedules;
					ScheduledService::Schedules departureSchedules;
					JourneyPattern::StopsWithDepartureArrivalAuthorization stops;
				};
				typedef std::vector<ServiceDetail> ServiceDetailVector;

				typedef std::map<std::string, const PTUseRule*> PTUseRuleBlockMasks;
				PTUseRuleBlockMasks _ptUseRuleBlockMasks;
				static std::string _serializePTUseRuleBlockMasks(const PTUseRuleBlockMasks& object);

				std::string _getValue(const std::size_t field ) const;
				void _loadLine(const std::string& line) const;

				mutable impex::ImportableTableSync::ObjectBySource<CommercialLineTableSync> _lines;
				mutable impex::ImportableTableSync::ObjectBySource<StopAreaTableSync> _stopAreas;
				mutable impex::ImportableTableSync::ObjectBySource<StopPointTableSync> _stopPoints;

			protected:

				virtual bool _checkPathsMap() const;

				virtual bool _parse(
					const boost::filesystem::path& filePath,
					std::ostream& os,
					const std::string& key,
					boost::optional<const admin::AdminRequest&> adminRequest
				) const;


			public:
				Importer_(
					util::Env& env,
					const impex::DataSource& dataSource
				);



				//////////////////////////////////////////////////////////////////////////
				/// Import screen to include in the administration console.
				/// @param os stream to write the result on
				/// @param request request for display of the administration console
				/// @since 3.2.0
				/// @date 2010
				virtual void displayAdmin(
					std::ostream& os,
					const admin::AdminRequest& request
				) const;



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



				virtual db::DBTransaction _save() const;
			};

			typedef impex::NoExportPolicy<ServicesCSVFileFormat> Exporter_;
		};
	}
}

#endif
