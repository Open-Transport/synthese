
/** NewScenarioSendAction class header.
	@file NewScenarioSendAction.h

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

#ifndef SYNTHESE_NewScenarioSendAction_H__
#define SYNTHESE_NewScenarioSendAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace messages
	{
		class ScenarioTemplate;
		class SentScenario;

		/** Action class : sends a new message.
			@ingroup m17Actions refActions
			
			The new message can be created by 3 ways :
				- copy of an existent message (single message or scenario)
				- creation of a sent scenario, according to a scenario template
				- creation of a blank single message (no text predefined, no recipient)
			
			The action method is determinated by two parameters :
			
			<table><tr><th>PARAMETER_TEMPLATE=-1</th><th>PARAMETER_TEMPLATE>0</th></tr>
			<tr><th>PARAMETER_MESSAGE_TO_COPY=-1</th><td>Blank single message</td><td>Sent scenario from the template</td></tr>
			<tr><th>PARAMETER_MESSAGE_TO_COPY>0</th><td colspan="2">Copy of the selected message or scenario</td></tr>
			</table>
		*/
		class NewScenarioSendAction
		:	public util::FactorableTemplate<server::Action, NewScenarioSendAction>
		{
		public:
			static const std::string PARAMETER_TEMPLATE;
			static const std::string PARAMETER_MESSAGE_TO_COPY;

		private:
			boost::shared_ptr<const ScenarioTemplate>	_template;
			
			boost::shared_ptr<const SentScenario>		_scenarioToCopy;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			server::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run();

			virtual bool _isAuthorized() const;
		};
	}
}

#endif // SYNTHESE_NewScenarioSendAction_H__
