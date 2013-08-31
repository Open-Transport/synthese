
/** LinePhysicalStop class header.
	@file LinePhysicalStop.hpp

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

#ifndef SYNTHESE_pt_LinePhysicalStop_hpp__
#define SYNTHESE_pt_LinePhysicalStop_hpp__

#include "LineStop.h"

namespace synthese
{
	namespace pt
	{
		/** LinePhysicalStop class.
			@ingroup m35
		*/
		class LinePhysicalStop:
			public LineStop
		{
		private:

		public:
			LinePhysicalStop(
				util::RegistryKeyType id = 0,
				JourneyPattern* line = NULL,
				std::size_t rankInPath = 0,
				bool isDeparture = true,
				bool isArrival = true,
				double metricOffset = 0,
				StopPoint* stop = NULL
			);

			//! @name Getters
			//@{
				StopPoint*	getPhysicalStop()	const;
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
				void setPhysicalStop(StopPoint& stop);


				//////////////////////////////////////////////////////////////////////////
				/// Clears all the links between the linestop and the stop.
				void clearPhysicalStopLinks();
			//@}
		};
}	}

#endif // SYNTHESE_pt_LinePhysicalStop_hpp__
