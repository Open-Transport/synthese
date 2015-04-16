
//////////////////////////////////////////////////////////////////////////
/// WebsiteRankCleanupAction class implementation.
/// @file WebsiteRankCleanupAction.cpp
/// @author hromain
/// @date 2013
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

#include "WebsiteRankCleanupAction.hpp"

#include "ActionException.h"
#include "DBTransaction.hpp"
#include "ParametersMap.h"
#include "Request.h"
#include "WebPageTableSync.h"
#include "Website.hpp"

using namespace std;

namespace synthese
{
	using namespace db;
	using namespace server;
	using namespace security;
	using namespace util;
	
	template<>
	const string FactorableTemplate<Action, cms::WebsiteRankCleanupAction>::FACTORY_KEY = "website_rank_cleanup";

	namespace cms
	{
		const string WebsiteRankCleanupAction::PARAMETER_WEBSITE = Action_PARAMETER_PREFIX + "_website";
		
		
		
		ParametersMap WebsiteRankCleanupAction::getParametersMap() const
		{
			ParametersMap map;
			if(_site)
			{
				map.insert(PARAMETER_WEBSITE, _site->getKey());
			}
			return map;
		}
		
		
		
		void WebsiteRankCleanupAction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_site = Env::GetOfficialEnv().get<Website>(
					map.get<RegistryKeyType>(PARAMETER_WEBSITE)
				).get();
			}
			catch(ObjectNotFoundException<Website>& e)
			{
				throw ActionException("No such website");
			}
		}
		
		
		
		void WebsiteRankCleanupAction::run(
			Request& request
		){
			// Detect needed changes
			PagesToUpdate pagesToUpdate;
			_checkPages(
				_site->getChildren(),
				pagesToUpdate
			);
			
			// Apply updates
			if(!pagesToUpdate.empty())
			{
				DBTransaction transaction;
				Env env;
				BOOST_FOREACH(const PagesToUpdate::value_type& it, pagesToUpdate)
				{
					// Load the page
					boost::shared_ptr<Webpage> page(
						WebPageTableSync::GetEditable(
							it.first,
							env
					)	);

					// Update the rank
					page->setRank(it.second);

					// Save the page
					WebPageTableSync::Save(page.get(), transaction);
				}
				transaction.run();
			}
		}
		
		
		
		bool WebsiteRankCleanupAction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		WebsiteRankCleanupAction::WebsiteRankCleanupAction():
			_site(NULL)
		{}



		void WebsiteRankCleanupAction::_checkPages(
			const Webpage::ChildrenType& children,
			PagesToUpdate& pagesToUpdate
		) const	{
			
			size_t nextExpectedRank(0);
			BOOST_FOREACH(const Webpage::ChildrenType::value_type& page, children)
			{
				// Check the rank of the page
				if(page.first != nextExpectedRank)
				{
					pagesToUpdate.insert(
						make_pair(
							page.second->getKey(),
							nextExpectedRank
					)	);
				}

				// Go to the next rank
				++nextExpectedRank;

				// Check the branch too
				_checkPages(page.second->getChildren(), pagesToUpdate);
			}
		}
}	}
