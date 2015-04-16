
//////////////////////////////////////////////////////////////////////////
/// WebPageUpdateAction class header.
///	@file WebPageUpdateAction.h
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

#ifndef SYNTHESE_WebPageUpdateAction_H__
#define SYNTHESE_WebPageUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "Webpage.h"

namespace synthese
{
	namespace util
	{
		class MimeType;
	}

	namespace cms
	{
		//////////////////////////////////////////////////////////////////////////
		/// 36.15 Action : Web page properties update.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Item_update
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m56Actions refActions
		/// @author Hugues Romain
		class WebPageUpdateAction:
			public util::FactorableTemplate<server::Action, WebPageUpdateAction>
		{
		public:
			static const std::string PARAMETER_WEB_PAGE_ID;
			static const std::string PARAMETER_UP_ID;
			static const std::string PARAMETER_SITE_ID;
			static const std::string PARAMETER_TEMPLATE_ID;
			static const std::string PARAMETER_START_DATE;
			static const std::string PARAMETER_END_DATE;
			static const std::string PARAMETER_MIME_TYPE;
			static const std::string PARAMETER_DO_NOT_USE_TEMPLATE;
			static const std::string PARAMETER_HAS_FORUM;
			static const std::string PARAMETER_SMART_URL_PATH;
			static const std::string PARAMETER_SMART_URL_DEFAULT_PARAMETER_NAME;
			static const std::string PARAMETER_CONTENT1;
			static const std::string PARAMETER_ABSTRACT;
			static const std::string PARAMETER_IMAGE;
			static const std::string PARAMETER_TITLE;
			static const std::string PARAMETER_IGNORE_WHITE_CHARS;
			static const std::string PARAMETER_DECODE_XML_ENTITIES_IN_CONTENT;
			static const std::string PARAMETER_RAW_EDITOR;
			static const std::string PARAMETER_DO_NOT_EVALUATE;
			static const std::string PARAMETER_MAX_AGE;

		private:
			boost::shared_ptr<Webpage> _page;

			boost::optional<std::string> _content1;
			boost::optional<std::string> _abstract;
			boost::optional<std::string> _image;
			boost::optional<std::string> _title;
			boost::optional<bool> _ignoreWhiteChars;
			boost::optional<boost::shared_ptr<Webpage> > _up;
			boost::optional<SpecificTemplate::Type> _template;
			boost::optional<boost::posix_time::ptime> _startDate;
			boost::optional<boost::posix_time::ptime> _endDate;
			boost::optional<boost::posix_time::time_duration> _maxAge;
			boost::optional<util::MimeType> _mimeType;
			boost::optional<bool> _doNotUseTemplate;
			boost::optional<bool> _hasForum;
			boost::optional<std::string> _smartURLPath;
			boost::optional<bool> _rawEditor;
			boost::optional<std::string> _smartURLDefaultParameterName;
			boost::optional<bool> _doNotEvaluate;
			boost::optional<boost::shared_ptr<Website> > _site;

			bool _decodeXMLEntitiesInContent;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Item_update#Request
			//////////////////////////////////////////////////////////////////////////
			/// @return The generated parameters map
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Item_update#Request
			//////////////////////////////////////////////////////////////////////////
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);

			void _updateSite(
				Webpage& page,
				db::DBTransaction& transaction
			) const;

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



			void setWebPage(boost::shared_ptr<Webpage> value);
			void setdecodeXMLEntitiesInContent(bool value){ _decodeXMLEntitiesInContent = value; }
			void setRawEditor(bool value){ _rawEditor = value; }
			void setUp(boost::shared_ptr<Webpage> value){ _up = value; }
			void setSite(boost::shared_ptr<Website> value) { _site = value; }
		};
}	}

#endif // SYNTHESE_WebPageUpdateAction_H__
