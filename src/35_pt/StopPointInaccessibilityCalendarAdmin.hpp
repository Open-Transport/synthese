
/** StopPointInaccessibilityCalendarAdmin class header.
	@file StopPointInaccessibilityCalendarAdmin.hpp

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

#ifndef SYNTHESE_StopPointInaccessibilityCalendarAdmin_H__
#define SYNTHESE_StopPointInaccessibilityCalendarAdmin_H__

#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace pt
	{
		class StopPointInaccessibilityCalendar;

		class StopPointInaccessibilityCalendarAdmin:
			public admin::AdminInterfaceElementTemplate<StopPointInaccessibilityCalendarAdmin>
		{
		public:
			static const std::string TAB_PROPERTIES;

		private:
			boost::shared_ptr<const pt::StopPointInaccessibilityCalendar>	_stopPointInaccessibilityCalendar;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Tests if two admin pages can be considered as the same one.
			/// @param other Other page to compare with. The other page will always be
			///	       of the same class.
			/// @return true if the other page can be considered as the same than the
			///         current one.
			virtual bool _hasSameContent(
				const AdminInterfaceElement& other
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Builds the tabs of the page.
			/// @param request The current request (can be used to determinate the
			///        current user rights.)
			virtual void _buildTabs(
				const security::Profile& profile
			) const;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			StopPointInaccessibilityCalendarAdmin();



			void setStopPointInaccessibilityCalendar(
				boost::shared_ptr<const StopPointInaccessibilityCalendar> value
			){
				_stopPointInaccessibilityCalendar = value;
			}



			//////////////////////////////////////////////////////////////////////////
			/// Initialization of the parameters from a parameters map.
			///	@param map The parameters map to use for the initialization.
			///	@throw AdminParametersException if a parameter has incorrect value.
			void setFromParametersMap(
				const util::ParametersMap& map
			);



			//////////////////////////////////////////////////////////////////////////
			/// Creation of the parameters map from the object attributes.
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content of the admin element.
			///	@param stream Stream to write the page content on.
			///	@param request The current request
			void display(
				std::ostream& stream,
				const server::Request& request
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Authorization control.
			/// Returns if the page can be displayed. In most cases, the needed right
			/// level is READ.
			///	@param request The current request
			///	@return bool True if the displayed page can be displayed
			bool isAuthorized(
				const security::User& user
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Title getter.
			///	@return The title of the page
			virtual std::string getTitle() const;
		};
	}
}

#endif // SYNTHESE_StopPointInaccessibilityCalendarAdmin_H__
