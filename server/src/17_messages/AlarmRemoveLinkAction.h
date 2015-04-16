////////////////////////////////////////////////////////////////////////////////
/// AlarmRemoveLinkAction class header.
///	@file AlarmRemoveLinkAction.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_AlarmRemoveLinkAction_H__
#define SYNTHESE_AlarmRemoveLinkAction_H__

#include "FactorableTemplate.h"
#include "Action.h"
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

		////////////////////////////////////////////////////////////////////
		/// Alarm remove link action class.
		/// @ingroup m17Actions refActions
		class AlarmRemoveLinkAction
		:	public util::FactorableTemplate<server::Action,AlarmRemoveLinkAction>
		{
		public:
			static const std::string PARAMETER_ALARM_ID;
			static const std::string PARAMETER_OBJECT_ID;

		private:
			boost::shared_ptr<const Alarm>	_alarm;
			util::RegistryKeyType		_objectId;

		protected:
			////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			util::ParametersMap getParametersMap() const;



			////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	Removes the used parameters from the map.
			///	@throws ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map) throw(server::ActionException);

		public:


			////////////////////////////////////////////////////////////////////
			///	Message setter.
			///	@param id ID of the message to edit
			///	@author Hugues Romain
			/// Loads the alarm from the specified ID
			/// @throws ActionException if the message was not found
			void setAlarmId(util::RegistryKeyType id) throw (server::ActionException);

			void setObjectId(util::RegistryKeyType id);


			////////////////////////////////////////////////////////////////////
			/// Action to run, defined by each subclass.
			void run(
				server::Request& request
			) throw(server::ActionException);

			virtual bool isAuthorized(const server::Session* session) const;
		};
	}
}

#endif // SYNTHESE_AlarmRemoveLinkAction_H__
