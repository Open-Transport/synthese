
/** DisplayScreenCPUMaintenanceUpdateAction class header.
	@file DisplayScreenCPUCreateAction.h

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

#ifndef SYNTHESE_DisplayScreenCPUMaintenanceUpdateAction_H__
#define SYNTHESE_DisplayScreenCPUMaintenanceUpdateAction_H__

#include "Action.h"
#include "FactorableTemplate.h"

#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

namespace synthese
{
	namespace departure_boards
	{
		class DisplayScreenCPU;

		/** Display screen maintenance update action class.
			@ingroup m54Actions refActions
		*/
		class DisplayScreenCPUMaintenanceUpdateAction :
			public util::FactorableTemplate<server::Action, DisplayScreenCPUMaintenanceUpdateAction>
		{
		public:
			static const std::string PARAMETER_CPU;
			static const std::string PARAMETER_IS_ONLINE;
			static const std::string PARAMETER_MAINTENANCE_MESSAGE;
			static const std::string PARAMETER_MONITORING_DELAY;

		private:
			boost::shared_ptr<DisplayScreenCPU>		_cpu;
			bool									_isOnline;
			std::string								_maintenanceMessage;
			boost::posix_time::time_duration		_monitoringDelay;

		protected:
			/** Conversion from attributes to generic parameter maps.
			*/
			util::ParametersMap getParametersMap() const;

			/** Conversion from generic parameters map to attributes.
			Removes the used parameters from the map.
			*/
			void _setFromParametersMap(const util::ParametersMap& map);

		public:
			/** Action to run, defined by each subclass.
			*/
			void run(server::Request& request);

			void setCPU(util::RegistryKeyType id);

			virtual bool isAuthorized(const server::Session* session) const;
		};
	}
}

#endif
