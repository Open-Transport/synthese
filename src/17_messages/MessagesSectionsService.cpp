
//////////////////////////////////////////////////////////////////////////////////////////
///	MessagesSectionsService class implementation.
///	@file MessagesSectionsService.cpp
///	@author hromain
///	@date 2013
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

#include "MessagesSectionsService.hpp"

#include "MessagesSection.hpp"
#include "RequestException.h"
#include "Request.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,messages::MessagesSectionsService>::FACTORY_KEY = "messages_sections";
	
	namespace messages
	{
		const string MessagesSectionsService::TAG_SECTION = "section";
		


		ParametersMap MessagesSectionsService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void MessagesSectionsService::_setFromParametersMap(const ParametersMap& map)
		{
			// Unique section id
			RegistryKeyType sectionId(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			if(sectionId > 0)
			{
				try
				{
					_section = Env::GetOfficialEnv().get<MessagesSection>(sectionId).get();
				}
				catch (ObjectNotFoundException<MessagesSection>&)
				{
					throw RequestException("No such messages section : "+ lexical_cast<string>(sectionId));
				}
			}
		}



		ParametersMap MessagesSectionsService::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap pm;

			if(_section)
			{
				_outputSection(pm, *_section);
			}
			else
			{
				BOOST_FOREACH(const MessagesSection::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<MessagesSection>())
				{
					_outputSection(pm, *it.second);
				}
			}

			return pm;
		}
		
		
		
		bool MessagesSectionsService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string MessagesSectionsService::getOutputMimeType() const
		{
			return "text/html";
		}



		MessagesSectionsService::MessagesSectionsService():
			_section(NULL)
		{

		}



		void MessagesSectionsService::_outputSection( util::ParametersMap& pm, const MessagesSection& section )
		{
			boost::shared_ptr<ParametersMap> sectionMap(new ParametersMap);
			section.toParametersMap(*sectionMap, true);
			pm.insert(TAG_SECTION, sectionMap);
		}
}	}
