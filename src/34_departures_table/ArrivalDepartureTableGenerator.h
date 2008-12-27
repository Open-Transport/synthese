
/** ArrivalDepartureTableGenerator class header.
	@file ArrivalDepartureTableGenerator.h

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

#ifndef SYNTHESE_DEPARTURESTABLE_ARRIVALDEPARTURETABLEGENERATOR_H
#define SYNTHESE_DEPARTURESTABLE_ARRIVALDEPARTURETABLEGENERATOR_H

#include <set>

#include "15_env/Types.h"

#include "34_departures_table/Types.h"

namespace synthese
{
	namespace env
	{
		class Line;
	}

	namespace departurestable
	{

		/** Arrivals/Departures Table generator interface.
			@ingroup m54

			@todo the place will be removed, replaced with the physical stops list which is enough for reaching the linestops
		*/
		class ArrivalDepartureTableGenerator
		{
		public:
			typedef enum { FORCE_UNLIMITED_SIZE, SIZE_AS_DEFINED } UnlimitedSize;
			static const size_t UNLIMITED_SIZE = 0;

		private:

		protected:
			DisplayedPlacesList _displayedPlaces;	//!< Places to be displayed according to the demand rules

			//! \name Parameters
			//@{
				const env::PhysicalStops		_physicalStops;
				const DeparturesTableDirection	_direction;
				const EndFilter					_endFilter;
				const LineFilter				_lineFilter;
				const ForbiddenPlacesList		_forbiddenPlaces;
				const time::DateTime			_startDateTime;
				const time::DateTime			_endDateTime;
				const size_t					_maxSize;	//!< Maximal size of the departure table according to the demand rules.
				const int						_blinkingDelay;
				const time::DateTime			_calculationDateTime;
			//@}

			//!	\name Results
			//@{
				ArrivalDepartureList _result; //!< The result
			//@}

			
			/** Autorisation d'afficher une ligne sur le tableau de départs.
				@param linestop linestop to test
			*/
				bool _allowedLineStop(const env::LineStop*) const;
			
			/** Insertion.
				@return iterator on the element.
			*/
				ArrivalDepartureList::iterator _insert(
					const env::ServicePointer& servicePointer
					, UnlimitedSize unlimitedSize=SIZE_AS_DEFINED
					);

			/** Constructor.
				@param maxSize Maximal size of the departure table (default = unlimited).
			*/
			ArrivalDepartureTableGenerator(
				const env::PhysicalStops&
				, const DeparturesTableDirection&
				, const EndFilter&
				, const LineFilter&
				, const DisplayedPlacesList&
				, const ForbiddenPlacesList&
				, const time::DateTime& startDateTime
				, const time::DateTime& endDateTime
				, int blinkingDelay
				, size_t maxSize = UNLIMITED_SIZE
			);

		public:
			
			virtual const ArrivalDepartureList& generate() = 0;
			virtual ~ArrivalDepartureTableGenerator() {}

		};

	}
}

#endif
