
/** PTRoutesListFunction class implementation.
	@file PTRoutesListFunction.cpp
	@author Hugues Romain
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "PTRoutesListFunction.hpp"
#include "CommercialLine.h"
#include "Line.h"
#include "Env.h"
#include "PTRoutesListItemInterfacePage.hpp"
#include "WebPage.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace pt;
	using namespace transportwebsite;
	using namespace graph;

	template<> const string FactorableTemplate<Function, PTRoutesListFunction>::FACTORY_KEY("PTRoutesListFunction");
	
	namespace pt
	{
		const string PTRoutesListFunction::PARAMETER_PAGE_ID("p");
		const string PTRoutesListFunction::PARAMETER_MERGE_INCLUDING_ROUTES("mir");
		const string PTRoutesListFunction::PARAMETER_MERGE_SAME_ROUTES("msr");
		
		ParametersMap PTRoutesListFunction::_getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			map.insert(PARAMETER_MERGE_INCLUDING_ROUTES, _mergeIncludingRoutes);
			map.insert(PARAMETER_MERGE_SAME_ROUTES, _mergeSameRoutes);
			if(_line.get())
			{
				map.insert(Request::PARAMETER_OBJECT_ID, _line->getKey());
			}
			return map;
		}

		void PTRoutesListFunction::_setFromParametersMap(const ParametersMap& map)
		{
			optional<RegistryKeyType> id(map.getOptional<RegistryKeyType>(PARAMETER_PAGE_ID));
			if(id) try
			{
				_page = Env::GetOfficialEnv().get<WebPage>(*id);
			}
			catch (ObjectNotFoundException<WebPage>&)
			{
				throw RequestException("No such page");
			}
			_mergeIncludingRoutes = map.getDefault<bool>(PARAMETER_MERGE_INCLUDING_ROUTES, false);
			_mergeSameRoutes = map.getDefault<bool>(PARAMETER_MERGE_SAME_ROUTES, false);

			try
			{
				_line = Env::GetOfficialEnv().getRegistry<CommercialLine>().get(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			}
			catch (ObjectNotFoundException<CommercialLine>&)
			{
				throw RequestException("No such line");
			}
		}

		void PTRoutesListFunction::run(
			std::ostream& stream,
			const Request& request
		) const {
			
			set<const Line*> routes;
			// Selection of routes by same comparison
			BOOST_FOREACH(const Path* path, _line->getPaths())
			{
				bool toInsert(true);

				if(_mergeSameRoutes)
				{
					BOOST_FOREACH(const Line* existingRoute, routes)
					{
						if(existingRoute->sameContent(*path, false, false))
						{
							toInsert = false;
							break;
						}
					}
				}

				if(toInsert)
				{
					routes.insert(dynamic_cast<const Line*>(path));
				}
			}

			// Selection of routes by inclusion
			if(_mergeIncludingRoutes)
			{
				vector<const Line*> routesToRemove;
				BOOST_FOREACH(const Line* route, routes)
				{
					BOOST_FOREACH(const Line* otherRoute, routes)
					{
						if(otherRoute == route)
						{
							continue;
						}
						if(otherRoute->includes(*route, false))
						{
							routesToRemove.push_back(route);
							break;
						}
					}
				}
				BOOST_FOREACH(const Line* route, routesToRemove)
				{
					routes.erase(route);
				}
			}


			if(!_page.get())
			{
				// XML header
			}

			size_t rank(0);
			BOOST_FOREACH(const Line* route, routes)
			{
				if(_page.get())
				{
					PTRoutesListItemInterfacePage::Display(
						stream,
						_page,
						request,
						*route,
						rank++
					);
				}
				else
				{
					// XML
				}
			}

			if(!_page.get())
			{
				// XML footer
			}
		}
		
		
		
		bool PTRoutesListFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string PTRoutesListFunction::getOutputMimeType() const
		{
			return _page.get() ? _page->getMimeType() : "text/xml";
		}
	}
}
