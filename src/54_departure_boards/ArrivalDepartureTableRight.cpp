
/** ArrivalDepartureTableRight class implementation.
	@file ArrivalDepartureTableRight.cpp

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

#include "ArrivalDepartureTableRight.h"
#include "PTModule.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace security;
	using namespace departure_boards;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<Right, ArrivalDepartureTableRight>::FACTORY_KEY("ArrivalDepartureTable");
	}

	namespace security
	{
		template<> const string RightTemplate<ArrivalDepartureTableRight>::NAME("Tableaux de départs");
		template<> const bool RightTemplate<ArrivalDepartureTableRight>::USE_PRIVATE_RIGHTS(false);

		template<>
		ParameterLabelsVector RightTemplate<ArrivalDepartureTableRight>::getStaticParametersLabels()
		{
			ParameterLabelsVector m;
			m.push_back(std::pair<boost::optional<std::string>, std::string>(boost::optional<std::string>("*"),"(tous les afficheurs)"));

			PTModule::getNetworkLinePlaceRightParameterList(m);

			return m;
		}
	}

	namespace departure_boards
	{
	}
}
