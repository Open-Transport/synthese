
/** ResaCustomerAdmin class header.
	@file ResaCustomerAdmin.h
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

#ifndef SYNTHESE_ResaCustomerAdmin_H__
#define SYNTHESE_ResaCustomerAdmin_H__

#include "ResultHTMLTable.h"
#include "AdminInterfaceElementTemplate.h"
#include "DBLogHTMLView.h"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace resa
	{
		/** ResaCustomerAdmin Class.
			@ingroup m51Admin refAdmin
			@author Hugues Romain
			@date 2008
		*/
		class ResaCustomerAdmin :
			public admin::AdminInterfaceElementTemplate<ResaCustomerAdmin>
		{
			boost::shared_ptr<const security::User>		_user;
			dblog::DBLogHTMLView						_log;

			static const std::string TAB_PROPERTIES;
			static const std::string TAB_PARAMETERS;
			static const std::string TAB_LOG;
			static const std::string TAB_RESERVATIONS;
			static const std::string TAB_CANCELS;
			static const std::string TAB_CANCELS_DEADLINE;
			static const std::string TAB_ABSENCES;

		public:

			ResaCustomerAdmin();

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

			/** Gets sub page of the designed parent page, which are from the current class.
				@param parentLink Link to the parent page
				@param currentPage Currently displayed page
				@return PageLinks each subpage of the parent page designed in parentLink
				@author Hugues Romain
				@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const server::ModuleClass& module,
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;

			/** Title generator.
				@return The title of the page
				@author Hugues Romain
				@date 2008
			*/
			virtual std::string getTitle() const;


			virtual void _buildTabs(
				const security::Profile& profile
			) const;

			void setUser(boost::shared_ptr<security::User> value);
			void setUser(boost::shared_ptr<const security::User> value);
			boost::shared_ptr<const security::User> getUser() const;

			virtual bool _hasSameContent(const AdminInterfaceElement& other) const;
		};
	}
}

#endif // SYNTHESE_ResaCustomerAdmin_H__
