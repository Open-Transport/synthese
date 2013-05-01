
/** IncludeExpression class implementation.
	@file IncludeExpression.cpp

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

#include "IncludeExpression.hpp"

#include "ParametersMap.h"
#include "Webpage.h"

#include <sstream>
#include <boost/foreach.hpp>

using namespace std;

namespace synthese
{
	namespace cms
	{
		using namespace util;



		void IncludeExpression::display(
			std::ostream& stream,
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{

			// Parameters
			ParametersMap pm;
			BOOST_FOREACH(const Parameters::value_type& param, _parameters)
			{
				pm.insert(
					param.first,
					param.second.eval(request, additionalParametersMap, page, variables)
				);
			}
			pm.merge(additionalParametersMap);

			Webpage* includedPage(
				page.getRoot()->getPageBySmartURL(
					_pageName.eval(request, additionalParametersMap, page, variables)
			)	);
			if(includedPage)
			{
				includedPage->display(stream, request, pm, variables);
			}
		}



		std::string IncludeExpression::eval(
			const server::Request& request,
			const util::ParametersMap& additionalParametersMap,
			const Webpage& page,
			util::ParametersMap& variables
		) const	{
			stringstream s;
			display(s, request, additionalParametersMap, page, variables);
			return s.str();
		}



		IncludeExpression::IncludeExpression(
			std::string::const_iterator& it,
			std::string::const_iterator end
		){
			// page name
			string pageName("!");
			for(;it != end && *it != '&' && *it != '#'; ++it)
			{
				pageName.push_back(*it);
			}
			_pageName.setCode(pageName);

			// parameters
			if(it != end && *it == '#')
			{
				it += 2;
			}
			else
			{
				set<string> functionTermination;
				functionTermination.insert("&");
				functionTermination.insert("#>");
				while(it != end && *it == '&')
				{
					stringstream parameterName;
					++it;
					ParseText(parameterName, it, end, "=");

					if(it != end)
					{
						CMSScript parameterNodes(
							it,
							end,
							functionTermination
						);
						_parameters.push_back(
							make_pair(
								ParametersMap::Trim(parameterName.str()),
								parameterNodes
						)	);
						if(*(it-1) != '&')
						{
							break;
						}
						--it;
					}
				}
			}
		}
}	}
