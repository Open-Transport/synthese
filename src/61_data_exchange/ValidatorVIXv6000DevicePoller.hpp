
/** ValidatorVIXv6000Poller class header.
	@file ValidatorVIXv6000Poller.hpp

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

#ifndef SYNTHESE_pt_ValidatorVIXv6000Poller_hpp__
#define SYNTHESE_pt_ValidatorVIXv6000Poller_hpp__

#include <string>

#include "DeviceTemplate.h"
#include "Poller.hpp"
#include "UtilTypes.h"

namespace synthese
{
	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// Test poller.
		//////////////////////////////////////////////////////////////////////////
		/// @author RCS
		/// @ingroup m61
		class ValidatorVIXv6000DevicePoller:
			public server::DeviceTemplate<ValidatorVIXv6000DevicePoller>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Poller_:
				public server::Poller
			{
			public:
				static const std::string PARAMETER_VALIDATOR_COM_PORT_NUMBER;
				static const std::string PARAMETER_VALIDATOR_COM_PORT_RATE;
				static const std::string PARAMETER_VALIDATOR_DATA_SOURCE_KEY;

			private:
				static int _ComPortNb;
				static int _ComPortRate;
				static util::RegistryKeyType _dataSourceKey;

			protected:

				virtual bool launchPoller() const;

			public:
				Poller_(
					util::Env& env,
					const server::PermanentThread& permanentThread,
					util::ParametersMap& pm
					);

				void startPolling() const;

				//////////////////////////////////////////////////////////////////////////
				/// Conversion from attributes to generic parameter maps.
				/// @return Generated parameters map
				/// @author RCS
				/// @date 2013
				/// @since 3.9.0
				virtual util::ParametersMap getParametersMap() const;

				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author RCS
				/// @date 2013
				/// @since 3.9.0
				virtual void setFromParametersMap(const util::ParametersMap& map);
			};
		};
	}
}

#endif // SYNTHESE_pt_ValidatorVIXv6000Poller_hpp__

