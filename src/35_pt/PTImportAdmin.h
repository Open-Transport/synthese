

//////////////////////////////////////////////////////////////////////////
/// PTImportAdmin class header.
///	@file PTImportAdmin.h
///	@author Hugues
///	@date 2009
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

#ifndef SYNTHESE_PTImportAdmin_H__
#define SYNTHESE_PTImportAdmin_H__

#include "ResultHTMLTable.h"
#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace impex
	{
		class DataSource;
	}

	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// 35.14 Data sources access.
		///	@ingroup m35Admin refAdmin
		///	@author Hugues Romain
		///	@date 2009-2010
		//////////////////////////////////////////////////////////////////////////
		/// Key : PTImportAdmin
		class PTImportAdmin:
			public admin::AdminInterfaceElementTemplate<PTImportAdmin>
		{
		public:
	
		private:
	
		protected:

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@author Hugues
			///	@date 2009
			PTImportAdmin();
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Initialization of the parameters from a parameters map.
			///	@param map The parameters map to use for the initialization.
			///	@throw AdminParametersException if a parameter has incorrect value.
			///	@author Hugues
			///	@date 2009
			void setFromParametersMap(
				const server::ParametersMap& map
			);

			
			
			//////////////////////////////////////////////////////////////////////////
			/// Creation of the parameters map from the object attributes.
			///	@author Hugues
			///	@date 2009
			server::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content of the admin element.
			///	@param stream Stream to write the page content on.
			///	@param variables Environment variables defined by the interface
			///	@param request The current request
			///	@author Hugues
			///	@date 2009
			void display(
				std::ostream& stream,
				interfaces::VariablesMap& variables,
				const admin::AdminRequest& request
			) const;


			
			//////////////////////////////////////////////////////////////////////////
			/// Authorization control.
			/// Returns if the page can be displayed. In most cases, the needed right
			/// level is READ.
			///	@param request The current request
			///	@return bool True if the displayed page can be displayed
			///	@author Hugues
			///	@date 2009
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
			///	@date 2009
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const std::string& moduleKey,
				const AdminInterfaceElement& currentPage,
				const admin::AdminRequest& request
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Sub pages getter.
			///	@return PageLinks Ordered vector of sub pages links
			///	@param currentPage Currently displayed page
			/// @param request Current request
			///	@author Hugues Romain
			///	@date 2010
			///	@since 3.1.18
			//////////////////////////////////////////////////////////////////////////
			///	The only one subpage is PTStopsImportWizardAdmin
			virtual PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const admin::AdminRequest& request
			) const;
		};
	}
}

#endif // SYNTHESE_PTImportAdmin_H__
