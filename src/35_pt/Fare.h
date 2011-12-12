
/** Fare class header.
	@file Fare.h

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

#ifndef SYNTHESE_ENV_FARE_H
#define SYNTHESE_ENV_FARE_H

#include "Registrable.h"
#include "Registry.h"
#include "FareType.hpp"

#include <string>
#include <boost/optional/optional.hpp>

namespace synthese
{
	namespace pt
	{
		/** Fare handling class
			@ingroup m35
		*/
		class Fare
		:	public virtual util::Registrable
		{
		 public:

			/// Chosen registry class.
			typedef util::Registry<Fare>	Registry;

		 private:

			std::string _name; //!< Fare name
			boost::shared_ptr<FareType> _type; //!< Fare type
			std::string _currency; //!< Fare currency
			boost::optional<int> _permittedConnectionsNumber; //!< Number of permitted connections
			bool _requiredContinuity;
			int _validityPeriod; //!< Maximal validity period (minutes)

		public:
			Fare(
				util::RegistryKeyType key = 0
			);
			~Fare ();


			//! @name Getters
			//@{
				const std::string& getName() const { return _name; }
				const boost::shared_ptr<FareType> getType() const { return _type; }
				const std::string& getCurrency() const { return _currency; }
				const boost::optional<int> getPermittedConnectionsNumber() const { return _permittedConnectionsNumber; }
				bool getRequiredContinuity() const { return _requiredContinuity; }
				int getValidityPeriod() const { return _validityPeriod; }
			//@}

			//! @name Setters
			//@{
				void setName(const std::string& name) { _name = name; }
				void setTypeNumber(FareType::FareTypeNumber number);
				void setCurrency(const std::string& currency) { _currency = currency; }
				void setPermittedConnectionsNumber(const boost::optional<int> permittedConnectionsNumber) { _permittedConnectionsNumber = permittedConnectionsNumber; }
				void setRequiredContinuity(const bool requiredContinuity) { _requiredContinuity = requiredContinuity; }
				void setValidityPeriod(const int validityPeriod) { _validityPeriod = validityPeriod; }
			//@}
		};
	}
}

#endif
