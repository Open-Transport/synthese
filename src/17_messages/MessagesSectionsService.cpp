
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
		}



		ParametersMap MessagesSectionsService::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap pm;

			// Sorting
			struct ElementLess : public std::binary_function<const MessagesSection*, const MessagesSection*, bool>
			{
				bool operator()(const MessagesSection* left, const MessagesSection* right) const
				{
					if(left && right && left->get<Rank>() != right->get<Rank>())
					{
						return left->get<Rank>() < right->get<Rank>();
					}
					return left < right;
				}
			};
			typedef set<const MessagesSection*, ElementLess> Sections;
			Sections sections;
			BOOST_FOREACH(const MessagesSection::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<MessagesSection>())
			{
				sections.insert(it.second.get());
			}

			// Output
			BOOST_FOREACH(const Sections::value_type& section, sections)
			{
				shared_ptr<ParametersMap> sectionMap(new ParametersMap);
				section->toParametersMap(*sectionMap, true);
				pm.insert(TAG_SECTION, sectionMap);
			}

			// Return
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



		void MessagesSectionsService::_outputSection( util::ParametersMap& pm, const MessagesSection& section )
		{
			boost::shared_ptr<ParametersMap> sectionMap(new ParametersMap);
			section.toParametersMap(*sectionMap, true);
			pm.insert(TAG_SECTION, sectionMap);
		}
}	}
