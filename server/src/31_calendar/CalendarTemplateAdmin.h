
/** CalendarTemplateAdmin class header.
	@file CalendarTemplateAdmin.h
	@author Hugues Romain
	@date 2008

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

#ifndef SYNTHESE_CalendarTemplateAdmin_H__
#define SYNTHESE_CalendarTemplateAdmin_H__

#include "AdminInterfaceElementTemplate.h"
#include "ResultHTMLTable.h"

#include <boost/date_time/gregorian/greg_date.hpp>

namespace synthese
{
	namespace calendar
	{
		class CalendarTemplate;

		//////////////////////////////////////////////////////////////////////////
		/// 31.14 Admin : Calendar template editor.
		/// @ingroup m31Admin refAdmin
		///	@author Hugues Romain
		///	@date 2008
		//////////////////////////////////////////////////////////////////////////
		/// Key : CalendarTemplateAdmin
		///
		/// Parameters :
		///	<ul>
		///		<li>roid : calendar template to edit</li>
		///		<li>prs : start of calendar result check</li>
		///		<li>pre : end of calendar result check</li>
		///	</ul>
		class CalendarTemplateAdmin:
			public admin::AdminInterfaceElementTemplate<CalendarTemplateAdmin>
		{
		public:
			static const std::string TAB_SOURCE;
			static const std::string TAB_RESULT;
			static const std::string TAB_DATASOURCE;

			static const std::string PARAMETER_RESULT_START;
			static const std::string PARAMETER_RESULT_END;

		private:
			boost::shared_ptr<const CalendarTemplate>	_calendar;
			boost::gregorian::date	_resultStartDate;
			boost::gregorian::date	_resultEndDate;
			html::ResultHTMLTable::RequestParameters _requestParameters;

		public:
			CalendarTemplateAdmin();

			void setCalendar(boost::shared_ptr<const CalendarTemplate> value){ _calendar = value; }
			boost::shared_ptr<const CalendarTemplate> getCalendar() const { return _calendar; }

			/** Initialization of the parameters from a parameters map.
				@param map The parameters map to use for the initialization.
				@throw AdminParametersException if a parameter has incorrect value.
				@author Hugues Romain
				@date 2008
			*/
			void setFromParametersMap(
				const util::ParametersMap& map
			);



			/** Parameters map generator, used when building an url to the admin page.
					@return util::ParametersMap The generated parameters map
					@author Hugues Romain
					@date 2007
			*/
			virtual util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Builds the tabs of the page.
			/// @param request The current request (can be used to determinate the
			///        current user rights.)
			/// @author Hugues
			/// @date 2009
			virtual void _buildTabs(
				const security::Profile& profile
			) const;



			/** Display of the content of the admin element.
				@param stream Stream to write on.
				@author Hugues Romain
				@date 2008
			*/
			void display(
				std::ostream& stream,
				const server::Request& _request
			) const;

			/** Authorization control.
				@return bool True if the displayed page can be displayed
				@author Hugues Romain
				@date 2008
			*/
			bool isAuthorized(
				const security::User& user
			) const;



			/** Sub pages getter.
				@param currentPage Currently displayed page
				@param request User request
				@return PageLinks each subpage of the current page
				@author Hugues Romain
				@date 2011
			*/
			virtual AdminInterfaceElement::PageLinks getSubPages(
				const admin::AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;



			/** Title generator.
				@return The title of the page
				@author Hugues Romain

				@date 2008
			*/
			virtual std::string getTitle() const;


			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;
		};
	}
}

#endif // SYNTHESE_CalendarTemplateAdmin_H__
