
//////////////////////////////////////////////////////////////////////////////////////////
/// WebPageDisplayFunction class header.
///	@file WebPageDisplayFunction.h
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

#ifndef SYNTHESE_WebPageDisplayFunction_H__
#define SYNTHESE_WebPageDisplayFunction_H__

#include "FactorableTemplate.h"
#include "FunctionWithSite.h"

namespace synthese
{
	namespace cms
	{
		class Webpage;

		//////////////////////////////////////////////////////////////////////////
		/// 36.15 Function : display of a web page.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Page_display
		//////////////////////////////////////////////////////////////////////////
		///	@author Hugues Romain
		///	@date 2010
		///	@ingroup m56Functions refFunctions
		class WebPageDisplayFunction:
			public util::FactorableTemplate<FunctionWithSite<false>, WebPageDisplayFunction>
		{
		private:
			static const std::string DATA_CONTENT;

		public:
			static const std::string PARAMETER_PAGE_ID;
			static const std::string PARAMETER_USE_TEMPLATE;
			static const std::string PARAMETER_HOST_NAME;
			static const std::string PARAMETER_SMART_URL;
			static const std::string PARAMETER_DONT_REDIRECT_IF_SMART_URL;

		protected:
			//! \name Page parameters
			//@{
				const Webpage*	_page;
				bool			_useTemplate;
				std::string _smartURL;
				bool _dontRedirectIfSmartURL;
			//@}


		public:
			//////////////////////////////////////////////////////////////////////////
			/// Conversion from attributes to generic parameter maps.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Page_display#Request
			///	@return Generated parameters map
			/// @author Hugues
			/// @date 2010
			util::ParametersMap _getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Conversion from generic parameters map to attributes.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Page_display#Request
			///	@param map Parameters map to interpret
			/// @author Hugues
			/// @date 2010
			virtual void _setFromParametersMap(
				const util::ParametersMap& map
			);


			WebPageDisplayFunction();

			//! @name Setters
			//@{
				void setPage(const Webpage* value) { _page = value; }
				void setUseTemplate(bool value){ _useTemplate = value; }
				void setDontRedirectIfSmartURL(bool value){ _dontRedirectIfSmartURL = value; }
			//@}

			//! @name Getters
			//@{
				const Webpage* getPage() const { return _page; }
				bool getUseTemplate() const { return _useTemplate; }
				const std::string& getSmartURL() const { return _smartURL; }
			//@}



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content generated by the function.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Page_display#Response
			/// @param stream Stream to display the content on.
			/// @param request the current request
			/// @author Hugues
			/// @date 2010
			virtual util::ParametersMap run(std::ostream& stream, const server::Request& request) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets if the function can be run according to the user of the session.
			/// @param session the current session
			/// @return true if the function can be run
			/// @author Hugues
			/// @date 2010
			virtual bool isAuthorized(const server::Session* session) const;



			//////////////////////////////////////////////////////////////////////////
			/// Gets the Mime type of the content generated by the function.
			/// @return the Mime type of the content generated by the function
			/// @author Hugues
			/// @date 2010
			virtual std::string getOutputMimeType() const;
		};
	}
}

#endif // SYNTHESE_WebPageDisplayFunction_H__
