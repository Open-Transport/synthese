

//////////////////////////////////////////////////////////////////////////
/// PTUseRulesAdmin class header.
///	@file PTUseRulesAdmin.h
///	@author Hugues Romain
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#ifndef SYNTHESE_PTUseRulesAdmin_H__
#define SYNTHESE_PTUseRulesAdmin_H__

#include "ResultHTMLTable.h"
#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// 35.14 Admin : Transport use rules list.
		///	@ingroup m35Admin refAdmin
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.16
		//////////////////////////////////////////////////////////////////////////
		/// Key : PTUseRulesAdmin
		///
		/// Parameters :
		///	<ul>
		///		<li>na : filter on the name of the rules</li>
		///	</ul>
		class PTUseRulesAdmin:
			public admin::AdminInterfaceElementTemplate<PTUseRulesAdmin>
		{
		public:
			/// @name Parameter identifiers
			//@{
				static const std::string PARAMETER_SEARCH_NAME;
			//@}

		private:
			/// @name Search parameters
			//@{
				std::string									_searchName;
				html::ResultHTMLTable::RequestParameters	_requestParameters;
			//@}

		protected:


		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@author Hugues Romain
			///	@date 2010
			PTUseRulesAdmin();
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Initialization of the parameters from a parameters map.
			///	@param map The parameters map to use for the initialization.
			///	@throw AdminParametersException if a parameter has incorrect value.
			///	@author Hugues Romain
			///	@date 2010
			void setFromParametersMap(
				const server::ParametersMap& map
			);

			
			
			//////////////////////////////////////////////////////////////////////////
			/// Creation of the parameters map from the object attributes.
			///	@author Hugues Romain
			///	@date 2010
			server::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content of the admin element.
			///	@param stream Stream to write the page content on.
			///	@param request The current request
			///	@author Hugues Romain
			///	@date 2010
			void display(
				std::ostream& stream,
				const admin::AdminRequest& _request
			) const;


			
			//////////////////////////////////////////////////////////////////////////
			/// Authorization control.
			/// Returns if the page can be displayed. In most cases, the needed right
			/// level is READ.
			///	@param request The current request
			///	@return bool True if the displayed page can be displayed
			///	@author Hugues Romain
			///	@date 2010
			bool isAuthorized(
				const security::User& user
			) const;


			
			//////////////////////////////////////////////////////////////////////////
			/// Builds links to the pages of the current class to put directly under
			/// a module admin page in the pages tree.
			///	@param moduleKey Key of the module
			///	@param currentPage Currently displayed page
			/// @param request Current request
			///	@return PageLinks each page to put under the module page in the page
			///	@author Hugues Romain
			///	@date 2010
			/// @todo A DEFAULT IMPLEMENTATION RETURNS NOTHING.
			///       REMOVE THIS METHOD OVERLOAD IF IT IS NOT NECESSARY.
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const std::string& moduleKey,
				const AdminInterfaceElement& currentPage,
				const admin::AdminRequest& request
			) const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Builds links to the pages to put directly under the current page in
			/// the pages tree.
			///	@param currentPage Currently displayed page
			/// @param request Current request
			///	@return PageLinks each page to put under the current page
			///	@author Hugues Romain
			///	@date 2010
			/// @todo A DEFAULT IMPLEMENTATION RETURNS NOTHING.
			///       REMOVE THIS METHOD OVERLOAD IF IT IS NOT NECESSARY.
			virtual AdminInterfaceElement::PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const admin::AdminRequest& request
			) const;
		};
	}
}

#endif // SYNTHESE_PTUseRulesAdmin_H__
