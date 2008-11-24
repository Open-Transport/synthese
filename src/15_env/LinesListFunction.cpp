
/** LinesListFunction class implementation.
	@file LinesListFunction.cpp
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

// env
#include "LinesListFunction.h"
#include "TransportNetwork.h"
#include "TransportNetworkTableSync.h"
#include "CommercialLine.h"
#include "CommercialLineTableSync.h"

// Boost
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;

	template<> const string util::FactorableTemplate<Function,env::LinesListFunction>::FACTORY_KEY("LinesListFunction");
	
	namespace env
	{
		const string LinesListFunction::PARAMETER_NETWORK_ID("ni");
		
		ParametersMap LinesListFunction::_getParametersMap() const
		{
			ParametersMap map;
			map.insert(PARAMETER_NETWORK_ID, _network->getKey());
			return map;
		}

		void LinesListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			uid id(map.getUid(PARAMETER_NETWORK_ID, false, FACTORY_KEY));
			try
			{
				_network = TransportNetworkTableSync::Get(id);
			}
			catch (...)
			{
				throw RequestException("Transport network " + Conversion::ToString(id) + " not found");
			}
		}

		void LinesListFunction::_run( std::ostream& stream ) const
		{
			BOOST_FOREACH(shared_ptr<CommercialLine> line, CommercialLineTableSync::search(_network->getKey()))
			{
				stream << line->getKey() << ";" << line->getShortName() << "+\n";
			}
		}
	}
}
