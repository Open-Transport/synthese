
/** TimetableRowGroup class header.
	@file TimetableRowGroup.hpp

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

#ifndef SYNTHESE_timetable_TimetableRowGroup_hpp__
#define SYNTHESE_timetable_TimetableRowGroup_hpp__

#include "NumericField.hpp"
#include "EnumObjectField.hpp"
#include "Object.hpp"
#include "Registrable.h"
#include "Registry.h"

#include "SchemaMacros.hpp"
#include "Timetable.h"

namespace synthese
{
	namespace timetables
	{
		class TimetableRowGroupItem;

		enum TimetableRowRuleEnum
		{
			NeutralRow = 0,
			NecessaryRow = 1,
			SufficientRow = 2,
			FirstIsForbidden = 3
		};

		FIELD_BOOL(Display)
		FIELD_ENUM(TimetableRowRule, TimetableRowRuleEnum)
		FIELD_BOOL(AutoRowsOrder)
		

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Timetable),
			FIELD(Rank),
			FIELD(IsDeparture),
			FIELD(IsArrival),
			FIELD(Display),
			FIELD(TimetableRowRule),
			FIELD(AutoRowsOrder)
		> TimetableRowGroupRecord;



		/** TimetableRowGroup class.
			@ingroup m55
		*/
		class TimetableRowGroup:
			public Object<TimetableRowGroup, TimetableRowGroupRecord>
		{
		public:
			typedef util::Registry<TimetableRowGroup>	Registry;

			TimetableRowGroup(
				util::RegistryKeyType id = 0
			);

			struct ItemsSort
			{
				bool operator()(
					const TimetableRowGroupItem* it1,
					const TimetableRowGroupItem* it2
				) const;
			};

			typedef std::set<
				const TimetableRowGroupItem*,
				ItemsSort
			> Items;

		private:
			Items _items;

		public:
			void setItems(const Items& value){ _items = value; }
			const Items& getItems() const { return _items; }

			/// @name Services
			//@{
				bool contains(const pt::StopArea& stopArea) const;
			//@}

			/// @name Modifiers
			//@{
				void addItem(const TimetableRowGroupItem& item);
				void removeItem(const TimetableRowGroupItem& item);
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
				virtual void beforeCreate(boost::optional<db::DBTransaction&> transaction) const;
				virtual void afterDelete(boost::optional<db::DBTransaction&> transaction) const;
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_timetable_TimetableRowGroup_hpp__

