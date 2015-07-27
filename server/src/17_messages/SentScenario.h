////////////////////////////////////////////////////////////////////////////////
/// SentScenario class header.
///	@file SentScenario.h
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

#ifndef SYNTHESE_SentScenario_h__
#define SYNTHESE_SentScenario_h__

#include "Scenario.h"

#include "MessageApplicationPeriod.hpp"
#include "MessagesTypes.h"

#include <map>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace synthese
{
	namespace messages
	{
		class SentAlarm;
		class ScenarioTemplate;

		////////////////////////////////////////////////////////////////////
		/// Sent scenario instance class.
		///	@ingroup m17
		class SentScenario:
			public Scenario
		{
		public:
			static const std::string DATA_ID;
			static const std::string DATA_NAME;
			static const std::string DATA_EVENT_START_DATE;
			static const std::string DATA_EVENT_END_DATE;
			static const std::string DATA_START_DATE;
			static const std::string DATA_END_DATE;
			static const std::string DATA_ACTIVE;
			static const std::string DATA_SCENARIO_ID;
			static const std::string DATA_IS_TEMPLATE;
			static const std::string DATA_ARCHIVED;
			static const std::string DATA_CODE;
			static const std::string DATA_VALUE;

			static const std::string TAG_VARIABLE;
			static const std::string TAG_MESSAGE;
			static const std::string TAG_TEMPLATE_SCENARIO;
			static const std::string TAG_SECTION;
			static const std::string TAG_CALENDAR;


			////////////////////////////////////////////////////////////////////
			/// Left : variable code
			/// Right : variable value
			typedef std::map<std::string, std::string> VariablesMap;

		private:
			bool _isEnabled;
			boost::posix_time::ptime _eventStart; //!< Alarm applicability period start
			boost::posix_time::ptime _eventEnd;   //!< Alarm applicability period end
			boost::posix_time::ptime _periodStart; //!< Alarm applicability period start
			boost::posix_time::ptime _periodEnd;   //!< Alarm applicability period end
			const ScenarioTemplate*	_template;
			VariablesMap _variables;
			bool _archived;


		public:
			/** Basic constructor
			 *
			 * @param key id of the sent scenario
			 */
			SentScenario(util::RegistryKeyType key = 0);

			/** Template instantiation constructor.
			 *
			 * @param source the template
			 */
			SentScenario(
				const ScenarioTemplate& source
			);


			/** Copy constructor.
				@param source Scenario to copy
				The dates are not copied
			*/
			SentScenario(
				const SentScenario& source
			);

			~SentScenario();

			/// @name Getters
			//@{
				const boost::posix_time::ptime&	getEventStart() const { return _eventStart; }
				const boost::posix_time::ptime&	getEventEnd() const { return _eventEnd; }
				const boost::posix_time::ptime&	getPeriodStart() const { return _periodStart; }
				const boost::posix_time::ptime&	getPeriodEnd() const { return _periodEnd; }
				bool getIsEnabled()	const { return _isEnabled; }
				bool getArchived() const { return _archived; }
				const ScenarioTemplate*	getTemplate() const { return _template; }
				const VariablesMap&	getVariables() const { return _variables; }
			//@}

			/// @name Setters
			//@{
				////////////////////////////////////////////////////////////////////
				/// Start broadcast date setter.
				///	Updates the alarms too.
				///	@param periodStart Start broadcast date
				void setPeriodStart ( const boost::posix_time::ptime& periodStart);

				void setEventStart ( const boost::posix_time::ptime& value){ _eventStart = value; }


				////////////////////////////////////////////////////////////////////
				/// End broadcast date setter.
				///	Updates the alarms too.
				///	@param periodEnd End broadcast date
				void setPeriodEnd ( const boost::posix_time::ptime& periodEnd);

				void setEventEnd ( const boost::posix_time::ptime& value){ _eventEnd = value; }
				void setIsEnabled(bool value){ _isEnabled = value; }
				void setTemplate(const ScenarioTemplate* value);
				void setVariables(const VariablesMap& value);
				void setArchived(bool value){ _archived = value; }
			//@}


			/** Applicability test.
				@param start Start of applicability period
				@param end End of applicability period
				@return true if the message is not empty and is valid for the whole period given as argument.
			*/
			bool isApplicable ( const boost::posix_time::ptime& start, const boost::posix_time::ptime& end ) const;

			bool isApplicable(const boost::posix_time::ptime& date) const;



			//////////////////////////////////////////////////////////////////////////
			/// Export of the content of the object into a ParametersMap.
			/// @param pm the ParametersMap object to populate
			/// @author Hugues Romain
			/// @date 2012
			void toParametersMap(
				util::ParametersMap& pm
			) const;
		};
}	}

#endif // SYNTHESE_SentScenario_h__
