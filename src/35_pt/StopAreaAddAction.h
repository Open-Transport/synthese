
//////////////////////////////////////////////////////////////////////////
/// StopAreaAddAction class header.
///	@file StopAreaAddAction.h
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

#ifndef SYNTHESE_StopAreaAddAction_H__
#define SYNTHESE_StopAreaAddAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "ImportableTemplate.hpp"

namespace synthese
{
	namespace geography
	{
		class City;
	}

	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : Stop area creation.
		/// @ingroup m35Actions refActions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.18
		class StopAreaAddAction:
			public util::FactorableTemplate<server::Action, StopAreaAddAction>
		{
		public:
			static const std::string PARAMETER_CITY_ID;
			static const std::string PARAMETER_CITY_NAME;
			static const std::string PARAMETER_CREATE_CITY_IF_NECESSARY;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_DATA_SOURCE_LINKS;

		private:
			boost::shared_ptr<geography::City> _city;
			std::string _cityName;
			bool _createCityIfNecessary;
			std::string _name;
			impex::Importable::DataSourceLinks _dataSourceLinks;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Stop_area_creation
			//////////////////////////////////////////////////////////////////////////
			/// @return The generated parameters map
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Stop_area_creation
			//////////////////////////////////////////////////////////////////////////
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			StopAreaAddAction():
				_createCityIfNecessary(false)
			{}


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
				void setCity(boost::shared_ptr<geography::City> value){ _city = value; }
				void setCreateCityIfNecessary(bool value) { _createCityIfNecessary = value; }
				void setCityName(const std::string& value) { _cityName = value; }
				void setName(const std::string& value) { _name = value; }
				void setDataSourceLinks(const impex::Importable::DataSourceLinks value){ _dataSourceLinks = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_StopAreaAddAction_H__
