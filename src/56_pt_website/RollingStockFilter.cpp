
/** RollingStockFilter class implementation.
	@file RollingStockFilter.cpp

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

#include "RollingStockFilter.h"
#include "RollingStock.hpp"
#include "Env.h"

using namespace std;

namespace synthese
{
	using namespace graph;
	using namespace util;
	using namespace vehicle;

	namespace util
	{
		template<> const string Registry<pt_website::RollingStockFilter>::KEY("RollingStockFilter");
	}

	namespace pt_website
	{
		graph::AccessParameters::AllowedPathClasses RollingStockFilter::getAllowedPathClasses() const
		{
			graph::AccessParameters::AllowedPathClasses result;
			result.insert(0);
			if(_authorized_only)
			{
				BOOST_FOREACH(const List::value_type& element, _list)
				{
					result.insert(element->getIdentifier());
				}
			}
			else
			{
				BOOST_FOREACH(RollingStock::Registry::value_type element, Env::GetOfficialEnv().getEditableRegistry<RollingStock>())
				{
					if(_list.find(element.second.get()) != _list.end())
					{
						continue;
					}
					result.insert(element.second->getIdentifier());
				}
			}
			return result;
		}
	}
}
