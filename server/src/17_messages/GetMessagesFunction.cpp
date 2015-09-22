
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
#include "BroadcastPoint.hpp"
#include "DBModule.h"
#include "MessagesModule.h"
#include "RequestException.h"
#include "Request.h"
#include "Webpage.h"
#include "Alarm.h"
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
			// Broadcast point
			RegistryKeyType broadcastPointId(
				map.get<RegistryKeyType>(PARAMETER_RECIPIENT_ID)
			);
			try
			{
				const Registrable* object(
					DBModule::GetObject(broadcastPointId, Env::GetOfficialEnv()).get()
				);
				if(dynamic_cast<const BroadcastPoint*>(object))
				{
					_broadcastPoint = dynamic_cast<const BroadcastPoint*>(object);
				}
			}
			catch(ObjectNotFoundException<Registrable>&)
			{
				throw RequestException("No such broadcast object");
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

			// Parameters
			_parameters.merge(map);
			_parameters.remove(PARAMETER_CMS_TEMPLATE_ID);
			_parameters.remove(PARAMETER_RECIPIENT_ID);
			_parameters.remove(PARAMETER_MAX_MESSAGES_NUMBER);
			_parameters.remove(PARAMETER_BEST_PRIORITY_ONLY);
			_parameters.remove(PARAMETER_PRIORITY_ORDER);
			_parameters.remove(PARAMETER_DATE);
			_parameters.remove(PARAMETER_END_DATE);
			_parameters.remove(PARAMETER_OUTPUT_FORMAT);
		}



		util::ParametersMap GetMessagesFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap pm;

			// Getting messages
			MessagesModule::ActivatedMessages messages;
			if (_broadcastPoint)
			{
				// If the date is now (more or less 60 seconds), use the cached function
				time_duration diff = second_clock::local_time() - _date;
				if ( abs(diff.total_seconds()) < 60 )
				{
					messages = MessagesModule::GetActivatedMessages(
						*_broadcastPoint,
						_parameters
					);
				}
				else
				{
					messages = MessagesModule::GetActivatedMessagesAt(
						*_broadcastPoint,
						_parameters,
						_date
					);
				}
			}

			size_t number(0);

			// This code works because of "messages" priority sorting
			optional<AlarmLevel> bestPriority;
			if(_priorityOrder)
			{
				BOOST_FOREACH(boost::shared_ptr<const Alarm> message, messages)
				{
					if(_maxMessagesNumber && number >= *_maxMessagesNumber)
					{
						break;
					}
					if(_bestPriorityOnly && bestPriority && message->getLevel() != *bestPriority)
					{
						break;
					}
					bestPriority = message->getLevel();

					boost::shared_ptr<ParametersMap> messagePM(new ParametersMap);
					message->toParametersMap(*messagePM, true, string(), true);
					pm.insert(DATA_MESSAGE, messagePM);

					++number;
				}
			}
			else
			{
				BOOST_FOREACH(boost::shared_ptr<Alarm> message, messages)
				{
					bestPriority = message->getLevel();
					break;
				}
				BOOST_REVERSE_FOREACH(boost::shared_ptr<Alarm> message, messages)
				{
					if(_maxMessagesNumber && number >= *_maxMessagesNumber)
					{
						break;
					}
					if(_bestPriorityOnly && message->getLevel() != *bestPriority)
					{
						continue;
					}
					boost::shared_ptr<ParametersMap> messagePM(new ParametersMap);
					message->toParametersMap(*messagePM, true, string(), true);
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
			const BroadcastPoint* broadcastPoint,
			util::ParametersMap parameters,
			boost::optional<size_t> maxMessagesNumber /*= boost::optional<size_t>()*/,
			bool bestPriorityOnly /*= true*/,
			bool priorityOrder /*= true*/,
			boost::posix_time::ptime date /*= second_clock::local_time()*/,
			boost::posix_time::ptime endDate /*= second_clock::local_time() */
		):	_broadcastPoint(broadcastPoint),
			_parameters(parameters),
			_maxMessagesNumber(maxMessagesNumber),
			_bestPriorityOnly(bestPriorityOnly),
			_priorityOrder(priorityOrder),
			_date(date),
			_endDate(endDate)
		{}
}	}
