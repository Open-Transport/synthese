////////////////////////////////////////////////////////////////////////////////
/// ScenarioSaveAction class header.
///	@file ScenarioSaveAction.h
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

#ifndef SYNTHESE_ScenarioSaveAction_H__
#define SYNTHESE_ScenarioSaveAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "SentScenario.h"
#include "ActionException.h"
#include "MessagesTypes.h"

#include <string>

namespace synthese
{
	namespace server
	{
		class ActionException;
	}

	namespace messages
	{
		class SentAlarm;
		class ScenarioFolder;
		class ScenarioTemplate;

		////////////////////////////////////////////////////////////////////
		/// Scenario parameters update action class.
		/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Updating_a_scenario
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m17Actions refActions
		/// @author Hugues Romain
		class ScenarioSaveAction:
			public util::FactorableTemplate<server::Action, ScenarioSaveAction>
		{
		public:
			static const std::string PARAMETER_CREATE_TEMPLATE;
			static const std::string PARAMETER_START_DATE;
			static const std::string PARAMETER_END_DATE;
			static const std::string PARAMETER_ENABLED;
			static const std::string PARAMETER_SCENARIO_ID;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_FOLDER_ID;
			static const std::string PARAMETER_VARIABLE;
			static const std::string PARAMETER_TEMPLATE;
			static const std::string PARAMETER_MESSAGE_TO_COPY;
			static const std::string PARAMETER_MESSAGE_TO_CREATE;
			static const std::string PARAMETER_RECIPIENT_ID;
			static const std::string PARAMETER_LEVEL;
			static const std::string PARAMETER_RECIPIENT_DATASOURCE_ID;
			static const std::string PARAMETER_RECIPIENT_TYPE;
			static const std::string PARAMETER_SCENARIO_DATASOURCE_ID;

		private:
			//! @name Datasources
			//@{
				boost::shared_ptr<const impex::DataSource> _scenarioDataSource;
				boost::shared_ptr<const impex::DataSource> _recipientDataSource;
			//@}

			//! @name Scenario to update
			//@{
				boost::shared_ptr<Scenario>				_scenario;
				boost::shared_ptr<SentScenario>			_sscenario;
				boost::shared_ptr<ScenarioTemplate>		_tscenario;
			//@}

			//! @name New values
			//@{
				boost::optional<std::string>						_name;
				boost::optional<boost::shared_ptr<ScenarioFolder> >	_folder;
				boost::optional<bool>								_enabled;
				boost::optional<boost::posix_time::ptime>			_startDate;
				boost::optional<boost::posix_time::ptime>			_endDate;
				SentScenario::VariablesMap							_variables;
				boost::optional<std::string>						_messageToCreate;
				boost::optional<std::vector<util::RegistryKeyType> >	_recipients;
				std::string											_recipientType;
				boost::optional<AlarmLevel>							_level;
				std::string											_dataSourceLinkId;
				boost::shared_ptr<SentAlarm>						_message;
				boost::optional<impex::Importable::DataSourceLinks> _dataSourceLinks;
			//@}

			//! @name Action to do
			//@{
				bool _creation;
				boost::shared_ptr<const ScenarioTemplate>	_template;
				boost::shared_ptr<const SentScenario>		_source;
			//@}

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Updating_a_scenario#Request
			//////////////////////////////////////////////////////////////////////////
			/// @author Hugues Romain
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet-rcsmobility.com/projects/synthese/wiki/Updating_a_scenario#Request
			//////////////////////////////////////////////////////////////////////////
			/// @author Hugues Romain
			///	@exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			/** Constructor.
			*/
			ScenarioSaveAction();

			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			virtual bool isAuthorized(const server::Session* session) const;



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

#endif // SYNTHESE_ScenarioSaveAction_H__
