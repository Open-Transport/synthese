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
	namespace util
	{
		template<> const string Registry<messages::ScenarioTemplate>::KEY("ScenarioTemplate");
	}

	namespace messages
	{

		ScenarioTemplate::ScenarioTemplate( const ScenarioTemplate& source, const std::string& name)
			: ScenarioSubclassTemplate<AlarmTemplate>(name)
			, util::Registrable(UNKNOWN_VALUE)
		{
			for (AlarmsSet::const_iterator it = source.getAlarms().begin(); it != source.getAlarms().end(); ++it)
				addAlarm(new AlarmTemplate(this,**it ));
		}

		ScenarioTemplate::ScenarioTemplate(const std::string name)
			: ScenarioSubclassTemplate<AlarmTemplate>(name)
			, util::Registrable(UNKNOWN_VALUE)
			, _folderId(UNKNOWN_VALUE)
		{

		}



		ScenarioTemplate::ScenarioTemplate( util::RegistryKeyType key )
			: util::Registrable(key)
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



		void ScenarioTemplate::setVariablesFromAlarms(
		){
			_variables.clear();
			BOOST_FOREACH(AlarmTemplate* alarm, getAlarms())
			{
				string text(alarm->getLongMessage());
				for(string::const_iterator it(text.begin()); it != text.end(); ++it)
				{
					if (*it == '$')
					{
						if (it+1 != text.end() && *(it+1) == '$' && it+2 != text.end() && *(it+2) == '$')
						{
							it += 2;
							continue;
						}
						Variable v;
						if (*(it+1) == '$')
						{
							++it;
							v.compulsory = true;
						}
						else
						{
							v.compulsory = false;
						}
						string::const_iterator it2(it);
						for(; it != text.end() && *it != '|' && *it != '$'; ++it);
						if (it == text.end()) continue;
						v.code = text.substr(it2-text.begin(), it-it2);

						if (*it == '|')
						{
							++it;
							it2 = it;
							for(; it != text.end() && *it != '$'; ++it);
							if (it == text.end()) continue;
							v.helpMessage = text.substr(it2-text.begin(), it-it2);
						}
						_variables.insert(make_pair(v.code, v));
						if (v.compulsory) ++it;
					}
				}
			}
		}
	}
}
