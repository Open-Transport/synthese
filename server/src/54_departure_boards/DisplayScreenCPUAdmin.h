////////////////////////////////////////////////////////////////////////////////
/// DisplayScreenCPUAdmin class header.
///	@file DisplayScreenCPUAdmin.h
///	@author Hugues Romain
///	@date 2008-12-26 14:51
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

#ifndef SYNTHESE_DisplayScreenCPUAdmin_H__
#define SYNTHESE_DisplayScreenCPUAdmin_H__

#include "AdminInterfaceElementTemplate.h"
#include "DBLogHTMLView.h"

namespace synthese
{
	namespace departure_boards
	{
		class DisplayScreenCPU;

		/** Display screen central unit edition admin page class.
			@ingroup m54Admin refAdmin
			@author Hugues Romain
			@date 2008
		*/
		class DisplayScreenCPUAdmin
		:	public admin::AdminInterfaceElementTemplate<DisplayScreenCPUAdmin>
		{
			boost::shared_ptr<const DisplayScreenCPU>	_cpu;
			dblog::DBLogHTMLView						_generalLogView;
			dblog::DBLogHTMLView						_maintenanceLogView;

		public:
			static const std::string TAB_TECHNICAL;
			static const std::string TAB_MAINTENANCE;
			static const std::string TAB_LOG;
			static const std::string TAB_DISPLAYS;

			DisplayScreenCPUAdmin();

			void setCPU(boost::shared_ptr<const DisplayScreenCPU> value);
			boost::shared_ptr<const DisplayScreenCPU> getCPU() const;

			/** Initialization of the parameters from a parameters map.
				@param map The parameters map to use for the initialization.
				@throw AdminParametersException if a parameter has incorrect value.
				@author Hugues Romain
				@date 2008
			*/
			virtual void setFromParametersMap(
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


			/** Title generator.
				@return The title of the page
				@author Hugues Romain
				@date 2008
			*/
			virtual std::string getTitle() const;

			virtual AdminInterfaceElement::PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;

			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;

			////////////////////////////////////////////////////////////////////
			/// Tabs builder.
			/// @author Hugues Romain
			/// @date 2008
			/// This method has in charge to :
			///		- the control of the profile of the user to determine the
			///			tabs list.
			///		- to set _tabBuilded at true to avoid the method to be
			///			relaunched
			////////////////////////////////////////////////////////////////////
			virtual void _buildTabs(
				const security::Profile& profile
			) const;
		};
	}
}

#endif // SYNTHESE_DisplayScreenCPUAdmin_H__
