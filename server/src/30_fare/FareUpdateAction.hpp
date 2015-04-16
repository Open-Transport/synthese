
//////////////////////////////////////////////////////////////////////////
/// FareUpdateAction class header.
///	@file FareUpdateAction.hpp
///	@author Gaël Sauvanet
///	@date 2011
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

#ifndef SYNTHESE_FareUpdateAction_H__
#define SYNTHESE_FareUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "Fare.hpp"
#include "FareType.hpp"

namespace synthese
{
	namespace fare
	{
		//////////////////////////////////////////////////////////////////////////
		/// 30.15 Action : FareUpdateAction.
		/// @ingroup m30Actions refActions
		///	@author Gaël Sauvanet
		///	@date 2011
		/// @since 3.2.1
		//////////////////////////////////////////////////////////////////////////
		/// Key : FareUpdateAction
		///
		/// Parameters :
		///	<dl>
		///	<dt>actionParamid</dt><dd>id of the object to update</dd>
		//	</dl>
		class FareUpdateAction:
			public util::FactorableTemplate<server::Action, FareUpdateAction>
		{
		public:
			static const std::string PARAMETER_FARE_ID;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_TYPE;
			static const std::string PARAMETER_CURRENCY;
			static const std::string PARAMETER_PERMITTED_CONNECTIONS_NUMBER;
			static const std::string PARAMETER_REQUIRED_CONTINUITY;
			static const std::string PARAMETER_VALIDITY_PERIOD;

			static const std::string PARAMETER_ACCESS_PRICE;
			static const std::string PARAMETER_SLICES;
			static const std::string PARAMETER_UNIT_PRICE;
			static const std::string PARAMETER_MATRIX;
			static const std::string PARAMETER_SUB_FARES;

		private:
			boost::shared_ptr<Fare> _fare;
			boost::optional<std::string> _name;
			boost::optional<FareType::FareTypeNumber> _type;
			boost::optional<std::string> _currency;
			boost::optional<int> _permittedConnectionsNumber;
			boost::optional<bool> _requiredContinuity;
			boost::optional<int> _validityPeriod;

			boost::optional<double> _accessPrice;
			boost::optional<FareType::Slices> _slices;
			boost::optional<bool> _isUnitPrice;
			boost::optional<std::string> _matrix;
			boost::optional<std::string> _subFares;

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
				void setFare(boost::shared_ptr<Fare> value) { _fare = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_FareUpdateAction_H__
