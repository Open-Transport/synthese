
//////////////////////////////////////////////////////////////////////////
/// PTQualityControlAdmin class header.
///	@file PTQualityControlAdmin.hpp
///	@author Hugues Romain
///	@date 2011
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

#ifndef SYNTHESE_PTQualityControlAdmin_H__
#define SYNTHESE_PTQualityControlAdmin_H__

#include "ResultHTMLTable.h"
#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// PTQualityControlAdmin Admin compound class.
		///	@ingroup m35Admin refAdmin
		///	@author Hugues Romain
		///	@date 2011
		class PTQualityControlAdmin:
			public admin::AdminInterfaceElementTemplate<PTQualityControlAdmin>
		{
		public:
			/// @name Parameter identifiers
			//@{
				static const std::string PARAM_RUN_CONTROL;

				static const std::string TAB_STOPS_WITHOUT_COORDINATE;
				static const std::string TAB_CITIES_WITHOUT_MAIN_STOP;
				static const std::string TAB_EDGES_AND_GEOMETRIES;
				static const std::string TAB_RANK_CONTINUITY;
				static const std::string TAB_DOUBLE_ROUTES;
				static const std::string TAB_SPEED;
				static const std::string TAB_ASCENDING_SCHEDULES;
			//@}

		private:
			/// @name Search parameters
			//@{
				bool _runControl;
			//@}

		protected:



			//////////////////////////////////////////////////////////////////////////
			/// Builds the tabs of the page.
			/// @param profile The profile of the current user
			/// @author Hugues Romain
			/// @date 2011
			virtual void _buildTabs(
				const security::Profile& profile
			) const;

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@author Hugues Romain
			///	@date 2011
			PTQualityControlAdmin();

			void setRunControl(bool value){ _runControl = value; }

			//////////////////////////////////////////////////////////////////////////
			/// Initialization of the parameters from a parameters map.
			///	@param map The parameters map to use for the initialization.
			///	@throw AdminParametersException if a parameter has incorrect value.
			///	@author Hugues Romain
			///	@date 2011
			void setFromParametersMap(
				const util::ParametersMap& map
			);



			//////////////////////////////////////////////////////////////////////////
			/// Creation of the parameters map from the object attributes.
			///	@author Hugues Romain
			///	@date 2011
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content of the admin element.
			///	@param stream Stream to write the page content on.
			///	@param request The current request
			///	@author Hugues Romain
			///	@date 2011
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
			///	@date 2011
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
			///	@date 2011
			virtual AdminInterfaceElement::PageLinks getSubPagesOfModule(
				const server::ModuleClass& module,
				const AdminInterfaceElement& currentPage,
				const admin::AdminRequest& request
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Title getter.
			///	@return The title of the page
			///	@author Hugues Romain
			///	@date 2011
			virtual std::string getTitle() const;
		};
	}
}

#endif // SYNTHESE_PTQualityControlAdmin_H__
