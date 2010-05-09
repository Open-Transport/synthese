
//////////////////////////////////////////////////////////////////////////
/// PhysicalStopAddAction class header.
///	@file PhysicalStopAddAction.hpp
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

#ifndef SYNTHESE_PhysicalStopAddAction_H__
#define SYNTHESE_PhysicalStopAddAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

namespace synthese
{
	namespace pt
	{
		class PublicTransportStopZoneConnectionPlace;

		//////////////////////////////////////////////////////////////////////////
		/// 35.15 Action : Physical stop creation.
		/// @ingroup m35Actions refActions
		///	@author Hugues Romain
		///	@date 2010
		/// @since 3.1.17
		//////////////////////////////////////////////////////////////////////////
		/// Key : PhysicalStopAddAction
		///
		/// Parameters :
		///	<ul>
		///		<li>actionParampl : place</li>
		///		<li>actionParamna (optional) : name</li>
		///		<li>actionParamoc (optional) : operator code</li>
		///		<li>actionParamx (optional) : x</li>
		///		<li>actionParamy (optional) : y</li>
		///	</ul>
		class PhysicalStopAddAction:
			public util::FactorableTemplate<server::Action, PhysicalStopAddAction>
		{
		public:
			static const std::string PARAMETER_X;
			static const std::string PARAMETER_Y;
			static const std::string PARAMETER_OPERATOR_CODE;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_PLACE_ID;

		private:
			boost::shared_ptr<const PublicTransportStopZoneConnectionPlace> _place;
			double _x;
			double _y;
			std::string _operatorCode;
			std::string _name;

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
			PhysicalStopAddAction(): _x(0), _y(0) {}

			//////////////////////////////////////////////////////////////////////////
			/// The action execution code.
			/// @param request the request which has launched the action
			void run(server::Request& request);



			//////////////////////////////////////////////////////////////////////////
			/// Tests if the action can be launched in the current session.
			/// @param session the current session
			/// @return true if the action can be launched in the current session
			virtual bool isAuthorized(const server::Session* session) const;

			void setPlace(boost::shared_ptr<const PublicTransportStopZoneConnectionPlace> value){ _place = value; }
		};
	}
}

#endif // SYNTHESE_PhysicalStopOperatorCodeAddAction_H__
