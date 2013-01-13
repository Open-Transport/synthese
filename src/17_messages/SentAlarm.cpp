////////////////////////////////////////////////////////////////////////////////
/// SentAlarm class implementation.
///	@file SentAlarm.cpp
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

#include "SentAlarm.h"

#include "AlarmRecipient.h"
#include "Factory.h"
#include "Registry.h"
#include "SentScenario.h"
#include "AlarmTemplate.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;

	namespace messages
	{
		SentAlarm::SentAlarm(
			util::RegistryKeyType key ,
			const SentScenario* scenario /*= NULL */
		):	Alarm(key, scenario),
			Registrable(key),
			_template(NULL)
		{}



		SentAlarm::SentAlarm(
			const SentScenario& scenario,
			const AlarmTemplate& source
		):	Alarm(source, &scenario),
			Registrable(0),
			_template(&source)
		{}



		SentAlarm::SentAlarm(
			const SentScenario& scenario,
			const SentAlarm& source
		):	Alarm(source, &scenario),
			Registrable(0),
			_template(source._template)
		{}



		SentAlarm::~SentAlarm()
		{}



		const SentScenario* SentAlarm::getScenario() const
		{
			return static_cast<const SentScenario*>(Alarm::getScenario());
		}



		const AlarmTemplate* SentAlarm::getTemplate() const
		{
			return _template;
		}



		void SentAlarm::setTemplate( const AlarmTemplate* value )
		{
			_template = value;
		}
}	}
