
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
			
			struct PageLink
			{
				std::string name;
				std::string factoryKey;
				std::string parameterName;
				std::string parameterValue;
				std::string icon;

				bool operator==(const PageLink& other) const;

				std::string getURL(const server::FunctionRequest<admin::AdminRequest>* request) const;
			};
			
			typedef std::vector<PageLink> PageLinks;
			
			struct PageLinksTree
			{
				PageLink					pageLink;
				std::vector<PageLinksTree>	subPages;
				bool						isNodeOpened;

				PageLinksTree()
					: pageLink(), subPages(), isNodeOpened(false) {}
			};


		protected:
			mutable PageLinks		_treePosition;
			mutable PageLinksTree	_tree;
			util::Env				_env;

			PageLinksTree	_buildTreeRecursion(
				const AdminInterfaceElement* page
				, const server::FunctionRequest<admin::AdminRequest>* request
				, const PageLinks position
			) const;

			void _buildTree(
				const server::FunctionRequest<admin::AdminRequest>* request
			) const;

		public:
			const PageLinks&		getTreePosition(const server::FunctionRequest<admin::AdminRequest>* request) const;
			const PageLinksTree&	getTree(const server::FunctionRequest<admin::AdminRequest>* request)	const;

			static AdminInterfaceElement*	GetAdminPage(const PageLink& pageLink);


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
				virtual bool isAuthorized(const server::FunctionRequest<AdminRequest>* request) const = 0;

				/** Display of the content of the admin element.
					@param stream Stream to write on.
					@param variables Execution variables
					@param request Current request
				*/
				virtual void display(std::ostream& stream, interfaces::VariablesMap& variables, const server::FunctionRequest<AdminRequest>* request=NULL) const = 0;

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
					, const server::FunctionRequest<admin::AdminRequest>* request
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
					, const server::FunctionRequest<admin::AdminRequest>* request
				) const;



				/** Gets the opening position of the node in the tree view.
					@return bool true = the page is visble, all the superior nodes are open, false = the page must not be visible, and will be hidden if no one another page of the same level must be visible.
					@author Hugues Romain
					@date 2008					
				*/
				virtual bool isPageVisibleInTree(const AdminInterfaceElement& currentPage) const;
			//@}
		};
	}
}

#endif

