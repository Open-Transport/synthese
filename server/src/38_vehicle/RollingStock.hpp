
/** RollingStock class header.
	@file RollingStock.hpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef SYNTHESE_ENV_ROLLING_STOCK_H
#define SYNTHESE_ENV_ROLLING_STOCK_H

#include "Object.hpp"
#include "DataSourceLinksField.hpp"
#include "Registrable.h"
#include "Registry.h"
#include "PathClass.h"
#include "ImportableTemplate.hpp"

#include <string>

namespace synthese
{
	FIELD_STRING(Article)
	FIELD_STRING(IndicatorLabel)
	FIELD_STRING(TridentKey)
	FIELD_BOOL(IsTridentReference)
	FIELD_DOUBLE(CO2Emissions)
	FIELD_DOUBLE(EnergyConsumption)
	
	typedef boost::fusion::map<
		FIELD(Key),
		FIELD(Name),
		FIELD(Article),
		FIELD(IndicatorLabel),
		FIELD(TridentKey),
		FIELD(IsTridentReference),
		FIELD(CO2Emissions),
		FIELD(EnergyConsumption),
		FIELD(impex::DataSourceLinks)
	> RollingStockSchema;
	
	namespace util
	{
		class ParametersMap;
	}

	namespace vehicle
	{
		//////////////////////////////////////////////////////////////////////////
		/// Rolling stock.
		/// TRIDENT = VehicleType
		/// @ingroup m38
		//////////////////////////////////////////////////////////////////////////
		class RollingStock:
			public virtual Object<RollingStock, RollingStockSchema>,
			public virtual util::Registrable,
			public graph::PathClass,
			public impex::ImportableTemplate<RollingStock>
		{
		public:
			/// Chosen registry class.
			typedef util::Registry<RollingStock> Registry;

			static const unsigned int CO2_EMISSIONS_DISTANCE_UNIT_IN_METERS;
			static const unsigned int ENERGY_CONSUMPTION_DISTANCE_UNIT_IN_METERS;

		public:

			RollingStock(util::RegistryKeyType key = 0);

			~RollingStock();


			//! @name Getters
			//@{
				const std::string getGTFSKey() const;
			//@}

			//! @name Services
			//@{
				virtual PathClass::Identifier getIdentifier() const;

				virtual std::string getName() const { return get<Name>(); }
			//@}
		};
	}
}

#endif
