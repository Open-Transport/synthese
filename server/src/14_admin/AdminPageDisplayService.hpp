
//////////////////////////////////////////////////////////////////////////////////////////
///	AdminPageDisplayService class header.
///	@file AdminPageDisplayService.hpp
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

#ifndef SYNTHESE_AdminPageDisplayService_H__
#define SYNTHESE_AdminPageDisplayService_H__

#include "FactorableTemplate.h"
#include "Function.h"

#include "AdminInterfaceElement.h"

namespace synthese
{
	namespace admin
	{
		//////////////////////////////////////////////////////////////////////////
		///	14.15 Function : AdminPageDisplayService.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m14Functions refFunctions
		///	@author Hugues Romain
		///	@date 2012
		/// @since 3.5.0
		class AdminPageDisplayService:
			public util::FactorableTemplate<server::Function,AdminPageDisplayService>
		{
		public:
			static const std::string PARAMETER_NO_CMS;
			static const std::string PARAMETER_PAGE;
			static const std::string PARAMETER_TAB;
			static const std::string PARAMETER_ACTION_FAILED_PAGE;
			static const std::string PARAMETER_ACTION_FAILED_TAB;

			static const std::string TAG_TREE_NODE;
			static const std::string TAG_POSITION_NODE;

			static const std::string DATA_TABS;
			static const std::string DATA_CONTENT;

		protected:
			//! \name Page parameters
			//@{
				boost::shared_ptr<AdminInterfaceElement>	_page;
				bool _noCMS;
			//@}
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/#Request
			//////////////////////////////////////////////////////////////////////////
			///	@return Generated parameters map
			/// @author Hugues Romain
			/// @date 2012
			/// @since 3.5.0
			util::ParametersMap _getParametersMap() const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/#Request
			//////////////////////////////////////////////////////////////////////////
			///	@param map Parameters map to interpret
			/// @author Hugues Romain
			/// @date 2012
			/// @since 3.5.0
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);

		private:
			static const std::string DATA_DEPTH;
			static const std::string DATA_IS_LAST;

			//////////////////////////////////////////////////////////////////////////
			/// Displays an element of the position string through a CMS template.
			/// Values sent to the template :
			/// <dl>
			/// <dt>depth</dt><dd>Depth of the element in the tree</dd>
			/// <dt>is_last</dt><dd>The element is the current compound (last of the serie)</dd>
			/// <dt>icon</dt><dd>icon of the compound</dd>
			/// <dt>title</dt><dd>title of the compound</dd>
			/// </dl>
			boost::shared_ptr<util::ParametersMap> _getPositionNode(
				boost::shared_ptr<const AdminInterfaceElement> link,
				std::size_t depth,
				bool isLast
			) const;


			static const std::string DATA_SUBTREE;
			static const std::string DATA_IS_OPENED;
			static const std::string DATA_IS_CURRENT;

			//////////////////////////////////////////////////////////////////////////
			/// Displays an element of the position string through a CMS template.
			/// Values sent to the template :
			/// <dl>
			/// <dt>icon</dt><dd>icon of the compound</dd>
			/// <dt>title</dt><dd>title of the compound</dd>
			/// <dt>subtree</dt><dd>subtree content</dd>
			/// <dt>is_opened</dt><dd>1|0 the node is opened</dd>
			/// <dt>is_last</dt><dd>1|0 the node is the last one of its serie</dd>
			/// <dt>is_current</dt><dd>1|0 the node corresponds to the currently displayed compound</dd>
			/// </dl>
			void _getTreeNode(
				util::ParametersMap& pm,
				const AdminInterfaceElement::PageLinksTree& tree,
				boost::shared_ptr<const AdminInterfaceElement> currentCompound,
				std::size_t depth,
				bool isLast
			) const;

			
			
		public:
			AdminPageDisplayService();



			//! @name Setters
			//@{
				void setPage(boost::shared_ptr<AdminInterfaceElement> value){ _page = value; }
			//@}





			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues Romain
			/// @date 2012
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;
			
			
			
			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author Hugues Romain
			/// @date 2012
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues Romain
			/// @date 2012
			virtual std::string getOutputMimeType() const;
		};
}	}

#endif // SYNTHESE_AdminPageDisplayService_H__

