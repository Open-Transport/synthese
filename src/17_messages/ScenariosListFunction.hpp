
//////////////////////////////////////////////////////////////////////////////////////////
/// ScenariosListFunction class header.
///	@file ScenariosListFunction.hpp
///	@author Hugues Romain
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_ScenariosListFunction_H__
#define SYNTHESE_ScenariosListFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

namespace synthese
{
	namespace cms
	{
		class Webpage;
	}

	namespace messages
	{
		class ScenarioFolder;
		class SentScenario;
		class ScenarioTemplate;

		//////////////////////////////////////////////////////////////////////////
		///	17.15 Function : ScenariosListFunction.
		///	@ingroup m17Functions refFunctions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.2.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : scenarios
		///
		/// Parameters :
		///	<dl>
		///	<dt>t</dt><dd>1|0 : 1 : show templates, 0 : show sent scenarios</dd>
		///	<dt>f</dt><dd>id of the folder (0 = root) (applicable for templates only)</dd>
		/// <dt>p</dt><dd>id of the CMS template to use for the display of each scenario. The template will become the parameters produced by ScenariosListFunction::_displayScenario.</dd>
		/// <dt>c</dt><dd>1|0 : 1=show currently displayed scenarios only, 0=show future displays only</dd>
		///	</dl>
		class ScenariosListFunction:
			public util::FactorableTemplate<server::Function,ScenariosListFunction>
		{
		public:
			static const std::string PARAMETER_SHOW_TEMPLATES;
			static const std::string PARAMETER_FOLDER_ID;
			static const std::string PARAMETER_CMS_TEMPLATE_ID;
			static const std::string PARAMETER_CURRENTLY_DISPLAYED;
			
		protected:
			//! \name Page parameters
			//@{
				bool _showTemplates;
				bool _showCurrentlyDisplayed;
				boost::shared_ptr<const ScenarioFolder> _parentFolder;
				boost::shared_ptr<const cms::Webpage> _cmsTemplate;
			//@}
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2010
			server::ParametersMap _getParametersMap() const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2010
			virtual void _setFromParametersMap(
				const server::ParametersMap& map
			);
			
		private:

			static const std::string DATA_NAME;
			static const std::string DATA_START_DATE;
			static const std::string DATA_END_DATE;

			//////////////////////////////////////////////////////////////////////////
			/// Display of a sent scenario.
			/// Values sent to the CMS template :
			/// <dl>
			/// <dt>roid</dt><dd>id of the scenario</dd>
			/// <dt>name</dt><dd>name of the scenario</dd>
			/// <dt>start_date</dt><dd>beginning of the broadcast</dd>
			/// <dt>end_date</dt><dd>ending of the broadcast (empty = no time limit)</dd>
			/// </dl>
			void _displaySentScenario(
				std::ostream& stream,
				const server::Request& request,
				const SentScenario& scenario
			) const;


			static const std::string DATA_FOLDER_ID;

			//////////////////////////////////////////////////////////////////////////
			/// Display of a sent scenario.
			/// Values sent to the CMS template :
			/// <dl>
			/// <dt>roid</dt><dd>id of the scenario</dd>
			/// <dt>name</dt><dd>name of the scenario</dd>
			/// <dt>folder_id</dt><dd>id of the parent folder (undefined if the scenario is located in the root folder)</dd>
			/// </dl>
			void _displayScenarioTemplate(
				std::ostream& stream,
				const server::Request& request,
				const ScenarioTemplate& scenario
			) const;
			
		public:
			ScenariosListFunction();

			//! @name Setters
			//@{
			//	void setObject(boost::shared_ptr<const Object> value) { _object = value; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2010
			virtual void run(std::ostream& stream, const server::Request& request) const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author Hugues Romain
			/// @date 2010
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues Romain
			/// @date 2010
			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_ScenariosListFunction_H__
