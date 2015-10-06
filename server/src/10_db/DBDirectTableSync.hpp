
/** DBDirectTableSync class header.
	@file DBDirectTableSync.hpp

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

#ifndef SYNTHESE_db_DBDirectTableSync_hpp__
#define SYNTHESE_db_DBDirectTableSync_hpp__

#include "DBTypes.h"
#include "DBResult.hpp"
#include "Registrable.h"
#include "Session.h"

namespace synthese
{
	namespace util
	{
		class Env;
		class RegistryBase;
	}

	namespace db
	{
		class DBTransaction;

		/** DBDirectTableSync class.
			@ingroup m10
		*/
		class DBDirectTableSync
		{
		protected:
			DBDirectTableSync(){}

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Checks if the table contains an element with the specified id.
			/// @param key the id of the element
			/// @return true if the table contains the specified element
			/// @author Hugues Romain
			/// @date 2013
			virtual bool contains(
				util::RegistryKeyType key
			) const = 0;



			virtual boost::shared_ptr<const util::Registrable> getRegistrable(
				util::RegistryKeyType key,
				util::Env& environment,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			) const = 0;



			virtual boost::shared_ptr<util::Registrable> getEditableRegistrable(
				util::RegistryKeyType key,
				util::Env& environment,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			) const = 0;



			virtual boost::shared_ptr<util::Registrable> createRegistrable(
				const DBResultSPtr& row
			) const = 0;



			virtual boost::shared_ptr<util::Registrable> newObject(
			) const = 0;



			virtual boost::shared_ptr<util::Registrable> newObject(
				const Record& record
			) const = 0;



			virtual void saveRegistrable(
				util::Registrable& obj,
				boost::optional<DBTransaction&> transaction = boost::optional<DBTransaction&>()
			) const = 0;



			virtual void loadRegistrable(
				util::Registrable& obj,
				const DBResultSPtr& rows,
				util::Env& environment,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			) const = 0;


			typedef std::vector<boost::shared_ptr<util::Registrable> > RegistrableSearchResult;

			virtual RegistrableSearchResult search(
				const std::string& whereClause,
				util::Env& environment,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			) const = 0;



			virtual DBResultSPtr searchRecords(
				const std::string& whereClause
			) const = 0;
			
			
			virtual std::string whereClauseDefault(
			) const { return std::string(); }


			virtual bool allowList( const server::Session* session
			) const { return false; }



			virtual const util::RegistryBase& getRegistry(
				const util::Env& env
			) const = 0;


			virtual util::RegistryBase& getEditableRegistry(
				util::Env& env
			) const = 0;
		};
	}
}

#endif // SYNTHESE_db_DBDirectTableSync_hpp__

