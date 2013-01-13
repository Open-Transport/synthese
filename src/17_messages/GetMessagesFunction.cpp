
/** GetMessagesFunction class implementation.
	@file GetMessagesFunction.cpp
	@author Hugues Romain
	@date 2010

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "GetMessagesFunction.hpp"

#include "AlarmObjectLink.h"
#include "AlarmRecipient.h"
#include "DBModule.h"
#include "MessagesModule.h"
#include "RequestException.h"
#include "Request.h"
#include "Webpage.h"
#include "SentAlarm.h"
#include "StaticFunctionRequest.h"
#include "SentScenario.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function,messages::GetMessagesFunction>::FACTORY_KEY("messages");

	namespace messages
	{
		const string GetMessagesFunction::PARAMETER_RECIPIENT_KEY = "recipient_key";
		const string GetMessagesFunction::PARAMETER_RECIPIENT_ID("r");
		const string GetMessagesFunction::PARAMETER_MAX_MESSAGES_NUMBER("n");
		const string GetMessagesFunction::PARAMETER_BEST_PRIORITY_ONLY("b");
		const string GetMessagesFunction::PARAMETER_PRIORITY_ORDER("o");
		const string GetMessagesFunction::PARAMETER_DATE("d");
		const string GetMessagesFunction::PARAMETER_END_DATE = "end_date";
		const string GetMessagesFunction::PARAMETER_CMS_TEMPLATE_ID("t");

		const string GetMessagesFunction::DATA_MESSAGES("messages");
		const string GetMessagesFunction::DATA_MESSAGE("message");
		const string GetMessagesFunction::DATA_RANK("rank");
		const string GetMessagesFunction::ATTR_PARAMETER = "parameter";



		ParametersMap GetMessagesFunction::_getParametersMap() const
		{
			ParametersMap map;
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
			else
			{
				map.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}


			// Date
			if(!_date.is_not_a_date_time())
			{
				map.insert(PARAMETER_DATE, _date);
			}

			// End date
			if(!_endDate.is_not_a_date_time() && _endDate != _date)
			{
				map.insert(PARAMETER_END_DATE, _endDate);
			}

			return map;
		}



		void GetMessagesFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// Recipient class
			string recipientClass(map.getDefault<string>(PARAMETER_RECIPIENT_KEY));
			if(!recipientClass.empty())
			{
				if(!Factory<AlarmRecipient>::contains(recipientClass))
				{
					throw RequestException("No such recipient class");
				}
				_recipientClass.reset(Factory<AlarmRecipient>::create(recipientClass));
			}

			// Recipient
			try
			{
				_recipient = DBModule::GetEditableObject(
					map.get<RegistryKeyType>(PARAMETER_RECIPIENT_ID),
					*_env
				).get();
			}
			catch(ObjectNotFoundException<Registrable>&)
			{
				throw RequestException("No such recipient");
			}

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

			// End date
			if(!map.getDefault<string>(PARAMETER_END_DATE).empty())
			{
				_endDate = time_from_string(map.get<string>(PARAMETER_END_DATE));
			}
			else
			{
				_endDate = _date;
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
			if(!_cmsTemplate.get())
			{
				setOutputFormatFromMap(map, "");
			}
		}



		util::ParametersMap GetMessagesFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap pm;

			// Getting messages
			AlarmRecipient::ObjectLinks::mapped_type links;
			if(_recipientClass)
			{
				links = _recipientClass->getLinkedAlarms(*_recipient);
			}
			else
			{
				BOOST_FOREACH(const shared_ptr<AlarmRecipient>& ar, Factory<AlarmRecipient>::GetNewCollection())
				{
					const AlarmRecipient::ObjectLinks::mapped_type& ol(ar->getLinkedAlarms(*_recipient));
					links.insert(ol.begin(), ol.end());
				}
			}

			size_t number(0);

			optional<AlarmLevel> bestPriority;
			if(_priorityOrder) // TODO HOW THIS CAN WORK ?
			{
				BOOST_FOREACH(const AlarmRecipient::ObjectLinks::mapped_type::value_type& it, links)
				{
					if( !dynamic_cast<const SentScenario*>(it->getAlarm()->getScenario()) ||
						!static_cast<const SentScenario*>(it->getAlarm()->getScenario())->isApplicable(_date, _endDate)
					){
						continue;
					}
					if(_maxMessagesNumber && number >= *_maxMessagesNumber)
					{
						break;
					}
					if(_bestPriorityOnly && bestPriority && it->getAlarm()->getLevel() != *bestPriority)
					{
						break;
					}
					bestPriority = it->getAlarm()->getLevel();

					shared_ptr<ParametersMap> messagePM(new ParametersMap);
					it->getAlarm()->toParametersMap(*messagePM, true);
					messagePM->insert(ATTR_PARAMETER, it->getParameter());
					pm.insert(DATA_MESSAGE, messagePM);

					++number;
				}
			}
			else
			{
				BOOST_FOREACH(const AlarmRecipient::ObjectLinks::mapped_type::value_type& it, links)
				{
					if( !dynamic_cast<const SentScenario*>(it->getAlarm()->getScenario()) ||
						!static_cast<const SentScenario*>(it->getAlarm()->getScenario())->isApplicable(_date, _endDate)
					){
						continue;
					}
					bestPriority = it->getAlarm()->getLevel();
					break;
				}
				BOOST_REVERSE_FOREACH(const AlarmRecipient::ObjectLinks::mapped_type::value_type& it, links)
				{
					if( !dynamic_cast<const SentScenario*>(it->getAlarm()->getScenario()) ||
						!static_cast<const SentScenario*>(it->getAlarm()->getScenario())->isApplicable(_date, _endDate)
					){
						continue;
					}
					if(_maxMessagesNumber && number >= *_maxMessagesNumber)
					{
						break;
					}
					if(_bestPriorityOnly && it->getAlarm()->getLevel() != *bestPriority)
					{
						continue;
					}
					shared_ptr<ParametersMap> messagePM(new ParametersMap);
					it->getAlarm()->toParametersMap(*messagePM, true);
					messagePM->insert(ATTR_PARAMETER, it->getParameter());
					pm.insert(DATA_MESSAGE, messagePM);
					++number;
				}
			}

			if(_cmsTemplate.get())
			{
				size_t rank(0);
				BOOST_FOREACH(ParametersMap::SubParametersMap::mapped_type::value_type pmMessage, pm.getSubMaps(DATA_MESSAGE))
				{
					// Template parameters
					pmMessage->merge(getTemplateParameters());

					// Rank
					pmMessage->insert(DATA_RANK, rank++);

					// Display
					_cmsTemplate->display(stream, request, *pmMessage);
				}
			}
			else
			{
				outputParametersMap(
					pm,
					stream,
					DATA_MESSAGES,
					"https://extranet.rcsmobility.com/svn/synthese3/trunk/src/17_messages/GetMessagesFunction.xsd"
				);
			}

			return pm;
		}



		bool GetMessagesFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string GetMessagesFunction::getOutputMimeType() const
		{
			return _cmsTemplate.get() ? _cmsTemplate->getMimeType() : getOutputMimeTypeFromOutputFormat("text/plain");
		}



		GetMessagesFunction::GetMessagesFunction(
			util::Registrable* recipient /*= NULL*/,
			boost::shared_ptr<AlarmRecipient> _recipientClass /*= boost::shared_ptr<AlarmRecipient>()*/,
			boost::optional<size_t> maxMessagesNumber /*= boost::optional<size_t>()*/,
			bool bestPriorityOnly /*= true*/,
			bool priorityOrder /*= true*/,
			boost::posix_time::ptime date /*= second_clock::local_time()*/,
			boost::posix_time::ptime endDate /*= second_clock::local_time() */
		):	_recipient(recipient),
			_recipientClass(_recipientClass),
			_maxMessagesNumber(maxMessagesNumber),
			_bestPriorityOnly(bestPriorityOnly),
			_priorityOrder(priorityOrder),
			_date(date),
			_endDate(endDate)
		{}
}	}
