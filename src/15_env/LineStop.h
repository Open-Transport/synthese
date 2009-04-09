
/** LineStop class header.
	@file LineStop.h

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

#ifndef SYNTHESE_ENV_LINESTOP_H
#define SYNTHESE_ENV_LINESTOP_H

#include <string>

#include "Registry.h"
#include "Edge.h"

namespace synthese
{
	namespace env
	{
		class PhysicalStop;
		class Line;
		class PublicTransportStopZoneConnectionPlace;

		/** Association class between line and physical stop.
			The linestop is the implementation of the edge of a transport line.
			TRIDENT = PtLink
			@ingroup m35
		*/
		class LineStop
		:	public graph::Edge
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<LineStop>	Registry;

		private:
			bool	_scheduleInput;
			bool	_isDeparture;		//!< The departure from the vertex is allowed
			bool	_isArrival;			//!< The arrival at the vertex is allowed


		protected:
			virtual bool _isDepartureAllowed() const;
			virtual bool _isArrivalAllowed() const;

		public:

			LineStop(
				util::RegistryKeyType id = UNKNOWN_VALUE,
				const Line* line = NULL,
				int rankInPath = UNKNOWN_VALUE,
				bool isDeparture = true,
				bool isArrival = true,
				double metricOffset = UNKNOWN_VALUE,
				PhysicalStop* physicalStop = NULL
			);
			
			~LineStop();


			//! @name Getters
			//@{
				const PhysicalStop*	getPhysicalStop()	const;
				Line*				getLine()			const;
				bool				getScheduleInput()	const;
				bool				getIsDeparture()	const;
				bool				getIsArrival()		const;
			//@}
				
			//!	@name Setters
			//@{
				/** Physical stop setter.
					@param stop the physical stop supporting the linestop

					The physical stop setter builds the links from the physical stop to the linestop.

					@warning the isArrival and the isDeparture attributes must be up to date to avoid false links in the physical stop.
				*/
				void				setPhysicalStop(PhysicalStop* stop);

				void				setScheduleInput(bool value);
				void				setLine(const Line* line);
				void				setIsDeparture(bool value);
				void				setIsArrival(bool value);
			//@}


			//! @name Query methods
			//@{
				/*! Estimates consistency of line stops sequence according to 
					metric offsets and physical stops coordinates.
					@param other Other line stop to compare.
					@return true if data seems consistent, false otherwise.
				*/
				bool seemsGeographicallyConsistent (const LineStop& other) const;


			//@}


			//! @name Update methods
			//@{
		 
		    //@}
		};
	}
}

#endif
