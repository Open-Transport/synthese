
////////////////////////////////////////////////////////////////////////////////
/// DbModuleConfigTableSync class header.
///	@file DbModuleConfigTableSync.h
///	@author Hugues Romain
///	@date 2008-12-14 23:54
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////


#ifndef SYNTHESE_DB_DBMODULECONFIGTABLESYNC_H
#define SYNTHESE_DB_DBMODULECONFIGTABLESYNC_H


#include <string>
#include <iostream>

#include "DBTableSyncTemplate.hpp"
#include "DBResult.hpp"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace db
	{
		class DB;
	}

	namespace server
	{
		/** DbModuleConfig table synchronizer.
			@ingroup m15LS refLS
		*/
		class DbModuleConfigTableSync:
			public db::DBTableSyncTemplate<DbModuleConfigTableSync>
		{
		 public:

			static const std::string COL_PARAMNAME;
			static const std::string COL_PARAMVALUE;

		
			virtual const std::string& getTableName() const;
			void rowsAdded(
				db::DB* db,
				const db::DBResultSPtr& rows
			) const;

			void rowsUpdated(
				db::DB* db,
				const db::DBResultSPtr& rows
			) const;

			void rowsRemoved(
				db::DB* db,
				const db::RowIdList& rowIds
			) const;


			static FieldsList GetFieldsList()
			{
				FieldsList l;
				for(size_t i(0); !_FIELDS[i].empty(); ++i)
				{
					l.push_back(_FIELDS[i]);
				}
				return l;
			}
			virtual FieldsList getFieldsList() const { return GetFieldsList(); }

		};
}	}

#endif
