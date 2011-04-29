
/** RoutePlanningUserPrefPanel class implementation.
	@file RoutePlanningUserPrefPanel.cpp

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

#include "RoutePlanningUserPrefPanel.h"

using namespace std;

namespace synthese
{
	using namespace security;
	using namespace routeplanner;

	namespace util
	{
		template<>
		const string FactorableTemplate<UserPrefPanel,RoutePlanningUserPrefPanel>::FACTORY_KEY("RoutePlanningUserPrefPanel");
	}

	namespace security
	{
		template<>
		const string UserPrefPanelTemplate<RoutePlanningUserPrefPanel>::NAME("Recherche d'itinéraires");
	}

	namespace routeplanner
	{
		void RoutePlanningUserPrefPanel::display( std::ostream& stream, const User* user ) const
		{

		}
	}
}
