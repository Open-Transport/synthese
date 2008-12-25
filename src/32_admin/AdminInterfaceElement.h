
/** AdminInterfaceElement class header.
	@file AdminInterfaceElement.h

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

#ifndef SYNTHESE_INTERFACES_ADMIN_INTERFACE_ELEMENT_H
#define SYNTHESE_INTERFACES_ADMIN_INTERFACE_ELEMENT_H

#include <string>

#include "FactoryBase.h"
#include "11_interfaces/Types.h"
#include "FunctionRequest.h"
#include "Env.h"

#include <vector>

namespace synthese
{
	namespace security
	{
		class Right;
	}

	namespace admin
	{
	    class AdminRequest;

		////////////////////////////////////////////////////////////////////
		/// Composant d'administration.
		///	@ingroup m14
		///	
		///	Un composant d'administration est un formulaire paramétrable, destiné à effectuer une écriture en temps réel dans la base SYNTHESE, dans un but de paramétrage de l'application (il ne s'agit pas du seul moyen d'écriture).
		///	Une même donnée peut être mise à jour par autant de composants que nécessaire, défini avant tout sur la base d'une ergonomie de qualité.
		///	Chaque composant est rattaché au module correspondant aux données à modifier sous forme d'une sous-classe de AdminInterfaceElement. 
		///	Le comportement des composants d'administration est en général défini d'après les @ref defRight "habilitations" de l'utilisateur connecté.
		///	Par exemple :
		///		- un utilisateur anonyme n'a accès à aucun composant d'administration
		///		- un administrateur a accès à tous les composants d'administration.
		////////////////////////////////////////////////////////////////////
		class AdminInterfaceElement
		:	public util::FactoryBase<AdminInterfaceElement>
		{
		public:
			////////////////////////////////////////////////////////////////////
			/// Link to the administrative page.
			////////////////////////////////////////////////////////////////////
			struct PageLink
			{
				std::string name;
				std::string factoryKey;
				std::string parameterName;
				std::string parameterValue;
				std::string icon;
				const server::FunctionRequest<admin::AdminRequest>* request;

				bool operator==(const PageLink& other) const;

				
				
				////////////////////////////////////////////////////////////////////
				///	URL generator.
				///	@return std::string the URL to access to the page
				///	@author Hugues Romain
				///	@date 2008
				////////////////////////////////////////////////////////////////////
				std::string getURL(
				) const;

				
				
				////////////////////////////////////////////////////////////////////
				///	Admin Page generator.
				///	@return AdminInterfaceElement* the page (delete after use)
				///	@author Hugues Romain
				///	@date 2008
				////////////////////////////////////////////////////////////////////
				AdminInterfaceElement* getAdminPage(
				) const;
			};
			
			typedef std::vector<PageLink> PageLinks;
			
			////////////////////////////////////////////////////////////////////
			/// Tree of links to administrative pages.
			////////////////////////////////////////////////////////////////////
			struct PageLinksTree
			{
				PageLink					pageLink;
				std::vector<PageLinksTree>	subPages;
				bool						isNodeOpened;

				PageLinksTree()
					: pageLink(), subPages(), isNodeOpened(false) {}
			};



			////////////////////////////////////////////////////////////////////
			/// Tab and its corresponding div in content of administrative page.
			///
			///	To use tabs :
			///		- call begin method to start displaying the div corresponding
			///			to the first tab. Test the output of the method to decide if 
			///			the content can be displayed. 
			///		- call begin for each following tab (the ending of the
			///			preceding one is automatic
			///		- call end to close the last div.
			///	
			///	The tabs list is displayed by the library interface element
			///	AdminTabsInterfaceElement
			///
			/// The generated HTML code is a div element by tab containing its
			/// corresponding content. The class of the div is @c tabdiv.
			/// The id of the div is @c tab_(id)_content.
			////////////////////////////////////////////////////////////////////
			class Tab
			{
			private:
				std::string	_title;
				std::string	_id;
				bool		_writePermission;



			public:
				////////////////////////////////////////////////////////////////////
				///	Tab constructor.
				///	@param title Title of the tab
				/// @param id ID of the tab
				/// @param writePermission the user has write permissions on the 
				///		content of the tab
				///	@author Hugues Romain
				///	@date 2008
				////////////////////////////////////////////////////////////////////
				Tab(
					std::string title = std::string(),
					std::string id = std::string(),
					bool writePermission = true
				);
				
				
				////////////////////////////////////////////////////////////////////
				///	Title getter.
				///	@return Title of the tab.
				///	@author Hugues Romain
				///	@date 2008
				////////////////////////////////////////////////////////////////////
				const std::string& getTitle() const;

				
				
				////////////////////////////////////////////////////////////////////
				///	ID getter.
				///	@return ID of the tab.
				///	@author Hugues Romain
				///	@date 2008
				////////////////////////////////////////////////////////////////////
				const std::string& getId() const;

				
				
				////////////////////////////////////////////////////////////////////
				///	Write permission getter.
				///	@return bool true if the user can use the content of the tab
				///		with write permissions
				///	@author Hugues Romain
				///	@date 2008
				////////////////////////////////////////////////////////////////////
				bool getWritePermission() const;
			};

			////////////////////////////////////////////////////////////////////
			/// Tab vector type.
			////////////////////////////////////////////////////////////////////
			typedef std::vector<Tab>	Tabs;

			static const std::string PARAMETER_TAB;

		private:
			mutable const Tab*	_currentTab;

		protected:

			//! \name Cache
			//@{
				mutable PageLinks		_treePosition;
				mutable PageLinksTree	_tree;
				mutable	Tabs			_tabs;
				mutable bool			_tabBuilded;
			//@}

			//! \name Temporary data
			//@{
				mutable util::Env	_env;
			//@}

			//! \name Properties
			//@{
				const server::FunctionRequest<admin::AdminRequest>*		_request;
			//@}

			PageLinksTree	_buildTreeRecursion(
				const AdminInterfaceElement* page,
				const PageLinks position
			) const;

			
			
			////////////////////////////////////////////////////////////////////
			///	Tree generator.
			///	@author Hugues Romain
			///	@date 2008
			/// Generates the whole tree depending on the context.
			/// This method has to be called by the current displayed admin page.
			////////////////////////////////////////////////////////////////////
			void _buildTree(
			) const;

			
			
			//! \name Tabs management
			//@{
				////////////////////////////////////////////////////////////////////
				///	Tabs generator.
				///	@author Hugues Romain
				///	@date 2008
				/// Virtual method :
				///		- default behavior : creates nothing
				///		- can be overloaded : tabs creation depending on the context
				/// The _buildTabs method is in charge of : 
				///		- the control of the profile of the user to determine the 
				///			tabs list. 
				///		- to set _tabBuilded at true to avoid the method to be
				///			relaunched
				////////////////////////////////////////////////////////////////////
				virtual void _buildTabs() const;



				////////////////////////////////////////////////////////////////////
				///	Displays a begin of a content linked by a tab.
				/// @param stream Stream to display the code on.
				///	@param id ID of the tab
				/// @return true if the content must be displayed
				///	@author Hugues Romain
				///	@date 2008
				/// The code is displayed only if the tab is present in the tabs
				/// list of the page (depending of the rights of the user)
				/// 
				/// If necessary, this method closes automatically the last displayed
				/// tab.
				////////////////////////////////////////////////////////////////////
				bool openTabContent(
					std::ostream& stream,
					const std::string& id
				) const;

				
				
				////////////////////////////////////////////////////////////////////
				///	Displays the end of a content linked by a tab.
				/// @param stream Stream to display the code on.
				///	@author Hugues Romain
				///	@date 2008
				////////////////////////////////////////////////////////////////////
				void closeTabContent(
					std::ostream& stream
				) const;

				
				
				////////////////////////////////////////////////////////////////////
				///	Test if the user has write permissions on the currently
				///	displayed tab.
				///	@return bool True if the user has write permissions
				///	@author Hugues Romain
				///	@date 2008
				////////////////////////////////////////////////////////////////////
				bool tabHasWritePermissions() const;
			//@}
				


		public:
			////////////////////////////////////////////////////////////////////
			///	AdminInterfaceElement constructor.
			///	@author Hugues Romain
			///	@date 2008
			////////////////////////////////////////////////////////////////////
			AdminInterfaceElement();

			//! \name Setters
			//@{
				////////////////////////////////////////////////////////////////////
				///	Request setter.
				///	@param value the request calling the page
				///	@author Hugues Romain
				///	@date 2008
				////////////////////////////////////////////////////////////////////
				void setRequest(const server::FunctionRequest<admin::AdminRequest>*	value);
			//@}


			//! \name Getters
			//@{
				const PageLinks&		getTreePosition()	const;
				const PageLinksTree&	getTree()			const;
				const Tabs&				getTabs()			const;
			//@}


			//! \name Queries
			//@{
				////////////////////////////////////////////////////////////////////
				///	Tab getter.
				///	@param key key of the tab
				///	@return const Tab& the found tab
				///	@author Hugues Romain
				///	@date 2008
				/// @throw Exception if tab was not found
				////////////////////////////////////////////////////////////////////
				const Tab&	getTab(const std::string& key) const;
			//@}
			
			
			////////////////////////////////////////////////////////////////////
			///	displayTabs.
			///	@param stream
			///	@param variables
			///	@param request
			///	@return void
			///	@author Hugues Romain
			///	@date 2008
			////////////////////////////////////////////////////////////////////
			void displayTabs(
				std::ostream& stream,
				interfaces::VariablesMap& variables
			) const;



			//! \name Virtual initialization method
			//@{
				/// Initialization of the parameters from a request.
				///	@param request The request to use for the initialization.
				///
				virtual void setFromParametersMap(const server::ParametersMap& map) = 0;
			//@}

			//! \name Virtual output methods
			//@{
				/** Parameters map generator, used when building an url to the admin page.
					@return server::ParametersMap The generated parameters map
					@author Hugues Romain
					@date 2007					
				*/
				virtual server::ParametersMap getParametersMap() const { return server::ParametersMap(); }

				/** Authorization control.
					@param request The current request
					@return bool True if the displayed page can be displayed
					@author Hugues Romain
					@date 2007					
				*/
				virtual bool isAuthorized() const = 0;

				/** Display of the content of the admin element.
					@param stream Stream to write on.
					@param variables Execution variables
					@param request Current request
				*/
				virtual void display(
					std::ostream& stream,
					interfaces::VariablesMap& variables
				) const = 0;

				virtual std::string getIcon() const = 0;
				virtual std::string getTitle() const = 0;
				virtual std::string getParameterName() const;
				virtual std::string getParameterValue() const;

				/** Current page link getter.
					@return const PageLink& Current page link
					@author Hugues Romain
					@date 2008					
				*/
				PageLink getPageLink() const;

				/** Gets sub page of the designed parent page, which are from the current class.
					@param factoryKey Key of the parent class
					@param request User request
					@return PageLinks Ordered vector of sub pages links
					@author Hugues Romain
					@date 2008
					
				*/
				virtual PageLinks getSubPagesOfParent(
					const PageLink& parentLink
					, const AdminInterfaceElement& currentPage
				) const = 0;


				/** Sub pages getter.
					@param request User request
					@return PageLinks Ordered vector of sub pages links
					@author Hugues Romain
					@date 2008
					
					The default implementation handles the auto registration of administrative components by getSuperiorVirtual() method.
					This method can be overloaded to create customized sub tree.
				*/
				virtual PageLinks getSubPages(
					const AdminInterfaceElement& currentPage
				) const;



				/** Gets the opening position of the node in the tree view.
					@return bool true = the page is visible, all the superior nodes are open, false = the page must not be visible, and will be hidden if no one another page of the same level must be visible.
					@author Hugues Romain
					@date 2008					
				*/
				virtual bool isPageVisibleInTree(
					const AdminInterfaceElement& currentPage
				) const;
			//@}
		};
	}
}

#endif

