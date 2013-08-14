
/** LineStop class header.
	@file LineStop.h

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

#ifndef SYNTHESE_ENV_LINESTOP_H
#define SYNTHESE_ENV_LINESTOP_H

#include <string>

#include "Registry.h"
#include "Edge.h"

namespace synthese
{
	namespace pt
	{
		class StopPoint;
	}

	namespace pt
	{
		class JourneyPattern;
		class StopArea;

		/** Association class between line and physical stop.
			The linestop is the implementation of the edge of a transport line.
			TRIDENT = PtLink
			@ingroup m35
		*/
		class LineStop:
			public graph::Edge
		{
		public:

		private:
			bool	_isDeparture;		//!< The departure from the vertex is allowed
			bool	_isArrival;			//!< The arrival at the vertex is allowed


		public:
			/// Chosen registry class.
			typedef util::Registry<LineStop> Registry;

			LineStop(
				util::RegistryKeyType id = 0,
				JourneyPattern* line = NULL,
				std::size_t rankInPath = 0,
				bool isDeparture = true,
				bool isArrival = true,
				double metricOffset = 0,
				graph::Vertex* vertex = NULL
			);

			~LineStop();


			//! @name Getters
			//@{
				bool getIsDeparture()	const { return _isDeparture; }
				bool getIsArrival()		const { return _isArrival; }
			//@}

			//!	@name Setters
			//@{
				void				setIsDeparture(bool value) { _isDeparture = value; }
				void				setIsArrival(bool value) { _isArrival = value; }
			//@}


			//! @name Services
			//@{
				/*! Estimates consistency of line stops sequence according to
					metric offsets and physical stops coordinates.
					@param other Other line stop to compare.
					@return true if data seems consistent, false otherwise.
				*/
				bool seemsGeographicallyConsistent (const LineStop& other) const;

				virtual bool isDepartureAllowed() const;
				virtual bool isArrivalAllowed() const;

				virtual bool getScheduleInput() const = 0;
				virtual bool getReservationNeeded() const { return true; }

				JourneyPattern*				getLine()			const;
			//@}


			//! @name Modifiers
			//@{
				void setLine(JourneyPattern* line);

				
				
				virtual bool loadFromRecord(
					const Record& record,
					util::Env& env
				);
			//@}
		};
}	}

#endif
