
/** Importer class header.
	@file Importer.hpp

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

#ifndef SYNTHESE_impex_Importer_hpp__
#define SYNTHESE_impex_Importer_hpp__

#include "Env.h"
#include "ParametersMap.h"
#include "SQLiteTransaction.h"
#include "AdminInterfaceElement.h"

#include <ostream>
#include <boost/optional.hpp>

namespace synthese
{
	namespace impex
	{
		class DataSource;

		/** Importer class.
			@ingroup m16
		*/
		class Importer
		{
		public:
			Importer(
				util::Env& env,
				const DataSource& dataSource
			):	_env(env),
				_dataSource(dataSource)
			{}
			
		protected:
			mutable util::Env&						_env;
			const DataSource&						_dataSource;

			virtual db::SQLiteTransaction _save() const = 0;

		public:
			//! @name Getters
			//@{
				const DataSource& getDataSource() const { return _dataSource; }
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// @param map Parameters map to interpret
			/// @param doImport tests if the parameters are valid for an import
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			virtual void setFromParametersMap(
				const server::ParametersMap& map,
				bool doImport
			) = 0;

			virtual server::ParametersMap getParametersMap() const = 0;


			//////////////////////////////////////////////////////////////////////////
			/// Launches the parsing of the files, with output on an admin page.
			/// @param os output stream
			/// @param request admin
			/// @return true if it is allowed to save the data
			virtual bool parseFiles(
				std::ostream& os,
				boost::optional<const admin::AdminRequest&> request
			) const = 0;



			//////////////////////////////////////////////////////////////////////////
			/// Interface for the save method.
			/// @return transaction to run
			db::SQLiteTransaction save() const
			{
				db::SQLiteTransaction result(_save());
				_env.clear();
				return result;
			}

			//////////////////////////////////////////////////////////////////////////
			/// Import screen to include in the administration console.
			/// @param os stream to write the result on
			/// @param request request for display of the administration console
			/// @since 3.2.0
			/// @date 2010
			virtual void displayAdmin(
				std::ostream& os,
				const admin::AdminRequest& request
			) const = 0;
		};
}	}

#endif // SYNTHESE_impex_Importer_hpp__
