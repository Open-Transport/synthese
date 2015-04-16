
/** CMSRight class implementation.
	@file CMSRight.cpp
	@author Hugues Romain
	@date 2011

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

#include "CMSRight.hpp"
#include "Env.h"
#include "SecurityConstants.hpp"
#include "Webpage.h"
#include "WebPageTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace cms;
	using namespace security;
	using namespace util;

	namespace util
	{
		template<> const std::string FactorableTemplate<Right, CMSRight>::FACTORY_KEY("cms");
	}

	namespace security
	{
		template<> const string RightTemplate<CMSRight>::NAME("CMS");
		template<> const bool RightTemplate<CMSRight>::USE_PRIVATE_RIGHTS(false);

		template<>
		ParameterLabelsVector RightTemplate<CMSRight>::getStaticParametersLabels()
		{
			ParameterLabelsVector m;
			m.push_back(make_pair(GLOBAL_PERIMETER, "(all)"));

			return m;
		}
	}

	namespace cms
	{
		string CMSRight::displayParameter(Env& env) const
		{
			if (_parameter == GLOBAL_PERIMETER)
				return "all";

			try
			{
				util::RegistryKeyType id(boost::lexical_cast<util::RegistryKeyType>(_parameter));
				util::RegistryTableType tableId(util::decodeTableId(id));

				if (tableId == WebPageTableSync::TABLE.ID)
				{
					boost::shared_ptr<const Webpage> page(
						WebPageTableSync::Get(id, env)
					);
					return page->getFullName();
				}
			}
			catch(bad_lexical_cast&)
			{
				return "bad perimeter";
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				return "page "+ _parameter +" not found";
			}

			return "unknown";
		}



		bool CMSRight::perimeterIncludes(const string& perimeter, Env& env) const
		{
			if (_parameter == GLOBAL_PERIMETER)
				return true;

			return false;
		}
}	}
