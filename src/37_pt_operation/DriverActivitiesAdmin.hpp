
//////////////////////////////////////////////////////////////////////////
///	DriverActivitiesAdmin class header.
///	@file DriverActivitiesAdmin.hpp
///	@author Hugues Romain
///	@date 2012
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

#ifndef SYNTHESE_DriverActivitiesAdmin_H__
#define SYNTHESE_DriverActivitiesAdmin_H__

#include "AdminInterfaceElementTemplate.h"


namespace synthese
{
	namespace pt_operation
	{
		//////////////////////////////////////////////////////////////////////////
		/// DriverActivitiesAdmin Admin compound class.
		///	@ingroup m37Admin refAdmin
		///	@author Hugues Romain
		///	@date 2012
		class DriverActivitiesAdmin:
			public admin::AdminInterfaceElementTemplate<DriverActivitiesAdmin>
		{
		public:
			/// @name Parameter identifiers
			//@{
			// static const std::string PARAM_SEARCH_;
			//@}

		private:
			/// @name Search parameters
			//@{
			// boost::optional<std::string>				_searchXxx;
			// html::ResultHTMLTable::RequestParameters	_requestParameters;
			//@}

		protected:

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@author Hugues Romain
			///	@date 2012
			DriverActivitiesAdmin();



			//////////////////////////////////////////////////////////////////////////
			/// Initialization of the parameters from a parameters map.
			///	@param map The parameters map to use for the initialization.
			///	@throw AdminParametersException if a parameter has incorrect value.
			///	@author Hugues Romain
			///	@date 2012
			void setFromParametersMap(
				const util::ParametersMap& map
			);



			//////////////////////////////////////////////////////////////////////////
			/// Creation of the parameters map from the object attributes.
			///	@author Hugues Romain
			///	@date 2012
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content of the admin element.
			///	@param stream Stream to write the page content on.
			///	@param request The current request
			///	@author Hugues Romain
			///	@date 2012
			void display(
				std::ostream& stream,
				const server::Request& _request
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Authorization check.
			/// Returns if the page can be displayed. In most cases, the needed right
			/// level is READ.
			///	@param request The current request
			///	@return bool True if the displayed page can be displayed
			///	@author Hugues Romain
			///	@date 2012
			bool isAuthorized(
				const security::User& user
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Builds links to the pages to put directly under the current page in
			/// the pages tree.
			///	@param currentPage Currently displayed page
			/// @param request Current request
			///	@return PageLinks each page to put under the current page
			///	@author Hugues Romain
			///	@date 2012
			virtual AdminInterfaceElement::PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Builds links to the pages of the current class to put directly under
			/// a module admin page in the pages tree.
			///	@param module The module
			///	@param currentPage Currently displayed page
			/// @param request Current request
			///	@return PageLinks each page to put under the module page in the page
			///	@author Hugues Romain
			///	@date 2012
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const server::ModuleClass& module,
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;
		};
}	}

#endif // SYNTHESE_DriverActivitiesAdmin_H__

