
//////////////////////////////////////////////////////////////////////////
/// WebPageMoveAction class header.
///	@file WebPageMoveAction.hpp
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

#ifndef SYNTHESE_WebPageMoveAction_H__
#define SYNTHESE_WebPageMoveAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace cms
	{
		class Webpage;

		//////////////////////////////////////////////////////////////////////////
		/// 36.15 Action : WebPageMoveAction.
		/// @ingroup m56Actions refActions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.18
		//////////////////////////////////////////////////////////////////////////
		/// Key : WebPageMoveAction
		///
		/// Parameters :
		///	<ul>
		///		<li>actionParamid : id of the page to move</li>
		///		<li>actionParamdi : direction : 1=up, 0=down</li>
		///	</ul>
		class WebPageMoveAction:
			public util::FactorableTemplate<server::Action, WebPageMoveAction>
		{
		public:
			static const std::string PARAMETER_PAGE_ID;
			static const std::string PARAMETER_DIRECTION;

		private:
			boost::shared_ptr<Webpage> _page;
			boost::shared_ptr<Webpage> _switchedPage;
			bool _up;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @return The generated parameters map
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			WebPageMoveAction();

			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);



			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;



			//! @name Setters
			//@{
				void setPage(boost::shared_ptr<Webpage> value) { _page = value; }
				void setUp(bool value){ _up = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_WebPageMoveAction_H__
