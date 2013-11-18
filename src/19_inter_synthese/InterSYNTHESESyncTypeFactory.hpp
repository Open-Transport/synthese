
/** InterSYNTHESESyncTypeFactory class header.
	@file InterSYNTHESESyncTypeFactory.hpp

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

#ifndef SYNTHESE_inter_synthese_InterSYNTHESESyncTypeFactory_hpp__
#define SYNTHESE_inter_synthese_InterSYNTHESESyncTypeFactory_hpp__

#include "FactoryBase.h"

#include <vector>

namespace synthese
{
	namespace inter_synthese
	{
		class InterSYNTHESEIdFilter;
		class InterSYNTHESESlave;
		class InterSYNTHESEConfigItem;



		/** InterSYNTHESESyncTypeFactory class.
			@ingroup m19
		*/
		class InterSYNTHESESyncTypeFactory:
			public util::FactoryBase<InterSYNTHESESyncTypeFactory>
		{
		public:
			InterSYNTHESESyncTypeFactory();

			virtual void initSync(
			) const = 0;

			virtual bool sync(
				const std::string& parameter,
				const InterSYNTHESEIdFilter* idFilter
			) const = 0;

			virtual void closeSync(
			) const = 0;

			virtual void initQueue(
				const InterSYNTHESESlave& slave,
				const std::string& perimeter
			) const = 0;

			virtual bool mustBeEnqueued(
				const std::string& configPerimeter,
				const std::string& messagePerimeter
			) const = 0;

			typedef std::vector<const InterSYNTHESEConfigItem*> SortedItems;
			typedef std::vector<const InterSYNTHESEConfigItem*> RandomItems;
			virtual SortedItems sort(const RandomItems& randItems) const = 0;
		};
	}
}

#endif // SYNTHESE_inter_synthese_InterSYNTHESESyncTypeFactory_hpp__

