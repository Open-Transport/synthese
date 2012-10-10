
/** Vehicle class header.
	@file Vehicle.hpp

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

#ifndef SYNTHESE_vehicle_Vehicle_hpp__
#define SYNTHESE_vehicle_Vehicle_hpp__

#include "Named.h"
#include "ImportableTemplate.hpp"
#include "Registrable.h"
#include "Registry.h"

#include <set>
#include <vector>

namespace synthese
{
	namespace pt
	{
		class CommercialLine;
	}

	namespace util
	{
		class ParametersMap;
	}

	namespace vehicle
	{
		/** Vehicle class.
			@ingroup m38
		*/
		class Vehicle:
			public util::Named,
			public virtual util::Registrable,
			public impex::ImportableTemplate<Vehicle>
		{
		public:
			typedef util::Registry<Vehicle> Registry;
			typedef std::set<const pt::CommercialLine*> AllowedLines;
			typedef std::vector<std::string> Seats;

			static const std::string DATA_PICTURE;
			static const std::string DATA_NAME;
			static const std::string DATA_NUMBER;
			static const std::string DATA_SEATS;
			static const std::string DATA_VEHICLE_ID;

		private:
			AllowedLines _allowedLines;
			std::string _number;
			Seats _seats;
			std::string _picture;
			bool _available;
			std::string _url;
			std::string _registrationNumbers;

		public:
			Vehicle(util::RegistryKeyType id=0);

			//! @name Setters
			//@{
				void setAllowedLines(const AllowedLines& value){ _allowedLines = value; }
				void setNumber(const std::string& value){ _number = value; }
				void setSeats(const Seats& value){ _seats = value; }
				void setPicture(const std::string& value){ _picture = value; }
				void setAvailable(bool value){ _available = value; }
				void setURL(const std::string& value){ _url = value; }
				void setRegistrationNumbers(const std::string& value){ _registrationNumbers = value; }
			//@}

			//! @name Getters
			//@{
				const AllowedLines& getAllowedLines() const { return _allowedLines; }
				const std::string& getNumber() const { return _number; }
				const Seats& getSeats() const { return _seats; }
				const std::string& getPicture() const { return _picture; }
				bool getAvailable() const { return _available; }
				const std::string& getURL() const { return _url; }
				const std::string& getRegistrationNumbers() const { return _registrationNumbers; }
			//@}

			//! @name Updaters
			//@{
			//@}

			//! @name Services
			//@{
				void toParametersMap(
					util::ParametersMap& pm
				) const;
			//@}
		};
}	}

#endif // SYNTHESE_pt_operations_Vehicle_hpp__
