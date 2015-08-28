/** MediaLibrary class implementation.
	@file MediaLibrary.cpp
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

#include <Field.hpp>
#include <MediaLibrary.hpp>
#include <Registry.h>
#include <SimpleObjectFieldDefinition.hpp>

#include <boost/fusion/container/map.hpp>

#include <string>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace messages;
	using namespace util;

	CLASS_DEFINITION(MediaLibrary, "t122_media_libraries", 122)

	namespace messages {
		FIELD_DEFINITION_OF_OBJECT(MediaLibrary, "media_library_id", "media_library_ids")

		FIELD_DEFINITION_OF_TYPE(ServerHostnamePort, "website_hostname_port", SQL_TEXT)
		FIELD_DEFINITION_OF_TYPE(LibraryType, "type", SQL_INTEGER)
		FIELD_DEFINITION_OF_TYPE(DefaultView, "default_view_mode", SQL_INTEGER)
		FIELD_DEFINITION_OF_TYPE(AcceptedMediaTypes, "accepted_media_types", SQL_TEXT)

        const string MediaLibrary::TAG_WEBSITE_NAME = "website_name";



		/// Default constructor as a Registrable
		/// @param id registry key type
		MediaLibrary::MediaLibrary(
			RegistryKeyType id /*= 0*/
		) : Registrable(id),
			Object<MediaLibrary, MediaLibraryRecord>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(ServerHostnamePort),
					FIELD_DEFAULT_CONSTRUCTOR(cms::Website),
					FIELD_VALUE_CONSTRUCTOR(LibraryType, INTERNAL),
					FIELD_VALUE_CONSTRUCTOR(DefaultView, THUMBNAILS),
					FIELD_DEFAULT_CONSTRUCTOR(AcceptedMediaTypes)
				)	)
		{ };



		void MediaLibrary::addAdditionalParameters(
			util::ParametersMap& map,
			std::string prefix)
		const {
			// CMS site media name is included for rendering in admin page
			if (get<cms::Website>()) {
				map.insert(TAG_WEBSITE_NAME, get<cms::Website>()->get<Name>());
			}
		}

	}
}
