
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
	namespace pt
	{
		class PhysicalStop;
	}

	namespace pt
	{
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


		public:

			LineStop(
				util::RegistryKeyType id = 0,
				Line* line = NULL,
				std::size_t rankInPath = 0,
				bool isDeparture = true,
				bool isArrival = true,
				double metricOffset = 0,
				pt::PhysicalStop* physicalStop = NULL
			);
			
			~LineStop();


			//! @name Getters
			//@{
				bool getScheduleInput()	const { return _scheduleInput; }
				bool getIsDeparture()	const { return _isDeparture; }
				bool getIsArrival()		const { return _isArrival; }
			//@}
				
			//!	@name Setters
			//@{
				void				setScheduleInput(bool value) { _scheduleInput = value; }
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

				PhysicalStop*	getPhysicalStop()	const;

				Line*				getLine()			const;
			//@}


			//! @name Modifiers
			//@{
				//////////////////////////////////////////////////////////////////////////
 				/// Physical stop modifier.
				///	@param stop the physical stop supporting the linestop
				///	The physical stop setter builds the links from the physical stop to the linestop.
				///	Use this method instead of setFromVertex.
				///
				///	@warning the isArrival and the isDeparture attributes must be up to date to avoid false links in the physical stop.
				void setPhysicalStop(PhysicalStop* stop);

				void setLine(Line* line);
		    //@}
		};
	}
}

#endif
