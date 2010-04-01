
/** TimetableRow class header.
	@file TimetableRow.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "Registrable.h"
#include "UId.h"
#include "Registry.h"

namespace synthese
{
	namespace env
	{
		class PublicTransportStopZoneConnectionPlace;
	}

	namespace timetables
	{
		/** Time table row class.

			caract�ris�e par:
				- un point d'arr�t
				- un type de dessete (Depart/Arrivee/Passage)
				- un caractere obligatoire ou facultatif (pour la selection des lignes du tableau)

			@author Hugues Romain
			@date 2001
			@ingroup m55
		*/
		class TimetableRow
		:	public virtual util::Registrable
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<TimetableRow>	Registry;
		
			enum tTypeGareIndicateur
			{
				PassageFacultatif = -1
				, PassageObligatoire = 0
				, PassageSuffisant = 1
				, CompulsoryHidden = 2
			};

		private:
			// Variables
			const env::PublicTransportStopZoneConnectionPlace*	_place;
			bool												_isDeparture;
			bool												_isArrival;
			tTypeGareIndicateur									_compulsory;
			std::size_t											_rank;
			uid													_timetableId;

		public:
			// Constructor
			TimetableRow(
				util::RegistryKeyType id = UNKNOWN_VALUE
			);

			//! @name Getters
			//@{
				const env::PublicTransportStopZoneConnectionPlace*	getPlace()			const { return _place; }
				tTypeGareIndicateur 								getCompulsory()		const { return _compulsory; }
				std::size_t											getRank()			const { return _rank; }
				uid													getTimetableId()	const { return _timetableId; }
				bool												getIsArrival()		const { return _isArrival; }
				bool												getIsDeparture()	const { return _isDeparture; }
			//@}

			//! @name Setters
			//@{
				void setRank(std::size_t value) { _rank = value; }
				void setPlace(const env::PublicTransportStopZoneConnectionPlace* place) { _place = place; }
				void setCompulsory(tTypeGareIndicateur compulsory) { _compulsory = compulsory; }
				void setTimetableId(uid id) { _timetableId = id; }
				void setIsArrival(bool value) { _isArrival = value; }
				void setIsDeparture(bool value) { _isDeparture = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_timetables_TimetableRow_h__
