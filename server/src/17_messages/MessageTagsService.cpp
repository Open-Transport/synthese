
//////////////////////////////////////////////////////////////////////////////////////////
///	MessageTagsService class implementation.
///	@file MessageTagsService.cpp
///	@author Camille Hue
///	@date 2015
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

#include "MessageTagsService.hpp"

#include "MessageTag.hpp"
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
	const string FactorableTemplate<Function,messages::MessageTagsService>::FACTORY_KEY = "message_tags";


	
	namespace messages
	{
		const string MessageTagsService::TAG_TAG = "tag";
		const string MessageTagsService::TAG_TAGS = "tags";
		


		ParametersMap MessageTagsService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void MessageTagsService::_setFromParametersMap(const ParametersMap& map)
		{
			// Message tag
			RegistryKeyType tagId(
				map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0)
			);
			if(tagId > 0)
			{
				try
				{
					_messageTag = Env::GetOfficialEnv().get<MessageTag>(tagId).get();
				}
				catch (ObjectNotFoundException<MessageTag>&)
				{
					throw RequestException("No such message tag");
				}
			}

			Function::setOutputFormatFromMap(map,string());
		}



		ParametersMap MessageTagsService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap pm;

			if(_messageTag)
			{
				boost::shared_ptr<ParametersMap> tagPM(new ParametersMap);
				_messageTag->toParametersMap(*tagPM);
				pm.insert(TAG_TAG, tagPM);
			}
			else // Entire registry
			{
				BOOST_FOREACH(const MessageTag::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<MessageTag>())
				{
					boost::shared_ptr<ParametersMap> tagPM(new ParametersMap);
					it.second->toParametersMap(*tagPM);
					pm.insert(TAG_TAG, tagPM);
				}
			}

			if (_outputFormat == MimeTypes::JSON)
			{
				pm.outputJSON(stream, TAG_TAGS);
			}

			return pm;
		}
		
		
		
		bool MessageTagsService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string MessageTagsService::getOutputMimeType() const
		{
			return "text/html";
		}



		MessageTagsService::MessageTagsService():
			_messageTag(NULL)
		{

		}
}	}
