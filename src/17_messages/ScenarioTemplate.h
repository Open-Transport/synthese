////////////////////////////////////////////////////////////////////////////////
/// ScenarioTemplate class header.
///	@file ScenarioTemplate.h
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

#ifndef SYNTHESE_ScenarioTemplate_h__
#define SYNTHESE_ScenarioTemplate_h__

#include "ScenarioSubclassTemplate.h"
#include "Registry.h"

#include <map>
#include <string>

namespace synthese
{
	namespace messages
	{
		class AlarmTemplate;

		////////////////////////////////////////////////////////////////////
		/// Scenario template class.
		///	@ingroup m17
		///
		/// Variables handling : scan all the contained messages to find all 
		/// variables declarations and fill in the _variables attribute
		class ScenarioTemplate
		:	public ScenarioSubclassTemplate<AlarmTemplate>
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<ScenarioTemplate>	Registry;

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
			uid				_folderId;
			VariablesMap	_variables;

		public:
			/// @name constructors and destructor
			//@{
				ScenarioTemplate(const std::string name = std::string());
				ScenarioTemplate(const ScenarioTemplate& source, const std::string& name);
				ScenarioTemplate(util::RegistryKeyType key);
				~ScenarioTemplate();
			//@}

			/// @name Getters
			//@{
				uid	getFolderId()	const;
				const VariablesMap& getVariables() const;
			//@}

			/// @name Setters
			//@{
				void setFolderId(uid value);
			//@}

			/// @name Modifiers
			//@{
				////////////////////////////////////////////////////////////////////
				///	Scans each contained messages to find variables definitions.
				///	@author Hugues Romain
				///	@date 2009
				void setVariablesFromAlarms();
			//@}
		};
	}
}

#endif // SYNTHESE_ScenarioTemplate_h__
