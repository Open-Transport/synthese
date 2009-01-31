
/** AlarmTemplate class implementation.
	@file AlarmTemplate.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "AlarmTemplate.h"
#include "Registry.h"

using namespace std;

namespace synthese
{
	namespace util
	{
		template<> const string Registry<messages::AlarmTemplate>::KEY("AlarmTemplate");
	}

	namespace messages
	{
		AlarmTemplate::~AlarmTemplate()
		{

		}



		AlarmTemplate::AlarmTemplate(
			util::RegistryKeyType key,
			const ScenarioTemplate* scenario
		):	Alarm(key),
			Registrable(key),
			_scenario(scenario)
		{

		}

		AlarmTemplate::AlarmTemplate( const AlarmTemplate& source)
			: Alarm(source)
			, _scenario(source._scenario)
		{

		}

		AlarmTemplate::AlarmTemplate(
			const ScenarioTemplate& scenario,
			const AlarmTemplate& source
		):	Alarm(source)
			, _scenario(&scenario)
		{

		}



		const ScenarioTemplate* AlarmTemplate::getScenario() const
		{
			return _scenario;
		}

		void AlarmTemplate::setScenario( const ScenarioTemplate* scenario )
		{
			_scenario = scenario;
		}
	}
}
