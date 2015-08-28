////////////////////////////////////////////////////////////////////////////////
/// ScenarioSaveAction class header.
///	@file ScenarioSaveAction.h
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

#ifndef SYNTHESE_ScenarioSaveAction_H__
#define SYNTHESE_ScenarioSaveAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "SentScenario.h"
#include "ActionException.h"
#include "MessagesTypes.h"
#include "BaseImportableUpdateAction.hpp"

#include <string>
#include <boost/tuple/tuple.hpp>
#include <boost/property_tree/ptree.hpp>

namespace synthese
{
	namespace server
	{
		class ActionException;
	}

	namespace messages
	{
		class AlarmRecipient;
		class MessageType;
		class Alarm;
		class ScenarioFolder;
		class ScenarioTemplate;

		////////////////////////////////////////////////////////////////////
		/// Scenario parameters update action class.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Updating_a_scenario
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m17Actions refActions
		/// @author Hugues Romain
		class ScenarioSaveAction:
			public util::FactorableTemplate<server::Action, ScenarioSaveAction>,
			public impex::BaseImportableUpdateAction
		{
		public:
			// Parameters for action determination
			static const std::string PARAMETER_CREATE_TEMPLATE;
			static const std::string PARAMETER_TEMPLATE;
			static const std::string PARAMETER_MESSAGE_TO_COPY;
			static const std::string PARAMETER_SCENARIO_ID;
			static const std::string PARAMETER_SCENARIO_DATASOURCE_ID;

			// Parameters for properties
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_ENABLED;
			static const std::string PARAMETER_START_DATE;
			static const std::string PARAMETER_END_DATE;
			static const std::string PARAMETER_START_TIME;
			static const std::string PARAMETER_END_TIME;
			static const std::string PARAMETER_VARIABLE;
			static const std::string PARAMETER_SECTIONS;
			static const std::string PARAMETER_FOLDER_ID;
			static const std::string PARAMETER_ARCHIVED;

			// Parameters for unused properties
			static const std::string PARAMETER_EVENT_START_DATE;
			static const std::string PARAMETER_EVENT_END_DATE;
			static const std::string PARAMETER_EVENT_START_TIME;
			static const std::string PARAMETER_EVENT_END_TIME;

			// Parameters for A/B/G/H message update full method
			static const std::string PARAMETER_JSON;

			/// @name Parameters for A/B/G/H message update simplified method
			//@{
				static const std::string PARAMETER_MESSAGE_TO_CREATE;
				static const std::string PARAMETER_CREATED_MESSAGE_TITLE;
				static const std::string PARAMETER_RECIPIENT_ID;
				static const std::string PARAMETER_LEVEL;
				static const std::string PARAMETER_TAGS;
				static const std::string PARAMETER_DISPLAY_DURATION;
				static const std::string PARAMETER_DIGITIZED_VERSION;
				static const std::string PARAMETER_RECIPIENT_DATASOURCE_ID;
				static const std::string PARAMETER_RECIPIENT_TYPE;
				static const std::string PARAMETER_ENCODING;
				static const std::string PARAMETER_RECIPIENTS_;
				static const std::string PARAMETER_MESSAGE_SECTION;
			//@}

			static const std::string VALUES_SEPARATOR;
			static const std::string VALUES_PARAMETERS_SEPARATOR;

		private:
			//! @name Determination of the action
			//@{
				boost::shared_ptr<Scenario>				_scenario;
				boost::shared_ptr<SentScenario>			_sscenario;
				boost::shared_ptr<ScenarioTemplate>		_tscenario;
				bool _creation;
				boost::shared_ptr<const ScenarioTemplate>	_template;
				boost::shared_ptr<const SentScenario>		_source;
				boost::shared_ptr<const impex::DataSource> _scenarioDataSource;
			//@}

			//! @name Properties
			//@{
				boost::optional<std::string>						_name;
				boost::optional<boost::shared_ptr<ScenarioFolder> >	_folder;
				boost::optional<bool>								_enabled;
				boost::optional<bool>								_archived;
				boost::optional<boost::posix_time::ptime>			_startDate;
				boost::optional<boost::posix_time::ptime>			_endDate;
				boost::optional<boost::posix_time::ptime>			_eventStartDate;
				boost::optional<boost::posix_time::ptime>			_eventEndDate;
				boost::optional<std::set<MessagesSection*> >  _sections;
			//@}

			//! @name Values for A/B/G/H message update full method
			//@{
				boost::optional<boost::property_tree::ptree> _messagesAndCalendars;
			//@}

			//! @name Values for A/B/G/H message update simplified method
			//@{
				boost::shared_ptr<const impex::DataSource> _recipientDataSource;
				boost::optional<std::string>						_messageToCreate;
				boost::optional<std::string>						_messageToCreateTitle;
				typedef 
				boost::optional<
					std::vector<
						std::pair<
							std::string,
							std::pair<util::RegistryKeyType, boost::optional<std::string> >
				>	>	> Recipients;
				Recipients _recipients;
				std::string											_recipientType;
				boost::optional<AlarmLevel>							_level;
				std::string											_tags;
				boost::optional<std::size_t>						_display_duration;
				std::string											_digitizedVersion;
				boost::shared_ptr<const MessagesSection>			_messageSection;
				std::string											_dataSourceLinkId;
				boost::shared_ptr<Alarm>							_message;
			//@}


		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Updating_a_scenario#Request
			//////////////////////////////////////////////////////////////////////////
			/// @author Hugues Romain
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Updating_a_scenario#Request
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

			// Setters
			void setMessagesAndCalendars(
				boost::optional<boost::property_tree::ptree> messagesAndCalendars
			);
			void setScenario(
				boost::shared_ptr<Scenario>	scenario
			);
			void setSScenario(
				boost::shared_ptr<SentScenario> sscenario
			);
		};
	}
}

#endif // SYNTHESE_ScenarioSaveAction_H__
