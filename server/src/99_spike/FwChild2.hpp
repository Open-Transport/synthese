
/** Fwchild2 class header.
	@file Fwchild2.hpp

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

#ifndef SYNTHESE_spike_Fwchild2_hpp__
#define SYNTHESE_spike_Fwchild2_hpp__

#include "Object.hpp"

#include "PointerField.hpp"
#include "StringField.hpp"
#include "EnumObjectField.hpp"

#include "SchemaMacros.hpp"
#include "FwParent.hpp"

#include <set>
#include <vector>

namespace synthese
{
	namespace spike
	{
		class FwChild2;

		FIELD_STRING(Property2)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Property0),
			FIELD(Property2)
		> FwChild2Schema;

		/** Fwchild2 class.
		@ingroup m62
		*/
		class FwChild2:
			public FwParent, public Object<FwChild2, FwChild2Schema>
		{
		public:
            
		private:


		public:
			FwChild2(util::RegistryKeyType id=0);
			~FwChild2();

		};
}	}

#endif // SYNTHESE_spike_Fwchild2_hpp__
