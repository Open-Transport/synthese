/** Device class header.
	@file Device.h

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

#ifndef SYNTHESE_server_Device_h__
#define SYNTHESE_server_Device_h__

#include "FactoryBase.h"

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

////////////////////////////////////////////////////////////////////
/// @defgroup refFile 16 Device
///	@ingroup ref

namespace synthese
{
	namespace util
	{
		class Env;
		class ParametersMap;
	}

	namespace server
	{
		class PermanentThread;
		class Poller;

		class Device:
			public util::FactoryBase<Device>
		{
		public:
			static const std::string ATTR_KEY;

			virtual boost::shared_ptr<Poller> getPoller(
				util::Env& env,
				const PermanentThread& permanentThread,
				util::ParametersMap& pm
			) const = 0;


			void toParametersMap(
				util::ParametersMap& pm
			) const;
		};
}	}

#endif // SYNTHESE_server_Device_h__
