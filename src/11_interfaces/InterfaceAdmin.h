

//////////////////////////////////////////////////////////////////////////
/// InterfaceAdmin class header.
///	@file InterfaceAdmin.h
///	@author Hugues
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

#ifndef SYNTHESE_InterfaceAdmin_H__
#define SYNTHESE_InterfaceAdmin_H__

#include "ResultHTMLTable.h"
#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace interfaces
	{
		class Interface;

		//////////////////////////////////////////////////////////////////////////
		/// InterfaceAdmin Admin compound class.
		///	@ingroup m11Admin refAdmin
		///	@author Hugues
		///	@date 2010
		class InterfaceAdmin:
			public admin::AdminInterfaceElementTemplate<InterfaceAdmin>
		{
		public:
			/// @name Parameter identifiers
			//@{
			// static const std::string PARAM_SEARCH_;
			//@}

		private:
			/// @name Search parameters
			//@{
				boost::shared_ptr<const Interface> _interface;
			//@}

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Tests if two admin pages can be considered as the same one.
			/// @param other Other page to compare with. The other page will always be
			///	       of the same class.
			/// @return true if the other page can be considered as the same than the
			///         current one.
			/// @author Hugues
			/// @date 2010
			virtual bool _hasSameContent(
				const AdminInterfaceElement& other
			) const;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@author Hugues
			///	@date 2010
			InterfaceAdmin();



			//////////////////////////////////////////////////////////////////////////
			/// Initialization of the parameters from a parameters map.
			///	@param map The parameters map to use for the initialization.
			///	@throw AdminParametersException if a parameter has incorrect value.
			///	@author Hugues
			///	@date 2010
			void setFromParametersMap(
				const util::ParametersMap& map
			);



			//////////////////////////////////////////////////////////////////////////
			/// Creation of the parameters map from the object attributes.
			///	@author Hugues
			///	@date 2010
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content of the admin element.
			///	@param stream Stream to write the page content on.
			///	@param request The current request
			///	@author Hugues
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
			///	@author Hugues
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
			///	@author Hugues
			///	@date 2010
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const server::ModuleClass& module,
				const AdminInterfaceElement& currentPage,
				const admin::AdminRequest& request
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Builds links to the pages to put directly under the current page in
			/// the pages tree.
			///	@param currentPage Currently displayed page
			/// @param request Current request
			///	@return PageLinks each page to put under the current page
			///	@author Hugues
			///	@date 2010
			virtual AdminInterfaceElement::PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const admin::AdminRequest& request
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Title getter.
			///	@return The title of the page
			///	@author Hugues
			///	@date 2010
			virtual std::string getTitle() const;
		};
	}
}

#endif // SYNTHESE_InterfaceAdmin_H__
