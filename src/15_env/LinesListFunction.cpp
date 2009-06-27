////////////////////////////////////////////////////////////////////////////////
/// LinesListFunction class implementation.
///	@file LinesListFunction.cpp
///	@author Hugues Romain
///	@date 2008
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "RequestException.h"
#include "LinesListFunction.h"
#include "TransportNetwork.h"
#include "TransportNetworkTableSync.h"
#include "CommercialLine.h"
#include "CommercialLineTableSync.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace pt;

	template<> const string util::FactorableTemplate<Function,env::LinesListFunction>::FACTORY_KEY(
		"LinesListFunction2"
	);
	
	namespace env
	{
		const string LinesListFunction::PARAMETER_NETWORK_ID("ni");
		
		ParametersMap LinesListFunction::_getParametersMap() const
		{
			ParametersMap result;
			if (_network.get() != NULL)
			{
				result.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}
			return result;
		}

		void LinesListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			setNetworkId(map.getUid(PARAMETER_NETWORK_ID, false, FACTORY_KEY));
		}


		void LinesListFunction::_run( std::ostream& stream ) const
		{
			CommercialLineTableSync::Search(_env, _network->getKey());
			
			BOOST_FOREACH(shared_ptr<const CommercialLine> line, _env.getRegistry<CommercialLine>())
			{
				stream << line->getKey() << ";" << line->getShortName() << "\n";
			}
		}



		bool LinesListFunction::_isAuthorized(
		) const {
			return true;
		}



		void LinesListFunction::setNetworkId(
			util::RegistryKeyType id
		) throw(RequestException) {
			try
			{
				_network = TransportNetworkTableSync::Get(id, _env);
			}
			catch (...)
			{
				throw RequestException("Transport network " + lexical_cast<string>(id) + " not found");
			}
		}

		std::string LinesListFunction::getOutputMimeType() const
		{
			return "text/csv";
		}
	}
}
