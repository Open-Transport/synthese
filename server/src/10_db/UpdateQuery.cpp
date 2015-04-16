
/** UpdateQuery class implementation.
	@file UpdateQuery.cpp
	@author Sylvain Pasche
	@date 2011

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

#include "10_db/UpdateQuery.hpp"
#include "DBModule.h"
#include "DBTransaction.hpp"
#include "DB.hpp"

namespace synthese
{
	namespace db
	{
		std::string DynamicUpdateQuery::_whereQueryPart() const
		{
			std::stringstream s;

			// Where
			if(!_wheres.empty())
			{
				s << " WHERE ";
				bool first(true);
				BOOST_FOREACH(const WheresType::value_type& where, _wheres)
				{
					if(!first)
					{
						s << " AND ";
					}
					s << where->toString();
					first = false;
				}
			}

			return s.str();
		}



		void DynamicUpdateQuery::execute(boost::optional<DBTransaction&> transaction) const
		{
			// TODO: this should run inside of a transaction, an id could appear or disappear between the SELECT and the UPDATE.

			DB* db = DBModule::GetDB();

			std::string selectQuery("SELECT " + TABLE_COL_ID + " FROM " + _table + _whereQueryPart());
			DBResultSPtr rows = db->execQuery(selectQuery);

			// TODO: call rowUpdated directly from this loop once checkModificationEvents() infrastructure is removed.
			std::vector<util::RegistryKeyType> updatedIds;
			while (rows->next())
			{
				updatedIds.push_back(rows->getLongLong(TABLE_COL_ID));
			}

			std::stringstream updateQuery;
			updateQuery << "UPDATE " << _table << " SET ";

			bool first(true);
			BOOST_FOREACH(const UpdateType& update, _updates)
			{
				if(!first)
				{
					updateQuery << ",";
				}
				updateQuery << update.first << "=" << update.second->toString();
				first = false;
			}

			updateQuery << _whereQueryPart() << ";";

			db->execUpdate(updateQuery.str(), transaction);

			BOOST_FOREACH(util::RegistryKeyType updatedId, updatedIds)
			{
				db->addDBModifEvent(
					DB::DBModifEvent(
						_table,
						DB::MODIF_UPDATE,
						updatedId
					),
					transaction
				);
			}
#ifdef DO_VERIFY_TRIGGER_EVENTS
			db->checkModificationEvents();
#endif
		}

}	}
