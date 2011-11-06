
/** GetMessagesFunction class implementation.
	@file GetMessagesFunction.cpp
	@author Hugues Romain
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "GetMessagesFunction.hpp"
#include "Webpage.h"
#include "SentAlarm.h"
#include "MessagesModule.h"
#include "StaticFunctionRequest.h"
#include "SentScenario.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function,messages::GetMessagesFunction>::FACTORY_KEY("messages");

	namespace messages
	{
		const string GetMessagesFunction::PARAMETER_RECIPIENT_ID("r");
		const string GetMessagesFunction::PARAMETER_MAX_MESSAGES_NUMBER("n");
		const string GetMessagesFunction::PARAMETER_BEST_PRIORITY_ONLY("b");
		const string GetMessagesFunction::PARAMETER_PRIORITY_ORDER("o");
		const string GetMessagesFunction::PARAMETER_DATE("d");
		const string GetMessagesFunction::PARAMETER_CMS_TEMPLATE_ID("t");
		const string GetMessagesFunction::PARAMETER_OUTPUT_FORMAT("of");

		const string GetMessagesFunction::DATA_MESSAGES("messages");
		const string GetMessagesFunction::DATA_MESSAGE("message");
		const string GetMessagesFunction::DATA_RANK("rank");

		const string GetMessagesFunction::FORMAT_JSON("json");
		const string GetMessagesFunction::FORMAT_XML("xml");

		ParametersMap GetMessagesFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_RECIPIENT_ID, _recipientId);
			if(_maxMessagesNumber)
			{
				map.insert(PARAMETER_MAX_MESSAGES_NUMBER, *_maxMessagesNumber);
			}
			map.insert(PARAMETER_BEST_PRIORITY_ONLY, _bestPriorityOnly);
			map.insert(PARAMETER_PRIORITY_ORDER, _priorityOrder);
			if(_cmsTemplate.get())
			{
				map.insert(PARAMETER_CMS_TEMPLATE_ID, _cmsTemplate->getKey());
			}
			if(!_date.is_not_a_date_time())
			{
				map.insert(PARAMETER_DATE, _date);
			}

			// Output format
			if(!_outputFormat.empty())
			{
				map.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}

			return map;
		}



		void GetMessagesFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Output format
			_outputFormat = map.getDefault<string>(PARAMETER_OUTPUT_FORMAT);

			_recipientId = map.get<RegistryKeyType>(PARAMETER_RECIPIENT_ID);
			_maxMessagesNumber = map.getOptional<size_t>(PARAMETER_MAX_MESSAGES_NUMBER);
			_bestPriorityOnly = map.getDefault<bool>(PARAMETER_BEST_PRIORITY_ONLY, true);
			_priorityOrder = map.getDefault<bool>(PARAMETER_PRIORITY_ORDER, true);

			// date
			if(!map.getDefault<string>(PARAMETER_DATE).empty() && map.getDefault<string>(PARAMETER_DATE) != "A")
			{
				_date = time_from_string(map.get<string>(PARAMETER_DATE));
			}
			else
			{
				_date = second_clock::local_time();
			}

			// CMS template
			try
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_CMS_TEMPLATE_ID));
				if(id)
				{
					_cmsTemplate = Env::GetOfficialEnv().get<Webpage>(*id);
				}
			}
			catch (ObjectNotFoundException<Webpage>& e)
			{
				throw RequestException("No such CMS template : "+ e.getMessage());
			}
		}



		void GetMessagesFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap pm(getTemplateParameters());
			MessagesModule::MessagesByRecipientId::mapped_type messages(MessagesModule::GetMessages(_recipientId));
			size_t number(0);

			optional<AlarmLevel> bestPriority;
			if(_priorityOrder)
			{
				BOOST_FOREACH(const MessagesModule::MessagesByRecipientId::mapped_type::value_type& it, messages)
				{
					if(!it->getScenario()->isApplicable(_date))
					{
						continue;
					}
					if(_maxMessagesNumber && number >= *_maxMessagesNumber)
					{
						break;
					}
					if( _bestPriorityOnly && bestPriority && it->getLevel() != *bestPriority)
					{
						break;
					}
					bestPriority = it->getLevel();

					shared_ptr<ParametersMap> messagePM(new ParametersMap(getTemplateParameters()));
					it->toParametersMap(*messagePM, true);
					pm.insert(DATA_MESSAGE, messagePM);

					++number;
				}
			}
			else
			{
				BOOST_FOREACH(const MessagesModule::MessagesByRecipientId::mapped_type::value_type& it, messages)
				{
					if(!it->getScenario()->isApplicable(_date))
					{
						bestPriority = it->getLevel();
						break;
					}
				}
				BOOST_REVERSE_FOREACH(const MessagesModule::MessagesByRecipientId::mapped_type::value_type& it, messages)
				{
					if(!it->getScenario()->isApplicable(_date))
					{
						continue;
					}
					if(_maxMessagesNumber && number >= *_maxMessagesNumber)
					{
						break;
					}
					if( _bestPriorityOnly && it->getLevel() != *bestPriority)
					{
						continue;
					}
					shared_ptr<ParametersMap> messagePM(new ParametersMap(getTemplateParameters()));
					it->toParametersMap(*messagePM, true);
					pm.insert(DATA_MESSAGE, messagePM);
					++number;
				}
			}

			if(_cmsTemplate.get())
			{
				size_t rank(0);
				BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmMessage, pm.getSubMaps(DATA_MESSAGE))
				{
					pmMessage->insert(DATA_RANK, rank++);
					_cmsTemplate->display(stream, request, *pmMessage);
				}
			}
			else if(_outputFormat == FORMAT_XML) // XML output
			{
				pm.outputXML(
					stream,
					DATA_MESSAGES,
					true,
					"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/17_messages/GetMessagesFunction.xsd"
				);
			}
			else if(_outputFormat == FORMAT_JSON) // JSON output
			{
				pm.outputJSON(
					stream,
					DATA_MESSAGES
				);
			}
			else
			{
				BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmMessage, pm.getSubMaps(DATA_MESSAGE))
				{
					stream << pmMessage->get<string>(Alarm::DATA_CONTENT);
				}
			}
		}



		bool GetMessagesFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string GetMessagesFunction::getOutputMimeType() const
		{
			std::string mimeType;
			if(_cmsTemplate.get())
			{
				mimeType = _cmsTemplate->getMimeType();
			}
			else if(_outputFormat == FORMAT_XML)
			{
				mimeType = "text/xml";
			}
			else if(_outputFormat == FORMAT_JSON)
			{
				mimeType = "application/json";
			}
			else
			{
				mimeType = "text/plain";
			}
			return mimeType;
		}



		GetMessagesFunction::GetMessagesFunction():
			_bestPriorityOnly(true),
			_priorityOrder(true)
		{
		}
}	}
