
/** HastusInterfaceFileFormat class header.
	@file HastusInterfaceFileFormat.hpp

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


#ifndef SYNTHESE_IMPEX_HastusFileFormat_H
#define SYNTHESE_IMPEX_HastusFileFormat_H

#include "FileFormatTemplate.h"
#include "OneFileTypeImporter.hpp"
#include "PTDataCleanerFileFormat.hpp"
#include "NoExportPolicy.hpp"
#include "ScheduledService.h"
#include "GraphTypes.h"

namespace synthese
{
	namespace pt
	{
		class TransportNetwork;

		//////////////////////////////////////////////////////////////////////////
		/// Hastus Interface file format.
		/// @ingroup m35File refFile
		///
		class HastusInterfaceFileFormat:
			public impex::FileFormatTemplate<HastusInterfaceFileFormat>
		{
		public:
			class Importer_:
				public impex::OneFileTypeImporter<Importer_>,
				public PTDataCleanerFileFormat
			{
			private:
				//! @name Import parameters
				//@{
					boost::shared_ptr<TransportNetwork> _network;
				//@}

				struct Record
				{
					double recordNumber;
					std::string content;
				};

				mutable std::ifstream _file;
				mutable boost::optional<Record> _record;

				struct TemporaryService
				{
					std::string lineCode;
					std::string calendar;
					std::string code;
					std::string routeCode;
					ScheduledService::Schedules schedules;
					std::vector<graph::MetricOffset> scheduledStopsDistances;
					std::vector<std::string> scheduledStops;
					ScheduledService::Schedules allSchedules;
					std::vector<std::string> stops;
					bool wayBack;
				};

				//////////////////////////////////////////////////////////////////////////
				/// @return Record : recordNumber == 0 => end of file
				void _loadNextRecord(
					double recordNumber
				) const;

				void _loadNextRecord(
				) const;

				void _openFile(
					const boost::filesystem::path& filePath
				) const;


				//////////////////////////////////////////////////////////////////////////
				/// Gets a text field in the file.
				/// @param start the position of the field in the line according to the file
				/// format specification (begins at 1 instead of 0 as usually)
				/// @param length length of the field
				/// @return the returned text is trimmed
				std::string _getTextField(
					std::size_t start,
					std::size_t length
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets a fields vector in the file.
				/// @param numberPosition the position of the length field in the first line according to the file
				/// format specification (begins at 1 instead of 0 as usually)
				/// @param startPosition the position of the first field in the first line according to the file
				/// format specification (begins at 1 instead of 0 as usually)
				/// @param recordLength length of each field
				/// @return all the returned text elements are trimmed
				std::vector<std::string> _getNextVector(
					double recordNumber,
					std::size_t numberPosition,
					std::size_t startPosition,
					std::size_t recordLength
				) const;

				bool _eof() const;

			public:
				static const std::string PARAMETER_TRANSPORT_NETWORK_ID;

				Importer_(
					util::Env& env,
					const impex::DataSource& dataSource
				);



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from attributes to generic parameter maps.
				/// @return Generated parameters map
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				virtual util::ParametersMap _getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				virtual void _setFromParametersMap(const util::ParametersMap& map);



				//////////////////////////////////////////////////////////////////////////
				/// File import.
				/// @param filePath path of the file to import
				/// @param os stream to write information messages on
				/// @author Hugues Romain
				virtual bool _parse(
					const boost::filesystem::path& filePath,
					std::ostream& os,
					boost::optional<const admin::AdminRequest&> adminRequest
				) const;

				virtual db::DBTransaction _save() const;



				//////////////////////////////////////////////////////////////////////////
				/// Import screen to include in the administration console.
				/// @param os stream to write the result on
				/// @param request request for display of the administration console
				/// @author Hugues Romain
				/// @since 3.2.0
				/// @date 2010
				virtual void displayAdmin(
					std::ostream& os,
					const admin::AdminRequest& request
				) const;

			};

			class Exporter_:
				public impex::NoExportPolicy<HastusInterfaceFileFormat>
			{
			public:
			};

			friend class Importer_;
			friend class Exporter_;
		};
	}
}

#endif
