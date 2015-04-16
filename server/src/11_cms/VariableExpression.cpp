
/** VariableExpression class implementation.
	@file VariableExpression.cpp

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

#include "VariableExpression.hpp"

#include "ParametersMap.h"
#include "Request.h"
#include "Webpage.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;

	namespace cms
	{
		const string VariableExpression::FIELD_ID = "id";
		const string VariableExpression::FIELD_VALUE = "value";



		VariableExpression::VariableExpression(
			const Items& variable
		):	_variable(variable)
		{}



		std::string VariableExpression::eval(
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{

			// Special values
			if(_variable.size() == 1 && !_variable.begin()->index.get())
			{
				if(_variable.begin()->key == "client_url")
				{
					return request.getClientURL();
				}
				if(_variable.begin()->key == "host_name")
				{
					return request.getHostName();
				}
				if(page.getRoot() && _variable.begin()->key == "site")
				{
					return lexical_cast<string>(page.getRoot()->getKey());
				}
				if(_variable.begin()->key == "p")
				{
					return lexical_cast<string>(page.getKey());
				}
			}

			// Loop on items
			const ParametersMap* pm(NULL);
			for(size_t rank(0); rank < _variable.size(); ++rank)
			{
				const Item& item(_variable[rank]);

				// Submap read
				if(	rank+1 < _variable.size() ||
					item.index.get()
				){
					// If first rank, select the source map
					if(rank == 0)
					{
						// Variable first
						if(variables.hasSubMaps(item.key))
						{
							pm = &variables;
						} // Else parameters
						else if(additionalParametersMap.hasSubMaps(item.key))
						{
							pm = &additionalParametersMap;
						}
						else // Sub map does not exist
						{
							return string();
						}
					}

					// Submap check
					if(!pm->hasSubMaps(item.key))
					{
						return string();
					}

					// If no index specified, take the first submap item
					if(!item.index.get())
					{
						pm = pm->getSubMaps(item.key).begin()->get();
					}
					else // Else search for the specified item
					{
						string idx(
							item.index->eval(request,additionalParametersMap,page,variables)
						);
						const ParametersMap::SubParametersMap::mapped_type& subMaps(
							pm->getSubMaps(item.key)
						);
						pm = NULL;
						BOOST_FOREACH(const boost::shared_ptr<ParametersMap>& subMapItem, subMaps)
						{
							if(subMapItem->getDefault<string>(FIELD_ID) == idx)
							{
								pm = subMapItem.get();
								break;
							}
						}

						// Index was not found
						if(!pm)
						{
							return string();
						}
					}
				}

				if(	rank+1 == _variable.size())
				{
					string toFind(item.index.get() ? FIELD_VALUE : item.key);
					string value;
					if(!pm)
					{
						value = variables.getDefault<string>(toFind, string(), false);
					}
					if(value.empty())
					{
						if(pm)
						{
							value = pm->getDefault<string>(toFind, string(), false);
						}
						else
						{
							value = additionalParametersMap.getDefault<string>(toFind, string(), false);
						}
					}
					if(value.empty() && !pm)
					{
						value = request.getParametersMap().getDefault<string>(toFind, string(), false);
					}
					return value;
				}
			}

			// Should never happen
			return string();
		}
}	}
