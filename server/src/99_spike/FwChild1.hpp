
/** Fwchild1 class header.
	@file Fwchild1.hpp

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

#ifndef SYNTHESE_spike_Fwchild1_hpp__
#define SYNTHESE_spike_Fwchild1_hpp__

#include "Object.hpp"

#include "PointerField.hpp"
#include "StringField.hpp"
#include "EnumObjectField.hpp"
#include "FwParent.hpp"


#include "SchemaMacros.hpp"

#include <set>
#include <vector>

namespace synthese
{
	namespace spike
	{
		class FwChild1;

		FIELD_STRING(Property1)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Property0),
			FIELD(Property1)
		> FwChild1Schema;

		/** Fwchild1 class.
		@ingroup m62
		*/
		class FwChild1:
			public FwParent
		{
		public:
            
		private:


		public:
			FwChild1(util::RegistryKeyType id=0);
			~FwChild1();

		};
}	}

#endif // SYNTHESE_spike_Fwchild1_hpp__
