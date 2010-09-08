
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
#include <map>
#include <boost/date_time/posix_time/ptime.hpp>

#include "UtilTypes.h"
#include "DeparturesTableTypes.h"

namespace synthese
{
	namespace pt
	{
		class JourneyPattern;
	}

	namespace pt
	{
		class StopPoint;
	}

	namespace departure_boards
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

			typedef std::map<util::RegistryKeyType, const pt::StopPoint*> PhysicalStops;
		private:

			void _push_back(
				ActualDisplayedArrivalsList& list,
				const graph::ServicePointer& serviceUse
			);

		protected:
			DisplayedPlacesList _displayedPlaces;	//!< Places to be displayed according to the demand rules

			//! \name Parameters
			//@{
				const PhysicalStops				_physicalStops;
				const DeparturesTableDirection	_direction;
				const EndFilter					_endFilter;
				const LineFilter				_lineFilter;
				const ForbiddenPlacesList		_forbiddenPlaces;
				const TransferDestinationsList	_transferDestinations;
				const boost::posix_time::ptime	_startDateTime;
				const boost::posix_time::ptime	_endDateTime;
				const size_t					_maxSize;	//!< Maximal size of the departure table according to the demand rules.
			//@}

			//!	\name Results
			//@{
				ArrivalDepartureList _result; //!< The result
			//@}

			
			/** Autorisation d'afficher une ligne sur le tableau de départs.
				@param linestop linestop to test
			*/
				bool _allowedLineStop(const pt::LineStop*) const;
			
			/** Insertion.
				@return iterator on the element.
			*/
				ArrivalDepartureList::iterator _insert(
					const graph::ServicePointer& servicePointer
					, UnlimitedSize unlimitedSize=SIZE_AS_DEFINED
				);

			/** Constructor.
				@param maxSize Maximal size of the departure table (default = unlimited).
			*/
			ArrivalDepartureTableGenerator(
				const PhysicalStops&
				, const DeparturesTableDirection&
				, const EndFilter&
				, const LineFilter&
				, const DisplayedPlacesList&
				, const ForbiddenPlacesList&,
				const TransferDestinationsList&
				, const boost::posix_time::ptime& startDateTime
				, const boost::posix_time::ptime& endDateTime
				, size_t maxSize = UNLIMITED_SIZE
			);

		public:
			
			virtual const ArrivalDepartureList& generate() = 0;
			virtual ~ArrivalDepartureTableGenerator() {}

		};

	}
}

#endif
