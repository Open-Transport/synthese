
//////////////////////////////////////////////////////////////////////////
/// WebPageAddAction class implementation.
/// @file WebPageAddAction.cpp
/// @author Hugues
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

#include "WebPageAddAction.h"

#include "ActionException.h"
#include "CMSRight.hpp"
#include "ParametersMap.h"
#include "Profile.h"
#include "Request.h"
#include "Session.h"
#include "User.h"
#include "WebPageTableSync.h"
#include "WebsiteTableSync.hpp"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	using namespace db;

	namespace util
	{
		template<> const string FactorableTemplate<Action, cms::WebPageAddAction>::FACTORY_KEY("WebPageAddAction");
	}

	namespace cms
	{
		const string WebPageAddAction::PARAMETER_TITLE = Action_PARAMETER_PREFIX + "ti";
		const string WebPageAddAction::PARAMETER_SITE_ID = Action_PARAMETER_PREFIX + "si";
		const string WebPageAddAction::PARAMETER_TEMPLATE_ID = Action_PARAMETER_PREFIX + "te";
		const string WebPageAddAction::PARAMETER_PARENT_ID = Action_PARAMETER_PREFIX + "pi";



		ParametersMap WebPageAddAction::getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_TITLE, _title);
			if(_site.get())
			{
				map.insert(PARAMETER_SITE_ID, _site->getKey());
			}
			if(_template.get())
			{
				map.insert(PARAMETER_TEMPLATE_ID, _template->getKey());
			}
			if(_parent.get())
			{
				map.insert(PARAMETER_PARENT_ID, _parent->getKey());
			}
			return map;
		}



		void WebPageAddAction::_setFromParametersMap(const ParametersMap& map)
		{
			_title = map.getDefault<string>(PARAMETER_TITLE);

			if(map.getDefault<RegistryKeyType>(PARAMETER_TEMPLATE_ID, 0))
			{
				try
				{
					_template = WebPageTableSync::Get(
						map.get<RegistryKeyType>(PARAMETER_TEMPLATE_ID),
						*_env
					);
					_rank = _template->getRank();
					_site = WebsiteTableSync::GetEditable(_template->getRoot()->getKey(), *_env);
				}
				catch(ObjectNotFoundException<Webpage>&)
				{
					throw ActionException("No such web page template");
				}
			}
			else if(map.getDefault<RegistryKeyType>(PARAMETER_PARENT_ID, 0))
			{
				try
				{
					_parent = WebPageTableSync::GetEditable(
						map.get<RegistryKeyType>(PARAMETER_PARENT_ID),
						*_env
					);
					_site = WebsiteTableSync::GetEditable(_parent->getRoot()->getKey(), *_env);

					WebPageTableSync::SearchResult result(
						WebPageTableSync::Search(
							Env::GetOfficialEnv(),
							_site->getKey(),
							_parent->getKey(),
							optional<size_t>(),
							0,
							1,
							true,
							false,
							false
					)	);
					_rank = result.empty() ? 0 : (*result.begin())->getRank() + 1;
				}
				catch(ObjectNotFoundException<Webpage>&)
				{
					throw ActionException("No such web page template");
				}
			}
			else
			{
				try
				{
					_site = WebsiteTableSync::GetEditable(
						map.get<RegistryKeyType>(PARAMETER_SITE_ID),
						*_env
					);

					WebPageTableSync::SearchResult result(
						WebPageTableSync::Search(
							Env::GetOfficialEnv(),
							_site->getKey(),
							RegistryKeyType(0),
							optional<size_t>(),
							0,
							1,
							true,
							false,
							false
					)	);
					_rank = result.empty() ? 0 : (*result.begin())->getRank() + 1;
				}
				catch(ObjectNotFoundException<Website>&)
				{
					throw ActionException("No such website");
				}
			}
		}



		void WebPageAddAction::run(
			Request& request
		){
			DBTransaction transaction;

			Webpage object;
			object.set<Title>(_title);
			object.setRoot(_site.get());
			object.set<StartTime>(second_clock::local_time());
			object.setRank(_rank);

			if(_template.get())
			{
				object.set<WebpageContent>(
					WebpageContent(
						_template->get<WebpageContent>().getCMSScript().getCode()
				)	);
				object.setParent(_template->getParent());
			}

			if(_parent.get())
			{
				object.setParent(_parent.get());
			}

			WebPageTableSync::ShiftRank(
				_site->getKey(),
				object.getParent(true) ? object.getParent()->getKey() : 0,
				_rank,
				true,
				transaction
			);

			transaction.run();

//			WebPageTableSync::Save(&object, transaction);
			WebPageTableSync::Save(&object);

//			transaction.run();

//			::AddCreationEntry(object, request.getUser().get());
		}



		bool WebPageAddAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<CMSRight>(WRITE);
		}
	}
}
