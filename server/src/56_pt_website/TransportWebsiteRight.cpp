
/** TransportWebsiteRight class implementation.
	@file TransportWebsiteRight.cpp
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

#include "TransportWebsiteRight.h"

#include "SecurityConstants.hpp"

using namespace std;

namespace synthese
{
	using namespace pt_website;
	using namespace security;

	namespace util
	{
		template<> const std::string FactorableTemplate<Right, TransportWebsiteRight>::FACTORY_KEY("pt_website");
	}

	namespace security
	{
		template<> const string RightTemplate<TransportWebsiteRight>::NAME("Site web transport public");
		template<> const bool RightTemplate<TransportWebsiteRight>::USE_PRIVATE_RIGHTS(true);

		template<>
		ParameterLabelsVector RightTemplate<TransportWebsiteRight>::getStaticParametersLabels()
		{
			ParameterLabelsVector m;
			m.push_back(make_pair(GLOBAL_PERIMETER, "(all)"));

			return m;
		}
	}

	namespace pt_website
	{
		string TransportWebsiteRight::displayParameter(
			util::Env& env
		) const	{
			if (_parameter == GLOBAL_PERIMETER)
				return "all";

			return _parameter;
		}

		bool TransportWebsiteRight::perimeterIncludes(
			const string& perimeter,
			util::Env& env
		) const	{
			if (_parameter == GLOBAL_PERIMETER)
				return true;

			return false;
		}
	}
}
