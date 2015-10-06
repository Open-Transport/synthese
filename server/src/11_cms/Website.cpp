
/** Website class implementation.
	@file Website.cpp

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

#include "Website.hpp"

#include "CMSModule.hpp"
#include "Env.h"
#include "Webpage.h"

#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace cms;
	
	CLASS_DEFINITION(Website, "t025_sites", 25)
	FIELD_DEFINITION_OF_OBJECT(Website, "website_id", "websites")

	FIELD_DEFINITION_OF_TYPE(HostName, "host_name", SQL_TEXT)
	// TODO: rename to client_url once complex schema updates are in place (issue #9453).
	FIELD_DEFINITION_OF_TYPE(ClientURL, "cient_url", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DefaultTemplate, "default_page_template_id", SQL_INTEGER)

	namespace cms
	{
		Website::Website(RegistryKeyType id):
			Registrable(id),
			Object<Website, WebsiteRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_VALUE_CONSTRUCTOR(StartDate, posix_time::not_a_date_time),
					FIELD_VALUE_CONSTRUCTOR(EndDate, posix_time::not_a_date_time),
					FIELD_DEFAULT_CONSTRUCTOR(HostName),
					FIELD_DEFAULT_CONSTRUCTOR(ClientURL),
					FIELD_DEFAULT_CONSTRUCTOR(DefaultTemplate)
			)	),
			_config(NULL)
		{
		}



		void Website::addPage( Webpage& page )
		{
			if(!page.get<SmartURLPath>().empty())
			{
				mutex::scoped_lock lock (_smartURLMutex);
				_webpagesBySmartURL.insert(make_pair(page.get<SmartURLPath>(), &page));
			}
		}



		void Website::removePage( const std::string& page )
		{
			if(!page.empty())
			{
				mutex::scoped_lock lock (_smartURLMutex);
				_webpagesBySmartURL.erase(page);
			}
		}



		Webpage* Website::getPageBySmartURL( const std::string& key ) const
		{
			if(key.empty())
			{
				return NULL;
			}
			boost::mutex::scoped_lock lock (_smartURLMutex);
			WebpagesBySmartURL::const_iterator it(_webpagesBySmartURL.find(key));
			if(it == _webpagesBySmartURL.end())
			{
				return NULL;
			}
			return it->second;
		}



		bool Website::dateCheck() const
		{
			boost::gregorian::date tempDate(boost::gregorian::day_clock::local_day());
			return tempDate >= get<StartDate>() && tempDate <= get<EndDate>();
		}



		Webpage* Website::getPageByIdOrSmartURL( const std::string& key ) const
		{
			if(key.empty())
			{
				return NULL;
			}

			Webpage* result(NULL);
			if(isdigit(key[0]))
			{	// Load by id
				try
				{
					result = Env::GetOfficialEnv().getEditable<Webpage>(
						lexical_cast<RegistryKeyType>(key)
					).get();
				}
				catch(ObjectNotFoundException<Webpage>&)
				{
				}
				catch(bad_lexical_cast&)
				{
				}
			}
			if(!result)
			{
				result = getPageBySmartURL("!" + key);
			}
			return result;
		}



		synthese::SubObjects Website::getSubObjects() const
		{
			SubObjects r;
			BOOST_FOREACH(const ChildrenType::value_type& page, getChildren())
			{
				r.push_back(page.second);
			}
			return r;
		}



		void Website::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			// Registration of the site if loaded in official env
			if(&env == &Env::GetOfficialEnv())
			{
				CMSModule::AddSite(*this);
			}

		}



		void Website::unlink()
		{
			CMSModule::RemoveSite(*this);
		}



		Website::WebpagesList Website::getPagesList(
			const std::string& rootLabel
		) const {
			WebpagesList result;
			if(!rootLabel.empty())
			{
				result.push_back(make_pair(0, rootLabel));
			}
			BOOST_FOREACH(const SubObjects::value_type& so, getSubObjects())
			{
				const Webpage& page(dynamic_cast<const Webpage&>(*so));
				result.push_back(make_pair(page.getKey(), "   " + page.getName()));
				page.getPagesList(result, "      ");
			}
			return result;
		}
}	}
