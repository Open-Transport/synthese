
/** ResaRight class implementation.
	@file ResaRight.cpp
	@author Hugues Romain
	@date 2007

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

#include "ResaRight.h"


#include "SecurityConstants.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace resa;
	using namespace security;
	using namespace pt;

	namespace util
	{
		template<> const std::string FactorableTemplate<Right, ResaRight>::FACTORY_KEY("resa");
	}

	namespace security
	{
		template<> const string RightTemplate<ResaRight>::NAME("Reservation");
		template<> const bool RightTemplate<ResaRight>::USE_PRIVATE_RIGHTS(true);

		template<>
		ParameterLabelsVector RightTemplate<ResaRight>::getStaticParametersLabels()
		{
			return NetworkLineBasedRight<ResaRight>::_getStaticParametersLabels();
		}
	}

	namespace resa
	{

		ResaRight::ResaRight()
			: pt::NetworkLineBasedRight<ResaRight>()
		{
			setParameter(GLOBAL_PERIMETER);
		}
	}
}
