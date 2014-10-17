////////////////////////////////////////////////////////////////////////////////
/// ScenarioTemplate class header.
///	@file ScenarioTemplate.h
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

#ifndef SYNTHESE_ScenarioTemplate_h__
#define SYNTHESE_ScenarioTemplate_h__

#include "Scenario.h"
#include "SentScenario.h"

#include <map>
#include <string>

namespace synthese
{
	namespace messages
	{
		class AlarmTemplate;
		class ScenarioFolder;

		////////////////////////////////////////////////////////////////////
		/// Scenario template class.
		///	@ingroup m17
		///
		/// Variables handling : scan all the contained messages to find all
		/// variables declarations and fill in the _variables attribute
		class ScenarioTemplate:
			public Scenario
		{
		public:
			static const std::string DATA_SCENARIO_ID;
			static const std::string DATA_NAME;
			static const std::string DATA_FOLDER_ID;
			static const std::string DATA_FOLDER_NAME;
			static const std::string DATA_IS_TEMPLATE;
			static const std::string DATA_CODE;
			static const std::string DATA_HELP_MESSAGE;
			static const std::string DATA_REQUIRED;

			static const std::string TAG_VARIABLE;
			static const std::string TAG_MESSAGE;
			static const std::string TAG_SECTION;
			static const std::string TAG_CALENDAR;

			struct Variable
			{
				std::string code;
				std::string helpMessage;
				bool		compulsory;
			};

			////////////////////////////////////////////////////////////////////
			/// Left : variable code
			/// Right : Help message
			typedef std::map<std::string, Variable> VariablesMap;

		private:
			ScenarioFolder*		_folder;
			VariablesMap		_variables;

		public:
			/// @name constructors and destructor
			//@{

				/** Basic constructor.
				 *
				 * @param name Name of the scenario template
				 * @param folderId ID of the folder of the scenario
				 */
				ScenarioTemplate(
					const std::string name = std::string(),
					ScenarioFolder* folder = NULL
				);


				/** Copy constructor.
				 *
				 * @param source
				 * @param name
				 */
				ScenarioTemplate(
					const ScenarioTemplate& source,
					const std::string& name
				);

				ScenarioTemplate(
					const SentScenario& source,
					const std::string& name
				);

				ScenarioTemplate(util::RegistryKeyType key);
				~ScenarioTemplate();
			//@}

			/// @name Getters
			//@{
				ScenarioFolder*	getFolder()	const;
				const VariablesMap& getVariables() const;
			//@}

			/// @name Setters
			//@{
				void setFolder(ScenarioFolder* value);
				void setVariablesMap(const VariablesMap& value);
			//@}

			/// @name Modifiers
			//@{
			//@}



			/** Parses a string to find variables informations and stores it into a variables list.
				@param text text to parse
				@param result variables list to populate
				@author Hugues Romain
				@date 2009

				The method searches a special pattern in the text :
				 - $xxx$ = optional variable named xxx
				 - $$xxx$ = compulsory variable named xxx
				 - $$$ = $ character
				 - $xxx|yyy$ = optional variable named xxx, with additional informations to display yyy
				 - $$xxx|yyy$ = compulsory variable named xxx with additional informations to display yyy
			*/
			static void GetVariablesInformations(
				const std::string& text,
				ScenarioTemplate::VariablesMap& result
			);



			/** Creates a string from the template text and the variables content.
				@param text text to parse
				@param values variables values
				@return std::string generated text
				@author Hugues Romain
				@date 2009

				The following replacements are done by the method:
				 - $xxx$ => value of the xxx variable (nothing if undefined)
				 - $$xxx$ => value of the xxx variable (nothing if undefined)
				 - $$$ => $
				 - $xxx|yyy$ => value of the xxx variable (nothing if undefined)
				 - $$xxx|yyy$ => value of the xxx variable (nothing if undefined)

				@warning the presence of compulsory variables is not controlled by this method. Use ControlCompulsoryVariables to do it.
			*/
			static std::string WriteTextFromVariables(
				const std::string& text,
				const SentScenario::VariablesMap& values
			);




			/** Checks that all compulsory variables are valued.
				@param variables variables informations
				@param values values of the variables
				@return bool true if each compulsory variable is defined and non empty
				@author Hugues Romain
				@date 2009
			*/
			static bool CheckCompulsoryVariables(
				const ScenarioTemplate::VariablesMap& variables,
				const SentScenario::VariablesMap& values
			);



			//////////////////////////////////////////////////////////////////////////
			/// Export of the content of the object into a ParametersMap.
			/// @param pm the ParametersMap object to populate
			/// @author Hugues Romain
			/// @date 2012
			virtual void toParametersMap(
				util::ParametersMap& pm,
				bool withAdditionalParameters,
				boost::logic::tribool withFiles = boost::logic::indeterminate,
				std::string prefix = std::string()
			) const;
		};
	}
}

#endif // SYNTHESE_ScenarioTemplate_h__
