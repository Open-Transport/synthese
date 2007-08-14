
/** DBLogRight class implementation.
	@file DBLogRight.cpp

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

#include "12_security/Constants.h"

#include "13_dblog/DBLog.h"
#include "13_dblog/DBLogRight.h"

#include "01_util/Factory.h"

using namespace std;

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

		template<>
		ParameterLabelsVector RightTemplate<DBLogRight>::getStaticParametersLabels()
		{
			ParameterLabelsVector m;
			m.push_back(make_pair(GLOBAL_PERIMETER,"(tous les journaux)"));
			for (Factory<DBLog>::Iterator it = Factory<DBLog>::begin(); it != Factory<DBLog>::end(); ++it)
				m.push_back(make_pair(it.getKey(), it->getName()));
			return m;
		}
	}

	namespace dblog
	{
		std::string DBLogRight::displayParameter() const
		{
			if (Factory<DBLog>::contains(_parameter))
				return Factory<DBLog>::create(_parameter)->getName();
			else
				return _parameter;
		}

		bool DBLogRight::perimeterIncludes( const std::string& perimeter ) const
		{
			if (_parameter == GLOBAL_PERIMETER)
				return true;

			return _parameter == perimeter;
		}
	}
}
