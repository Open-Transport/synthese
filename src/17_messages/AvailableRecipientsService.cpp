
//////////////////////////////////////////////////////////////////////////////////////////
///	AvailableRecipientsService class implementation.
///	@file AvailableRecipientsService.cpp
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

#include "AvailableRecipientsService.hpp"

#include "AlarmRecipient.h"
#include "RequestException.h"
#include "Request.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,messages::AvailableRecipientsService>::FACTORY_KEY = "available_recipients";


	
	namespace messages
	{
		const string AvailableRecipientsService::PARAMETER_RECIPIENT_KEY = "recipient_key";
		const string AvailableRecipientsService::TAG_RECIPIENT = "recipient";
		const string AvailableRecipientsService::ATTR_ID = "id";
		const string AvailableRecipientsService::ATTR_NAME = "name";
		const string AvailableRecipientsService::ATTR_PARAMETER = "parameter";
		


		ParametersMap AvailableRecipientsService::_getParametersMap() const
		{
			ParametersMap map;
			if(_recipient.get())
			{
				map.insert(PARAMETER_RECIPIENT_KEY, _recipient->getFactoryKey());
			}
			return map;
		}



		void AvailableRecipientsService::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_recipient.reset(
					Factory<AlarmRecipient>::create(map.get<string>(PARAMETER_RECIPIENT_KEY))
				);
			}
			catch(FactoryException<AlarmRecipient>&)
			{
				throw RequestException("No such recipient key");
			}
		}



		ParametersMap AvailableRecipientsService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;

			shared_ptr<AlarmRecipient::AvailableRecipients> recipients(
				_recipient->getAvailableRecipients()
			);
			_recipientToParametersMap(
				*recipients,
				map
			);
		
			return map;
		}
		
		
		
		bool AvailableRecipientsService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string AvailableRecipientsService::getOutputMimeType() const
		{
			return "text/html";
		}



		void AvailableRecipientsService::_recipientToParametersMap(
			const AlarmRecipient::AvailableRecipients& recipient,
			util::ParametersMap& pm
		) const	{

			// Properties
			pm.insert(ATTR_ID, recipient.id);
			pm.insert(ATTR_NAME, recipient.name);
			pm.insert(ATTR_PARAMETER, recipient.parameter);

			// Recursion
			BOOST_FOREACH(
				const AlarmRecipient::AvailableRecipients::Tree::value_type& item,
				recipient.tree
			){
				shared_ptr<ParametersMap> itemPM(new ParametersMap);
				_recipientToParametersMap(*item, *itemPM);
				pm.insert(TAG_RECIPIENT, itemPM);
			}
		}
}	}
