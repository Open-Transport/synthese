
/** TimetableRow class implementation.
	@file TimetableRow.cpp

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

#include "TimetableRow.h"

#include "Profile.h"
#include "Session.h"
#include "StopArea.hpp"
#include "TimetableRight.h"
#include "User.h"

using namespace std;

namespace synthese
{
	using namespace timetables;
	using namespace util;

	CLASS_DEFINITION(TimetableRow, "t053_timetable_rows", 53)
	FIELD_DEFINITION_OF_OBJECT(TimetableRow, "timetable_row_id", "timetable_row_ids")

	FIELD_DEFINITION_OF_TYPE(TimetableParent, "timetable_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(TimetableStopArea, "place_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(IsDeparture, "is_departure", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(IsArrival, "is_arrival", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(Compulsory, "is_compulsory", SQL_INTEGER)

	namespace timetables
	{
		TimetableRow::TimetableRow(
			RegistryKeyType id
		):	Registrable(id),
			Object<TimetableRow, TimetableRowSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_VALUE_CONSTRUCTOR(TimetableParent, 0),
					FIELD_VALUE_CONSTRUCTOR(Rank, 0),
					FIELD_DEFAULT_CONSTRUCTOR(TimetableStopArea),
					FIELD_VALUE_CONSTRUCTOR(IsDeparture, true),
					FIELD_VALUE_CONSTRUCTOR(IsArrival, true),
					FIELD_VALUE_CONSTRUCTOR(Compulsory, PassageFacultatif)
			)	)
		{
		}

		const pt::StopArea* TimetableRow::getPlace() const
		{
			if (get<TimetableStopArea>())
			{
				return get<TimetableStopArea>().get_ptr();
			}

			return NULL;
		}

		void TimetableRow::setPlace(const pt::StopArea* place)
		{
			set<TimetableStopArea>(place
				? boost::optional<pt::StopArea&>(*const_cast<pt::StopArea*>(place))
				: boost::none);
		}

		bool TimetableRow::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TimetableRight>(security::WRITE);
		}

		bool TimetableRow::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TimetableRight>(security::WRITE);
		}

		bool TimetableRow::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TimetableRight>(security::DELETE_RIGHT);
		}
}	}
