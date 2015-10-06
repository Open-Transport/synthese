
/** TimetableRowGroup class implementation.
	@file TimetableRowGroup.cpp

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

#include "TimetableRowGroup.hpp"

#include "Profile.h"
#include "RankUpdateQuery.hpp"
#include "SchemaMacros.hpp"
#include "TimetableRight.h"
#include "TimetableRowGroupTableSync.hpp"
#include "TimetableRowGroupItem.hpp"
#include "User.h"

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace timetables;

	CLASS_DEFINITION(TimetableRowGroup, "t090_timetable_rowgroups", 90)
	FIELD_DEFINITION_OF_OBJECT(TimetableRowGroup, "row_group_id", "row_group_ids")

	FIELD_DEFINITION_OF_TYPE(IsDeparture, "is_departure", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(IsArrival, "is_arrival", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(Display, "display", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(TimetableRowRule, "rule", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(AutoRowsOrder, "auto_rows_order", SQL_BOOLEAN)
	
	namespace timetables
	{
		TimetableRowGroup::TimetableRowGroup(
			RegistryKeyType id /*= 0 */
		):	Registrable(id),
			Object<TimetableRowGroup, TimetableRowGroupRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Timetable),
					FIELD_VALUE_CONSTRUCTOR(Rank, 0),
					FIELD_VALUE_CONSTRUCTOR(IsDeparture, true),
					FIELD_VALUE_CONSTRUCTOR(IsArrival, true),
					FIELD_VALUE_CONSTRUCTOR(Display, true),
					FIELD_VALUE_CONSTRUCTOR(TimetableRowRule, NeutralRow),
					FIELD_VALUE_CONSTRUCTOR(AutoRowsOrder, false)
			)	)
		{}



		bool TimetableRowGroup::contains( const pt::StopArea& stopArea ) const
		{
			BOOST_FOREACH(const Items::value_type& item, _items)
			{
				if(	item->get<StopArea>() &&
					&(*item->get<StopArea>()) == &stopArea)
				{
					return true;
				}
			}
			return false;
		}



		void TimetableRowGroup::addItem(
			const TimetableRowGroupItem& item
		){
			_items.insert(&item);
		}



		void TimetableRowGroup::removeItem(
			const TimetableRowGroupItem& item
		){
			_items.erase(&item);
		}



		void TimetableRowGroup::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			if(get<Timetable>())
			{
				get<Timetable>()->addRowGroup(*this);
			}
		}



		void TimetableRowGroup::unlink()
		{
			if(get<Timetable>())
			{
				get<Timetable>()->removeRowGroup(*this);
			}
		}



		void TimetableRowGroup::afterDelete( boost::optional<db::DBTransaction&> transaction ) const
		{
			if(!get<Timetable>())
			{
				return;
			}

			RankUpdateQuery<TimetableRowGroupTableSync> query(Rank::FIELD.name, -1, get<Rank>());
			query.addWhereField(Timetable::FIELD.name, get<Timetable>()->getKey());
			query.execute(transaction);
		}



		void TimetableRowGroup::beforeCreate( boost::optional<db::DBTransaction&> transaction ) const
		{
			if(!get<Timetable>())
			{
				return;
			}

			RankUpdateQuery<TimetableRowGroupTableSync> query(Rank::FIELD.name, 1, get<Rank>());
			query.addWhereField(Timetable::FIELD.name, get<Timetable>()->getKey());
			query.execute(transaction);
		}



		bool TimetableRowGroup::ItemsSort::operator()(
			const TimetableRowGroupItem* it1,
			const TimetableRowGroupItem* it2
		) const	{

			assert(it1);
			assert(it2);

			if(	it1 && it2 &&
				it1->get<Rank>() != it2->get<Rank>()
			){
				return it1->get<Rank>() < it2->get<Rank>();
			}
			
			return it1 < it2;
		}


		bool TimetableRowGroup::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TimetableRight>(security::WRITE);
		}

		bool TimetableRowGroup::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TimetableRight>(security::WRITE);
		}

		bool TimetableRowGroup::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TimetableRight>(security::DELETE_RIGHT);
		}
}	}
