
//////////////////////////////////////////////////////////////////////////////////////////
/// WebPageMenuFunction class header.
///	@file WebPageMenuFunction.hpp
///	@author Hugues Romain
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

#ifndef SYNTHESE_WebPageMenuFunction_H__
#define SYNTHESE_WebPageMenuFunction_H__

#include "FactorableTemplate.h"
#include "Function.h"

#include <map>

namespace synthese
{
	namespace cms
	{
		class Webpage;

		//////////////////////////////////////////////////////////////////////////
		///	36.15 Function : WebPageMenuFunction.
		///	@ingroup m56Functions refFunctions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.16
		//////////////////////////////////////////////////////////////////////////
		/// Key : menu
		///
		/// Parameters :
		///	<ul>
		///		<li>root : id of the parent page of each item of first range (default is the current displayed page, 0 is the root of the site)</li>
		///		<li>min_depth : minimal depth of items to display (depth is relative to the root, default=1)</li>
		///		<li>max_depth : maximal depth of items to display (depth is relative to the root, default=1)</li>
		///		<li>The code to generate is a combination af 5 parameters by depth level : </li>
		///		<ul>
		///			<li>beginning_x (x = depth) : code to output before the item if the item does not correspond to the current displayed page</li>
		///			<li>ending_x (x = depth) : code to output after the item if the item does not correspond to the current displayed page</li>
		///			<li>beginning_selected_x (x = depth) : code to output before the item if the item corresponds to the current displayed page (default is like beginning_x)</li>
		///			<li>ending_selected_x (x = depth) : code to output after the item if the item corresponds to the current displayed page (default is like ending_x)</li>
		///			<li>beginning_before_submenu_x (x = depth) : code to output before a sub menu</li>
		///			<li>ending_after_submenu_x (x = depth) : code to output after a sub menu</li>
		///		</ul>
		///	</ul>
		class WebPageMenuFunction:
			public util::FactorableTemplate<server::Function,WebPageMenuFunction>
		{
		public:
			static const std::string PARAMETER_ROOT_ID;
			static const std::string PARAMETER_MIN_DEPTH;
			static const std::string PARAMETER_MAX_DEPTH;
			static const std::string PARAMETER_BEGINNING;
			static const std::string PARAMETER_ENDING;
			static const std::string PARAMETER_BEGINNING_SELECTED;
			static const std::string PARAMETER_ENDING_SELECTED;
			static const std::string PARAMETER_BEGINNING_BEFORE_SUBMENU;
			static const std::string PARAMETER_ENDING_AFTER_SUBMENU;

		protected:
			struct MenuDefinition_
			{
				std::string beginning;
				std::string ending;
				std::string beginningSelected;
				std::string endingSelected;
				std::string beginningBeforeSubmenu;
				std::string endingAfterSubmenu;
			};

			//! \name Page parameters
			//@{
				boost::optional<util::RegistryKeyType> _rootId;
				boost::shared_ptr<const Webpage> _root;
				std::size_t _minDepth;
				std::size_t _maxDepth;
				std::map<std::size_t, MenuDefinition_> _menuDefinition;
			//@}
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2010
			server::ParametersMap _getParametersMap() const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2010
			virtual void _setFromParametersMap(
				const server::ParametersMap& map
			);
			

			/** Recursive menu generator.
				@param current_page Page déterminant la racine du menu (non affichée : produit la liste d'éléments inférieurs)
				@param depth Profondeur en cours de tracé
				@param page_in_branch Page is in current submenu : return the current page code
				@return page_in_branch Page is in submenus from the current page
			*/
			bool _getMenuContentRecursive(
				std::ostream& stream,
				const server::Request& request /*= NULL*/,
				boost::shared_ptr<const Webpage> root,
				std::size_t depth,
				boost::shared_ptr<const Webpage> currentPage
			) const;

		public:
			WebPageMenuFunction();
			//! @name Setters
			//@{
			//	void setObject(boost::shared_ptr<const Object> value) { _object = value; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2010
			virtual void run(std::ostream& stream, const server::Request& request) const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author Hugues Romain
			/// @date 2010
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues Romain
			/// @date 2010
			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_WebPageMenuFunction_H__
