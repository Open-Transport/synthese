
/** Fare class header.
	@file Fare.hpp

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
#include "ServicePointer.h"

#include <string>
#include <vector>
#include <boost/optional/optional.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

namespace synthese
{
	namespace fare
	{
		/** Fare handling class
			@ingroup m30
		*/
		class Fare:
			public virtual util::Registrable
		{
		 public:
			/// Chosen registry class.
			typedef util::Registry<Fare>	Registry;

		 private:

			std::string _name; //!< Fare name
			boost::shared_ptr<FareType> _type; //!< Fare type
			std::string _currency; //!< Fare currency
			boost::optional<int> _permittedConnectionsNumber; //!< Number of permitted connections
			bool _requiredContinuity; //!< continuity requirement
			int _validityPeriod; //!< Maximal validity period (minutes)

		public:
			//////////////////////////////////////////////////////////////////////////
			/// Constructor.
			/// @param key id of the object (optional)
			/// @author Gaël Sauvanet
			/// @date 2011
			Fare(
				util::RegistryKeyType key = 0
			);



			//////////////////////////////////////////////////////////////////////////
			/// Set the FareType according to the corresponding FareType number
			/// @param number the number of the FareType
			/// @author Gaël Sauvanet
			/// @date 2011
			void setTypeNumber(FareType::FareTypeNumber number);



			//! @name Getters
			//@{
				virtual std::string getName() const { return _name; }
				const boost::shared_ptr<FareType> getType() const { return _type; }
				const std::string& getCurrency() const { return _currency; }
				const boost::optional<int> getPermittedConnectionsNumber() const { return _permittedConnectionsNumber; }
				bool isRequiredContinuity() const { return _requiredContinuity; }
				int getValidityPeriod() const { return _validityPeriod; }
			//@}

			//! @name Setters
			//@{
				void setName(const std::string& name) { _name = name; }
				void setCurrency(const std::string& currency) { _currency = currency; }
				void setPermittedConnectionsNumber(const boost::optional<int> permittedConnectionsNumber) { _permittedConnectionsNumber = permittedConnectionsNumber; }
				void setRequiredContinuity(const bool requiredContinuity) { _requiredContinuity = requiredContinuity; }
				void setValidityPeriod(const int validityPeriod) { _validityPeriod = validityPeriod; }
			//@}
		};
	}
}

#endif
