

//////////////////////////////////////////////////////////////////////////
/// LoginAdmin class header.
///	@file LoginAdmin.hpp
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

#ifndef SYNTHESE_LoginAdmin_H__
#define SYNTHESE_LoginAdmin_H__

#include "ResultHTMLTable.h"
#include "AdminInterfaceElementTemplate.h"

namespace synthese
{
	namespace admin
	{
		//////////////////////////////////////////////////////////////////////////
		/// LoginAdmin Admin compound class.
		/// Key : login
		/// Parameters :
		///	<dl>
		///	<dt>ul</dt><dd>URL to launch if the login is successful (optional : default value is display of HomeAdmin)
		/// </dl>
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m14Admin refAdmin
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.2.0
		class LoginAdmin:
			public admin::AdminInterfaceElementTemplate<LoginAdmin>
		{
		public:
			/// @name Parameter identifiers
			//@{
				static const std::string PARAM_URL_TO_LAUNCH;
			//@}

		private:
			/// @name parameters
			//@{
				std::string _urlToLaunch;
			//@}


		public:
			/// @name Setters
			//@{
				void setURLToLaunch(const std::string& value){ _urlToLaunch = value; }
			//@}

			//////////////////////////////////////////////////////////////////////////
			/// Initialization of the parameters from a parameters map.
			///	@param map The parameters map to use for the initialization.
			///	@throw AdminParametersException if a parameter has incorrect value.
			///	@author Hugues Romain
			///	@date 2010
			void setFromParametersMap(
				const util::ParametersMap& map
			);



			//////////////////////////////////////////////////////////////////////////
			/// Creation of the parameters map from the object attributes.
			///	@author Hugues Romain
			///	@date 2010
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Display of the content of the admin element.
			///	@param stream Stream to write the page content on.
			///	@param request The current request
			///	@author Hugues Romain
			///	@date 2010
			void display(
				std::ostream& stream,
				const admin::AdminRequest& _request
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Authorization control.
			/// Returns if the page can be displayed. In most cases, the needed right
			/// level is READ.
			///	@param request The current request
			///	@return bool True if the displayed page can be displayed
			///	@author Hugues Romain
			///	@date 2010
			bool isAuthorized(
				const security::User& user
			) const;
		};
	}
}

#endif // SYNTHESE_LoginAdmin_H__
