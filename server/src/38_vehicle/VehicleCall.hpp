
/** VehicleCall class header.
	@file VehicleCall.hpp

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

#ifndef SYNTHESE_vehicle_VehicleCall_hpp__
#define SYNTHESE_vehicle_VehicleCall_hpp__

#include "ImportableTemplate.hpp"
#include "PtimeField.hpp"
#include "EnumObjectField.hpp"
#include "Registrable.h"
#include "Registry.h"
#include "Vehicle.hpp"

#include "FrameworkTypes.hpp"

#include <set>
#include <vector>

namespace synthese
{
	namespace vehicle
	{
		class VehicleCall;



		FIELD_PTIME(CallTime)
		FIELD_BOOL(Priority)
		FIELD_PTIME(ClosureTime)



		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Vehicle),
			FIELD(CallTime),
			FIELD(Priority),
			FIELD(ClosureTime)
		> VehicleCallSchema;

		/** VehicleCall class.
		@ingroup m38
		*/
		class VehicleCall:
			public Object<VehicleCall, VehicleCallSchema>,
			public impex::ImportableTemplate<VehicleCall>
		{
		private:


		public:
			VehicleCall(util::RegistryKeyType id=0);
			~VehicleCall();

			//! @name Updaters
			//@{
			//@}

			//! @name Services
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();

				virtual void addAdditionalParameters(
				util::ParametersMap& map,
				std::string prefix = std::string()
				) const;
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_vehicle_VehicleCall_hpp__
