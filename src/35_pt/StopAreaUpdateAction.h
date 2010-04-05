
//////////////////////////////////////////////////////////////////////////
/// StopAreaUpdateAction class header.
///	@file StopAreaUpdateAction.h
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

#ifndef SYNTHESE_StopAreaUpdateAction_H__
#define SYNTHESE_StopAreaUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include <boost/date_time/time_duration.hpp>

namespace synthese
{
	namespace pt
	{
		class PublicTransportStopZoneConnectionPlace;
	}

	namespace pt
	{
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
		///	</ul>
		class StopAreaUpdateAction:
			public util::FactorableTemplate<server::Action, StopAreaUpdateAction>
		{
		public:
			static const std::string PARAMETER_PLACE_ID;
			static const std::string PARAMETER_ALLOWED_CONNECTIONS;
			static const std::string PARAMETER_DEFAULT_TRANSFER_DURATION;

		private:
			boost::shared_ptr<pt::PublicTransportStopZoneConnectionPlace> _place;
			bool _allowedConnections;
			boost::posix_time::time_duration _defaultTransferDuration;

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
				void setPlace(boost::shared_ptr<pt::PublicTransportStopZoneConnectionPlace> value) { _place = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_StopAreaUpdateAction_H__
