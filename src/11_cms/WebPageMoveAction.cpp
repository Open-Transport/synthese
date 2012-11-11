
//////////////////////////////////////////////////////////////////////////
/// WebPageMoveAction class implementation.
/// @file WebPageMoveAction.cpp
/// @author Hugues Romain
/// @date 2010
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

#include "WebPageMoveAction.hpp"

#include "ActionException.h"
#include "CMSRight.hpp"
#include "DBTransaction.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "User.h"
#include "WebPageTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, cms::WebPageMoveAction>::FACTORY_KEY("WebPageMoveAction");
	}

	namespace cms
	{
		const string WebPageMoveAction::PARAMETER_PAGE_ID = Action_PARAMETER_PREFIX + "id";
		const string WebPageMoveAction::PARAMETER_DIRECTION = Action_PARAMETER_PREFIX + "di";



		ParametersMap WebPageMoveAction::getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			map.insert(PARAMETER_DIRECTION, _up);
			return map;
		}



		void WebPageMoveAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_page = WebPageTableSync::GetEditable(map.get<RegistryKeyType>(PARAMETER_PAGE_ID), *_env);
			}
			catch(ObjectNotFoundException<WebPageTableSync>&)
			{
				throw ActionException("No such page");
			}

			_up = map.getDefault<bool>(PARAMETER_DIRECTION, false);

			WebPageTableSync::SearchResult pages(
				WebPageTableSync::Search(
					Env::GetOfficialEnv(),
					_page->getParent() ? optional<RegistryKeyType>() : _page->getRoot()->getKey(),
					_page->getParent() ? _page->getParent()->getKey() : optional<RegistryKeyType>(),
					_up ? _page->getRank() - 1 : _page->getRank() + 1,
					0,
					1
			)	);

			if(pages.empty())
			{
				throw ActionException(
					_up ?
					"Forst page cannot be moved before its position" :
					"Last page cannot be moved after its position"
				);
			}
			_switchedPage = pages.front();
		}



		void WebPageMoveAction::run(
			Request& request
		){
//			stringstream text;
//			::appendToLogIfChange(text, "Parameter ", _object->getAttribute(), _newValue);

			DBTransaction t;

			_page->setRank(_up ? _page->getRank() - 1 : _page->getRank() + 1);
			WebPageTableSync::Save(_page.get(), t);

			_switchedPage->setRank(_up ? _switchedPage->getRank() + 1 : _switchedPage->getRank() - 1);
			WebPageTableSync::Save(_switchedPage.get(), t);

			t.run();

//			::AddUpdateEntry(*_object, text.str(), request.getUser().get());
		}



		bool WebPageMoveAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<CMSRight>(WRITE);
		}



		WebPageMoveAction::WebPageMoveAction():
			_up(false)
		{

		}
}	}
