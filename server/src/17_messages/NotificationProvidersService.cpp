/** NotificationProvidersService class implementation.
	@file NotificationProvidersService.cpp
	@author yves.martin
	@date 2015

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2015 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include <Env.h>
#include <Factory.h>
#include <NotificationChannel.hpp>
#include <NotificationProvidersService.hpp>
#include <Object.hpp>
#include <ObjectNotFoundException.h>
#include <Registry.h>
#include <Request.h>
#include <RequestException.h>
#include <StringField.hpp>
#include <UtilTypes.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <map>
#include <utility>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace messages;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function, NotificationProvidersService>::FACTORY_KEY = "notification_providers";

	namespace messages
	{
		const string NotificationProvidersService::TAG_NOTIFICATION_PROVIDER = "provider";
		const string NotificationProvidersService::PARAMETER_LIST_CHANNELS = "list_channels";
		const string NotificationProvidersService::TAG_CHANNEL = "channel";



		ParametersMap NotificationProvidersService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void NotificationProvidersService::_setFromParametersMap(const ParametersMap& map)
		{
			// Reference to notification provider by unique ID
			RegistryKeyType objectId(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0));
			if(objectId)
			{
				try
				{
					_notificationProvider = Env::GetOfficialEnv().getEditable<NotificationProvider>(objectId).get();
				}
				catch (ObjectNotFoundException<NotificationProvider>&)
				{
					throw RequestException("No such notification provider : "+ lexical_cast<string>(objectId));
				}
			}

			_listChannels = map.getDefault<bool>(PARAMETER_LIST_CHANNELS, false);
		}



		ParametersMap NotificationProvidersService::run(
			std::ostream& stream,
			const Request& request
		) const {
			// Result
			ParametersMap map;

			// List channels, aka notification provider keys
			if(_listChannels) {
				Factory<NotificationChannel>::Keys keys(Factory<NotificationChannel>::GetKeys());
				// Do not use GetNewCollections for only a key list
				BOOST_FOREACH(const Factory<NotificationChannel>::Keys::value_type& channelKey, keys)
				{
					boost::shared_ptr<ParametersMap> channelPM(new ParametersMap);
					channelPM->insert(NotificationChannel::ATTR_KEY, channelKey);
					map.insert(TAG_CHANNEL, channelPM);
				}
				return map;
			}

			// Build the list of objects to export
			typedef multimap<string, const NotificationProvider*> NotificationProviderLists;
			NotificationProviderLists providers;
			if(_notificationProvider)
			{
				// Case unique list
				providers.insert(make_pair(_notificationProvider->get<Name>(), _notificationProvider));
			}
			else
			{
				BOOST_FOREACH(
					const Registry<NotificationProvider>::value_type& it,
					Env::GetOfficialEnv().getRegistry<NotificationProvider>()
				){
					// Providers are enlisted for output
					providers.insert(
						make_pair(it.second->get<Name>(), it.second.get())
					);
				}
			}

			// Loop on notification providers
			BOOST_FOREACH(const NotificationProviderLists::value_type& it, providers)
			{
				// New submap
				boost::shared_ptr<ParametersMap> providerParameters(new ParametersMap);

				// Export the submap
				it.second->toParametersMap(*providerParameters, true);

				// Store the submap
				map.insert(TAG_NOTIFICATION_PROVIDER, providerParameters);
			}

			// End
			return map;
		}



		bool NotificationProvidersService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string NotificationProvidersService::getOutputMimeType() const
		{
			return "text/html";
		}



		NotificationProvidersService::NotificationProvidersService():
			_listChannels(false),
			_notificationProvider(NULL)
		{ }

}	}
