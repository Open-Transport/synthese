//////////////////////////////////////////////////////////////////////////
/// AnalysisRight class implementation.
///	@file AnalysisRight.cpp
///	@author Gaël Sauvanet
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "AnalysisRight.hpp"
#include "Env.h"
#include "SecurityConstants.hpp"

using namespace std;

namespace synthese
{
	using namespace analysis;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const std::string FactorableTemplate<Right, AnalysisRight>::FACTORY_KEY("AnalysisRight");
	}

	namespace security
	{
		template<> const string RightTemplate<AnalysisRight>::NAME("Analyses");
		template<> const bool RightTemplate<AnalysisRight>::USE_PRIVATE_RIGHTS(false);

		template<>
		ParameterLabelsVector RightTemplate<AnalysisRight>::getStaticParametersLabels()
		{
			ParameterLabelsVector m;
			m.push_back(make_pair(GLOBAL_PERIMETER, "(all)"));

			return m;
		}
	}

	namespace analysis
	{
		string AnalysisRight::displayParameter(Env& env) const
		{
			if (_parameter == GLOBAL_PERIMETER)
				return "all";

			return _parameter;
		}

		bool AnalysisRight::perimeterIncludes(const string& perimeter, Env& env) const
		{
			if (_parameter == GLOBAL_PERIMETER)
				return true;

			return false;
		}
	}
}
