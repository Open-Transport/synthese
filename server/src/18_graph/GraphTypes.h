////////////////////////////////////////////////////////////////////////////////
/// GraphTypes class header.
///	@file GraphTypes.h
///	@author Hugues Romain (RCS)
///	@date dim jan 25 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
		typedef unsigned int GraphIdType;

		typedef unsigned int UserClassCode;

		typedef unsigned int HubScore;

		typedef double MetricOffset;

		const HubScore NO_TRANSFER_HUB_SCORE(0);
		const HubScore MIN_HUB_SCORE(1);
		const HubScore MAX_HUB_SCORE(100);
	}
}

#endif
