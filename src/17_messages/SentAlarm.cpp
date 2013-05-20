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
		const string SentAlarm::TAG_APPLICATION_PERIOD = "application_period";



		SentAlarm::SentAlarm(
			util::RegistryKeyType key ,
			const SentScenario* scenario /*= NULL */
		):	Registrable(key),
			Alarm(key, scenario),
			_template(NULL)
		{}



		SentAlarm::SentAlarm(
			const SentScenario& scenario,
			const AlarmTemplate& source
		):	Registrable(0),
			Alarm(source, &scenario),
			_template(&source)
		{}



		SentAlarm::SentAlarm(
			const SentScenario& scenario,
			const SentAlarm& source
		):	Registrable(0),
			Alarm(source, &scenario),
			_template(source._template)
		{}



		SentAlarm::SentAlarm(
			const SentScenario& scenario,
			const Alarm& source
		):	Registrable(0),
			Alarm(source, &scenario),
			_template(
				dynamic_cast<const AlarmTemplate*>(&source) ?
				static_cast<const AlarmTemplate*>(&source) :
				static_cast<const SentAlarm&>(source)._template
			)
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



		void SentAlarm::toParametersMap( util::ParametersMap& pm, bool withScenario, std::string prefix /*= std::string()*/, bool withRecipients /*= false */ ) const
		{
			Alarm::toParametersMap(pm, withScenario, prefix, withRecipients);

			BOOST_FOREACH(const MessageApplicationPeriod::ApplicationPeriods::value_type& it, _applicationPeriods)
			{
				boost::shared_ptr<ParametersMap> apPM(new ParametersMap);
				it->toParametersMap(*apPM);
				pm.insert(TAG_APPLICATION_PERIOD, apPM);
			}
		}



		//////////////////////////////////////////////////////////////////////////
		/// Checks if the current message is active at the specified time.
		/// @param when the time to check
		/// @return true if the message must be displayed at the specified time
		bool SentAlarm::isApplicable( boost::posix_time::ptime& when ) const
		{
			// Check if the event is active first
			if( !getScenario() ||
				!getScenario()->getIsEnabled()
			){
				return false;
			}

			// Then check if specific application periods are defined for the current message
			if(!_applicationPeriods.empty())
			{
				// Search for an application period including the checked date
				BOOST_FOREACH(
					const MessageApplicationPeriod::ApplicationPeriods::value_type& period,
					_applicationPeriods
				){
					if(period->getValue(when))
					{
						return true;
					}
				}

				// No period was found : the message is inactive
				return false;
			}

			// Then check if application periods are defined for the event
			if(!getScenario()->getApplicationPeriods().empty())
			{
				// Search for an application period including the checked date
				BOOST_FOREACH(
					const MessageApplicationPeriod::ApplicationPeriods::value_type& period,
					getScenario()->getApplicationPeriods()
				){
					if(period->getValue(when))
					{
						return true;
					}
				}

				// No period was found : the message is inactive
				return false;
			}

			// Then refer to the simple start/end date of the scenario
			return
				getScenario()->getPeriodStart() <= when &&
				getScenario()->getPeriodEnd() >= when;
		}



		void SentAlarm::clearBroadcastPointsCache() const
		{
			_broadcastPointsCache.clear();
		}



		bool SentAlarm::isOnBroadcastPoint(
			const BroadcastPoint& point,
			const util::ParametersMap& parameters
		) const	{

			BroadcastPointsCache::key_type pp(
				make_pair(&point, parameters)
			);
			BroadcastPointsCache::const_iterator it(
				_broadcastPointsCache.find(pp)
			);
			if(it == _broadcastPointsCache.end())
			{
				it = _broadcastPointsCache.insert(
					make_pair(
						pp,
						_isOnBroadcastPoint(point, parameters)
				)	).first;
			}
			return it->second;
		}
}	}
