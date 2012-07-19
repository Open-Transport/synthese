
/** TimetableRowGroupItem class implementation.
	@file TimetableRowGroupItem.cpp

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

#include "TimetableRowGroupItem.hpp"

#include "TimetableRowGroupItemTableSync.hpp"
#include "RankUpdateQuery.hpp"

namespace synthese
{
	using namespace db;
	using namespace pt;
	using namespace timetables;
	using namespace util;

	CLASS_DEFINITION(TimetableRowGroupItem, "t091_timetable_rowgroup_items", 91)

	namespace timetables
	{
		TimetableRowGroupItem::TimetableRowGroupItem(
			RegistryKeyType id /*= 0 */
		):	Registrable(id),
			Object<TimetableRowGroupItem, TimetableRowGroupItemRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(TimetableRowGroup),
					FIELD_VALUE_CONSTRUCTOR(Rank, 0),
					FIELD_DEFAULT_CONSTRUCTOR(StopArea),
					FIELD_DEFAULT_CONSTRUCTOR(Title)
			)	)
		{}



		void TimetableRowGroupItem::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(get<TimetableRowGroup>())
			{
				get<TimetableRowGroup>()->addItem(*this);
			}
		}



		void TimetableRowGroupItem::unlink()
		{
			if(get<TimetableRowGroup>())
			{
				get<TimetableRowGroup>()->removeItem(*this);
			}
		}



		void TimetableRowGroupItem::beforeCreate( boost::optional<db::DBTransaction&> transaction ) const
		{
			if(!get<TimetableRowGroup>())
			{
				return;
			}

			RankUpdateQuery<TimetableRowGroupItemTableSync> query(Rank::FIELD.name, 1, get<Rank>());
			query.addWhereField(TimetableRowGroup::FIELD.name, get<TimetableRowGroup>()->getKey());
			query.execute(transaction);
		}



		void TimetableRowGroupItem::afterDelete( boost::optional<db::DBTransaction&> transaction ) const
		{
			if(!get<TimetableRowGroup>())
			{
				return;
			}

			RankUpdateQuery<TimetableRowGroupItemTableSync> query(Rank::FIELD.name, -1, get<Rank>());
			query.addWhereField(TimetableRowGroup::FIELD.name, get<TimetableRowGroup>()->getKey());
			query.execute(transaction);
		}
}	}

