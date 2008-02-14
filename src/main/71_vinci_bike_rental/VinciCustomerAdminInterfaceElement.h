
/** VinciCustomerAdminInterfaceElement class header.
	@file VinciCustomerAdminInterfaceElement.h

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Raphaël Murat - Vinci Park <rmurat@vincipark.com>

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

#ifndef SYNTHESE_VinciCustomerAdminInterfaceElement_H__
#define SYNTHESE_VinciCustomerAdminInterfaceElement_H__

#include "32_admin/AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace security
	{
		class User;
	}

	namespace vinci
	{
		class VinciContract;

		/** VinciCustomerAdminInterfaceElement Class.
			@ingroup m71Admin refAdmin
		*/
		class VinciCustomerAdminInterfaceElement : public admin::AdminInterfaceElementTemplate<VinciCustomerAdminInterfaceElement>
		{
			boost::shared_ptr<const VinciContract>	_contract;
			boost::shared_ptr<const security::User>	_user;

		public:
			VinciCustomerAdminInterfaceElement();
			
			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			void setFromParametersMap(const server::ParametersMap& map);

			void display(std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request = NULL) const;

			bool isAuthorized(const server::FunctionRequest<admin::AdminRequest>* request) const;

			/** Gets sub page of the designed parent page, which are from the current class.
				@param factoryKey Key of the parent class
				@param request User request
				@return PageLinks A link to the page if the parent is DBLogList and if the page is the currently displayed one
				@author Hugues Romain
				@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPagesOfParent(
				const PageLink& parentLink
				, const AdminInterfaceElement& currentPage
			) const;
		};
	}
}

#endif // SYNTHESE_VinciCustomerAdminInterfaceElement_H__
