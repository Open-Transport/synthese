
/** UpdateAlarmMessagesFromTemplateAction class header.
	@file UpdateAlarmMessagesFromTemplateAction.h

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

#ifndef SYNTHESE_UpdateAlarmMessagesFromTemplateAction_H__
#define SYNTHESE_UpdateAlarmMessagesFromTemplateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "ActionException.h"

#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace server
	{
		class ActionException;
	}

	namespace messages
	{
		class Alarm;

		/** UpdateAlarmMessagesFromTemplateAction action class.
			@ingroup m17Actions refActions
		*/
		class UpdateAlarmMessagesFromTemplateAction : public util::FactorableTemplate<server::Action, UpdateAlarmMessagesFromTemplateAction>
		{
		public:
			static const std::string PARAMETER_ALARM_ID;
			static const std::string PARAMETER_TEMPLATE_ID;

		private:
			boost::shared_ptr<const Alarm>	_template;
			boost::shared_ptr<Alarm>		_message;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(
				const util::ParametersMap& map
			) throw(server::ActionException);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(
				server::Request& request
			) throw(server::ActionException);

			void setAlarmId(
				util::RegistryKeyType id
			) throw(server::ActionException);

			virtual bool isAuthorized(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_UpdateAlarmMessagesFromTemplateAction_H__
