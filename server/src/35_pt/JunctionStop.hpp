
/** JunctionStop class header.
	@file JunctionStop.hpp

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

#ifndef SYNTHESE_pt_JunctionStop_hpp__
#define SYNTHESE_pt_JunctionStop_hpp__

#include "Edge.h"

namespace synthese
{
	namespace pt
	{
		class Junction;
		class StopPoint;

		/** Edge of a junction.
			@ingroup m35
			@author Hugues Romain
			@date 2010
			@since 3.1.16
		*/
		class JunctionStop:
			public graph::Edge
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<JunctionStop>	Registry;

		public:

			//////////////////////////////////////////////////////////////////////////
			/// Start junction constructor.
			/// @param parentPath the junction
			/// @param fromVertex the physical stop
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			///
			/// The start junction constructor is also the default constructor, but should not be used as it
			JunctionStop(
				Junction* parentPath = NULL,
				StopPoint* fromVertex = NULL
			);

			//////////////////////////////////////////////////////////////////////////
			/// End junction constructor.
			/// @param length length of the junction
			/// @param parentPath the junction
			/// @param fromVertex the physical stop
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			JunctionStop(
				double length,
				Junction* parentPath,
				StopPoint* fromVertex
			);

			//! @name Services
			//@{
				virtual bool isDepartureAllowed() const;
				virtual bool isArrivalAllowed() const;
			//@}
		};
	}
}

#endif // SYNTHESE_pt_JunctionStop_hpp__
