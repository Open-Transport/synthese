
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

#include "Record.hpp"

#include "PointerField.hpp"
#include "StringField.hpp"
#include "EnumObjectField.hpp"
#include "FwParent.hpp"

// includes from Object.hpp
#include "PointerField.hpp"
#include "NumericField.hpp"
#include "ParametersMap.h"
#include "PointersVectorField.hpp"
#include "UtilTypes.h"
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/sequence.hpp>
#include <boost/optional.hpp>
#include <boost/fusion/include/map.hpp>
#include <boost/fusion/include/map_fwd.hpp>
#include <boost/fusion/include/prior.hpp>
#include <boost/fusion/include/end.hpp>
// end of includes


#include "SchemaMacros.hpp"

#include <set>
#include <vector>


namespace synthese
{
	namespace spike
	{

		FIELD_STRING(Property1)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Property0),
			FIELD(Property1)
		> FwChild1Schema;

		/** Fwchild1 class.
		@ingroup m62
		*/
		class FwChild1:	public FwParent
		{
		public:
			RECORD_WRAPPER(FwChild1, FwChild1Schema)

			RECORD_GETSET(Property0)
			RECORD_GETSET(Property1)

			TABLE_REGISTRY_DECL(FwChild1)
		};
	}
}

#endif // SYNTHESE_spike_Fwchild1_hpp__
