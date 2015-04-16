
/** ForumTopicsFunction class implementation.
	@file ForumTopicsFunction.cpp
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
#include "ForumInterfacePage.hpp"
#include "ForumTopicsFunction.hpp"
#include "ForumTopicTableSync.hpp"
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

	template<> const string util::FactorableTemplate<Function,forum::ForumTopicsFunction>::FACTORY_KEY("forum_topics");

	namespace forum
	{
		const string ForumTopicsFunction::PARAMETER_NODE_ID("node_id");
		const string ForumTopicsFunction::PARAMETER_NODE_EXTENSION("node_extension");
		const string ForumTopicsFunction::PARAMETER_DISPLAY_TEMPLATE("page");
		const string ForumTopicsFunction::PARAMETER_DATE_DISPLAY_TEMPLATE("date_page");
		const string ForumTopicsFunction::PARAMETER_RAISING_ORDER("sort");

		ParametersMap ForumTopicsFunction::_getParametersMap() const
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
			map.insert(PARAMETER_NODE_ID, _nodeId);
			map.insert(PARAMETER_NODE_EXTENSION, _nodeExtension);
			map.insert(PARAMETER_RAISING_ORDER, _raisingOrder);
			return map;
		}

		void ForumTopicsFunction::_setFromParametersMap(const ParametersMap& map)
		{
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
			_nodeId = map.get<RegistryKeyType>(PARAMETER_NODE_ID);
			_nodeExtension = map.getDefault<string>(PARAMETER_NODE_EXTENSION);
			_raisingOrder = map.getDefault<bool>(PARAMETER_RAISING_ORDER, false);
		}



		util::ParametersMap ForumTopicsFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			ForumTopicTableSync::SearchResult topics(
				ForumTopicTableSync::Search(
					*_env,
					_nodeId,
					_nodeExtension,
					optional<string>(),
					optional<RegistryKeyType>(),
					true,
					0,
					optional<size_t>(),
					true,
					_raisingOrder
			)	);
			size_t rank(0);
			BOOST_FOREACH(ForumTopicTableSync::SearchResult::value_type topic, topics)
			{
				if(_displayTemplate.get())
				{
					ForumInterfacePage::DisplayTopic(stream, *_displayTemplate, _dateDisplayTemplate, request, *topic, rank);
				}
				++rank;
			}

			return util::ParametersMap();
		}



		bool ForumTopicsFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string ForumTopicsFunction::getOutputMimeType() const
		{
			return _displayTemplate.get() ? _displayTemplate->getMimeType() : "text/xml";
		}
	}
}
