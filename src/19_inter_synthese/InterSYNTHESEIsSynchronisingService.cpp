
//////////////////////////////////////////////////////////////////////////////////////////
///	InterSYNTHESEIsSynchronisingService class implementation.
///	@file InterSYNTHESEIsSynchronisingService.cpp
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

#include "InterSYNTHESEIsSynchronisingService.hpp"

#include "InterSYNTHESEQueue.hpp"
#include "InterSYNTHESESyncTypeFactory.hpp"
//#include "RequestException.h"
#include "Function.h"
#include "Log.h"

#include <boost/algorithm/string.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;

	template<>
	const string FactorableTemplate<server::Function,inter_synthese::InterSYNTHESEIsSynchronisingService>::FACTORY_KEY = "is_synchronising";
	
	namespace inter_synthese
	{
		const string InterSYNTHESEIsSynchronisingService::ATTR_IS_SYNCHRONISING = "is_synchronising";
		const string InterSYNTHESEIsSynchronisingService::PARAMETER_QUEUEIDS = "queue_ids";
		const string InterSYNTHESEIsSynchronisingService::TAG_INTERSYNTHESE_SYNCHRONISATION = "inter_synthese_synchronisation";

		const string InterSYNTHESEIsSynchronisingService::FORMAT_JSON("json");
		const string InterSYNTHESEIsSynchronisingService::QUEUE_IDS_SEPARATOR = ",";



		ParametersMap InterSYNTHESEIsSynchronisingService::_getParametersMap() const
		{
			ParametersMap map;
			// Output format
			if(!_outputFormat.empty())
			{
				map.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}
			if (!_queueIds.empty())
			{
				stringstream idsStream;
				unsigned int i = 1;
				BOOST_FOREACH(
					const QueueIds::value_type& qId,
					_queueIds)
				{
					if (i == _queueIds.size())
					{
						idsStream << qId;
					}
					else
					{
						idsStream << qId << QUEUE_IDS_SEPARATOR;
					}
				}
				map.insert(PARAMETER_QUEUEIDS, idsStream.str());
			}
			return map;
		}



		void InterSYNTHESEIsSynchronisingService::_setFromParametersMap(const ParametersMap& map)
		{
			// Output format
			_outputFormat = map.getDefault<string>(PARAMETER_OUTPUT_FORMAT);
			string queueIdsStr = map.getDefault<string>(PARAMETER_QUEUEIDS, "");
			// Split of the queueIdStr variable
			vector<string> queueIdVect;
			split(queueIdVect, queueIdsStr, is_any_of(","), token_compress_on);
			BOOST_FOREACH(string idstr, queueIdVect)
			{
				_queueIds.insert(lexical_cast<RegistryKeyType>(idstr));
			}
		}



		ParametersMap InterSYNTHESEIsSynchronisingService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			bool is_one_slave_synchronising = false;
			if (!_queueIds.empty())
			{
				BOOST_FOREACH(
					const QueueIds::value_type& qId,
					_queueIds
				)
				{
					bool queueIdExists(Env::GetOfficialEnv().getRegistry<InterSYNTHESEQueue>().contains(qId));
					if (queueIdExists)
					{
						is_one_slave_synchronising = true;
					}
				}
			}
			map.insert(ATTR_IS_SYNCHRONISING, _queueIds.empty() ? false : is_one_slave_synchronising);
			if (_outputFormat == FORMAT_JSON)
			{
				map.outputJSON(stream, TAG_INTERSYNTHESE_SYNCHRONISATION);
			}
			return map;
		}
		
		
		
		bool InterSYNTHESEIsSynchronisingService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string InterSYNTHESEIsSynchronisingService::getOutputMimeType() const
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
