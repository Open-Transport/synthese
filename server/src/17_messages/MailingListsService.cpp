
//////////////////////////////////////////////////////////////////////////////////////////
///	MailingListsService class implementation.
///	@file MailingListsService.cpp
///	@author hromain
///	@date 2013
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

#include "MailingListsService.hpp"

#include "MailingList.hpp"
#include "RequestException.h"
#include "Request.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace messages;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function, MailingListsService>::FACTORY_KEY = "mailing_lists";
	
	namespace messages
	{
		const string MailingListsService::PARAMETER_WITH_SUBSCRIPTIONS = "with_subscriptions";
		const string MailingListsService::PARAMETER_MANUAL_SENDING_FILTER = "manual_sending";
		const string MailingListsService::TAG_MAILING_LIST = "mailing_list";



		ParametersMap MailingListsService::_getParametersMap() const
		{
			ParametersMap map;

			// The unique mailing list
			if(_mailingList)
			{
				map.insert(Request::PARAMETER_OBJECT_ID, _mailingList->getKey());
			}

			// With recipients
			if(_withSubscriptions)
			{
				map.insert(PARAMETER_WITH_SUBSCRIPTIONS, 1);
			}

			// Manual sending
			if(_manualSendingFilter)
			{
				map.insert(PARAMETER_MANUAL_SENDING_FILTER, *_manualSendingFilter);
			}

			return map;
		}



		void MailingListsService::_setFromParametersMap(const ParametersMap& map)
		{
			// Unique mailing list
			RegistryKeyType objectId(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0));
			if(objectId)
			{
				try
				{
					_mailingList = Env::GetOfficialEnv().get<MailingList>(objectId).get();
				}
				catch (ObjectNotFoundException<MailingList>&)
				{
					throw RequestException("No such mailling list : "+ lexical_cast<string>(objectId));
				}
			}

			// With recipients
			_withSubscriptions = map.getDefault<bool>(PARAMETER_WITH_SUBSCRIPTIONS, false);

			// Manual sending
			if(!map.getDefault<string>(PARAMETER_MANUAL_SENDING_FILTER).empty())
			{
				_manualSendingFilter = map.getDefault<bool>(PARAMETER_MANUAL_SENDING_FILTER, false);
			}
		}



		ParametersMap MailingListsService::run(
			std::ostream& stream,
			const Request& request
		) const {

			// Build the list of objects to export
			typedef multimap<string, const MailingList*> MailingLists;
			MailingLists mailingLists;
			if(_mailingList)
			{ // Case unique list
				mailingLists.insert(make_pair(_mailingList->get<Name>(), _mailingList));
			}
			else
			{
				BOOST_FOREACH(
					const Registry<MailingList>::value_type& it,
					Env::GetOfficialEnv().getRegistry<MailingList>()
				){
					// Manual sending filter
					if(_manualSendingFilter &&
						it.second->get<ManualSending>() != *_manualSendingFilter
					){
						continue;
					}

					// The mailing list is registered for the output
					mailingLists.insert(
						make_pair(it.second->get<Name>(), it.second.get())
					);
				}
			}

			// Populate the map
			ParametersMap map;
			
			// Loop on mailing lists
			BOOST_FOREACH(const MailingLists::value_type& it, mailingLists)
			{
				// New submap
				boost::shared_ptr<ParametersMap> mlPM(new ParametersMap);

				// Export the submap
				it.second->toParametersMap(*mlPM, _withSubscriptions);

				// Store the submap
				map.insert(TAG_MAILING_LIST, mlPM);
			}

			// End
			return map;
		}
		
		
		
		bool MailingListsService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string MailingListsService::getOutputMimeType() const
		{
			return "text/html";
		}



		MailingListsService::MailingListsService():
			_mailingList(NULL),
			_withSubscriptions(false)
		{

		}
}	}
