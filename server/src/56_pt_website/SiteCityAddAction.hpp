
//////////////////////////////////////////////////////////////////////////
/// SiteCityAddAction class header.
///	@file SiteCityAddAction.hpp
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

#ifndef SYNTHESE_SiteCityAddAction_H__
#define SYNTHESE_SiteCityAddAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace geography
	{
		class City;
	}

	namespace pt_website
	{
		class PTServiceConfig;

		//////////////////////////////////////////////////////////////////////////
		/// 56.15 Action : SiteCityAddAction.
		/// @ingroup m56Actions refActions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.2.0
		//////////////////////////////////////////////////////////////////////////
		/// Key : SiteCityAddAction
		///
		/// Parameters :
		///	<ul>
		///		<li>actionParamid : id of the site to update</li>
		///		<li>actionParamcn : name of the city to add to the site</li>
		///	</ul>
		class SiteCityAddAction:
			public util::FactorableTemplate<server::Action, SiteCityAddAction>
		{
		public:
			static const std::string PARAMETER_SITE_ID;
			static const std::string PARAMETER_CITY_NAME;

		private:
			boost::shared_ptr<PTServiceConfig> _config;
			geography::City* _city;

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
			SiteCityAddAction(): _city(NULL) {}

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
				void setConfig(boost::shared_ptr<PTServiceConfig> value) { _config = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_SiteCityAddAction_H__
