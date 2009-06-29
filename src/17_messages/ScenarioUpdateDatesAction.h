////////////////////////////////////////////////////////////////////////////////
/// ScenarioUpdateDatesAction class header.
///	@file ScenarioUpdateDatesAction.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#ifndef SYNTHESE_ScenarioUpdateDatesAction_H__
#define SYNTHESE_ScenarioUpdateDatesAction_H__

#include "DateTime.h"
#include "Action.h"
#include "FactorableTemplate.h"
#include "SentScenario.h"
#include "ActionException.h"

#include <string>

namespace synthese
{
	namespace server
	{
		class ActionException;
	}

	namespace messages
	{
		class Scenario;
		class ScenarioFolder;
		class ScenarioTemplate;

		////////////////////////////////////////////////////////////////////
		/// Scenario parameters update action class.
		///
		/// This action updates :
		///  - for a scenario template :
		///    - the scenario name
		///    - the scenario folder
		///  - for a sent scenario :
		///    - the scenario name
		///    - the broadcast dates
		///    - the activation status
		///    - the content of the variables
		///
		///	@ingroup m17Actions refActions
		class ScenarioUpdateDatesAction:
			public util::FactorableTemplate<server::Action, ScenarioUpdateDatesAction>
		{
		public:
			static const std::string PARAMETER_START_DATE;
			static const std::string PARAMETER_END_DATE;
			static const std::string PARAMETER_ENABLED;
			static const std::string PARAMETER_VARIABLE;
			static const std::string PARAMETER_SCENARIO_ID;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_FOLDER_ID;

		private:
			std::string								_name;
			boost::shared_ptr<ScenarioFolder>		_folder;
			bool									_enabled;
			time::DateTime							_startDate;
			time::DateTime							_endDate;
			boost::shared_ptr<Scenario>				_scenario;
			boost::shared_ptr<SentScenario>			_sscenario;
			boost::shared_ptr<ScenarioTemplate>		_tscenario;
			SentScenario::VariablesMap				_variables;

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
			/** Constructor.
			*/
			ScenarioUpdateDatesAction();

			/** Action to run, defined by each subclass.
			*/
			void run();

			virtual bool _isAuthorized() const;

			
			
			////////////////////////////////////////////////////////////////////
			///	Scenario setter.
			///	@param id id of scenario
			///	@author Hugues Romain
			///	@date 2009
			void setScenarioId(
				const util::RegistryKeyType id
			) throw(server::ActionException);
		};
	}
}

#endif // SYNTHESE_ScenarioUpdateDatesAction_H__
