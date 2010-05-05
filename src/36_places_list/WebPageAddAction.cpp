
//////////////////////////////////////////////////////////////////////////
/// WebPageAddAction class implementation.
/// @file WebPageAddAction.cpp
/// @author Hugues
/// @date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#include "ActionException.h"
#include "ParametersMap.h"
#include "WebPageAddAction.h"
#include "TransportWebsiteRight.h"
#include "Request.h"
#include "WebPage.h"
#include "WebPageTableSync.h"
#include "Site.h"
#include "SiteTableSync.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace server;
	using namespace security;
	using namespace util;
	
	namespace util
	{
		template<> const string FactorableTemplate<Action, transportwebsite::WebPageAddAction>::FACTORY_KEY("WebPageAddAction");
	}

	namespace transportwebsite
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
					_site = _env->getEditableSPtr(_template->getRoot());
				}
				catch(ObjectNotFoundException<WebPage>& e)
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
					_site = _env->getEditableSPtr(_parent->getRoot());

					WebPageTableSync::SearchResult result(
						WebPageTableSync::Search(
							Env::GetOfficialEnv(),
							_site->getKey(),
							_parent->getKey(),
							0,
							1,
							true,
							false,
							false
					)	);
					_rank = result.empty() ? 0 : (*result.begin())->getRank() + 1;
				}
				catch(ObjectNotFoundException<WebPage>& e)
				{
					throw ActionException("No such web page template");
				}
			}
			else
			{
				try
				{
					_site = SiteTableSync::GetEditable(
						map.get<RegistryKeyType>(PARAMETER_SITE_ID),
						*_env
					);

					WebPageTableSync::SearchResult result(
						WebPageTableSync::Search(
							Env::GetOfficialEnv(),
							_site->getKey(),
							RegistryKeyType(0),
							0,
							1,
							true,
							false,
							false
					)	);
					_rank = result.empty() ? 0 : (*result.begin())->getRank() + 1;
				}
				catch(ObjectNotFoundException<Site>& e)
				{
					throw ActionException("No such site");
				}
			}
		}
		
		
		
		void WebPageAddAction::run(
			Request& request
		){
			WebPage object;
			object.setName(_title);
			object.setRoot(_site.get());
			object.setRank(_rank);

			if(_template.get())
			{
				object.setContent(_template->getContent());
				object.setParent(_template->getParent());
			}

			if(_parent.get())
			{
				object.setParent(_parent.get());
			}

			WebPageTableSync::ShiftRank(
				_site->getKey(),
				object.getParent() ? object.getParent()->getKey() : 0,
				_rank,
				true
			);

			WebPageTableSync::Save(&object);

//			::AddCreationEntry(object, request.getUser().get());
		}
		
		
		
		bool WebPageAddAction::isAuthorized(
			const Session* session
		) const {
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportWebsiteRight>(WRITE);
		}



		void WebPageAddAction::setSite( boost::shared_ptr<Site> value )
		{
			_site = value;
		}



		void WebPageAddAction::setTitle( const std::string& value )
		{
			_title = value;
		}



		void WebPageAddAction::setParent( boost::shared_ptr<WebPage> value )
		{
			_parent = value;
		}
	}
}
