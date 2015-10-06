
/** Descent class header.
	@file Descent.hpp

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

#ifndef SYNTHESE_vehicle_Descent_hpp__
#define SYNTHESE_vehicle_Descent_hpp__

#include "DateField.hpp"
#include "ImportableTemplate.hpp"
#include "PointerField.hpp"
#include "PtimeField.hpp"
#include "Registrable.h"
#include "Registry.h"
#include "ScheduledService.h"
#include "StopPoint.hpp"
#include "User.h"

#include "FrameworkTypes.hpp"

#include <set>
#include <vector>

namespace synthese
{
	namespace vehicle
	{
		class Descent;

		FIELD_POINTER(Service, pt::ScheduledService)
		FIELD_POINTER(Stop, pt::StopPoint)
		FIELD_POINTER(ActivationUser, security::User)
		FIELD_POINTER(CancellationUser, security::User)
		FIELD_PTIME(ActivationTime)
		FIELD_PTIME(CancellationTime)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Service),
			FIELD(Stop),
			FIELD(Date),
			FIELD(ActivationTime),
			FIELD(CancellationTime),
			FIELD(ActivationUser),
			FIELD(CancellationUser)
		> DescentSchema;

		/** Descent class.
		@ingroup m38
		*/
		class Descent:
			public Object<Descent, DescentSchema>,
			public impex::ImportableTemplate<Descent>
		{
		public:
			static const std::string TAG_SERVICE;
			static const std::string TAG_STOP_POINT;

		private:


		public:
			Descent(util::RegistryKeyType id=0);
			~Descent();

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

#endif // SYNTHESE_vehicle_Descent_hpp__
