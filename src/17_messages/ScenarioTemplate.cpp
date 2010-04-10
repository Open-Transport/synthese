////////////////////////////////////////////////////////////////////////////////
/// ScenarioTemplate class implementation.
///	@file ScenarioTemplate.cpp
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

#include "ScenarioTemplate.h"
#include "AlarmTemplate.h"
#include "Registry.h"

#include <sstream>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	
	namespace util
	{
		template<> const string Registry<messages::ScenarioTemplate>::KEY("ScenarioTemplate");
	}

	namespace messages
	{

		ScenarioTemplate::ScenarioTemplate(
			const ScenarioTemplate& source,
			const std::string& name
		):	Scenario(name),
			Registrable(0),
			_folder(source._folder),
			_variables(source._variables)
		{
		}



		ScenarioTemplate::ScenarioTemplate(
			const std::string name,
			ScenarioFolder* folder
		):	Scenario(name)
			, util::Registrable(0)
			, _folder(folder)
		{
		}



		ScenarioTemplate::ScenarioTemplate(
			util::RegistryKeyType key
		):	util::Registrable(key)
			, Scenario()
			, _folder(NULL)
		{

		}



		ScenarioTemplate::~ScenarioTemplate()
		{

		}



		ScenarioFolder* ScenarioTemplate::getFolder() const
		{
			return _folder;
		}



		void ScenarioTemplate::setFolder(ScenarioFolder* value )
		{
			_folder = value;
		}



		const ScenarioTemplate::VariablesMap& ScenarioTemplate::getVariables(
		) const {
			return _variables;
		}



		void ScenarioTemplate::setVariablesMap(
			const VariablesMap& value
		){
			_variables = value;
		}



		void ScenarioTemplate::GetVariablesInformations(
			const std::string& text,
			ScenarioTemplate::VariablesMap& result
		){
			for(string::const_iterator it(text.begin()); it != text.end(); ++it)
			{
				// jump over other characters than $
				if (*it != '$') continue;

				++it;

				// $$$ = $
				if (it != text.end() && *it== '$' && it+1 != text.end() && *(it+1) == '$')
				{
					++it;
					continue;
				}

				// this is a variable definition
				ScenarioTemplate::Variable v;

				// compulsory variable
				if (*it == '$')
				{
					++it;
					v.compulsory = true;
				}
				else
				{
					v.compulsory = false;
				}

				// variable code
				string::const_iterator it2(it);
				for(; it != text.end() && *it != '|' && *it != '$'; ++it);
				if (it == text.end()) continue;
				v.code = text.substr(it2-text.begin(), it-it2);

				// variable information
				if (*it == '|')
				{
					++it;
					it2 = it;
					for(; it != text.end() && *it != '$'; ++it);
					if (it == text.end()) continue;
					v.helpMessage = text.substr(it2-text.begin(), it-it2);
				}

				// storage
				ScenarioTemplate::VariablesMap::iterator vmit(result.find(v.code));
				if(vmit == result.end())
				{
					result.insert(make_pair(v.code, v));
				}
				else
				{
					vmit->second.helpMessage += v.helpMessage;
					if(v.compulsory) vmit->second.compulsory = true;
				}
			}
		}



		std::string ScenarioTemplate::WriteTextFromVariables(
			const std::string& text,
			const SentScenario::VariablesMap& variables
		){
			stringstream stream;

			for(string::const_iterator it(text.begin()); it != text.end(); ++it)
			{
				// jump over other characters than $
				if (*it != '$')
				{
					stream << *it;
					continue;
				}

				++it;

				// $$$ = $
				if (it != text.end() && *it== '$' && it+1 != text.end() && *(it+1) == '$')
				{
					stream << "$";
					++it;
					continue;
				}

				// compulsory variable
				if (*it == '$')
				{
					++it;
				}

				// variable code
				string code;
				string::const_iterator it2(it);
				for(; it != text.end() && *it != '|' && *it != '$'; ++it);
				if (it == text.end()) continue;
				code = text.substr(it2-text.begin(), it-it2);

				// variable information
				if (*it == '|')
				{
					++it;
					it2 = it;
					for(; it != text.end() && *it != '$'; ++it);
					if (it == text.end()) continue;
				}

				// writing
				SentScenario::VariablesMap::const_iterator vmit(variables.find(code));
				if(vmit != variables.end())
				{
					stream << vmit->second;
				}
			}
			return stream.str();
		}



		bool ScenarioTemplate::ControlCompulsoryVariables(
			const ScenarioTemplate::VariablesMap& variables,
			const SentScenario::VariablesMap& values
		){
			BOOST_FOREACH(const ScenarioTemplate::VariablesMap::value_type& variable, variables)
			{
				if(!variable.second.compulsory) continue;

				if(values.find(variable.second.code) == values.end()) return false;
			}
			return true;
		}
	}
}
