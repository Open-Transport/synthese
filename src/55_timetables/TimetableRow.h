
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
#include "55_timetables/types.h"

namespace synthese
{
	namespace env
	{
		class PublicTransportStopZoneConnectionPlace;
	}

	namespace timetables
	{
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
		:	public virtual util::Registrable
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<TimetableRow>	Registry;
		
		private:
			// Variables
			const env::PublicTransportStopZoneConnectionPlace*	_place;
			bool												_isDeparture;
			bool												_isArrival;
			tTypeGareIndicateur									_compulsory;
			int													_rank;
			uid													_timetableId;

		public:
			// Constructor
			TimetableRow(
				util::RegistryKeyType id = UNKNOWN_VALUE
			);

			//! @name Getters
			//@{
				const env::PublicTransportStopZoneConnectionPlace*	getPlace()			const;
				tTypeGareIndicateur 								getCompulsory()		const;
				int													getRank()			const;
				uid													getTimetableId()	const;
				bool												getIsArrival()		const;
				bool												getIsDeparture()	const;
			//@}

			//! @name Setters
			//@{
				void setRank(int rank);
				void setPlace(const env::PublicTransportStopZoneConnectionPlace* place);
				void setCompulsory(tTypeGareIndicateur compulsory);
				void setTimetableId(uid id);
				void setIsArrival(bool value);
				void setIsDeparture(bool value);
			//@}
		};
	}
}

#endif // SYNTHESE_timetables_TimetableRow_h__
