
/** DatabaseReadImporter class header.
	@file DatabaseReadImporter.hpp

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

#ifndef SYNTHESE_impex_DatabaseReadImporter_hpp__
#define SYNTHESE_impex_DatabaseReadImporter_hpp__

#include "Importer.hpp"

namespace synthese
{
	namespace impex
	{
		/** DatabaseReadImporter class.
			@ingroup m16
		*/
		template<class FF>
		class DatabaseReadImporter:
			public Importer
		{
		public:
			static const bool IMPORTABLE;	//!< Name of the database to read at the import
			static const std::string PARAMETER_DATABASE;	//!< Name of the database to read at the import

			DatabaseReadImporter(
				util::Env& env,
				const DataSource& dataSource
			):	Importer(env, dataSource)
			{}

		protected:
			std::string _database;

			virtual bool _read(
				std::ostream& os,
				boost::optional<const admin::AdminRequest&> request
			) const = 0;

			virtual void _setFromParametersMap(const server::ParametersMap& map) {}

			virtual server::ParametersMap _getParametersMap() const { return server::ParametersMap(); }

			void setFromParametersMap(
				const server::ParametersMap& map,
				bool doImport
			){
				_database = map.getDefault<std::string>(PARAMETER_DATABASE);
				_setFromParametersMap(map);
			}


			server::ParametersMap getParametersMap() const
			{
				server::ParametersMap result(_getParametersMap());
				result.insert(PARAMETER_DATABASE, _database);
				return result;
			}


			bool parseFiles(
				std::ostream& os,
				boost::optional<const admin::AdminRequest&> request
			) const {
				bool result(true);
				result &= _read(os, request);
				return result;
			}
		};

		template<class FF>
		const bool DatabaseReadImporter<FF>::IMPORTABLE(true);

		template<class FF>
		const std::string DatabaseReadImporter<FF>::PARAMETER_DATABASE("db");
	}
}

#endif // SYNTHESE_impex_DatabaseReadImporter_hpp__
