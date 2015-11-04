
/** TimetableRow class header.
	@file TimetableRow.h

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

#ifndef SYNTHESE_timetables_TimetableRow_h__
#define SYNTHESE_timetables_TimetableRow_h__

#include "Object.hpp"

#include "EnumObjectField.hpp"

namespace synthese
{
	namespace pt
	{
		class StopArea;
	}

	namespace timetables
	{
		enum tTypeGareIndicateur
		{
			PassageFacultatif = -1
			, PassageObligatoire = 0
			, PassageSuffisant = 1
			, CompulsoryHidden = 2
		};

		FIELD_ID(TimetableParent)
		FIELD_POINTER(TimetableStopArea, pt::StopArea)
		FIELD_BOOL(IsDeparture)
		FIELD_BOOL(IsArrival)
		FIELD_ENUM(Compulsory, tTypeGareIndicateur)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(TimetableParent),
			FIELD(Rank),
			FIELD(TimetableStopArea),
			FIELD(IsDeparture),
			FIELD(IsArrival),
			FIELD(Compulsory)
		> TimetableRowSchema;

		/** Time table row class.

			caractérisée par:
				- un point d'arrêt
				- un type de dessete (Depart/Arrivee/Passage)
				- un caractere obligatoire ou facultatif (pour la selection des lignes du tableau)

			@author Hugues Romain
			@date 2001
			@ingroup m55
		*/
		class TimetableRow
		:	public Object<TimetableRow, TimetableRowSchema>
		{
		public:
			// Constructor
			TimetableRow(
				util::RegistryKeyType id = 0
			);

			//! @name Getters
			//@{
				const pt::StopArea*									getPlace()			const;
				tTypeGareIndicateur 								getCompulsory()		const { return get<Compulsory>(); }
				std::size_t											getRank()			const { return get<Rank>(); }
				util::RegistryKeyType								getTimetableId()	const { return get<TimetableParent>(); }
				bool												getIsArrival()		const { return get<IsArrival>(); }
				bool												getIsDeparture()	const { return get<IsDeparture>(); }
			//@}

			//! @name Setters
			//@{
				void setRank(std::size_t value) { set<Rank>(value); }
				void setPlace(const pt::StopArea* place);
				void setCompulsory(tTypeGareIndicateur compulsory) { set<Compulsory>(compulsory); }
				void setTimetableId(util::RegistryKeyType id) { set<TimetableParent>(id); }
				void setIsArrival(bool value) { set<IsArrival>(value); }
				void setIsDeparture(bool value) { set<IsDeparture>(value); }
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_timetables_TimetableRow_h__
