
/** SentScenario class implementation.
	@file SentScenario.cpp

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

#include "SentScenario.h"
#include "ScenarioTemplate.h"
#include "ScenarioSentAlarm.h"
#include "Registry.h"

using namespace std;

namespace synthese
{
	using namespace time;
	using namespace util;

	namespace util
	{
		template<> const string Registry<messages::SentScenario>::KEY("SentScenario");
	}


	namespace messages
	{
		SentScenario::SentScenario(
			util::RegistryKeyType key
		):	Registrable(key),
			ScenarioSubclassTemplate<ScenarioSentAlarm>()
			, _isEnabled(false)
			, _periodStart(TIME_UNKNOWN)
			, _periodEnd(TIME_UNKNOWN)
		{
		}

		SentScenario::SentScenario(
			const ScenarioTemplate& source,
			util::RegistryKeyType key,
			time::DateTime periodStart /*= time::DateTime(time::TIME_UNKNOWN) */
			, time::DateTime periodEnd /*= time::DateTime(time::TIME_UNKNOWN) */
		):	ScenarioSubclassTemplate<ScenarioSentAlarm>(source.getName()),
			Registrable(UNKNOWN_VALUE),
			_isEnabled(false),
			_periodStart(periodStart),
			_periodEnd(periodEnd)
		{
			for (ScenarioTemplate::AlarmsSet::const_iterator it = source.getAlarms().begin(); it != source.getAlarms().end(); ++it)
				addAlarm(new ScenarioSentAlarm(this, **it));
		}

		void SentScenario::setPeriodStart( const synthese::time::DateTime& periodStart )
		{
			_periodStart = periodStart;
		}

		void SentScenario::setPeriodEnd( const synthese::time::DateTime& periodEnd )
		{
			_periodEnd = periodEnd;
		}


		const time::DateTime& SentScenario::getPeriodStart() const
		{
			return _periodStart;
		}

		const time::DateTime& SentScenario::getPeriodEnd() const
		{
			return _periodEnd;
		}

		void SentScenario::setIsEnabled( bool value )
		{
			_isEnabled = value;
		}

		bool SentScenario::getIsEnabled() const
		{
			return _isEnabled;
		}

		SentScenario::~SentScenario()
		{

		}



		AlarmConflict SentScenario::getConflictStatus() const
		{
			AlarmConflict conflictStatus(ALARM_NO_CONFLICT);
			for (AlarmsSet::const_iterator it = getAlarms().begin(); it != getAlarms().end(); ++it)
			{
				AlarmConflict thisConflictStatus = (*it)->getConflictStatus();
				if (thisConflictStatus > conflictStatus)
					conflictStatus = thisConflictStatus;
				if (conflictStatus == ALARM_CONFLICT)
					return conflictStatus;
			}
			return conflictStatus;
		}

		bool SentScenario::isApplicable( const time::DateTime& start, const time::DateTime& end ) const
		{
			// Disabled alarm is never applicable
			if (!getIsEnabled())
				return false;

			// Start date control
			if (!getPeriodStart().isUnknown() && end < getPeriodStart())
				return false;

			// End date control
			if (!getPeriodEnd().isUnknown() && start >= getPeriodEnd())
				return false;

			return true;
		}

		bool SentScenario::isApplicable( const time::DateTime& date ) const
		{
			return isApplicable(date, date);
		}



		const ScenarioTemplate* SentScenario::getTemplate(
		) const {
			return _template;
		}



		void SentScenario::setTemplate(
			const ScenarioTemplate* value
		) {
			_template = value;
		}



		const SentScenario::VariablesMap& SentScenario::getVariables(
		) const {
			return _variables;
		}



		void SentScenario::setVariables(
			const VariablesMap& value
		) {
			_variables = value;
		}
	}
}
