
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

#include "Env.h"
#include "Webpage.h"

#include <boost/lexical_cast.hpp>

using namespace boost;

namespace synthese
{
	using namespace util;
	
	namespace cms
	{
		void Website::addPage( Webpage& page )
		{
			if(!page.getSmartURLPath().empty())
			{
				mutex::scoped_lock lock (_smartURLMutex);
				_webpagesBySmartURL.insert(make_pair(page.getSmartURLPath(), &page));
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
			return tempDate >= _startValidityDate && tempDate <= _endValidityDate;
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
}	}
