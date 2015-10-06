
/** StopPointWFSType class header.
	@file StopPointWFSType.hpp

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

#ifndef SYNTHESE_pt_StopPointWFSType_hpp__
#define SYNTHESE_pt_StopPointWFSType_hpp__

#include "WFSTypeTemplate.hpp"

namespace synthese
{
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// Stop point WFS type.
		///	@ingroup m35WFS
		/// @author Hugues Romain
		/// @version 3.2.1
		/// @date 2010
		class StopPointWFSType:
			public map::WFSTypeTemplate<StopPointWFSType>
		{
		public:
			StopPointWFSType(){}
		};
	}
}

#endif // SYNTHESE_pt_StopPointWFSType_hpp__
