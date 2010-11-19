
//////////////////////////////////////////////////////////////////////////
/// NamedPlaceUpdateAction class header.
///	@file NamedPlaceUpdateAction.hpp
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

#ifndef SYNTHESE_NamedPlaceUpdateAction_H__
#define SYNTHESE_NamedPlaceUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace geography
	{
		class City;
	}

	namespace pt
	{
		class StopArea;
	}

	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : Named place properties update.
		/// @ingroup m35Actions refActions
		/// @author Hugues Romain
		/// @date 2010
		/// @since 3.1.16
		///
		/// Parameters :
		///	<ul>
		///		<li>actionParamid : id of the place to update</li>
		///		<li>actionParamna : name</li>
		///		<li>actionParamsn : name for display on 13 characters wide screens</li>
		///		<li>actionParamln : name for display on 26 characters wide screens</li>
		///		<li>actionParamtn : timetable name</li>
		///		<li>actionParamci : city id</li>
		///		<li>actionParamco : code of the place in the source database (if imported)</li>
		///		<li>actionParamma : wether the stop is selected as default in the city when no stop is specified</li>
		///	</ul>
		class StopAreaNameUpdateAction:
			public util::FactorableTemplate<server::Action, StopAreaNameUpdateAction>
		{
		public:
			static const std::string PARAMETER_PLACE_ID;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_TIMETABLE_NAME;
			static const std::string PARAMETER_SHORT_NAME;
			static const std::string PARAMETER_LONG_NAME;
			static const std::string PARAMETER_CITY_ID;
			static const std::string PARAMETER_CODE;
			static const std::string PARAMETER_IS_MAIN;

		private:
			boost::shared_ptr<pt::StopArea> _place;
			std::string _name;
			std::string _shortName;
			std::string _longName;
			std::string _code;
			boost::shared_ptr<geography::City> _city;
			bool _isMain;
			std::string _timetableName;

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
			StopAreaNameUpdateAction():
			  _isMain(false) {}

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
				void setPlace(boost::shared_ptr<pt::StopArea> value) { _place = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_NamedPlaceUpdateAction_H__
