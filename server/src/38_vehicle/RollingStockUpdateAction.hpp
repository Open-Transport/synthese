
//////////////////////////////////////////////////////////////////////////
/// RollingStockUpdateAction class header.
///	@file RollingStockUpdateAction.hpp
///	@author Hugues Romain
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

#ifndef SYNTHESE_RollingStockUpdateAction_H__
#define SYNTHESE_RollingStockUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"
#include "RollingStock.hpp"
#include "BaseImportableUpdateAction.hpp"

namespace synthese
{
	namespace vehicle
	{
		//////////////////////////////////////////////////////////////////////////
		/// 38.15 Action : RollingStockUpdateAction.
		/// @ingroup m38Actions refActions
		///	@author Hugues Romain
		///	@date 2011
		/// @since 3.2.1
		//////////////////////////////////////////////////////////////////////////
		/// Key : RollingStockUpdateAction
		///
		/// Parameters :
		///	<dl>
		///	<dt>actionParamid</dt><dd>id of the object to update</dd>
		//	</dl>
		class RollingStockUpdateAction:
			public util::FactorableTemplate<server::Action, RollingStockUpdateAction>,
			public impex::BaseImportableUpdateAction
		{
		public:
			static const std::string PARAMETER_ROLLING_STOCK_ID;
			static const std::string PARAMETER_NAME;
			static const std::string PARAMETER_ARTICLE;
			static const std::string PARAMETER_CO2_EMISSIONS;
			static const std::string PARAMETER_ENERGY_CONSUMPTION;

		private:
			boost::shared_ptr<RollingStock> _rollingStock;
			boost::optional<std::string> _name;
			boost::optional<std::string> _article;
			boost::optional<double> _CO2Emissions;
			boost::optional<double> _energyConsumption;


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
				void setRollingStock(boost::shared_ptr<RollingStock> value) { _rollingStock = value; }
			//@}
		};
}	}

#endif // SYNTHESE_RollingStockUpdateAction_H__
