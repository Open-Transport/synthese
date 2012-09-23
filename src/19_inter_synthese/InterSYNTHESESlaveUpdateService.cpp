
//////////////////////////////////////////////////////////////////////////////////////////
///	InterSYNTHESESlaveUpdateService class implementation.
///	@file InterSYNTHESESlaveUpdateService.cpp
///	@author Hugues Romain
///	@date 2012
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "InterSYNTHESESlaveUpdateService.hpp"

#include "InterSYNTHESESyncTypeFactory.hpp"
#include "RequestException.h"
#include "Request.h"
#include "ServerConstants.h"

#include <boost/algorithm/string.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,inter_synthese::InterSYNTHESESlaveUpdateService>::FACTORY_KEY = "slave_update";
	
	namespace inter_synthese
	{
		const string InterSYNTHESESlaveUpdateService::SYNCS_SEPARATOR = "!$$$$!$$$$!%%%!\r\n";
		


		ParametersMap InterSYNTHESESlaveUpdateService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void InterSYNTHESESlaveUpdateService::_setFromParametersMap(const ParametersMap& map)
		{
			string contentStr = map.getDefault<string>(PARAMETER_POST_DATA);
			trim(contentStr);
			if(contentStr.empty())
			{
				return;
			}
			vector<string> rows;
			typedef split_iterator<string::iterator> string_split_iterator;
			for(string_split_iterator its=make_split_iterator(const_cast<string&>(contentStr), first_finder(SYNCS_SEPARATOR, is_iequal()));
				its != string_split_iterator();
				++its
			){
				string param(copy_range<std::string>(*its));
				if(param.empty())
				{
					continue;
				}

				size_t i(0);
				for(;i<param.size(); ++i)
				{
					if(param[i] == ':')
					{
						break;
					}
				}
				if(i == param.size() || i == 0)
				{
					continue;
				}

				_content.push_back(make_pair(param.substr(0, i-1), param.substr(i+1)));
			}
		}



		ParametersMap InterSYNTHESESlaveUpdateService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			BOOST_FOREACH(const Content::value_type& content, _content)
			{
				auto_ptr<InterSYNTHESESyncTypeFactory> interSYNTHESE(
					Factory<InterSYNTHESESyncTypeFactory>::create(content.first)
				);
				interSYNTHESE->sync(content.second);
			}
			stream << "OK";
			return map;
		}
		
		
		
		bool InterSYNTHESESlaveUpdateService::isAuthorized(
			const Session* session
		) const {
			return true;
//			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<>();
		}



		std::string InterSYNTHESESlaveUpdateService::getOutputMimeType() const
		{
			return "text/plain";
		}
}	}
