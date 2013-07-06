
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

#include "Registrable.h"
#include "Registry.h"
#include "PathClass.h"
#include "ImportableTemplate.hpp"

#include <string>

namespace synthese
{
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
			public virtual util::Registrable,
			public graph::PathClass,
			public impex::ImportableTemplate<RollingStock>
		{
		private:
			static const std::string DATA_ID;
			static const std::string DATA_NAME;
			static const std::string DATA_ARTICLE;
			static const std::string DATA_CO2_EMISSIONS;
			static const std::string DATA_ENERGY_CONSUMPTION;

		public:
			/// Chosen registry class.
			typedef util::Registry<RollingStock> Registry;

			static const unsigned int CO2_EMISSIONS_DISTANCE_UNIT_IN_METERS;
			static const unsigned int ENERGY_CONSUMPTION_DISTANCE_UNIT_IN_METERS;

		protected:
			std::string _article;
			std::string _indicator;
			std::string _tridentKey;
			double _CO2Emissions;
			double _energyConsumption;
			bool _isTridentKeyReference;
			std::string _name;

		public:

			RollingStock(util::RegistryKeyType key = 0);

			~RollingStock();


			//! @name Getters
			//@{
				const std::string& getArticle() const { return _article; }
				const std::string& getIndicator() const { return _indicator; }
				const std::string& getTridentKey() const { return _tridentKey; }
				const std::string getGTFSKey() const;
				bool getIsTridentKeyReference() const { return _isTridentKeyReference; }
				double getCO2Emissions() const { return _CO2Emissions; }
				double getEnergyConsumption() const { return _energyConsumption; }
				virtual std::string getName() const { return _name; }
			//@}


			//! @name Setters
			//@{
				void setArticle(const std::string& value) { _article = value; }
				void setIndicator(const std::string& value) { _indicator = value; }
				void setTridentKey(const std::string& value) { _tridentKey = value; }
				void setIsTridentKeyReference(bool value) { _isTridentKeyReference = value; }
				void setCO2Emissions(double value) { _CO2Emissions = value; }
				void setEnergyConsumption(double value) { _energyConsumption = value; }
				void setName(const std::string& value){ _name = value; }
			//@}

			//! @name Services
			//@{
				virtual PathClass::Identifier getIdentifier() const;

				virtual void toParametersMap(
					util::ParametersMap& pm,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;
			//@}
		};
	}
}

#endif
