
//////////////////////////////////////////////////////////////////////////
/// StopAreaUpdateAction class header.
///	@file StopAreaUpdateAction.h
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

#ifndef SYNTHESE_StopAreaUpdateAction_H__
#define SYNTHESE_StopAreaUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "BaseImportableUpdateAction.hpp"

#include <boost/date_time/time_duration.hpp>

namespace geos
{
	namespace geom
	{
		class Point;
	}
}

namespace synthese
{
	namespace geography
	{
		class City;
	}

	class CoordinatesSystem;

	namespace pt
	{
		class StopArea;

		//////////////////////////////////////////////////////////////////////////
		/// Updates stop area transfer attributes.
		/// @ingroup m35Actions refActions
		/// @author Hugues Romain
		/// @date 2010
		/// @since 3.1.16
		///
		/// Note :The name attributes can be updated by NamedPlaceUpdateAction.
		///
		/// Parameters :
		///	<ul>
		///		<li>actionParamid (compulsory) : the id of the stop area to update</li>
		///		<li>actionParamac (compulsory) : sets if connection is allowed in the stop area</li>
		///		<li>actionParamdt (compulsory) : sets the default transfer duration</li>
		///		<li>actionParamna : name</li>
		///		<li>actionParamsn : name for display on 13 characters wide screens</li>
		///		<li>actionParamln : name for display on 26 characters wide screens</li>
		///		<li>actionParamtn : timetable name</li>
		///		<li>actionParamci : city id</li>
		///		<li>actionParamma : wether the stop is selected as default in the city when no stop is specified</li>
		///	</ul>
		class StopAreaUpdateAction:
			public util::FactorableTemplate<server::Action, StopAreaUpdateAction>,
			public impex::BaseImportableUpdateAction
		{
		public:
			static const std::string PARAMETER_PLACE_ID;
			static const std::string PARAMETER_ALLOWED_CONNECTIONS;
			static const std::string PARAMETER_DEFAULT_TRANSFER_DURATION;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_TIMETABLE_NAME;
			static const std::string PARAMETER_SHORT_NAME;
			static const std::string PARAMETER_LONG_NAME;
			static const std::string PARAMETER_CITY_ID;
			static const std::string PARAMETER_IS_MAIN;
			static const std::string PARAMETER_X;
			static const std::string PARAMETER_Y;
			static const std::string PARAMETER_SRID;

		private:
			boost::shared_ptr<pt::StopArea> _place;
			boost::optional<bool> _allowedConnections;
			boost::optional<boost::posix_time::time_duration> _defaultTransferDuration;
			boost::optional<std::string> _name;
			boost::optional<std::string> _shortName;
			boost::optional<std::string> _longName;
			boost::shared_ptr<geography::City> _city;
			boost::optional<bool> _isMain;
			boost::optional<std::string> _timetableName;
			boost::shared_ptr<geos::geom::Point> _point;
			const CoordinatesSystem* _coordinatesSystem;


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
				void setPlace(boost::shared_ptr<pt::StopArea> value) { _place = value; }
			//@}
		};
}	}

#endif // SYNTHESE_StopAreaUpdateAction_H__
