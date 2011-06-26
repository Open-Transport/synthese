
/** MessagesObjectsCMSExporters class implementation.
	@file MessagesObjectsCMSExporters.cpp

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

#include "MessagesObjectsCMSExporters.hpp"
#include "Alarm.h"
#include "Webpage.h"
#include "ParametersMap.h"
#include "Scenario.h"
#include "Request.h"

using namespace std;

namespace synthese
{
	using namespace cms;
	using namespace server;
	using namespace util;

	namespace messages
	{
		const string MessagesObjectsCMSExporters::DATA_CONTENT("content");
		const string MessagesObjectsCMSExporters::DATA_PRIORITY("priority");
		const string MessagesObjectsCMSExporters::DATA_SCENARIO_ID("scenario_id");
		const string MessagesObjectsCMSExporters::DATA_SCENARIO_NAME("scenario_name");
		const string MessagesObjectsCMSExporters::DATA_TITLE("title");



		void MessagesObjectsCMSExporters::DisplayMessage(
			std::ostream& stream,
			const server::Request& request,
			boost::shared_ptr<const cms::Webpage> page,
			const Alarm& message
		){

			if(!page.get())
			{
				return;
			}
			ParametersMap pm(request.getFunction()->getSavedParameters());

			pm.insert(Request::PARAMETER_OBJECT_ID, message.getKey());
			pm.insert(DATA_TITLE, message.getShortMessage());
			pm.insert(DATA_CONTENT, message.getLongMessage());
			pm.insert(DATA_PRIORITY, static_cast<int>(message.getLevel()));
			pm.insert(DATA_SCENARIO_ID, message.getScenario()->getKey());
			pm.insert(DATA_SCENARIO_NAME, message.getScenario()->getName());

			page->display(stream, request, pm);
		}
}	}
