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
#include "PTLinesListItemInterfacePage.hpp"
#include "Interface.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace env;
	using namespace security;
	using namespace transportwebsite;
	using namespace interfaces;
	

	template<> const string util::FactorableTemplate<pt::LinesListFunction::_FunctionWithSite,pt::LinesListFunction>::FACTORY_KEY(
		"LinesListFunction2"
	);
	
	namespace pt
	{
		const string LinesListFunction::PARAMETER_NETWORK_ID("ni");
		
		ParametersMap LinesListFunction::_getParametersMap() const
		{
			ParametersMap result(FunctionWithSiteBase::_getParametersMap());
			if (_network.get() != NULL)
			{
				result.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}
			return result;
		}

		void LinesListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			setNetworkId(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID));

			_FunctionWithSite::_setFromParametersMap(map);
			if(_site.get() && _site->getInterface())
			{
				_page = _site->getInterface()->getPage<PTLinesListItemInterfacePage>();
			}
		}


		void LinesListFunction::run( std::ostream& stream, const Request& request ) const
		{
			CommercialLineTableSync::SearchResult lines(
				CommercialLineTableSync::Search(*_env, _network->getKey())
			);
			size_t rank(0);
			VariablesMap variables;
			BOOST_FOREACH(shared_ptr<const CommercialLine> line, lines)
			{
				if(_page)
				{
					_page->display(stream, *line, rank++, variables, &request);
				}
				else
				{
					stream << line->getKey() << ";" << line->getShortName() << "\n";
				}
			}
		}



		bool LinesListFunction::isAuthorized(
			const Session*
		) const {
			return true;
		}



		void LinesListFunction::setNetworkId(
			util::RegistryKeyType id
		){
			try
			{
				_network = TransportNetworkTableSync::Get(id, *_env);
			}
			catch (...)
			{
				throw RequestException("Transport network " + lexical_cast<string>(id) + " not found");
			}
		}

		std::string LinesListFunction::getOutputMimeType() const
		{
			return _page ? _page->getMimeType() : "text/csv";
		}
	}
}
