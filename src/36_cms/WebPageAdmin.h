

//////////////////////////////////////////////////////////////////////////
/// WebPageAdmin class header.
///	@file WebPageAdmin.h
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

#ifndef SYNTHESE_WebPageAdmin_H__
#define SYNTHESE_WebPageAdmin_H__

#include "AdminInterfaceElementTemplate.h"
#include "StaticActionRequest.h"
#include "WebPageTableSync.h"

namespace synthese
{
	namespace html
	{
		class HTMLTable;
		class HTMLForm;
	}

	namespace cms
	{
		class WebPageRemoveAction;
		class WebPageAddAction;
		class WebPageMoveAction;
		class WebPage;

		//////////////////////////////////////////////////////////////////////////
		/// 36.14 Admin : Web page edition.
		///	@ingroup m56Admin refAdmin
		///	@author Hugues Romain
		///	@date 2010
		//////////////////////////////////////////////////////////////////////////
		/// <h3>Parameters</h3>
		///	<ul>
		///		<li>roid : id of the page to edit</li>
		///	</ul>
		///
		/// <h3>Tabs</h3>
		/// @copydoc WebPageAdmin::_buildTabs
		class WebPageAdmin:
			public admin::AdminInterfaceElementTemplate<WebPageAdmin>
		{
		public:
			static const std::string TAB_CONTENT;
			static const std::string TAB_TREE;
			static const std::string TAB_LINKS;
			
		private:
			/// @name Attributes
			//@{
				boost::shared_ptr<const WebPage> _page;
			//@}

			static void _displaySubPages(
				std::ostream& stream,
				const WebPageTableSync::SearchResult& pages,
				server::StaticActionRequest<WebPageRemoveAction>& deleteRequest,
				server::StaticActionRequest<WebPageMoveAction>& moveRequest,
				const admin::AdminRequest& request,
				html::HTMLTable& t,
				html::HTMLForm& f,
				std::size_t depth = 0
			);

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
			WebPageAdmin();
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Initialization of the parameters from a parameters map.
			///	@param map The parameters map to use for the initialization.
			///	@throw AdminParametersException if a parameter has incorrect value.
			///	@author Hugues
			///	@date 2010
			void setFromParametersMap(
				const server::ParametersMap& map
			);

			
			
			//////////////////////////////////////////////////////////////////////////
			/// Creation of the parameters map from the object attributes.
			///	@author Hugues
			///	@date 2010
			server::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content of the admin element.
			///	@param stream Stream to write the page content on.
			///	@param variables Environment variables defined by the interface
			///	@param request The current request
			///	@author Hugues
			///	@date 2010
			void display(
				std::ostream& stream,
				interfaces::VariablesMap& variables,
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
			/// Gets the current tree branch.
			/// @return the parent page
			virtual AdminInterfaceElement::PageLinks _getCurrentTreeBranch() const;


			
			//////////////////////////////////////////////////////////////////////////
			/// Title getter.
			///	@return The title of the page
			///	@author Hugues
			///	@date 2010
			virtual std::string getTitle() const;



			//////////////////////////////////////////////////////////////////////////
			/// Builds the tabs of the page.
			/// @param profile The profile of the user
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.18
			//////////////////////////////////////////////////////////////////////////
			/// The following tabs are created :
			///	<ul>
			///		<li>co : Contenu : edition of the content of the page</li>
			///		<li>tr : Arborescence : edition of the position of the page in the site, handling of sub-pages</li>
			///		<li>li : Liens : edition of the links starting from the page</li>
			///	</ul>
			virtual void _buildTabs(
				const security::Profile& profile
			) const;


			//! @name Setters
			//@{
				void setPage(boost::shared_ptr<const WebPage> value) { _page = value; }
			//@}


			static void	DisplaySubPages(
				std::ostream& stream,
				util::RegistryKeyType parentId,
				server::StaticActionRequest<WebPageAddAction>& createRequest,
				server::StaticActionRequest<WebPageRemoveAction>& deleteRequest,
				server::StaticActionRequest<WebPageMoveAction>& moveRequest,
				const admin::AdminRequest& request
			);
		};
	}
}

#endif // SYNTHESE_WebPageAdmin_H__
