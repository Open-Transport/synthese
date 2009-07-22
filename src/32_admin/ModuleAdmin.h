
/** ModuleAdmin class header.
	@file ModuleAdmin.h
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

#ifndef SYNTHESE_ModuleAdmin_H__
#define SYNTHESE_ModuleAdmin_H__

#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace server
	{
		class ModuleClass;
	}

	namespace admin
	{
		/** ModuleAdmin Class.
			@ingroup m14Admin refAdmin
			@author Hugues Romain
			@date 2008
		*/
		class ModuleAdmin:
			public admin::AdminInterfaceElementTemplate<ModuleAdmin>
		{
		public:
			static const std::string PARAMETER_MODULE;
		
		private:
			boost::shared_ptr<const server::ModuleClass>	_moduleClass;
			
		public:
			ModuleAdmin();
			
			//! @name Getters
			//@{
				boost::shared_ptr<const server::ModuleClass>	getModuleClass() const;
			//@}
			
			//! @name Setters
			//@{
				void setModuleClass(boost::shared_ptr<const server::ModuleClass> value);
			//@}
			
			/** Initialization of the parameters from a parameters map.
				@param map The parameters map to use for the initialization.
				@throw AdminParametersException if a parameter has incorrect value.
				@author Hugues Romain
				@date 2008
			*/
			void setFromParametersMap(
				const server::ParametersMap& map,
				bool objectWillBeCreatedLater
			);
			
			
			
			/** Parameters map generator, used when building an url to the admin page.
					@return server::ParametersMap The generated parameters map
					@author Hugues Romain
					@date 2007
			*/
			virtual server::ParametersMap getParametersMap() const;



			/** Display of the content of the admin element.
				@param stream Stream to write on.
				@author Hugues Romain
				@date 2008
			*/
			void display(std::ostream& stream, interfaces::VariablesMap& variables,
					const server::FunctionRequest<admin::AdminRequest>& request
			) const;
			
			/** Authorization control.
				@return bool True if the displayed page can be displayed
				@author Hugues Romain
				@date 2008
			*/
			bool isAuthorized(
				const server::FunctionRequest<admin::AdminRequest>& _request
			) const;
			
			/** Gets sub page of the designed parent page, which are from the current class.
				@param factoryKey Key of the parent class
				@return PageLinks A link to the page if the parent is Home
				@author Hugues Romain
				@date 2008
			*/
			virtual AdminInterfaceElement::PageLinks getSubPages(
				boost::shared_ptr<const AdminInterfaceElement> currentPage,
					const server::FunctionRequest<admin::AdminRequest>& _request
			) const;

			virtual std::string getTitle() const;

			/** Gets the opening position of the node in the tree view.
				@return A module page is always visible in the tree
				@author Hugues Romain
				@date 2008					
			*/
			virtual bool isPageVisibleInTree(
				const AdminInterfaceElement& currentPage,
				const server::FunctionRequest<admin::AdminRequest>& request
			) const;
		};
	}
}

#endif // SYNTHESE_ModuleAdmin_H__
