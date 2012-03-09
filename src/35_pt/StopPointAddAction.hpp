
//////////////////////////////////////////////////////////////////////////
/// StopPointAddAction class header.
///	@file StopPointAddAction.hpp
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

#ifndef SYNTHESE_PhysicalStopAddAction_H__
#define SYNTHESE_PhysicalStopAddAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "ImportableTemplate.hpp"

namespace geos
{
	namespace geom
	{
		class Point;
	}
}

namespace synthese
{
	class CoordinatesSystem;

	namespace geography
	{
		class City;
	}

	namespace pt
	{
		class StopArea;

		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : Physical stop creation.
		/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Stop_creation
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m35Actions refActions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.17
		//////////////////////////////////////////////////////////////////////////
		class StopPointAddAction:
			public util::FactorableTemplate<server::Action, StopPointAddAction>
		{
		public:
			static const std::string PARAMETER_X;
			static const std::string PARAMETER_Y;
			static const std::string PARAMETER_OPERATOR_CODE;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_PLACE_ID;
			static const std::string PARAMETER_LONGITUDE;
			static const std::string PARAMETER_LATITUDE;
			static const std::string PARAMETER_CITY_ID;
			static const std::string PARAMETER_CITY_NAME;
			static const std::string PARAMETER_CREATE_CITY_IF_NECESSARY;
			static const std::string PARAMETER_SRID;

			static const std::string SESSION_VARIABLE_STOPPOINT_CREATION_SRID;

		private:
			boost::shared_ptr<StopArea> _place;
			impex::Importable::DataSourceLinks _operatorCode;
			std::string _name;
			boost::shared_ptr<geos::geom::Point> _point;
			boost::shared_ptr<geography::City> _city;
			boost::optional<std::string> _cityName;
			bool _createCityIfNecessary;
			const CoordinatesSystem* _coordinatesSystem;

		protected:
			//////////////////////////////////////////////////////////////////////////
			/// Generates a generic parameters map from the action parameters.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Stop_creation
			//////////////////////////////////////////////////////////////////////////
			/// @return The generated parameters map
			util::ParametersMap getParametersMap() const;



			//////////////////////////////////////////////////////////////////////////
			/// Reads the parameters of the action on a generic parameters map.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Stop_creation
			//////////////////////////////////////////////////////////////////////////
			/// @param map Parameters map to interpret
			/// @exception ActionException Occurs when some parameters are missing or incorrect.
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			StopPointAddAction();

			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// See https://extranet.rcsmobility.com/projects/synthese/wiki/Stop_creation
			//////////////////////////////////////////////////////////////////////////
			/// @param request the request which has launched the action
			void run(server::Request& request);



			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;

			//! @name Setters
			//@{
				void setPlace(boost::shared_ptr<StopArea> value){ _place = value; }
				void setPoint(boost::shared_ptr<geos::geom::Point> value){ _point = value; }
				void setDataSourceLinks(const impex::Importable::DataSourceLinks& value){ _operatorCode = value; }
				void setName(const std::string& value){ _name = value; }
				void setCityName(const std::string& value){ _cityName = value; }
				void setCreateCityIfNecessary(bool value){ _createCityIfNecessary = value; }
			//@}
		};
}	}

#endif // SYNTHESE_PhysicalStopOperatorCodeAddAction_H__
