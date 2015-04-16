
//////////////////////////////////////////////////////////////////////////
/// WebPageAddAction class header.
///	@file WebPageAddAction.h
///	@author Hugues
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

#ifndef SYNTHESE_WebPageAddAction_H__
#define SYNTHESE_WebPageAddAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "MimeType.hpp"

#include <boost/optional.hpp>
#include <string>

namespace synthese
{
	namespace cms
	{
		class Website;
		class Webpage;

		//////////////////////////////////////////////////////////////////////////
		/// Web page creation action.
		/// @ingroup m56Actions refActions
		///
		/// There is 3 ways to create a web page sorted by decreasing priority :
		///		- by specifying TEMPLATE_ID : insertion of a page after an existing page with the same parent
		///		- by specifying PARENT_ID : creation of a child of an other page after the last sibling if a child page already exists
		///		- by specifying SITE_ID : creation of a page at the last position of a web site
		class WebPageAddAction:
			public util::FactorableTemplate<server::Action, WebPageAddAction>
		{
		public:
			static const std::string PARAMETER_TITLE;
			static const std::string PARAMETER_SITE_ID;
			static const std::string PARAMETER_TEMPLATE_ID;
			static const std::string PARAMETER_PARENT_ID;
			static const std::string PARAMETER_MIME_TYPE;

		private:
			std::string _title;
			std::size_t _rank;
			boost::shared_ptr<Website> _site;
			boost::shared_ptr<const Webpage> _template;
			boost::shared_ptr<Webpage> _parent;
			boost::optional<util::MimeType> _mimeType;

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
				void setSite(boost::shared_ptr<Website> value) { _site = value; }
				void setTitle(const std::string& value) { _title = value; }
				void setParent(boost::shared_ptr<Webpage> value) { _parent = value; }
			//@}

		};
	}
}

#endif // SYNTHESE_WebPageAddAction_H__
