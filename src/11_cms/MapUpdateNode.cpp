
/** MapUpdateNode class implementation.
	@file MapUpdateNode.cpp

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

#include "MapUpdateNode.hpp"

#include "ParametersMap.h"
#include "ServiceExpression.hpp"

using namespace std;

namespace synthese
{
	using namespace util;

	namespace cms
	{
		const string MapUpdateNode::FIELD_ID = "id";
		const string MapUpdateNode::FIELD_VALUE = "value";



		MapUpdateNode::MapUpdateNode(
			const Items& variable,
			std::string::const_iterator& it,
			std::string::const_iterator end
		):	_variable(variable)
		{
			// Termination definition
			set<string> termination;
			termination.insert("@>");

			// Load of the value definition
			while(it != end)
			{
				if(*it != '<' || it+1 == end || *(it+1) != '?' || it+2 == end)
				{
					++it;
					continue;
				}

				++it;
				++it;

				// Parsing service node
				boost::shared_ptr<ServiceExpression> node(
					new ServiceExpression(it, end)
				);
				if(node->getFunctionCreator())
				{
					_services.push_back(node);
				}

				// Check if the termination was reached
				bool terminated(false);
				BOOST_FOREACH(const string& test, termination)
				{
					if(Expression::CompareText(it, end, test))
					{
						terminated = true;
						break;
					}
				}
				if(terminated)
				{
					break;
				}
			}
		}



		void MapUpdateNode::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{
			ParametersMap* pm(&variables);

			// Loop on items
			for(size_t rank(0); rank<_variable.size(); ++rank)
			{
				const Item& item(_variable[rank]);

				if(	rank+1 < _variable.size()||
					item.index.get()
				){
					// Submap creation
					if(!pm->hasSubMaps(item.key))
					{
						boost::shared_ptr<ParametersMap> subMap(new ParametersMap);
						if(item.index.get())
						{
							subMap->insert(
								FIELD_ID,
								item.index->eval(request, additionalParametersMap, page, variables)
							);
						}
						pm->insert(item.key, subMap);
						pm = subMap.get();
					}
					else if(item.index.get())
					{
						// Existing submap search
						string idx(item.index->eval(request, additionalParametersMap, page, variables));
						const ParametersMap::SubParametersMap::mapped_type& subMaps(
							pm->getSubMaps(item.key)
						);
						ParametersMap* pm2(NULL);
						BOOST_FOREACH(const boost::shared_ptr<ParametersMap>& subMapItem, subMaps)
						{
							if(subMapItem->getDefault<string>(FIELD_ID) == idx)
							{
								pm2 = subMapItem.get();
								break;
							}
						}

						// Creation if necessary
						if(!pm2)
						{
							boost::shared_ptr<ParametersMap> subMap(new ParametersMap);
							subMap->insert(
								FIELD_ID,
								item.index->eval(request, additionalParametersMap, page, variables)
							);
							pm->insert(item.key, subMap);
							pm = subMap.get();
						}
						else
						{
							pm = pm2;
						}
					}
				}

				if(rank+1 == _variable.size())
				{
					string fieldName(item.index.get() ? FIELD_VALUE : item.key);

					// Clean of existing submaps
					pm->removeSubMap(fieldName);

					// Execution of all services
					stringstream fakeStream;
					BOOST_FOREACH(const boost::shared_ptr<ServiceExpression> service, _services)
					{
						boost::shared_ptr<ParametersMap> servicePM(new ParametersMap);
						service->runService(
							fakeStream,
							*servicePM,
							request,
							additionalParametersMap,
							page,
							variables
						);
						pm->insert(fieldName, servicePM);
					}
				}
			}
		}
}	}
