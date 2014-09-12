

//////////////////////////////////////////////////////////////////////////
/// PTRoadsAdmin class header.
///	@file PTRoadsAdmin.h
///	@author Hugues
///	@date 2009
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

#ifndef SYNTHESE_PTRoadsAdmin_H__
#define SYNTHESE_PTRoadsAdmin_H__

#include "ResultHTMLTable.h"
#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace road
	{
		class RoadPlace;
	}

	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// PTRoadsAdmin Admin compound class.
		///	@ingroup m35Admin refAdmin
		///	@author Hugues
		///	@date 2009
		class PTRoadsAdmin:
			public admin::AdminInterfaceElementTemplate<PTRoadsAdmin>
		{
		public:
			/// @name Parameter identifiers
			//@{
			static const std::string TAB_DETAILS;
			static const std::string TAB_PROPERTIES;
			//@}

		private:
			/// @name Search parameters
			//@{
				boost::shared_ptr<const road::RoadPlace> _roadPlace;
				html::ResultHTMLTable::RequestParameters	_requestParameters;
			//@}

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			///	@author Hugues
			///	@date 2009
			PTRoadsAdmin();



			//////////////////////////////////////////////////////////////////////////
			/// Initialization of the parameters from a parameters map.
			///	@param map The parameters map to use for the initialization.
			///	@throw AdminParametersException if a parameter has incorrect value.
			///	@author Hugues
			///	@date 2009
			void setFromParametersMap(
				const util::ParametersMap& map
			);



			//////////////////////////////////////////////////////////////////////////
			/// Creation of the parameters map from the object attributes.
			///	@author Hugues
			///	@date 2009
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content of the admin element.
			///	@param stream Stream to write the page content on.
			///	@param request The current request
			///	@author Hugues
			///	@date 2009
			void display(
				std::ostream& stream,
				const server::Request& request
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
			/// Builds links to the pages to put directly under the current page in
			/// the pages tree.
			///	@param currentPage Currently displayed page
			/// @param request Current request
			///	@return PageLinks each page to put under the current page
			///	@author Hugues
			///	@date 2009
			virtual AdminInterfaceElement::PageLinks getSubPages(
				const AdminInterfaceElement& currentPage,
				const server::Request& request
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Builds the tabs of the page.
			/// @param profile The profile of the current user
			/// @author Thomas Puigt
			/// @date 2014
			virtual void _buildTabs(
				const security::Profile& profile
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Title getter.
			///	@return The title of the page
			///	@author Hugues
			///	@date 2009
			virtual std::string getTitle() const;

			void setRoadPlace(boost::shared_ptr<const road::RoadPlace> value);

			virtual PageLinks _getCurrentTreeBranch() const;
		};
	}
}

#endif // SYNTHESE_PTRoadsAdmin_H__
