
/** VariablesDebugExpression class implementation.
	@file VariablesDebugExpression.cpp

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

#include "VariablesDebugExpression.hpp"

#include "HTMLTable.h"
#include "ParametersMap.h"
#include "Request.h"
#include "Webpage.h"

#include <sstream>
#include <boost/foreach.hpp>

using namespace std;

namespace synthese
{
	using namespace html;
	using namespace util;

	namespace cms
	{
		VariablesDebugExpression::VariablesDebugExpression()
		{}



		std::string VariablesDebugExpression::eval(
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{

			stringstream s;
			display(s, request, additionalParametersMap, page, variables);
			return s.str();

		}



		void VariablesDebugExpression::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{

			HTMLTable::ColsVector c;
			c.push_back("name");
			c.push_back("value");
			HTMLTable t(c, "table table-striped table-condensed sortable");
			stream << t.open();
			t.body(stream);
			BOOST_FOREACH(const ParametersMap::Map::value_type& item, variables.getMap())
			{
				stream << t.row();
				stream << t.col() << item.first;
				stream << t.col() << item.second;
			}
			BOOST_FOREACH(const ParametersMap::Map::value_type& item, additionalParametersMap.getMap())
			{
				// Variables first
				if(variables.getMap().find(item.first) != variables.getMap().end())
				{
					continue;
				}
				stream << t.row();
				stream << t.col() << item.first;
				stream << t.col() << item.second;
			}
			ParametersMap requestMap(request.getParametersMap());
			BOOST_FOREACH(
				const ParametersMap::Map::value_type& item,
				requestMap.getMap()
				){
					// Variables first
					if(variables.getMap().find(item.first) != variables.getMap().end())
					{
						continue;
					}
					// Parameters first
					if(additionalParametersMap.getMap().find(item.first) != additionalParametersMap.getMap().end())
					{
						continue;
					}
					stream << t.row();
					stream << t.col() << item.first;
					stream << t.col() << item.second;
			}
			stream << t.row();
			stream << t.col() << "host_name";
			stream << t.col() << request.getHostName();
			stream << t.row();
			stream << t.col() << "client_url";
			stream << t.col() << request.getClientURL();
			const Website* site(page.getRoot());
			if(site)
			{
				stream << t.row();
				stream << t.col() << "site";
				stream << t.col() << site->getKey();
			}
			BOOST_FOREACH(const ParametersMap::SubMapsKeys::value_type& item, additionalParametersMap.getSubMapsKeys())
			{
				stream << t.row();
				stream << t.col() << item;
				stream << t.col() << "(submap)";
			}
			BOOST_FOREACH(const ParametersMap::SubMapsKeys::value_type& item, variables.getSubMapsKeys())
			{
				if(additionalParametersMap.hasSubMaps(item))
				{
					continue;
				}
				stream << t.row();
				stream << t.col() << item;
				stream << t.col() << "(submap)";
			}
			stream << t.close();

		}
}	}

