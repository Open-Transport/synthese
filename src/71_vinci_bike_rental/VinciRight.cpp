
/** VinciRight class implementation.
	@file VinciRight.cpp
	@author Hugues Romain
	@date 2008

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "VinciRight.h"

#include "12_security/Constants.h"

using namespace std;

namespace synthese
{
	using namespace vinci;
	using namespace security;

	namespace util
	{
		template<> const std::string FactorableTemplate<Right, VinciRight>::FACTORY_KEY("vinci");
	}

	namespace security
	{
		template<> const string RightTemplate<VinciRight>::NAME("Vinci");
		template<> const bool RightTemplate<VinciRight>::USE_PRIVATE_RIGHTS(true);

		template<>
		ParameterLabelsVector RightTemplate<VinciRight>::getStaticParametersLabels()
		{
			ParameterLabelsVector m;
			m.push_back(make_pair(GLOBAL_PERIMETER, "(all)"));

			return m;
		}
	}

	namespace vinci
	{
		string VinciRight::displayParameter() const
		{
			if (_parameter == GLOBAL_PERIMETER)
				return "all";
			
			return _parameter;
		}

		bool VinciRight::perimeterIncludes(const string& perimeter) const
		{
			if (_parameter == GLOBAL_PERIMETER)
				return true;

			return false;
		}
	}
}
