
/** ForumMessagesFunction class implementation.
	@file ForumMessagesFunction.cpp
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

#include "RequestException.h"
#include "Request.h"
#include "ForumTopicTableSync.hpp"
#include "ForumMessageTableSync.hpp"
#include "ForumMessagesFunction.hpp"
#include "ForumInterfacePage.hpp"
#include "Webpage.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;

	template<> const string util::FactorableTemplate<Function,forum::ForumMessagesFunction>::FACTORY_KEY("forum_messages");

	namespace forum
	{
		const string ForumMessagesFunction::PARAMETER_TOPIC_ID("topic_id");
		const string ForumMessagesFunction::PARAMETER_DISPLAY_TEMPLATE("page");
		const string ForumMessagesFunction::PARAMETER_DATE_DISPLAY_TEMPLATE("date_page");



		ParametersMap ForumMessagesFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_displayTemplate.get())
			{
				map.insert(PARAMETER_DISPLAY_TEMPLATE, _displayTemplate->getKey());
			}
			if(_dateDisplayTemplate.get())
			{
				map.insert(PARAMETER_DATE_DISPLAY_TEMPLATE, _dateDisplayTemplate->getKey());
			}
			if(_topic.get())
			{
				map.insert(PARAMETER_TOPIC_ID, _topic->getKey());
			}
			return map;
		}

		void ForumMessagesFunction::_setFromParametersMap(const ParametersMap& map)
		{
			try
			{
				_topic = ForumTopicTableSync::Get(map.get<RegistryKeyType>(PARAMETER_TOPIC_ID), *_env);
			}
			catch(ObjectNotFoundException<ForumTopic>&)
			{
				throw RequestException("No such topic");
			}
			{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_DISPLAY_TEMPLATE));
				if(id) try
				{
					_displayTemplate = Env::GetOfficialEnv().get<Webpage>(*id);
				}
				catch (ObjectNotFoundException<Webpage>&)
				{
					throw RequestException("No such display template");
				}
				_templateParameters.remove(PARAMETER_DISPLAY_TEMPLATE);
			}{
				optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_DATE_DISPLAY_TEMPLATE));
				if(id) try
				{
					_dateDisplayTemplate = Env::GetOfficialEnv().get<Webpage>(*id);
				}
				catch (ObjectNotFoundException<Webpage>&)
				{
					throw RequestException("No such date display template");
				}
				_templateParameters.remove(PARAMETER_DATE_DISPLAY_TEMPLATE);
			}
		}



		util::ParametersMap ForumMessagesFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			ForumMessageTableSync::SearchResult messages(
				ForumMessageTableSync::Search(
					*_env,
					_topic->getKey(),
					optional<string>(),
					optional<RegistryKeyType>(),
					true,
					0,
					optional<size_t>(),
					true,
					true
			)	);
			size_t rank(0);
			BOOST_FOREACH(ForumMessageTableSync::SearchResult::value_type message, messages)
			{
				if(_displayTemplate.get())
				{
					ForumInterfacePage::DisplayMessage(stream, *_displayTemplate, _dateDisplayTemplate, request, *message, rank);
				}
				++rank;
			}

			return util::ParametersMap();
		}



		bool ForumMessagesFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string ForumMessagesFunction::getOutputMimeType() const
		{
			return _displayTemplate.get() ? _displayTemplate->getMimeType() : "text/xml";
		}
	}
}
