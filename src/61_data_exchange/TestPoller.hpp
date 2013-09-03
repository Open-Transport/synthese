
/** TestPoller class header.
	@file TestPoller.hpp

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

#ifndef SYNTHESE_data_exchange_TestPoller_hpp__
#define SYNTHESE_data_exchange_TestPoller_hpp__

#include "DeviceTemplate.h"
#include "Poller.hpp"

namespace synthese
{
	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// Test poller.
		//////////////////////////////////////////////////////////////////////////
		/// @author Camille Hue
		/// @ingroup m61
		class TestPoller:
			public server::DeviceTemplate<TestPoller>
		{
		public:
			
			//////////////////////////////////////////////////////////////////////////
			class Poller_:
				public server::Poller
			{
			public:
				static const std::string PARAMETER_TEST_TO_DISPLAY;

			private:
				std::string _testToDisplay;

			protected:

				virtual bool launchPoller() const;

			private:

			public:
				Poller_(
					util::Env& env,
					const server::PermanentThread& permanentThread,
					util::ParametersMap& pm
				);

				void startDisplay() const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from attributes to generic parameter maps.
				/// @return Generated parameters map
				/// @author Camille Hue
				/// @date 2013
				/// @since 3.9.0
				virtual util::ParametersMap getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author Camille Hue
				/// @date 2013
				/// @since 3.9.0
				virtual void setFromParametersMap(const util::ParametersMap& map);
			};
		};
	}
}

#endif // SYNTHESE_data_exchange_TestPoller_hpp__
