
//////////////////////////////////////////////////////////////////////////////////////////
/// WebPageMenuFunction class header.
///	@file WebPageMenuFunction.hpp
///	@author Hugues Romain
///	@date 2010
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
		class Website;

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
		///	</ul>
		class WebPageMenuFunction:
			public util::FactorableTemplate<server::Function,WebPageMenuFunction>
		{
		private:
			static const std::string DATA_RANK;
			static const std::string DATA_DEPTH;
			static const std::string DATA_IS_LAST_PAGE;
			static const std::string DATA_IS_THE_CURRENT_PAGE;
			static const std::string DATA_CURRENT_PAGE_IN_BRANCH;
			static const std::string ATTR_URL;
			static const std::string ATTR_HREF;
			static const std::string TAG_PAGE;

		public:
			static const std::string PARAMETER_ROOT_ID;
			static const std::string PARAMETER_MIN_DEPTH;
			static const std::string PARAMETER_MAX_DEPTH;
			static const std::string PARAMETER_MAX_NUMBER;
			static const std::string PARAMETER_BEGINNING;
			static const std::string PARAMETER_ENDING;
			static const std::string PARAMETER_BEGINNING_SELECTED;
			static const std::string PARAMETER_ENDING_SELECTED;
			static const std::string PARAMETER_BEGINNING_BEFORE_SUBMENU;
			static const std::string PARAMETER_ENDING_AFTER_SUBMENU;
			static const std::string PARAMETER_ITEM_PAGE_ID;
			static const std::string PARAMETER_OUTPUT_FORMAT;
			static const std::string VALUE_RSS;
			static const std::string PARAMETER_RAW_DATA;

		protected:
			struct MenuDefinition_ // Deprecated
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
				boost::shared_ptr<const Website> _rootSite;
				boost::optional<std::size_t> _maxNumber;
				std::size_t _minDepth;
				std::size_t _maxDepth;
				boost::shared_ptr<const Webpage> _itemPage;
				std::map<std::size_t, MenuDefinition_> _menuDefinition; // Deprecated
				std::string _outputFormat;
				bool _rawData;
			//@}


			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2010
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2010
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);


			/** Recursive menu generator.
				@param current_page Page déterminant la racine du menu (non affichée : produit la liste d'éléments inférieurs)
				@param depth Profondeur en cours de tracé
				@param page_in_branch Page is in current submenu : return the current page code
				@param pm parameters map to populate in raw mode
				@return page_in_branch Page is in submenus from the current page
			*/
			bool _getMenuContentRecursive(
				std::ostream& stream,
				const server::Request& request /*= NULL*/,
				const Webpage* root,
				std::size_t depth,
				const Webpage* currentPage,
				std::size_t rank,
				bool isLastPage,
				util::ParametersMap& pm
			) const;

		public:
			WebPageMenuFunction();



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2010
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;



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
