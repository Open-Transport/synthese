
//////////////////////////////////////////////////////////////////////////
/// WebPageUpdateAction class header.
///	@file WebPageUpdateAction.h
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

#ifndef SYNTHESE_WebPageUpdateAction_H__
#define SYNTHESE_WebPageUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace transportwebsite
	{
		class WebPage;

		//////////////////////////////////////////////////////////////////////////
		/// 56.15 Action : Web page properties update.
		/// @ingroup m56Actions refActions
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// Key : WebPageUpdateAction
		///
		/// Parameters :
		/// <ul>
		///		<li>wp : web page to update</li>
		///		<li>ui : value of parent page id</li>
		///		<li>te : value of template id</li>
		///		<li>sd : value of publication start date</li>
		///		<li>ed : value of publication end date</li>
		///		<li>mt : value of mime type</li>
		///		<li>du : value of do not use template</li>
		///		<li>fo : value of has forum</li>
		/// </ul>
		class WebPageUpdateAction:
			public util::FactorableTemplate<server::Action, WebPageUpdateAction>
		{
		public:
			static const std::string PARAMETER_WEB_PAGE_ID;
			static const std::string PARAMETER_UP_ID;
			static const std::string PARAMETER_TEMPLATE_ID;
			static const std::string PARAMETER_START_DATE;
			static const std::string PARAMETER_END_DATE;
			static const std::string PARAMETER_MIME_TYPE;
			static const std::string PARAMETER_DO_NOT_USE_TEMPLATE;
			static const std::string PARAMETER_HAS_FORUM;

		private:
			boost::shared_ptr<WebPage> _page;
			boost::shared_ptr<WebPage> _up;
			boost::shared_ptr<WebPage> _template;
			boost::posix_time::ptime _startDate;
			boost::posix_time::ptime _endDate;
			std::string _mimeType;
			bool _doNotUseTemplate;
			bool _hasForum;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// @return The generated parameters map
			server::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const server::ParametersMap& map);

		public:
			WebPageUpdateAction();

			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);
			


			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;



			void setWebPage(boost::shared_ptr<WebPage> value);
		};
	}
}

#endif // SYNTHESE_WebPageUpdateAction_H__
