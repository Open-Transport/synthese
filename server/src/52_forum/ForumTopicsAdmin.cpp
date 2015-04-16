
//////////////////////////////////////////////////////////////////////////
/// ForumTopicsAdmin class implementation.
///	@file ForumTopicsAdmin.cpp
///	@author Hugues Romain
///	@date 2010
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

#include "ForumTopicsAdmin.hpp"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "User.h"
#include "ForumModule.hpp"
#include "ForumRight.hpp"
#include "ForumTopicTableSync.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace admin;
	using namespace server;
	using namespace util;
	using namespace security;
	using namespace forum;
	using namespace html;

	namespace util
	{
		template<> const string FactorableTemplate<AdminInterfaceElement, ForumTopicsAdmin>::FACTORY_KEY("ForumTopicsAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ForumTopicsAdmin>::ICON("comments.png");
		template<> const string AdminInterfaceElementTemplate<ForumTopicsAdmin>::DEFAULT_TITLE("Sujets de forum");
	}

	namespace forum
	{
		// const string ForumTopicsAdmin::PARAM_SEARCH_XXX("xx");



		ForumTopicsAdmin::ForumTopicsAdmin()
			: AdminInterfaceElementTemplate<ForumTopicsAdmin>()
		{ }



		void ForumTopicsAdmin::setFromParametersMap(
			const ParametersMap& map
		){
			/// @todo Initialize internal attributes from the map
			// 	string a = map.get<string>(PARAM_SEARCH_XXX);
			// 	string b = map.getDefault<string>(PARAM_SEARCH_XXX);
			// 	optional<string> c = map.getOptional<string>(PARAM_SEARCH_XXX);

			// Search table initialization
			// _requestParameters.setFromParametersMap(map.getMap(), PARAM_SEARCH_XXX, 30);
		}



		ParametersMap ForumTopicsAdmin::getParametersMap() const
		{
			ParametersMap m;
			// ParametersMap m(_requestParameters.getParametersMap());

			// if(_searchXxx)
			//	m.insert(PARAM_SEARCH_XXX, *_searchXxx);

			return m;
		}



		bool ForumTopicsAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<ForumRight>(READ);
		}



		void ForumTopicsAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{

			ForumTopicTableSync::SearchResult topics(
				ForumTopicTableSync::Search(
					*_env
			)	);
		}



		AdminInterfaceElement::PageLinks ForumTopicsAdmin::getSubPagesOfModule(
			const ModuleClass& module,
			const AdminInterfaceElement& currentPage,
			const server::Request& _request
		) const	{

			AdminInterfaceElement::PageLinks links;

			if(	dynamic_cast<const ForumModule*>(&module) &&
				_request.getUser() &&
				_request.getUser()->getProfile() &&
				isAuthorized(*_request.getUser())
			){
				links.push_back(getNewCopiedPage());
			}

			return links;
		}
	}
}
