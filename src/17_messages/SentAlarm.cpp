////////////////////////////////////////////////////////////////////////////////
/// SentAlarm class implementation.
///	@file SentAlarm.cpp
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

#include "SentAlarm.h"
#include "AlarmRecipient.h"
#include "Registry.h"
#include "SentScenario.h"
#include "AlarmTemplate.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;

	namespace util
	{
		template<> const string Registry<messages::SentAlarm>::KEY("SentAlarm");
	}

	namespace messages
	{

		SentAlarm::SentAlarm(
			util::RegistryKeyType key ,
			const SentScenario* scenario /*= NULL */
		):	Alarm(key, scenario),
			Registrable(key),
			_template(NULL)
		{
		}



		SentAlarm::SentAlarm(
			const SentScenario& scenario,
			const AlarmTemplate& source
		):	Alarm(source, &scenario),
			Registrable(0),
			_template(&source)
		{
		}



		SentAlarm::SentAlarm(
			const SentScenario& scenario,
			const SentAlarm& source
		):	Alarm(source, &scenario),
			Registrable(0),
			_template(source._template)
		{
		}



		SentAlarm::~SentAlarm()
		{

		}

		AlarmConflict SentAlarm::wereInConflictWith( const SentAlarm& other ) const
		{
/*			// If one of the two alarms are not enabled, no conflict
			if (!getIsEnabled() || !other.getIsEnabled())
				return ALARM_NO_CONFLICT;

			// Inverting the parameters if necessary
			if ((other.getPeriodStart().is_not_a_date_time() && !getPeriodStart().is_not_a_date_time())
				|| (!other.getPeriodStart().is_not_a_date_time() && !getPeriodStart().is_not_a_date_time() && other.getPeriodStart() < getPeriodStart()))
				return other.wereInConflictWith(*this);

			// No common period : no conflict
			if (!other.getPeriodStart().is_not_a_date_time() && !getPeriodEnd().is_not_a_date_time() && other.getPeriodStart() > getPeriodEnd())
				return ALARM_NO_CONFLICT;

			// Common period : different level gives priority to an alarm
			if (other.getLevel() != getLevel())
				return ALARM_WARNING_ON_INFO;
*/
//			return ALARM_CONFLICT;
			return ALARM_NO_CONFLICT;
		}

		AlarmConflict SentAlarm::getConflictStatus() const
		{
			AlarmConflict conflictStatus(ALARM_NO_CONFLICT);
			vector<shared_ptr<AlarmRecipient> > recipients(Factory<AlarmRecipient>::GetNewCollection());
			BOOST_FOREACH(const shared_ptr<AlarmRecipient> recipient, recipients)
			{
				AlarmConflict thisConflictStatus = recipient->getConflictStatus(this);
				if (thisConflictStatus > conflictStatus)
					conflictStatus = thisConflictStatus;
				if (conflictStatus == ALARM_CONFLICT)
					return conflictStatus;
			}
			return conflictStatus;
		}



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

/*		SentAlarm::Complements SentAlarm::getComplements() const
		{
			return _complements;
		}

		void SentAlarm::setComplements( const Complements& complements )
		{
			_complements = complements;
		}
*/	}
}
