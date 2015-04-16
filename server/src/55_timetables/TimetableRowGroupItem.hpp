
/** TimetableRowGroupItem class header.
	@file TimetableRowGroupItem.hpp

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

#ifndef SYNTHESE_timetable_TimetableRowGroupItem_hpp__
#define SYNTHESE_timetable_TimetableRowGroupItem_hpp__

#include "NumericField.hpp"
#include "Object.hpp"
#include "PointerField.hpp"
#include "Registrable.h"
#include "Registry.h"
#include "StringField.hpp"

#include "StopArea.hpp"
#include "TimetableRowGroup.hpp"

namespace synthese
{
	namespace timetables
	{
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(TimetableRowGroup),
			FIELD(Rank),
			FIELD(pt::StopArea),
			FIELD(Title)
		> TimetableRowGroupItemRecord;

		/** TimetableRowGroupItem class.
			@ingroup m55
		*/
		class TimetableRowGroupItem:
			public Object<TimetableRowGroupItem, TimetableRowGroupItemRecord>
		{
		public:
			typedef util::Registry<TimetableRowGroupItem>	Registry;

			TimetableRowGroupItem(
				util::RegistryKeyType id = 0
			);

			virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
			virtual void unlink();
			virtual void beforeCreate(boost::optional<db::DBTransaction&> transaction) const;
			virtual void afterDelete(boost::optional<db::DBTransaction&> transaction) const;
		};
}	}

#endif // SYNTHESE_timetable_TimetableRowGroupItem_hpp__
