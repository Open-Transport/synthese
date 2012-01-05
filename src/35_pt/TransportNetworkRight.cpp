
/** TransportNetworkRight class implementation.
	@file TransportNetworkRight.cpp
	@author Hugues Romain
	@date 2008

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

#include "TransportNetworkRight.h"

#include "SecurityConstants.hpp"

using namespace std;

namespace synthese
{
	using namespace pt;
	using namespace security;

	namespace util
	{
		template<> const std::string FactorableTemplate<Right, TransportNetworkRight>::FACTORY_KEY("transport_network");
	}

	namespace security
	{
		template<> const string RightTemplate<TransportNetworkRight>::NAME("Réseaux de transport public");
		template<> const bool RightTemplate<TransportNetworkRight>::USE_PRIVATE_RIGHTS(false);

		template<>
		ParameterLabelsVector RightTemplate<TransportNetworkRight>::getStaticParametersLabels()
		{
			return NetworkLineBasedRight<TransportNetworkRight>::_getStaticParametersLabels();
		}
	}

	namespace pt
	{
		TransportNetworkRight::TransportNetworkRight()
			: pt::NetworkLineBasedRight<TransportNetworkRight>()
		{
			setParameter(GLOBAL_PERIMETER);
		}
	}
}
