////////////////////////////////////////////////////////////////////////////////
/// DBLogRight class implementation.
///	@file DBLogRight.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "SecurityConstants.hpp"
#include "DBLog.h"
#include "DBLogRight.h"
#include "Factory.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace security;
	using namespace util;
	using namespace dblog;

	namespace util
	{
		template<> const std::string FactorableTemplate<Right, DBLogRight>::FACTORY_KEY("Logs");
	}

	namespace security
	{
		template<> const string RightTemplate<DBLogRight>::NAME("Journaux");
		template<> const bool RightTemplate<DBLogRight>::USE_PRIVATE_RIGHTS(true);

		template<>
		ParameterLabelsVector RightTemplate<DBLogRight>::getStaticParametersLabels()
		{
			ParameterLabelsVector m;
			m.push_back(make_pair(GLOBAL_PERIMETER,"(tous les journaux)"));
			vector<boost::shared_ptr<DBLog> > logs(Factory<DBLog>::GetNewCollection());
			BOOST_FOREACH(const boost::shared_ptr<DBLog> loge, logs)
			{
				m.push_back(make_pair(loge->getFactoryKey(), loge->getName()));
			}
			return m;
		}
	}

	namespace dblog
	{
		std::string DBLogRight::displayParameter(
			util::Env& env
		) const	{
			if (Factory<DBLog>::contains(_parameter))
				return Factory<DBLog>::create(_parameter)->getName();
			else
				return _parameter;
		}

		bool DBLogRight::perimeterIncludes(
			const std::string& perimeter,
			util::Env& env
		) const	{
			if (_parameter == GLOBAL_PERIMETER)
				return true;

			return _parameter == perimeter;
		}
	}
}
