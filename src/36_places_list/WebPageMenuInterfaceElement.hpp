
/** WebPageMenuInterfaceElement class header.
	@file WebPageMenuInterfaceElement.hpp
	@author Hugues
	@date 2010

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

#ifndef SYNTHESE_WebPageMenuInterfaceElement_H__
#define SYNTHESE_WebPageMenuInterfaceElement_H__

#include "LibraryInterfaceElement.h"
#include "FactorableTemplate.h"
#include "ValueElementList.h"
#include "Registry.h"

#include <boost/optional.hpp>

namespace synthese
{
	namespace transportwebsite
	{
		class WebPage;

		/** WebPageMenuInterfaceElement Library Interface Element Class.
			@author Hugues
			@date 2010
			@ingroup m56Library refLibrary

			@code menu @endcode

			Parameters :
				- 0 : ID of the root of the menu : can be the ID of a TransportWebSite object or of a WebPage object.
				- 1 :

			Object : 

		*/
		class WebPageMenuInterfaceElement
			: public util::FactorableTemplate<interfaces::LibraryInterfaceElement, WebPageMenuInterfaceElement>
		{
		private:
			struct MenuDefinition_
			{
				std::string beginning;
				std::string ending;
				std::string beginningSelected;
				std::string endingSelected;
				std::string endingAfterSubmenu;
			};

			// List of parameters to store
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _rootVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _minDepthVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _maxDepthVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _levelToMaskVIE;
			boost::shared_ptr<interfaces::LibraryInterfaceElement> _levelToDisplayVIE;

			interfaces::ValueElementList _htmlVEL;

			/** Recursive menu generator.
				@param current_page Page déterminant la racine du menu (non affichée : produit la liste d'éléments inférieurs)
				@param depth Profondeur en cours de tracé
				@param page_in_branch Page is in current submenu : return the current page code
				@return page_in_branch Page is in submenus from the current page
			*/	
			bool _getMenuContentRecursive(
				std::ostream& stream,
				const server::Request* request /*= NULL*/,
				util::RegistryKeyType rootId,
				std::size_t depth,
				boost::shared_ptr<const WebPage> currentPage,
				const std::vector<MenuDefinition_>& menuDefinition,
				std::size_t	_minDepth,
				std::size_t	_maxDepth,
				boost::optional<std::size_t> _levelToMask,
				std::size_t	_levelToDisplay
			) const;

		public:
			/** Controls and store the internals parameters.
				@param vel Parameters list to read
			*/
			void storeParameters(interfaces::ValueElementList& vel);
			
			/* Displays the object.
				@param stream Stream to write on
				@param parameters Runtime parameters used in the internal parameters reading
				@param variables Runtime variables
				@param object Object to read at the display
				@param request Source request
			*/
			std::string display(
				std::ostream& stream
				, const interfaces::ParametersVector& parameters
				, interfaces::VariablesMap& variables
				, const void* object = NULL
				, const server::Request* request = NULL
			) const;
				
			~WebPageMenuInterfaceElement();
		};
	}
}

#endif // SYNTHESE_WebPageMenuInterfaceElement_H__
