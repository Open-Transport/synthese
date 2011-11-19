
/** MySQLDBModifiedAction class header.
	@file MySQLDBModifiedAction.hpp
	@author Sylvain Pasche

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

#ifndef SYNTHESE_db_mysql_MySQLDBModifiedAction_h__
#define SYNTHESE_db_mysql_MySQLDBModifiedAction_h__

#include "Action.h"
#include "ActionException.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace db
	{

		//////////////////////////////////////////////////////////////////////////
		/// MySQL database modification action. This is meant to be used by a trigger
		/// installed on the MySQL database used by synthese, which calls this action
		/// to notify about changes to the database.
		///
		/// @ingroup m54Actions refActions
		/// @author Sylvain Pasche
		/// @date 2011
		//////////////////////////////////////////////////////////////////////////
		class MySQLDBModifiedAction : public util::FactorableTemplate<server::Action, MySQLDBModifiedAction>
		{
		public:
			static const std::string PARAMETER_SECRET_TOKEN;
			static const std::string PARAMETER_TABLE;
			static const std::string PARAMETER_TYPE;
			static const std::string PARAMETER_ID;

		private:
			std::string _secretToken;
			std::string _table;
			std::string _type;
			util::RegistryKeyType _id;

		protected:
			////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			util::ParametersMap getParametersMap() const;

			////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			///	@throws ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map) throw(server::ActionException);

			virtual bool isAuthorized(const server::Session* session) const;

		public:
			////////////////////////////////////////////////////////////////////
			/// Runs the action.
			void run(
				server::Request& request
			) throw(server::ActionException);
		};
	}
}


#endif // SYNTHESE_db_mysql_MySQLDBModifiedAction_h__
