
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

#include "DataSourceLinksField.hpp"
#include "InterSYNTHESESlave.hpp"
#include "NumericField.hpp"
#include "Object.hpp"
#include "PointersSetField.hpp"
#include "StringField.hpp"
#include "StringVectorField.hpp"

#include "CommercialLine.h"

namespace synthese
{
	namespace util
	{
		class ParametersMap;
	}

	namespace vehicle
	{
		class Vehicle;

		FIELD_STRING(Number)
		FIELD_POINTERS_SET(AllowedLines, pt::CommercialLine)
		FIELD_STRING_VECTOR(Seats)
		FIELD_STRING(Picture)
		FIELD_BOOL(Available)
		FIELD_STRING(URL)
		FIELD_STRING(RegistrationNumber)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(Number),
			FIELD(AllowedLines),
			FIELD(Seats),
			FIELD(Picture),
			FIELD(Available),
			FIELD(URL),
			FIELD(RegistrationNumber),
			FIELD(impex::DataSourceLinks),
			FIELD(inter_synthese::InterSYNTHESESlave)
		> VehicleRecord;

		/** Vehicle class.
			@ingroup m38
		*/
		class Vehicle:
			public virtual util::Registrable,
			public Object<Vehicle, VehicleRecord>,
			public impex::ImportableTemplate<Vehicle>
		{
		public:
			static const std::string TAG_SEAT;

		private:

		public:
			Vehicle(util::RegistryKeyType id=0);

			//! @name Setters
			//@{
			//@}

			//! @name Getters
			//@{
			//@}

			//! @name Updaters
			//@{
			//@}

			//! @name Services
			//@{
				virtual void addAdditionalParameters(
					util::ParametersMap& pm,
					std::string prefix = std::string()
				) const;
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();
				virtual std::string getName() const { return get<Name>(); }
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_pt_operations_Vehicle_hpp__
