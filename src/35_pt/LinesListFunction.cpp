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
#include "LineMarkerInterfacePage.h"
#include "Webpage.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace pt;
	using namespace security;
	using namespace cms;
	
	template<> const string util::FactorableTemplate<server::Function,pt::LinesListFunction>::FACTORY_KEY(
		"LinesListFunction2"
	);
	
	namespace pt
	{
		const string LinesListFunction::PARAMETER_NETWORK_ID("ni");
		const string LinesListFunction::PARAMETER_PAGE_ID("pi");
		
		ParametersMap LinesListFunction::_getParametersMap() const
		{
			ParametersMap result;
			if (_network.get() != NULL)
			{
				result.insert(PARAMETER_NETWORK_ID, _network->getKey());
			}
			if(_page.get())
			{
				result.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			return result;
		}

		void LinesListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			setNetworkId(map.get<RegistryKeyType>(PARAMETER_NETWORK_ID));
			
			optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_PAGE_ID));
			if(id)
			try
			{
				_page = Env::GetOfficialEnv().get<Webpage>(*id);
			}
			catch (ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such page");
			}			
		}


		void LinesListFunction::run( std::ostream& stream, const Request& request ) const
		{
			CommercialLineTableSync::SearchResult lines(
				CommercialLineTableSync::Search(*_env, _network->getKey())
			);
			size_t rank(0);
			BOOST_FOREACH(shared_ptr<const CommercialLine> line, lines)
			{
				if(_page.get())
				{
					LineMarkerInterfacePage::Display(
						stream,
						_page,
						request,
						*line,
						rank++
					);
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
			return _page.get() ? _page->getMimeType() : "text/csv";
		}
	}
}
