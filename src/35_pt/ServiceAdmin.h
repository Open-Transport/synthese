//////////////////////////////////////////////////////////////////////////
/// ServiceAdmin class header.
///	@file ServiceAdmin.h
///	@author Hugues
///	@date 2009
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_ServiceAdmin_H__
#define SYNTHESE_ServiceAdmin_H__

#include "ResultHTMLTable.h"
#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace pt
	{
		class SchedulesBasedService;
		class ScheduledService;
		class ContinuousService;

		//////////////////////////////////////////////////////////////////////////
		/// ServiceAdmin Admin compound class.
		///	@ingroup m35Admin refAdmin
		///	@author Hugues
		///	@date 2009
		class ServiceAdmin:
			public admin::AdminInterfaceElementTemplate<ServiceAdmin>
		{
		public:
			/// @name Parameter identifiers
			//@{
				static const std::string TAB_SCHEDULES;
				static const std::string TAB_REAL_TIME;
				static const std::string TAB_PROPERTIES;
				static const std::string TAB_CALENDAR;
			//@}

			/// @name Parameter Session variables
			//@{
				static const std::string SESSION_VARIABLE_SERVICE_ADMIN_ID;
				static const std::string SESSION_VARIABLE_SERVICE_ADMIN_SCHEDULE_RANK;
			//@}

		private:
			/// @name Search parameters
			//@{
				boost::shared_ptr<const SchedulesBasedService>	_service;
				boost::shared_ptr<const ScheduledService>	_scheduledService;
				boost::shared_ptr<const ContinuousService>	_continuousService;
			//@}

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Tests if two admin pages can be considered as the same one.
			/// @param other Other page to compare with. The other page will always be
			///	       of the same class.
			/// @return true if the other page can be considered as the same than the
			///         current one.
			/// @author Hugues
			/// @date 2009
			virtual bool _hasSameContent(
				const AdminInterfaceElement& other
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

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@author Hugues
			///	@date 2009
			ServiceAdmin();



			//////////////////////////////////////////////////////////////////////////
			/// Initialization of the parameters from a parameters map.
			///	@param map The parameters map to use for the initialization.
			///	@throw AdminParametersException if a parameter has incorrect value.
			///	@author Hugues
			///	@date 2009
			void setFromParametersMap(
				const util::ParametersMap& map
			);



			//////////////////////////////////////////////////////////////////////////
			/// Creation of the parameters map from the object attributes.
			///	@author Hugues
			///	@date 2009
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content of the admin element.
			///	@param stream Stream to write the page content on.
			///	@param request The current request
			///	@author Hugues Romain
			///	@date 2009
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
			///	@author Hugues Romain
			///	@date 2009
			bool isAuthorized(
				const security::User& user
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Title getter.
			///	@return The title of the page
			///	@author Hugues Romain
			///	@date 2009
			virtual std::string getTitle() const;



			//////////////////////////////////////////////////////////////////////////
			/// Icon getter.
			/// @return a car for scheduled service, a double arrow for continuous services.
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.2.0
			virtual std::string getIcon() const;



			void setService(boost::shared_ptr<const ScheduledService> value);
			void setService(boost::shared_ptr<const ContinuousService> value);
			boost::shared_ptr<const SchedulesBasedService> getService() const;

			virtual PageLinks _getCurrentTreeBranch() const;
		};
	}
}

#endif // SYNTHESE_ServiceAdmin_H__
