
/** VinciCustomerSearchAdminInterfaceElement class header.
	@file VinciCustomerSearchAdminInterfaceElement.h

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

#ifndef SYNTHESE_VinciCustomerSearchAdminInterfaceElement_H__
#define SYNTHESE_VinciCustomerSearchAdminInterfaceElement_H__

#include <vector>

#include <boost/shared_ptr.hpp>

#include "32_admin/AdminInterfaceElement.h"

namespace synthese
{
	namespace vinci
	{
		class VinciContract;

		/** VinciCustomerSearchAdminInterfaceElement Class.
			@ingroup m71Admin refAdmin
		*/
		class VinciCustomerSearchAdminInterfaceElement : public admin::AdminInterfaceElement
		{
			static const std::string PARAM_SEARCH_NAME;
			static const std::string PARAM_SEARCH_SURNAME;

			std::vector<boost::shared_ptr<VinciContract> > _contracts;
			std::string _searchName;
			std::string _searchSurname;
			bool _activeSearch;

		public:
			VinciCustomerSearchAdminInterfaceElement();
			
			/** Initialization of the parameters from a request.
				@param request The request to use for the initialization.
			*/
			void setFromParametersMap(const server::ParametersMap& map);
			std::string getTitle() const;
			void display(std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<admin::AdminRequest>* request = NULL) const;
			bool isAuthorized(const server::FunctionRequest<admin::AdminRequest>* request) const;
		};
	}
}

#endif // SYNTHESE_VinciCustomerSearchAdminInterfaceElement_H__
