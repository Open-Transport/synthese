
/** RollingStock class implementation.
	@file RollingStock.cpp

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

#include "RollingStock.h"
#include "Registry.h"
#include "ParametersMap.h"

using namespace std;

namespace synthese
{
	using namespace graph;
	using namespace util;

	namespace util
	{
		template<> const string Registry<pt::RollingStock>::KEY("RollingStock");
	}

	namespace pt
	{
		const string RollingStock::DATA_ID("id");
		const string RollingStock::DATA_NAME("name");
		const string RollingStock::DATA_ARTICLE("article");


		RollingStock::RollingStock(
			util::RegistryKeyType key
		):	graph::PathClass(),
			util::Registrable(key),
			_isTridentKeyReference(false)
		{
		}



		RollingStock::~RollingStock()
		{

		}



		PathClass::Identifier RollingStock::getIdentifier() const
		{
			return getKey();
		}



		void RollingStock::toParametersMap(
			util::ParametersMap& pm,
			std::string prefix /*= std::string() */
		) const {

			pm.insert(DATA_ID, getKey());
			pm.insert(DATA_NAME, getName());
			pm.insert(DATA_ARTICLE, getArticle());
		}
	}
}
