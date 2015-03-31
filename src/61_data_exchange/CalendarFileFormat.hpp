
/** CalendarFileFormat class header.
	@file CalendarFileFormat.hpp

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

#ifndef SYNTHESE_calendar_CalendarFileFormat_hpp__
#define SYNTHESE_calendar_CalendarFileFormat_hpp__

#include "Importer.hpp"

#include "ImportableTableSync.hpp"
#include "CalendarTemplateTableSync.h"

#include <boost/date_time/gregorian/gregorian_types.hpp>

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplateTableSync;
		class CalendarTemplate;
	}

	namespace data_exchange
	{

		//////////////////////////////////////////////////////////////////////////
		/// Calendar import helpers library.
		///	@ingroup m31
		class CalendarFileFormat:
			public virtual impex::Importer
		{
		public:
			CalendarFileFormat(
				util::Env& env,
				const impex::Import& import,
				impex::ImportLogLevel minLogLevel,
				const std::string& logPath,
				boost::optional<std::ostream&> outputStream,
				util::ParametersMap& pm
			);

			virtual ~CalendarFileFormat();

		protected:
			typedef std::vector<boost::gregorian::date> DatesVector;

			mutable std::set<boost::shared_ptr<calendar::CalendarTemplateElement> > _calendarElementsToRemove;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Get CalendarTemplate with DataSource link support.
			/// This pointer comes from Env/Registry. Do not copy it into a shared_ptr
			/// or neither try to free it.
			/// @param calendars CalendarTemplate from DataSource
			/// @param code object code
			/// @return CalendarTemplate with CalendarTemplateElement already loaded
			calendar::CalendarTemplate* _getCalendarTemplate(
				impex::ImportableTableSync::ObjectBySource<calendar::CalendarTemplateTableSync>& calendars,
				const std::string& code
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Create a new CalendarTemplateElement for a single date.
			/// It is not required to maintain the returned shared pointer.
			/// @param calendar related CalendarTemplate which element belongs to
			/// @param name CalendarTemplate name
			/// @param code object code
			/// @param source related DataSource
			/// @param day
			/// @return generated CalendarTemplateElement already registered in calendar
			boost::shared_ptr<calendar::CalendarTemplateElement> _createCalendarTemplateElement(
				boost::shared_ptr<calendar::CalendarTemplate> calendar,
				const size_t rank,
				const boost::gregorian::date day
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Create a new CalendarTemplate with DataSource link support
			/// It is not required to maintain the returned shared pointer.
			/// @param calendars CalendarTemplate from DataSource
			/// @param name CalendarTemplate name
			/// @param code object code
			/// @param source related DataSource
			/// @param dateList list of dates to include in CalendarTemplate
			/// @return generated CalendarTemplate with CalendarTemplateElement if provided
			boost::shared_ptr<calendar::CalendarTemplate> _createCalendarTemplate(
				impex::ImportableTableSync::ObjectBySource<calendar::CalendarTemplateTableSync>& calendars,
				const std::string& name,
				const std::string& code,
				const impex::DataSource& source,
				boost::optional<DatesVector> dateList
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Remove a CalendarElementTemplate from a CalendarTemplate
			/// @param calendar CalendarTemplate holding element
			/// @param element CalendarTemplateElement to discard
			void _removeCalendarTemplateElement(
				boost::shared_ptr<calendar::CalendarTemplate> calendar,
				boost::shared_ptr<calendar::CalendarTemplateElement> oldElement
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Create or update a new CalendarTemplate with DataSource link  support
			/// It is not required to maintain the returned shared pointer.
			/// @param calendars CalendarTemplate from DataSource
			/// @param name CalendarTemplate name
			/// @param code object code
			/// @param source related DataSource
			/// @param dateList list of dates to include in CalendarTemplate
			/// @return new or updated CalendarTemplate with CalendarTemplateElement if provided.
			/// If shared pointer unset, update aborted because of unhandled CalendarTemplateElement patterns
			boost::shared_ptr<calendar::CalendarTemplate> _createOrUpdateCalendarTemplate(
				impex::ImportableTableSync::ObjectBySource<calendar::CalendarTemplateTableSync>& calendars,
				const std::string& name,
				const std::string& code,
				const impex::DataSource& source,
				boost::optional<DatesVector> dateList
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Save CalendarTemplate and CalendarTemplateElement in provided transaction
			/// Also remove CalendarTemplateElement from _calendarElementsToRemove
			/// @param transaction current database transaction
			void _saveCalendarTemplates(db::DBTransaction& transaction) const;
		};
}	}

#endif // SYNTHESE_calendar_CalendarFileFormat_hpp__

