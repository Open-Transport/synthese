
/** PegaseFileFormat class header.
	@file PegaseFileFormat.hpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#ifndef SYNTHESE_pt_PegaseFileFormat_hpp__
#define SYNTHESE_pt_PegaseFileFormat_hpp__

#include "FileFormatTemplate.h"
#include "NoExportPolicy.hpp"
#include "OneFileTypeImporter.hpp"
#include "PTDataCleanerFileFormat.hpp"
#include "StopPointTableSync.hpp"
#include "ImportableTableSync.hpp"
#include "TransportNetworkTableSync.h"
#include "CommercialLineTableSync.h"

namespace synthese
{
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// Pegase file format.
		//////////////////////////////////////////////////////////////////////////
		/// @author Sylvain Pasche
		/// @ingroup m35
		class PegaseFileFormat:
			public impex::FileFormatTemplate<PegaseFileFormat>
		{
		public:
			class SQLDumpParser
			{
			private:
				std::ifstream& _inFile;

				std::string _line;
				std::string::const_iterator _ptr;
				std::string _tableToParse;

				std::map<std::string, int> _columnMap;
				typedef std::vector<std::string> Row;
				Row _row;

				typedef std::deque<Row> Rows;
				Rows _rows;

			public:
				SQLDumpParser(std::ifstream& inFile) : _inFile(inFile)
				{
				}

				void setTableToParse(const std::string& tableToParse);
				bool getRow();
				const std::string& getCell(const std::string& column);
				int getCellInt(const std::string& column);

			private:
				void _skipWhitespace();
				string _getToken();
				Row _parseTokenList();
				void _parseInsert();
				void _expect(char c);
				void _expect(const std::string& match);
				bool _hasNext(char c);
				bool _hasNext(const std::string& match);
			};



			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::OneFileTypeImporter<PegaseFileFormat>,
				public PTDataCleanerFileFormat
			{
			public:
				static const std::string PARAMETER_NETWORK_ID;
				// TODO: implement
				static const std::string PARAMETER_LINE_FILTER_MODE;
				static const std::string FILTER_MODE1;
				static const std::string FILTER_MODE2;

			private:
				boost::shared_ptr<TransportNetwork> _network;
				mutable impex::ImportableTableSync::ObjectBySource<StopPointTableSync> _stopPoints;
				mutable impex::ImportableTableSync::ObjectBySource<CommercialLineTableSync> _lines;
				std::string _lineFilterMode;

			protected:

				virtual bool _parse(
					const boost::filesystem::path& filePath,
					std::ostream& os,
					boost::optional<const admin::AdminRequest&> adminRequest
				) const;

			private:

				void _parseStopPoints() const;

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
				/// @author Sylvain Pasche
				/// @date 2011
				/// @since 3.2.1
				virtual util::ParametersMap _getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author Sylvain Pasche
				/// @date 2011
				/// @since 3.2.1
				virtual void _setFromParametersMap(const util::ParametersMap& map);



				virtual db::DBTransaction _save() const;
			};

			typedef impex::NoExportPolicy<PegaseFileFormat> Exporter_;
		};
	}
}

#endif // SYNTHESE_pt_PegaseFileFormat_hpp__
