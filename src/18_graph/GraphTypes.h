////////////////////////////////////////////////////////////////////////////////
/// GraphTypes class header.
///	@file GraphTypes.h
///	@author Hugues Romain (RCS)
///	@date dim jan 25 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_GRAPH_TYPES
#define SYNTHESE_GRAPH_TYPES

namespace synthese
{
	namespace graph
	{
		/** Service determination method.
			- DEPARTURE_TO_ARRIVAL = the service is chosen from a presence time before a departure.
				The arrival will be chosen in the following edges.
			- ARRIVAL_TO_DEPARTURE = the service is chosen from a presence time after an arrival.
				The departure will be chosen in the preceding edges.
			- UNDEFINED_DIRECTION = the direction is not defined, generally because no service is chosen
				at this stage
		*/
		typedef enum {
			ARRIVAL_TO_DEPARTURE,
			DEPARTURE_TO_ARRIVAL,
			UNDEFINED_DIRECTION
		} AccessDirection ;

		typedef unsigned int GraphIdType;

		typedef unsigned int UserClassCode;
		
		typedef unsigned int HubScore;
		
		const HubScore NO_TRANSFER_HUB_SCORE(0);
		const HubScore MIN_HUB_SCORE(1);
		const HubScore MAX_HUB_SCORE(100);
	}
}

#endif
