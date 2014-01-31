
/** ResaCustomerMergeAdmin class header.
	@file ResaCustomerMergeAdmin.hpp
	@date 2014

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

#ifndef SYNTHESE_ResaCustomerMergeAdmin_H__
#define SYNTHESE_ResaCustomerMergeAdmin_H__

#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace resa
	{
		//////////////////////////////////////////////////////////////////////////
        /// 31.14 Admin : Reservation customers merging admin page.
		///	@ingroup m51Admin refAdmin
		///	@date 2014
		//////////////////////////////////////////////////////////////////////////
		/// Key : ResaCustomerMergeAdmin
		///
		class ResaCustomerMergeAdmin : 
			public admin::AdminInterfaceElementTemplate<ResaCustomerMergeAdmin>
		{

			public :

				static const std::string PARAM_SEARCH_NAME;
				static const std::string PARAM_SEARCH_SURNAME;
				static const std::string PARAM_SEARCH_LOGIN;
				static const std::string PARAM_USER_TO_DELETE;

			private:
		
				boost::shared_ptr<const security::User>		_userToMerge;
				boost::shared_ptr<const security::User>		_userToDelete;
				bool										_search;
                std::string                                 _message;

			public:

				ResaCustomerMergeAdmin();

				/** Initialization of the parameters from a parameters map.
					@param map The parameters map to use for the initialization.
					@throw AdminParametersException if a parameter has incorrect value.
				*/
				void setFromParametersMap(
					const util::ParametersMap& map
				);



				/** Parameters map generator, used when building an url to the admin page.
					@return util::ParametersMap The generated parameters map
				*/
				virtual util::ParametersMap getParametersMap() const;



				/** Display of the content of the admin element.
					@param stream Stream to write on.
				*/
				void display(
					std::ostream& stream,
					const server::Request& _request
				) const;



				/** Authorization control.
					@return bool True if the displayed page can be displayed
				*/
				bool isAuthorized(
					const security::User& user
				) const;



				void setUserToMerge(boost::shared_ptr<const security::User> user) { _userToMerge = user; }
				void setUserToDelete(boost::shared_ptr<const security::User> user) { _userToDelete = user; }
		};
	}
}

#endif // SYNTHESE_ResaCustomerMergeAdmin_H__
