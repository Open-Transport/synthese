
//////////////////////////////////////////////////////////////////////////
/// WebPageContentUpdateAction class header.
///	@file WebPageContentUpdateAction.hpp
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

#ifndef SYNTHESE_WebPageContentUpdateAction_H__
#define SYNTHESE_WebPageContentUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace cms
	{
		class Webpage;

		//////////////////////////////////////////////////////////////////////////
		/// 36.16 Action : update of the content of a web page.
		/// @ingroup m56Actions refActions
		/// @author Hugues Romain
		//////////////////////////////////////////////////////////////////////////
		/// Key : WebPageContentUpdateAction
		///
		/// Parameters :
		/// <dl>
		///	<dt>actionParamwp</dt><dd>id of the page to update</dd>
		///	<dt>actionParamc1</dt><dd>new main content</dd>
		///	<dt>actionParamab</dt><dd>new abstract</dd>
		///	<dt>actionParamim</dt><dd>new image</dd>
		///	<dt>actionParamti</dt><dd>new title</dd>
		///	<dt>actionParamiw</dt><dd>the page output must ignore white chars</dd>
		///	</ul>
		class WebPageContentUpdateAction:
			public util::FactorableTemplate<server::Action, WebPageContentUpdateAction>
		{
		public:
			static const std::string PARAMETER_WEB_PAGE_ID;
			static const std::string PARAMETER_CONTENT1;
			static const std::string PARAMETER_ABSTRACT;
			static const std::string PARAMETER_IMAGE;
			static const std::string PARAMETER_TITLE;
			static const std::string PARAMETER_IGNORE_WHITE_CHARS;

		private:
			boost::shared_ptr<Webpage> _page;
			std::string _content1;
			std::string _abstract;
			std::string _image;
			std::string _title;
			bool _ignoreWhiteChars;

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
			WebPageContentUpdateAction();

			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);
			


			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;



			void setWebPage(boost::shared_ptr<Webpage> value);
		};
	}
}

#endif // SYNTHESE_WebPageContentUpdateAction_H__
