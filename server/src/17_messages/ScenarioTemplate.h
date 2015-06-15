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

#include <map>
#include <string>

namespace synthese
{
	namespace messages
	{
		class Alarm;
		class ScenarioFolder;		 
		class MessagesSection;

		FIELD_POINTER(Folder, ScenarioFolder)
		
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(Folder),
			FIELD(Sections)
			> ScenarioTemplateRecord;
		
		////////////////////////////////////////////////////////////////////
		/// Scenario template class.
		///	@ingroup m17
		///
		/// Variables handling : scan all the contained messages to find all
		/// variables declarations and fill in the _variables attribute
		class ScenarioTemplate:
			public Scenario,
			public impex::ImportableTemplate<ScenarioTemplate>,
			public Object<ScenarioTemplate, ScenarioTemplateRecord>
		{
		public:
			static const std::string DATA_SCENARIO_ID;
			static const std::string DATA_NAME;
			static const std::string DATA_FOLDER_ID;
			static const std::string DATA_FOLDER_NAME;
			static const std::string DATA_IS_TEMPLATE;

			static const std::string TAG_VARIABLE;
			static const std::string TAG_MESSAGE;
			static const std::string TAG_SECTION;
			static const std::string TAG_CALENDAR;


			/// @name constructors and destructor
			//@{

				ScenarioTemplate(util::RegistryKeyType key = 0);
				~ScenarioTemplate();
			//@}

			/// @name Getters
			//@{
				ScenarioFolder*	getFolder()	const;
				Sections::Type& getSections() const { return get<Sections>(); }
				std::string getName() const { return get<Name>(); }
				
			//@}

			/// @name Setters
			//@{
				void setFolder(ScenarioFolder* value);
			//@}

			/// @name Modifiers
			//@{
			//@}

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
