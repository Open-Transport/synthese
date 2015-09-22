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

#include "MessagesTypes.h"

#include <string>
#include "PtimeField.hpp"
#include "PointersSetField.hpp"
#include "ScenarioTemplate.h"

namespace synthese
{
	namespace messages
	{
		class Alarm;
		class MessagesSection;

		FIELD_BOOL(Enabled)
		FIELD_PTIME(PeriodStart)
		FIELD_PTIME(PeriodEnd)
		FIELD_POINTER(Template, ScenarioTemplate)
		FIELD_PTIME(EventStart)
		FIELD_PTIME(EventEnd)
		FIELD_BOOL(Archived)
		FIELD_BOOL(ManualOverride)
		
		
		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(Enabled),
			FIELD(PeriodStart),
			FIELD(PeriodEnd),
			FIELD(Template),
			FIELD(DataSourceLinksWithoutUnderscore),
			FIELD(Sections),
			FIELD(EventStart),
			FIELD(EventEnd),
			FIELD(Archived),
			FIELD(ManualOverride)
			> SentScenarioRecord;

		////////////////////////////////////////////////////////////////////
		/// Sent scenario instance class.
		///	@ingroup m17
		class SentScenario:
			public Scenario,
			public Object<SentScenario, SentScenarioRecord>,
			public impex::ImportableTemplate<SentScenario>
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
			static const std::string DATA_MANUAL_OVERRIDE;

			static const std::string TAG_VARIABLE;
			static const std::string TAG_MESSAGE;
			static const std::string TAG_TEMPLATE_SCENARIO;
			static const std::string TAG_SECTION;
			static const std::string TAG_CALENDAR;

			
			/** Basic constructor
			 *
			 * @param key id of the sent scenario
			 */
			SentScenario(util::RegistryKeyType key = 0);
			~SentScenario();

			/** Copy constructor.
				@param source Scenario to copy
				
				The dates are not copied
			*/
			SentScenario(const SentScenario& source);


			/// @name Getters
			//@{
			const boost::posix_time::ptime&	getEventStart() const { return get<EventStart>(); }
			const boost::posix_time::ptime&	getEventEnd() const { return get<EventEnd>(); }
			const boost::posix_time::ptime&	getPeriodStart() const { return get<PeriodStart>(); }
			const boost::posix_time::ptime&	getPeriodEnd() const { return get<PeriodEnd>(); }			 
			bool getIsEnabled()	const { return get<Enabled>(); }
			bool getManualOverride()	const { return get<ManualOverride>(); }
			bool getArchived() const { return get<Archived>(); }
			std::string getName() const { return get<Name>(); }
			
			ScenarioTemplate* getTemplate() const;
			Sections::Type& getSections() const { return get<Sections>(); }
			
			//@}

			/// @name Setters
			//@{
				////////////////////////////////////////////////////////////////////
				/// Start broadcast date setter.
				///	Updates the alarms too.
				///	@param periodStart Start broadcast date
				void setPeriodStart ( const boost::posix_time::ptime& periodStart) { set<PeriodStart>(periodStart); }

				void setEventStart ( const boost::posix_time::ptime& value) { set<EventStart>(value); }


				////////////////////////////////////////////////////////////////////
				/// End broadcast date setter.
				///	Updates the alarms too.
				///	@param periodEnd End broadcast date
				void setPeriodEnd ( const boost::posix_time::ptime& periodEnd) { set<PeriodEnd>(periodEnd); }

				void setEventEnd ( const boost::posix_time::ptime& value) { set<EventEnd>(value); }
				
				void setIsEnabled(bool value){ set<Enabled>(value); }
				void setManualOverride(bool value){ set<ManualOverride>(value); }
				void setTemplate(const ScenarioTemplate* value);
				void setArchived(bool value){ set<Archived>(value); }

				void setSections(const Sections::Type& sections) { set<Sections>(sections); }
				
			//@}


			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
			//@}
				
			/** Applicability test.
				@param start Start of applicability period
				@param end End of applicability period
				@return true if the message is not empty and is valid for the whole period given as argument.
			*/
			bool isApplicable(const boost::posix_time::ptime& start, const boost::posix_time::ptime& end ) const;

				bool isApplicable(const boost::posix_time::ptime& date) const;

				//////////////////////////////////////////////////////////////////////////
				/// Checks if this scenario belong to an automatic section.
				/// @return true if this belongs to at least one automatic section
				bool belongsToAnAutomaticSection() const;

				//////////////////////////////////////////////////////////////////////////
				/// Checks if this scenario should be enabled.
				/// @param time Time at which we should check if scenario should be enabled
				/// @return true if time inside this scenario calendar
				bool shouldBeEnabled(const boost::posix_time::ptime& time) const;

				//////////////////////////////////////////////////////////////////////////
				/// Checks if this scenario should be archived.
				/// @param time Time at which we should check if scenario should be archived
				/// @return true if time is after all application periods of all calendars
				bool shouldBeArchived(const boost::posix_time::ptime& time) const;
			//@}


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
