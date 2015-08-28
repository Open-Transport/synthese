/** MediaLibrary class header.
	@file MediaLibrary.hpp
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

#ifndef SYNTHESE_messages_MediaLibrary_hpp__
#define SYNTHESE_messages_MediaLibrary_hpp__

#include <FactorableTemplate.h>
#include <Object.hpp>
#include <SchemaMacros.hpp>
#include <UtilTypes.h>

#include <EnumObjectField.hpp>
#include <NumericField.hpp>
#include <StringField.hpp>
#include <Website.hpp>

#include <boost/fusion/container/map.hpp>
#include <boost/fusion/support/pair.hpp>

namespace synthese
{
	namespace messages
	{
		/**
			Media library type enumeration.
			INTERNAL for classical CMS management.
			EXTERNAL when resources are hold out of CMS.
		 */
		typedef enum
		{
			INTERNAL = 0,
			EXTERNAL = 1
		} MediaLibraryType;

		/**
			Media library view enumeration: thumbnails or list
		 */
		typedef enum
		{
			THUMBNAILS = 0,
			LIST = 1
		} MediaLibraryView;

		// Specific field declarations
		FIELD_STRING(ServerHostnamePort)
		FIELD_ENUM(LibraryType, MediaLibraryType)
		FIELD_ENUM(DefaultView, MediaLibraryView)

		/** Entity framework persistent field declaration. */
		typedef boost::fusion::map<
			FIELD(Key),

			// Reference to the server hostname:port providing resources
			FIELD(ServerHostnamePort),

			// Reference to the CMS site ID
			FIELD(cms::Website),

			// Type of media library: INTERNAL or EXTERNAL
			FIELD(LibraryType),

			// Default media library view: THUMBNAILS or LIST
			FIELD(DefaultView)

		> MediaLibraryRecord;

		/**
		@class MediaLibrary
		@ingroup m17

		Media Library.

		Allow the definition of multiple media libraries
                as source of resources to include in an event message.

        Default media library when selecting a resource is hold in
        global configuration entry "default_medialibrary_id"
		*/
		class MediaLibrary :
			public Object<MediaLibrary, MediaLibraryRecord>
		{
		public:
			/// Tag holding a media library target website name
			static const std::string TAG_WEBSITE_NAME;

			/** Default constructor for registry. */
			MediaLibrary(
				util::RegistryKeyType id = 0
			);

			/**
			Inserts target CMS website name as tag in map for rendering
			*/
			virtual void addAdditionalParameters(
				util::ParametersMap& map,
				std::string prefix = std::string()
			) const;

			/** Virtual destructor. */
			virtual ~MediaLibrary() { };
		};
	}
}

#endif // SYNTHESE_messages_MediaLibrary_hpp__
