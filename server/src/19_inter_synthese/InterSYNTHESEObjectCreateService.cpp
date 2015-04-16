
//////////////////////////////////////////////////////////////////////////////////////////
///	InterSYNTHESEObjectCreateService class implementation.
///	@file InterSYNTHESEObjectCreateService.cpp
///	@author Hugues Romain
///	@date 2012
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

#include "InterSYNTHESEObjectCreateService.hpp"

#include "InterSYNTHESEQueue.hpp"
#include "InterSYNTHESESyncTypeFactory.hpp"
#include "RequestException.h"
#include "Request.h"
#include "ServerConstants.h"
#include "ServerModule.h"

//#include <boost/algorithm/string.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace server;

	template<>
	const string FactorableTemplate<ObjectCreateService,inter_synthese::InterSYNTHESEObjectCreateService>::FACTORY_KEY = "inter_synthese_object_create";
	
	namespace inter_synthese
	{
		const string InterSYNTHESEObjectCreateService::QUEUE_IDS_SEPARATOR = ",";
		const string InterSYNTHESEObjectCreateService::NO_ITEM_IN_QUEUE = "item_created_not_found_in_queues";

		const string InterSYNTHESEObjectCreateService::FORMAT_JSON("json");

		const string InterSYNTHESEObjectCreateService::PARAMETER_GETQUEUEID = "getqueueid";

		const string InterSYNTHESEObjectCreateService::ATTR_QUEUEIDS = "queue_ids";
		const string InterSYNTHESEObjectCreateService::TAG_OBJECT_AND_INTERSYNTHESE_KEYS = "object_and_intersynthese_keys";



		ParametersMap InterSYNTHESEObjectCreateService::_getParametersMap() const
		{
			ParametersMap map;
			map = db::ObjectCreateService::_getParametersMap();

			if (_queueIdsRequested)
			{
				map.insert(PARAMETER_GETQUEUEID, _queueIdsRequested);
			}
			// Output format
			if(!_outputFormat.empty())
			{
				map.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}
			return map;
		}



		void InterSYNTHESEObjectCreateService::_setFromParametersMap(const ParametersMap& map)
		{
			// First, call of the mother class method
			db::ObjectCreateService::_setFromParametersMap(map);
			// Output format
			_outputFormat = map.getDefault<string>(PARAMETER_OUTPUT_FORMAT);
			// Then, check if the queue ids are requested
			_queueIdsRequested = map.getDefault<bool>(PARAMETER_GETQUEUEID, false);
		}



		ParametersMap InterSYNTHESEObjectCreateService::run(
			std::ostream& stream,
			const Request& request
		) const {
			// First, call of the mother class method
			ParametersMap map = ObjectCreateService::run(stream, request);

			// Using the id of the created object, find the matching queue id
			// of this new entry in the queue of each slave
			string content;

			if (_queueIdsRequested)
			{
				QueueIds queueIds;
				BOOST_FOREACH(
					const InterSYNTHESEQueue::Registry::value_type& it,
					Env::GetOfficialEnv().getRegistry<InterSYNTHESEQueue>()
				){
					// Creating stringstream containing 'rstmt:table_id', id of the table of the new object
					stringstream tableIdStream;
					tableIdStream << "rstmt:";
					RegistryTableType tableId(decodeTableId(_value->getKey()));
					tableIdStream << tableId;
					// Filling the content string with the content field of the item in the queue
					content = it.second->get<SyncContent>();
					// Getting the id of the new object in its table
					stringstream keystream;
					keystream << _value->getKey();
					// Look for the string 'rstmt:table_id' in the content of the queue item
					// and Look for the id of the new object in the content of the queue item
					if ((string::npos != content.find(tableIdStream.str())) && (string::npos != content.find(keystream.str())))
					{
						// New object has been found in the queue, let's store its queue id
						queueIds.insert(it.second->get<Key>());
					}
				}

				// If the new object has not been found in the queue
				if (queueIds.empty())
				{
					stream << NO_ITEM_IN_QUEUE;
				}
				else
				{
					// Serialize the _queueIds set and insert it in the map
					stringstream queueIdsStr;
					unsigned int i = 1;
					BOOST_FOREACH(
						const QueueIds::value_type& qId,
						queueIds
					){
						if (i == queueIds.size())
						{
							queueIdsStr << qId;
						}
						else
						{
							queueIdsStr << qId << QUEUE_IDS_SEPARATOR;
						}
						i++;
					}
					map.insert(ATTR_QUEUEIDS, queueIdsStr.str());
				}
			}
			if (_outputFormat == FORMAT_JSON)
			{
				map.outputJSON(stream, TAG_OBJECT_AND_INTERSYNTHESE_KEYS);
			}
			return map;
		}
		
		
		
		bool InterSYNTHESEObjectCreateService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		InterSYNTHESEObjectCreateService::InterSYNTHESEObjectCreateService():
			_queueIdsRequested(false)
        {}



		std::string InterSYNTHESEObjectCreateService::getOutputMimeType() const
		{
			std::string mimeType;
			if(_outputFormat == FORMAT_JSON)
			{
				mimeType = "application/json";
			}
			else // For empty result
			{
				mimeType = "text/html";
			}
			return mimeType;
		}



}	}
