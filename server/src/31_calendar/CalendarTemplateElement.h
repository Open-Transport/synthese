
/** CalendarTemplateElement class header.
	@file CalendarTemplateElement.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#ifndef SYNTHESE_timetables_CalendarTemplateElement_h__
#define SYNTHESE_timetables_CalendarTemplateElement_h__

#include "Object.hpp"
#include "Calendar.h"

#include "FrameworkTypes.hpp"
#include "DateField.hpp"
#include "CalendarTypes.h"
#include "EnumObjectField.hpp"
#include "DaysField.hpp"
#include "PointerField.hpp"

#include <boost/optional.hpp>

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplate;

		FIELD_POINTER(CalendarTemplateField, CalendarTemplate)
		FIELD_INT(RankField)
		FIELD_DATE(MinDate)
		FIELD_DATE(MaxDate)
		FIELD_DAYS(Step)
		FIELD_ENUM(Operation, CalendarTemplateElementOperation)
		FIELD_POINTER(IncludeCalendarTemplate, CalendarTemplate)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(CalendarTemplateField),
			FIELD(RankField),
			FIELD(MinDate),
			FIELD(MaxDate),
			FIELD(Step),
			FIELD(Operation),
			FIELD(IncludeCalendarTemplate)
		> CalendarTemplateElementSchema;

		/** Element of calendar template class.
			@ingroup m31

			An element of a calendar describes a range of dates by several ways :
				- single date d : minDate=d, maxDate=d
				- date range from s to e : minDate=s, maxDate=e, interval=1
				- day of a week from s to e : minDate=s, maxDate=e, interval=7
				- the maxDate can be unknown date (unlimited range)

		*/
		class CalendarTemplateElement:
				public Object<CalendarTemplateElement, CalendarTemplateElementSchema>
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<CalendarTemplateElement>	Registry;

		private:

		public:
			CalendarTemplateElement(
				util::RegistryKeyType id = 0
			);

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Applies the element definition on the result calendar according to the mask.
				/// @param result the result calendar
				/// @param mask the mask to apply
				void apply(Calendar& result, const Calendar& mask) const;



				//////////////////////////////////////////////////////////////////////////
				/// Min date getter which takes into account of included calendar.
				boost::gregorian::date	getRealMinDate()	const;

				//////////////////////////////////////////////////////////////////////////
				/// Max date getter which takes into account of included calendar.
				boost::gregorian::date	getRealMaxDate()	const;

				virtual void toParametersMap(
					util::ParametersMap& map,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;
			//@}

			//! @name Getters
			//@{
				size_t					getRank()		const;
				const boost::gregorian::date&	getMinDate()	const;
				const boost::gregorian::date&	getMaxDate()	const;
				const CalendarTemplate* getInclude()	const;
				const CalendarTemplate*	getCalendar() const;
				const boost::gregorian::date_duration& getStep() const;
				CalendarTemplateElementOperation	getOperation()	const;
			//@}

			//! @name Setters
			//@{
				void setCalendar(const CalendarTemplate* value);
				void setRank(size_t text);
				void setMinDate(const boost::gregorian::date& date);
				void setMaxDate(const boost::gregorian::date& date);
				void setInclude(const CalendarTemplate* value);
				void setStep(const boost::gregorian::date_duration& value);
				void setOperation(CalendarTemplateElementOperation value);

			//@}

			//! @name Modifiers
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();

				virtual LinkedObjectsIds getLinkedObjectsIds(
					const Record& record
				) const;
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;

		};
	}
}

#endif // SYNTHESE_timetables_CalendarTemplateElement_h__
