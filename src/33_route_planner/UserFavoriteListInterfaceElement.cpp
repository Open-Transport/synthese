
/** UserFavoriteListInterfaceElement class implementation.
	@file UserFavoriteListInterfaceElement.cpp
	@author Hugues Romain
	@date 2007

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

#include "UserFavoriteListInterfaceElement.h"
#include "UserFavoriteJourney.h"
#include "UserFavoriteJourneyTableSync.h"
#include "UserFavoriteInterfacePage.h"

#include "Request.h"
#include "User.h"
#include "ValueElementList.h"
#include "Interface.h"
#include "Env.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace interfaces;
	using namespace security;
	using namespace env;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<LibraryInterfaceElement, routeplanner::UserFavoriteListInterfaceElement>::FACTORY_KEY("user_favorite_journeys");
	}

	namespace routeplanner
	{
		void UserFavoriteListInterfaceElement::storeParameters(ValueElementList& vel)
		{
			_elementPageCode = vel.front();
		}

		string UserFavoriteListInterfaceElement::display(
			ostream& stream
			, const ParametersVector& parameters
			, VariablesMap& variables
			, const void* object /*= NULL*/
			, const server::Request* request /*= NULL*/
		) const {

			const UserFavoriteInterfacePage* page(_page->getInterface()->getPage<UserFavoriteInterfacePage>(_elementPageCode->getValue(parameters, variables, object, request)));

			const User* user(request->getUser().get());
			
			Env env;
			UserFavoriteJourneyTableSync::Search(env, user);
			BOOST_FOREACH(shared_ptr<UserFavoriteJourney> fav, env.getRegistry<UserFavoriteJourney>())
			{
				page->display(stream, fav.get(), variables, request);
			}

			return string();
		}

		UserFavoriteListInterfaceElement::~UserFavoriteListInterfaceElement()
		{
		}
	}
}
