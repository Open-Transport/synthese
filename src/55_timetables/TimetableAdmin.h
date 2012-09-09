
/** TimetableAdmin class header.
	@file TimetableAdmin.h
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

#ifndef SYNTHESE_TimetableAdmin_H__
#define SYNTHESE_TimetableAdmin_H__

#include "AdminInterfaceElementTemplate.h"
#include "ResultHTMLTable.h"

namespace synthese
{
	namespace timetables
	{
		class Timetable;
		class TimetableResult;

		//////////////////////////////////////////////////////////////////////////
		/// Timetable edition screen.
		///	@ingroup m55Admin refAdmin
		///	@author Hugues Romain
		///	@date 2008
		class TimetableAdmin:
			public admin::AdminInterfaceElementTemplate<TimetableAdmin>
		{
		public:
			static const std::string PARAMETER_RANK;
			static const std::string PARAMETER_TITLE;
			static const std::string TAB_PROPERTIES;
			static const std::string TAB_CONTENT;
			static const std::string TAB_RESULT;

		private:
			boost::shared_ptr<const Timetable>			_timetable;
			html::ResultHTMLTable::RequestParameters	_requestParameters;

			void _drawTable(
				std::ostream& stream,
				html::HTMLTable& tf,
				const TimetableResult& result,
				std::size_t depth = 0,
				bool isBefore = true
			) const;

		public:
			TimetableAdmin();

			void setTimetable(boost::shared_ptr<Timetable> timetable);

			boost::shared_ptr<const Timetable> getTimetable() const;

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



			/** Display of the content of the admin element.
				@param stream Stream to write on.
				@param request The current request
				@author Hugues Romain
				@date 2008
			*/
			void display(
				std::ostream& stream,
				const server::Request& _request
			) const;



			/** Authorization control.
				@param request The current request
				@return bool True if the displayed page can be displayed
				@author Hugues Romain
				@date 2008
			*/
			virtual bool isAuthorized(
				const security::User& user
			) const;

			/** Title generator.
				@return The title of the page
				@author Hugues Romain
				@date 2008
			*/
			virtual std::string getTitle() const;

			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;



			/** Gets sub page of the designed parent page, which are from the current class.
			@param parentLink Link to the parent page
			@param currentPage Currently displayed page
			@return PageLinks each subpage of the parent page designed in parentLink
			@author Hugues Romain
			@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const server::ModuleClass& module,
				const admin::AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;

			/** Sub pages getter.
			@param currentPage Currently displayed page
			@param request User request
			@return PageLinks each subpage of the current page
			@author Hugues Romain
			@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPages(
				const admin::AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;

			//////////////////////////////////////////////////////////////////////////
			/// Builds the tabs of the page.
			/// @param request The current request (can be used to determinate the
			///        current user rights.)
			/// @author Hugues
			/// @date 2009
			virtual void _buildTabs(
				const security::Profile& profile
			) const;

		};
	}
}

#endif // SYNTHESE_TimetableAdmin_H__

