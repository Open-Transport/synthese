////////////////////////////////////////////////////////////////////////////////
/// MessagesRight class implementation.
///	@file MessagesRight.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
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
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "SecurityConstants.hpp"
#include "MessagesRight.h"
#include "AlarmRecipient.h"

#include "MessagesSectionTableSync.hpp"

#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace synthese
{
	using namespace security;
	using namespace messages;
	using namespace util;

	namespace util
	{
		template<> const std::string FactorableTemplate<Right, MessagesRight>::FACTORY_KEY("Messages");
	}

    	namespace messages
    	{
    		const string MessagesRight::MESSAGES_SECTION_FACTORY_KEY = "messagessection";
    	}

	namespace security
	{
		template<> const string RightTemplate<MessagesRight>::NAME("Gestion des messages");
		template<> const bool RightTemplate<MessagesRight>::USE_PRIVATE_RIGHTS(false);

		template<>
		ParameterLabelsVector RightTemplate<MessagesRight>::getStaticParametersLabels()
		{
			ParameterLabelsVector m;
			m.push_back(make_pair(GLOBAL_PERIMETER,"(tous les messages)"));

			vector<boost::shared_ptr<AlarmRecipient> > recipients(Factory<AlarmRecipient>::GetNewCollection());
			BOOST_FOREACH(const boost::shared_ptr<AlarmRecipient> recipient, recipients)
			{
				m.push_back(make_pair(string(), "=== " + recipient->getTitle() + " ==="));
				recipient->getParametersLabels(m);
			}
            m.push_back(make_pair(string(), "=== Sections ==="));
            m.push_back(make_pair(MessagesRight::MESSAGES_SECTION_FACTORY_KEY + "/" + GLOBAL_PERIMETER,"(toutes les sections)"));
            MessagesSectionTableSync::SearchResult sections(
                        MessagesSectionTableSync::Search(Env::GetOfficialEnv()));
            BOOST_FOREACH(const boost::shared_ptr<MessagesSection>& section, sections)
            {
                m.push_back(make_pair(MessagesRight::MESSAGES_SECTION_FACTORY_KEY + "/" + lexical_cast<string>(section->get<Key>()), section->get<Name>()));
            }

			return m;
		}
	}

	namespace messages
	{
		std::string MessagesRight::displayParameter(
			util::Env& env
		) const	{

			vector<string> parts;
			split(parts, _parameter, is_any_of("/"));

			if(parts.size() == 2)
			{
                if (parts[0] == MESSAGES_SECTION_FACTORY_KEY)
                {
                    if (parts[1] != GLOBAL_PERIMETER)
                    {
                        util::RegistryKeyType id(boost::lexical_cast<util::RegistryKeyType>(parts[1]));
                        util::RegistryTableType tableId(util::decodeTableId(id));

                        if (tableId == MessagesSectionTableSync::TABLE.ID)
                        {
                            boost::shared_ptr<const MessagesSection> ms(
                                        MessagesSectionTableSync::Get(id, env)
                                        );
                            return parts[0] + ":" + ms->get<Name>();
                        }
                        else
                        {
                            return _parameter;
                        }
                    }
                    else
                    {
                        return parts[0] + ":all";
                    }
                }
                else
                {
                    boost::shared_ptr<AlarmRecipient> recipient(Factory<AlarmRecipient>::create(parts[0]));
                    boost::shared_ptr<Right> subright(recipient->getRight(parts[1]));
                    return parts[0] + ":" + subright->displayParameter();
                }
			}
			else
			{
				return _parameter;
			}
		}

		bool MessagesRight::perimeterIncludes(
			const std::string& perimeter,
			util::Env& env
		) const	{
			vector<string> parts;
			split(parts, _parameter, is_any_of("/"));

			if(parts.size() == 2)
			{
                if (parts[0] == MESSAGES_SECTION_FACTORY_KEY)
                {
                    if (parts[1] == GLOBAL_PERIMETER)
                        return true;
                    else
                    {
                        return false;
                    }
                }
				boost::shared_ptr<AlarmRecipient> recipient(Factory<AlarmRecipient>::create(parts[0]));
				boost::shared_ptr<Right> subright(recipient->getRight(parts[1]));
				return subright->perimeterIncludes(perimeter, env);
			}
			else
			{
				return true;
			}
		}
	}
}
