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
			Registrable(UNKNOWN_VALUE),
			_folderId(source._folderId),
			_variables(source._variables)
		{
		}



		ScenarioTemplate::ScenarioTemplate(
			const std::string name,
			RegistryKeyType folderId
		):	Scenario(name)
			, util::Registrable(UNKNOWN_VALUE)
			, _folderId(folderId)
		{
		}



		ScenarioTemplate::ScenarioTemplate(
			util::RegistryKeyType key
		):	util::Registrable(key)
			, Scenario()
			, _folderId(UNKNOWN_VALUE)
		{

		}

		ScenarioTemplate::~ScenarioTemplate()
		{

		}



		uid ScenarioTemplate::getFolderId() const
		{
			return _folderId;
		}



		void ScenarioTemplate::setFolderId( uid value )
		{
			_folderId = value;
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
	}
}
