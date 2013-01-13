
/** RSSFileFormat class header.
	@file RSSFileFormat.hpp

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

#ifndef SYNTHESE_data_exchange_RSSFileFormat_hpp__
#define SYNTHESE_data_exchange_RSSFileFormat_hpp__

#include "FileFormatTemplate.h"
#include "OneFileTypeImporter.hpp"
#include "NoExportPolicy.hpp"

#include <boost/date_time/posix_time/ptime.hpp>

namespace synthese
{
	namespace util
	{
		class Registrable;
	}

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// RSS feed file format.
		///	@ingroup m61
		/// @author Hugues Romain
		class RSSFileFormat:
			public impex::FileFormatTemplate<RSSFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			/// RSS feed importer.
			/// @ingroup m61
			/// @author Hugues Romain
			class Importer_:
				public impex::OneFileTypeImporter<RSSFileFormat>
			{
			protected:
				static const std::string PARAMETER_URL;
				static const std::string PARAMETER_RECIPIENT_KEY;
				static const std::string PARAMETER_RECIPIENT_ID;

				std::string _url;
				std::string _recipientKey;
				boost::shared_ptr<util::Registrable> _recipient;

				mutable std::set<util::RegistryKeyType> _scenariosToRemove;
				mutable std::set<util::RegistryKeyType> _messagesToRemove;

				virtual bool _parse(
					const boost::filesystem::path& filePath,
					std::ostream& os,
					boost::optional<const server::Request&> adminRequest
				) const;

				struct Item
				{
					std::string guid;
					std::string title;
					std::string content;
					boost::posix_time::ptime startDate;
				};
				typedef std::vector<Item> Items;


			public:
				Importer_(
					util::Env& env,
					const impex::DataSource& dataSource
				);



				//////////////////////////////////////////////////////////////////////////
				/// Import screen to include in the administration console.
				/// @param os stream to write the result on
				/// @param request request for display of the administration console
				/// @since 3.5.0
				/// @date 2012
				virtual void displayAdmin(
					std::ostream& os,
					const server::Request& request
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

			typedef impex::NoExportPolicy<RSSFileFormat> Exporter_;
		};
}	}

#endif
