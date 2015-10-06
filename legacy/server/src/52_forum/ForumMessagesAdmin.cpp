
//////////////////////////////////////////////////////////////////////////
/// ForumMessagesAdmin class implementation.
///	@file ForumMessagesAdmin.cpp
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

#include "ForumMessagesAdmin.hpp"

#include "AdminParametersException.h"
#include "ParametersMap.h"
#include "Profile.h"
#include "User.h"
#include "ForumModule.hpp"
#include "ForumRight.hpp"
#include "ForumTopic.hpp"
#include "ForumMessageTableSync.hpp"

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
		template<> const string FactorableTemplate<AdminInterfaceElement, ForumMessagesAdmin>::FACTORY_KEY("ForumMessagesAdmin");
	}

	namespace admin
	{
		template<> const string AdminInterfaceElementTemplate<ForumMessagesAdmin>::ICON("comment.png");
		template<> const string AdminInterfaceElementTemplate<ForumMessagesAdmin>::DEFAULT_TITLE("Message");
	}

	namespace forum
	{
		// const string ForumMessagesAdmin::PARAM_SEARCH_XXX("xx");



		ForumMessagesAdmin::ForumMessagesAdmin()
			: AdminInterfaceElementTemplate<ForumMessagesAdmin>()
		{ }



		void ForumMessagesAdmin::setFromParametersMap(
			const ParametersMap& map
		){

			// Search table initialization
			_requestParameters.setFromParametersMap(map, ForumMessageTableSync::COL_DATE, 50, false);
		}



		ParametersMap ForumMessagesAdmin::getParametersMap() const
		{
			ParametersMap m(_requestParameters.getParametersMap());

			// if(_searchXxx)
			//	m.insert(PARAM_SEARCH_XXX, *_searchXxx);

			return m;
		}



		bool ForumMessagesAdmin::isAuthorized(
			const security::User& user
		) const	{
			return user.getProfile()->isAuthorized<ForumRight>(READ);
		}



		void ForumMessagesAdmin::display(
			ostream& stream,
			const server::Request& _request
		) const	{

			ForumMessageTableSync::SearchResult result(
				ForumMessageTableSync::Search(
					*_env,
					_topic.get() ? _topic->getKey() : optional<RegistryKeyType>(),
					optional<string>(),
					optional<RegistryKeyType>(),
					logic::indeterminate,
					_requestParameters.first,
					_requestParameters.maxSize,
					_requestParameters.orderField == ForumMessageTableSync::COL_DATE,
					_requestParameters.raisingOrder
			)	);

			ResultHTMLTable::HeaderVector h;
			h.push_back(make_pair("Date", ForumMessageTableSync::COL_DATE));
			h.push_back(make_pair("Sujet", string()));
			h.push_back(make_pair("Message", string()));
			h.push_back(make_pair("Actions", string()));


		}



		AdminInterfaceElement::PageLinks ForumMessagesAdmin::getSubPagesOfModule(
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



		std::string ForumMessagesAdmin::getTitle() const
		{
			return _topic.get() ? _topic->getName() : DEFAULT_TITLE;
		}



		bool ForumMessagesAdmin::_hasSameContent(const AdminInterfaceElement& other) const
		{
			// return _value == static_cast<const ForumMessagesAdmin&>(other)._value;
			return true;
		}
	}
}
