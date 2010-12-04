////////////////////////////////////////////////////////////////////////////////
/// SentScenario class header.
///	@file SentScenario.h
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

#ifndef SYNTHESE_SentScenario_h__
#define SYNTHESE_SentScenario_h__

#include "MessagesTypes.h"
#include "Registry.h"
#include "Scenario.h"

#include <map>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace synthese
{
	namespace messages
	{
		class ScenarioTemplate;

		////////////////////////////////////////////////////////////////////
		/// Sent scenario instance class.
		///	@ingroup m17
		class SentScenario
		:	public Scenario
		{
		public:

			/// Chosen registry class.
			typedef util::Registry<SentScenario>	Registry;

			////////////////////////////////////////////////////////////////////
			/// Left : variable code
			/// Right : variable value
			typedef std::map<std::string, std::string> VariablesMap;

		private:
			bool					_isEnabled;
			boost::posix_time::ptime	_periodStart; //!< Alarm applicability period start
			boost::posix_time::ptime	_periodEnd;   //!< Alarm applicability period end
			const ScenarioTemplate*	_template;
			VariablesMap			_variables;

		public:
			/** Basic constructor
			 * 
			 * @param key id of the sent scenario 
			 */
			SentScenario(util::RegistryKeyType key = 0);
			
			/** Template instanciation constructor.
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

			const boost::posix_time::ptime&	getPeriodStart()	const;
			const boost::posix_time::ptime&	getPeriodEnd()		const;
			bool					getIsEnabled()		const;
			const ScenarioTemplate*	getTemplate()		const;
			const VariablesMap&		getVariables()		const;

			////////////////////////////////////////////////////////////////////
			/// Start broadcast date setter.
			///	Updates the alarms too.
			///	@param periodStart Start broadcast date
			void setPeriodStart ( const boost::posix_time::ptime& periodStart);
			

			////////////////////////////////////////////////////////////////////
			/// End broadcast date setter.
			///	Updates the alarms too.
			///	@param periodEnd End broadcast date
			void setPeriodEnd ( const boost::posix_time::ptime& periodEnd);
			void setIsEnabled(bool value);
			void setTemplate(const ScenarioTemplate* value);
			void setVariables(const VariablesMap& value);

			/** Gets the "worse" conflict status of each alarm contained in the scenario.
				@return synthese::messages::AlarmConflict The conflict status of the scenario.
				@author Hugues Romain
				@date 2007
			*/
			AlarmConflict getConflictStatus() const;

			/** Applicability test.
				@param start Start of applicability period
				@param end End of applicability period
				@return true if the message is not empty and is valid for the whole period given as argument.
			*/
			bool isApplicable ( const boost::posix_time::ptime& start, const boost::posix_time::ptime& end ) const;

			bool isApplicable(const boost::posix_time::ptime& date) const;

		};
	}
}

#endif // SYNTHESE_SentScenario_h__
