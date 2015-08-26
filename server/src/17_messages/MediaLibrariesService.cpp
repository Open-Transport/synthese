/** MediaLibrariesService class implementation.
	@file MediaLibrariesService.cpp
	@author Yves Martin
	@date 2015

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2015 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include <Env.h>
#include <MediaLibrariesService.hpp>
#include <Object.hpp>
#include <ObjectNotFoundException.h>
#include <Registry.h>
#include <Request.h>
#include <RequestException.h>
#include <UtilTypes.h>

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>

#include <iostream>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace server;
	using namespace util;

	template<>
	const string FactorableTemplate<Function, messages::MediaLibrariesService>::FACTORY_KEY = "media_libraries";

	namespace messages
	{
        const string MediaLibrariesService::TAG_MEDIA_LIBRARY = "medialibrary";

		ParametersMap MediaLibrariesService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void MediaLibrariesService::_setFromParametersMap(const ParametersMap& map)
		{
			// Reference to media library by unique ID
			RegistryKeyType objectId(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0));
			if(objectId)
			{
				try
				{
					_mediaLibrary = Env::GetOfficialEnv().getEditable<MediaLibrary>(objectId).get();
				}
				catch (ObjectNotFoundException<MediaLibrary>&)
				{
					throw RequestException("No such media library : "+ lexical_cast<string>(objectId));
					_mediaLibrary = NULL;
				}
			}
		}



		ParametersMap MediaLibrariesService::run(
			std::ostream& stream,
			const Request& request
		) const {
			// Result
			ParametersMap map;

			if(_mediaLibrary)
			{
				// Case single object list
				boost::shared_ptr<ParametersMap> libraryParameters(new ParametersMap);
				_mediaLibrary->toParametersMap(*libraryParameters, true);
				map.insert(TAG_MEDIA_LIBRARY, libraryParameters);
			}
			else
			{
				BOOST_FOREACH(
					const Registry<MediaLibrary>::value_type& it,
					Env::GetOfficialEnv().getRegistry<MediaLibrary>()
				){
					boost::shared_ptr<ParametersMap> libraryParameters(new ParametersMap);
					it.second->toParametersMap(*libraryParameters, true);
					map.insert(TAG_MEDIA_LIBRARY, libraryParameters);
				}
			}

			return map;
		}



		bool MediaLibrariesService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string MediaLibrariesService::getOutputMimeType() const
		{
			return "text/html";
		}



		MediaLibrariesService::MediaLibrariesService():
			_mediaLibrary(NULL)
		{ }

	} /* namespace messages */
} /* namespace synthese */
