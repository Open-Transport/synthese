
/** VinciAccountsAdminInterfaceElement class header.
	@file VinciAccountsAdminInterfaceElement.h
	@author Hugues Romain
	@date 2008

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#ifndef SYNTHESE_VinciAccountsAdminInterfaceElement_H__
#define SYNTHESE_VinciAccountsAdminInterfaceElement_H__

#include "32_admin/AdminInterfaceElementTemplate.h"

#include "05_html/ResultHTMLTable.h"

namespace synthese
{
	namespace vinci
	{
		/** VinciAccountsAdminInterfaceElement Class.
			@ingroup m71Admin refAdmin
			@author Hugues Romain
			@date 2008
		*/
		class VinciAccountsAdminInterfaceElement : public admin::AdminInterfaceElementTemplate<VinciAccountsAdminInterfaceElement>
		{
			html::ResultHTMLTable::RequestParameters	_requestParameters;
			std::string									_searchName;
			std::string									_searchCode;

		public:
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_CODE;
			static const std::string PARAMETER_UNIT_PRICE;
			static const std::string PARAMETER_LOCKED;
			static const std::string PARAMETER_STOCK;

			VinciAccountsAdminInterfaceElement();
			
			/** Initialization of the parameters from a parameters map.
				@param map The parameters map to use for the initialization.
				@throw AdminParametersException if a parameter has incorrect value.
				@author Hugues Romain
				@date 2008
			*/
			void setFromParametersMap(const server::ParametersMap& map);

			/** Display of the content of the admin element.
				@param stream Stream to write on.
				@param request The current request
				@author Hugues Romain
				@date 2008
			*/
			void display(std::ostream& stream, interfaces::VariablesMap& , const server::FunctionRequest<admin::AdminRequest>* request=NULL) const;

			
			/** Authorization control.
				@param request The current request
				@return bool True if the displayed page can be displayed
				@author Hugues Romain
				@date 2008
			*/
			bool isAuthorized(const server::FunctionRequest<admin::AdminRequest>* request) const;
			

			/** Gets sub page of the designed parent page, which are from the current class.
				@param factoryKey Key of the parent class
				@param request User request
				@return PageLinks A link to the page if the parent is Home
				@author Hugues Romain
				@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPagesOfParent(
				const PageLink& parentLink
				, const AdminInterfaceElement& currentPage
				, const server::FunctionRequest<admin::AdminRequest>* request
				) const;
		};
	}
}

#endif // SYNTHESE_VinciAccountsAdminInterfaceElement_H__
