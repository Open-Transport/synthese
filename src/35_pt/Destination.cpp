
/** Destination class implementation.
	@file Destination.cpp

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

#include "Destination.hpp"

namespace synthese
{
	using namespace util;
	using namespace pt;

	CLASS_DEFINITION(Destination, "t074_destinations", 74)
	FIELD_DEFINITION_OF_TYPE(DisplayedText, "displayed_text", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(TtsText, "tts_text", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Comment, "comment", SQL_TEXT)
	FIELD_DEFINITION_OF_OBJECT(Destination, "destination_id", "destination_ids")

	namespace pt
	{
		Destination::Destination( util::RegistryKeyType id /*= 0 */ ):
			Registrable(id),
			Object<Destination, DestinationSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(DisplayedText),
					FIELD_DEFAULT_CONSTRUCTOR(TtsText),
					FIELD_DEFAULT_CONSTRUCTOR(Comment),
					FIELD_DEFAULT_CONSTRUCTOR(impex::DataSourceLinks)
			)	)
		{
		}
}	}
