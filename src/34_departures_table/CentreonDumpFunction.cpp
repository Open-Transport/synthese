
/** CentreonDumpFunction class implementation.
	@file CentreonDumpFunction.cpp
	@author Hugues Romain
	@date 2008

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

// Util
#include "Conversion.h"

// Server
#include "RequestException.h"
#include "RequestMissingParameterException.h"

// departurestable
#include "CentreonDumpFunction.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;

	template<> const string util::FactorableTemplate<Function,departurestable::CentreonDumpFunction>::FACTORY_KEY("CentreonDumpFunction");
	
	namespace departurestable
	{
		/// @todo Parameter names declarations
		//const string CentreonDumpFunction::PARAMETER_NETWORK_ID("ni");

		
		ParametersMap CentreonDumpFunction::_getParametersMap() const
		{
			ParametersMap map;
			/// @todo Map filling
			// eg : map.insert(PARAMETER_NETWORK_ID, _network->getKey());
			return map;
		}


		void CentreonDumpFunction::_setFromParametersMap(const ParametersMap& map)
		{
			// @todo Fill it like :
			//uid id(map.getUid(PARAMETER_NETWORK_ID, false, FACTORY_KEY));
			//try
			//{
			//	_network = TransportNetworkTableSync::Get(id);
			//}
			//catch (...)
			//{
			//	throw RequestException("Transport network " + Conversion::ToString(id) + " not found");
			//}
		}


		void CentreonDumpFunction::_run( std::ostream& stream ) const
		{
			// Hostgroups
		}
	}
}
