
/** StopsInterSYNTHESEIdFilter class header.
	@file StopsInterSYNTHESEIdFilter.hpp

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

#ifndef SYNTHESE_pt_StopsInterSYNTHESEIdFilter_hpp__
#define SYNTHESE_pt_StopsInterSYNTHESEIdFilter_hpp__

#include "FactorableTemplate.h"
#include "InterSYNTHESEIdFilter.hpp"

namespace synthese
{
	namespace pt
	{
		/** StopsInterSYNTHESEIdFilter class.
			@ingroup m35
		*/
		class StopsInterSYNTHESEIdFilter:
			public util::FactorableTemplate<inter_synthese::InterSYNTHESEIdFilter, StopsInterSYNTHESEIdFilter>
		{
		public:
			virtual std::string convertId(
				util::RegistryTableType tableId,
				const std::string& fieldName,
				const std::string& objectId
			) const;
		};
	}
}

#endif // SYNTHESE_pt_StopsInterSYNTHESEIdFilter_hpp__

