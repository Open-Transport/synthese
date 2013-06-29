
/** IneoOperationFileFormat class header.
	@file IneoOperationFileFormat.hpp

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

#ifndef SYNTHESE_IneoOperationFileFormat_H__
#define SYNTHESE_IneoOperationFileFormat_H__

#include "FileFormatTemplate.h"
#include "MultipleFileTypesImporter.hpp"
#include "NoExportPolicy.hpp"
#include "PTDataCleanerFileFormat.hpp"
#include "PTFileFormat.hpp"
#include "PTOperationFileFormat.hpp"

#include "Calendar.h"
#include "DriverActivityTableSync.hpp"
#include "DriverAllocationTemplateTableSync.hpp"
#include "ImportableTableSync.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace synthese
{
	namespace impex
	{
		class DataSource;
	}

	namespace util
	{
		class Registrable;
	}

	namespace pt_operation
	{
		class DriverAllocation;
		class DriverAllocationTemplate;
		class DriverService;

		//////////////////////////////////////////////////////////////////////////
		/// Ineo file format.
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m35File refFile
		class IneoOperationFileFormat:
			public impex::FileFormatTemplate<IneoOperationFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::MultipleFileTypesImporter<IneoOperationFileFormat>,
				public data_exchange::PTFileFormat,
				public data_exchange::PTOperationFileFormat
			{
			public:
				static const std::string FILE_SAB; // Driver services
				static const std::string FILE_AFA; // Driver allocations

				static const std::string PARAMETER_PT_DATASOURCE_ID;

			private:
				//! @name Parameters
				//@{
					boost::shared_ptr<const impex::DataSource> _ptDatasource;
				//@}

				static const std::string SEP;

				typedef std::map<std::string, std::vector<std::string> > FieldMaps;
				mutable FieldMaps _fieldsMap;

				mutable std::set<boost::shared_ptr<const DriverAllocation> > _driverAllocationsToRemove;
				mutable std::set<boost::shared_ptr<const DriverAllocationTemplate> > _driverAllocationTemplatesToRemove;
				mutable std::set<boost::shared_ptr<const DriverService> > _driverServicesToRemove;

				mutable std::map<std::string, std::string> _line;
				mutable std::string _section;

				void _clearFieldsMap() const;
				std::string _getValue(const std::string& field) const;
				bool _readLine(std::ifstream& file) const;
				void _loadLine(const std::string& line) const;

				//! @name Parameters
				//@{
					boost::gregorian::date _startDate;
					boost::gregorian::date _endDate;
				//@}

				mutable impex::ImportableTableSync::ObjectBySource<pt_operation::DriverActivityTableSync> _activities;
				mutable impex::ImportableTableSync::ObjectBySource<pt_operation::DriverAllocationTemplateTableSync> _driverAllocationTemplates;


			protected:

				virtual bool _checkPathsMap() const;

				virtual bool beforeParsing();
				virtual bool afterParsing();

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

			typedef impex::NoExportPolicy<IneoOperationFileFormat> Exporter_;
		};
	}
}

#endif
