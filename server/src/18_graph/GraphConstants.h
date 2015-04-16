
/** GraphConstants class header.
	@file GraphConstants.h

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

#ifndef SYNTHESE_graph_GraphConstants_h__
#define SYNTHESE_graph_GraphConstants_h__

#include "GraphTypes.h"

#include <cstddef>

namespace synthese
{
	namespace graph
	{
		static const graph::UserClassCode USER_CLASS_CODE_OFFSET(35000);
		static const graph::UserClassCode USER_PEDESTRIAN(35001);
		static const graph::UserClassCode USER_HANDICAPPED(35002);
		static const graph::UserClassCode USER_BIKE(35003);
		static const graph::UserClassCode USER_CAR(35004);
		static const std::size_t USER_CLASSES_VECTOR_SIZE(5);
	}
}

#endif // SYNTHESE_graph_GraphConstants_h__
