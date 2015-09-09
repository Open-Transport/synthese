
/** ScenarioStopAction class header.
	@file ScenarioStopAction.h

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

#ifndef SYNTHESE_ScenarioStopAction_H__
#define SYNTHESE_ScenarioStopAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include <boost/property_tree/ptree.hpp>

namespace synthese
{
	namespace impex
	{
		class DataSource;
	}

	namespace messages
	{
		class SentScenario;

		/** ScenarioStopAction action class.
			@ingroup m17Actions refActions
		*/
		class ScenarioStopAction:
			public util::FactorableTemplate<server::Action, ScenarioStopAction>
		{
		public:
			static const std::string PARAMETER_SCENARIO_ID;
			static const std::string PARAMETER_DATASOURCE_ID;
			static const std::string PARAMETER_REMOVE;
			static const std::string PARAMETER_ARCHIVE;

		private:
			SentScenario*	_scenario;
			const impex::DataSource* _dataSource;
			const boost::posix_time::ptime	_stopDateTime;
			bool _remove;
			bool _archive;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
				Removes the used parameters from the map.
				@exception ActionException Occurs when some parameters are missing or incorrect.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			ScenarioStopAction();

			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			virtual bool isAuthorized(const server::Session* session) const;

			SentScenario* findScenarioByMessagesAndCalendars(
				boost::optional<boost::property_tree::ptree> messagesAndCalendars,
				bool compareRecipients = true
			);

			void setScenario(SentScenario* value){ _scenario = value; }

			void setRemove(bool remove) { _remove = remove; }

			void setArchive(bool archive) { _archive = archive; }

			void deleteAlarmObjectLinks(
				boost::optional<boost::property_tree::ptree> messagesAndCalendars,
				server::Request& request
			);
		};
	}
}

#endif // SYNTHESE_ScenarioStopAction_H__
